#pragma once
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <stdio.h>
#include <cstdarg>
#include <assert.h>
#include "Ast.h"
#include "Ctx.h"
#include "Log.h"


using namespace std;
using namespace yfpeg;
#define NodeVector vector<Node*>
#define NodeVectorItr vector<Node*>::iterator
#define NodeVectorFor(vec) for(NodeVectorItr it = vec.begin();it != vec.end(); ++it)

typedef bool PatternMatchCb(void*);

#define CAST_TO(ptr,type) (dynamic_cast<type*>(ptr))


class Node
{
public:
	Node():level(0),name(""),astNodeRef(nullptr){};
	~Node() = default;
	virtual bool eat(Ctx* ctx,string* matched,AstNode* astNode) = 0;
	virtual void walk_level(int parentLevel) {
		this->level = parentLevel + 1;
	};
	//DEBUG
	void log_eat_ch(const char* prefix,const char* suffix,char ch,int eat_level,string description = "") {
		if (sNodeEatDebug) {
			if (ch == ' ') {
				const char* fmt = "%s %s:%s%s [%s]";
				LOG(fmt,prefix,"SPACE",getIndent(eat_level).c_str(),suffix,(name + ":" + description).c_str());
			}
			else {
				const char* fmt = "%s %c:%s%s [%s]";
				LOG(fmt,prefix,ch,getIndent(eat_level).c_str(),suffix,(name + ":" + description).c_str());
			}
		}
	}
	void log_eat_str(const char* prefix,const char* suffix,const char* str,int eat_level) {
		if (sNodeEatDebug) {
			const char* fmt = "%s %s:\t%s%s";
			LOG(fmt,prefix,str,getIndent(eat_level,strlen(str)).c_str(),suffix);
		}
	}
	void log_dummy(const char* prefix, const char* suffix,int eat_level) {
		const char* fmt = "%s  :%s%s[%s]";
		LOG(fmt,prefix,getIndent(eat_level).c_str(),suffix,name.c_str());
	}
	int level;
	string name;
	AstNode* astNodeRef;
};
class CompoNode :public Node {
public:
	//virtual bool eat(string code,int offset,string* matched) = 0;
	void addChild(Node* child) {
		_children.push_back(child);
	};
	virtual void walk_level(int parentLevel) {
		Node::walk_level(parentLevel);
		NodeVectorFor(_children){
			Node* node = *it;
			node->walk_level(this->level);
		}
	};
protected:
	NodeVector _children;
};
class EmptyNode : public Node
{
public:
	EmptyNode() {
	};
	virtual bool eat(Ctx* ctx, string* matched, AstNode* astNode) {
		this->astNodeRef = nullptr;
		log_eat_str("[#]","EmptyNode start-end eat","",ctx->eat_level);
		return true;
	}
};

class CharNode : public Node
{
public:
	CharNode(char ch) {
		_ch = ch;
	}
	virtual bool eat(Ctx* ctx,string* matched,AstNode* astNode) {
		this->astNodeRef = nullptr;
		char ch = ctx->code[ctx->offset];
		if (ch == _ch) {
			*matched += ch;
			log_eat_ch("[#]","CharNode eat",ch,ctx->eat_level,string(1,_ch));
			ctx->offset += 1;
			return true;
		}
		else {
			log_eat_ch("[!]","CharNode eat",ch,ctx->eat_level,string(1,_ch));
			return false;
		}
	}
private:
	char _ch;
};
class StringNode : public Node
{
public:
	StringNode(const char* _str) {
		str = _str;
	}
	virtual bool eat(Ctx* ctx,string* matched,AstNode* astNode) {
		this->astNodeRef = nullptr;
		int iBegin = ctx->offset;
		log_dummy("[#]","StringNode start eat",ctx->eat_level);
		int len = matched->size();
		for (string::iterator it = str.begin(); it != str.end(); ++it) {
			char ch = ctx->code[ctx->offset];
			if (ch != *it) {
				log_eat_str("[!]","StringNode end eat",ctx->code.substr(iBegin,ctx->offset).c_str(),ctx->eat_level);
				*matched = matched->substr(0,len);
				ctx->offset = iBegin;
				return false;
			}
			else {
				*matched += ch;
				ctx->offset += 1;
			}
		}
		log_eat_str("[#]","StringNode end eat",ctx->code.substr(iBegin,ctx->offset-iBegin).c_str(),ctx->eat_level);
		return true;
	}
public:
	string str;
};
class RangeNode : public Node
{
public:
	RangeNode(char from,char to) {
		_from = from;
		_to = to;
	}
	virtual bool eat(Ctx* ctx,string* matched,AstNode* astNode) {
		this->astNodeRef = nullptr;
		char ch = ctx->code[ctx->offset];
		if (ch >= _from && ch <= _to) {
			*matched += ch;
			log_eat_ch("[#]","RangeNode eat",ch,ctx->eat_level,string(1,_from) + "," + string(1,_to));
			ctx->offset += 1;
			return true;
		}
		else {
			log_eat_ch("[!]","RangeNode eat",ch,ctx->eat_level,string(1,_from) + "," + string(1,_to));
			return false;
		}
	}
private:
	char _from;
	char _to;
};
class SetNode :public Node {
public:
	SetNode(string set_array) {
		_set_array = set_array;
		for_each(set_array.begin(), set_array.end(), [&](char ch) ->void {
			_set_map[ch] = true;
		});
	}
	virtual bool eat(Ctx* ctx,string* matched,AstNode* astNode) {
		this->astNodeRef = nullptr;
		char ch = ctx->code[ctx->offset];
		if (_set_map[ch]) {
			*matched += ch;
			log_eat_ch("[#]","SetNode eat",ch,ctx->eat_level,_set_array);
			ctx->offset += 1;
			return true;
		}
		else {
			log_eat_ch("[!]","SetNode eat",ch,ctx->eat_level,_set_array);
			return false;
		}
	}
private:
	map<char,bool> _set_map;
	string _set_array;
};
//InnerNode
class InnerNode : public Node
{
public:
	virtual void walk_level(int parentLevel) {
		Node::walk_level(parentLevel);
		_innerNode->walk_level(this->level);
	}
protected:
	Node* _innerNode;
};
//Node Controls Other Node
class ReferenceNode : public InnerNode
{
public:
	ReferenceNode() {
	};
	void setInnerNode(Node* innerNode) {
		_innerNode = innerNode;
	};
	virtual bool eat(Ctx* ctx,string* matched,AstNode* astNode) {
		this->astNodeRef = nullptr;
		log_dummy("[#]","ReferenceNode start eat",ctx->eat_level);
		ctx->eat_level++;
		bool ret = _innerNode->eat(ctx, matched, astNode);
		ctx->eat_level--;
		log_dummy("[#]","ReferneceNode end eat",ctx->eat_level);
		return ret;
	}
};
class RepeatNode : public InnerNode
{
public:
	RepeatNode(Node* innerNode,int repeatTimes = 0,bool atLeast = true) {
		_innerNode = innerNode;
		_repeatTimes = repeatTimes;
		_atLeast = atLeast;
	};
	virtual bool eat(Ctx* ctx,string* matched,AstNode* astNode) {
		this->astNodeRef = nullptr;
		if (_atLeast) {
			int count = 0;
			ctx->eat_level++;
			while (true) {
				log_dummy("[#]","RepeatNode start eat",ctx->eat_level);
				if (_innerNode->eat(ctx, matched, astNode)) {
					count += 1;
				}
				else {
					if (count >= _repeatTimes) {
						log_dummy("[#]","RepeatNode end eat",ctx->eat_level);
						if (name == "pat_name(builtin)") {
							printf("aa");
						}
						ctx->eat_level--;
						return true;
					}
					else {
						log_dummy("[!]","RepeatNode end eat",ctx->eat_level);
						ctx->eat_level--;
						return false;
					}
				}
			};
		}
		else {
			int count = 0;
			ctx->eat_level++;
			while (true) {
				log_dummy("[#]","RepeatNode start eat",ctx->eat_level);
				if (_innerNode->eat(ctx, matched, astNode)) {
					count += 1;
				}
				else {
					if (count <= _repeatTimes) {
						log_dummy("[#]","RepeatNode end eat",ctx->eat_level);
						ctx->eat_level--;
						return true;
					}
					else {
						log_dummy("[!]","RepeatNode end eat",ctx->eat_level);
						ctx->eat_level--;
						return false;
					}
				}
			};
		}
	}
private:
	int _repeatTimes;
	bool _atLeast;
};
class RepeatRangeNode : public InnerNode
{
public:
	RepeatRangeNode(Node* innerNode, int atLeast = 0, int atMost = 0) {
		_innerNode = innerNode;
		_atLeast = atLeast;
		_atMost = atMost;
	};
	virtual bool eat(Ctx* ctx,string* matched,AstNode* astNode) {
		this->astNodeRef = nullptr;
		int count = 0;
		ctx->eat_level++;
		while (true) {
			log_dummy("[#]","RepeatRangeNode start eat",ctx->eat_level);
			if (_innerNode->eat(ctx,matched,astNode)) {
				count += 1;
			}
			else {
				if (count >= _atLeast && count <= _atMost) {
					log_dummy("[#]","RepeatRangeNode end eat",ctx->eat_level);
					ctx->eat_level--;
					return true;
				}
				else {
					log_dummy("[!]","RepeatRangeNode end eat",ctx->eat_level);
					ctx->eat_level--;
					return false;
				}
			}
		};
	}
private:
	int _atLeast;
	int _atMost;
};

class NotNode : public InnerNode
{
public:
	NotNode(Node* innerNode) {
		_innerNode = innerNode;
	}
	virtual bool eat(Ctx* ctx,string* matched,AstNode* astNode) {
		this->astNodeRef = nullptr;
		int len = matched->size();
		int iBegin = ctx->offset;
		ctx->eat_level++;
		log_dummy("[#]","NotNode start eat",ctx->eat_level);
		bool ret = !_innerNode->eat(ctx,matched,astNode);
		if (ret) {
			log_dummy("[#]", "NotNode end eat", ctx->eat_level);
			ctx->offset = iBegin;
			*matched = matched->substr(0,len);
		}
		else{
			//ctx->offset = iBegin;
			//*matched = matched->substr(0,len);
			log_dummy("[!]","NotNode end eat",ctx->eat_level);
		}
		ctx->eat_level--;
		return ret;
	}
};

class AndNode : public CompoNode
{
public:
	AndNode() : CompoNode(){
	};
	virtual bool eat(Ctx* ctx,string* matched,AstNode* astNode) {
		int len = matched->size();
		int iBegin = ctx->offset;
		ctx->eat_level++;
		NodeVectorFor(_children){
			Node* node = *it;
			log_dummy("[#]","AndNode start eat",ctx->eat_level);
			if (!node->eat(ctx, matched, astNode)) {
				log_dummy("[!]","AndNode end eat",ctx->eat_level);
				ctx->offset = iBegin;
				*matched = matched->substr(0,len);
				ctx->eat_level--;
				return false;
			}
		}
		log_dummy("[!]","AndNode end eat",ctx->eat_level);
		ctx->eat_level--;
		return true;
	};
};

class OrNode : public CompoNode
{
public:
	OrNode() {
	};
	virtual bool eat(Ctx* ctx,string* matched,AstNode* astNode) {
		int len = matched->size();
		int iBegin = ctx->offset;
		ctx->eat_level++;
		NodeVectorFor(_children){
			Node* node = *it;
			log_dummy("[#]","OrNode start eat",ctx->eat_level);
			if (node->eat(ctx, matched, astNode)) {
				log_dummy("[#]","OrNode end eat",ctx->eat_level);
				ctx->eat_level--;
				return true;
			}
		}
		log_dummy("[!]","OrNode end eat",ctx->eat_level);
		ctx->offset = iBegin;
		*matched = matched->substr(0,len);
		ctx->eat_level--;
		return false;
	};
};
//ConditionNode can be implemented by NotNode,example 
//if ch == '.' then node1->eat() else node2->eat()   equals NotNode(ch('.')) and node2->eat() or node1->eat()
//by ConditionNode make description more readable
typedef bool ConditionCb(char ch);
class ConditionNode : public CompoNode
{
public:
	ConditionNode(ConditionCb* cond,Node* trueNode,Node* falseNode) {
		this->cond = cond;
		this->_children.push_back(trueNode);
		this->_children.push_back(falseNode);
	};
	virtual bool eat(Ctx* ctx,string* matched,AstNode* astNode) {
		this->astNodeRef = nullptr;
		char ch = ctx->code[ctx->offset];
		if (cond(ch)) {
			return _children[0]->eat(ctx, matched, astNode);
		}
		else {
			return _children[1]->eat(ctx, matched, astNode);
		}
	}
private:
	ConditionCb* cond;
};

class CaptureMetaNode : public InnerNode
{
public:
	CaptureMetaNode(Node* innerNode,string meta) {
		_innerNode = innerNode;
		astNodeRef = nullptr;
		this->meta = meta;
	};
	virtual bool eat(Ctx* ctx,string* matched,AstNode* astNode) {
		this->astNodeRef = nullptr;
		int iBegin = ctx->offset;
		log_eat_str("[#]","CaptureMetaNode start eat",meta.c_str(),ctx->eat_level);
		AstNode* child = new AstNode();
		child->scope = ctx->scope.curScope;
		ctx->eat_level++;
		bool ret = _innerNode->eat(ctx,matched,child);
		ctx->eat_level--;
		captured = ctx->code.substr(iBegin,ctx->offset - iBegin);
		if (captured != "") {
			astNode->add_child(child);
			astNodeRef = child;
			child->captured = captured;
			if (_innerNode->astNodeRef) {
				_innerNode->astNodeRef->add_meta(meta);
			}
			child->add_meta(meta);
			log_eat_str("[#]","CaptureMetaNode end eat",(meta + ":" + captured).c_str(),ctx->eat_level);
		}
		else {
			EmptyNode* emptyNode = CAST_TO(_innerNode,EmptyNode);
			if (emptyNode) {
				astNode->add_child(child);
				astNodeRef = child;
				child->captured = captured;
				if (_innerNode->astNodeRef) {
					_innerNode->astNodeRef->add_meta(meta);
				}
				child->add_meta(meta);
				log_eat_str("[#]","CaptureMetaNode end eat[EmptyNode]",meta.c_str(),ctx->eat_level);
			}
			else {
				log_eat_str("[!]","CaptureMetaNode end eat Failed!",meta.c_str(),ctx->eat_level);
			}
		}
		return ret;
	};
	string captured;
private:
	string meta;
};
class CaptureNode : public InnerNode
{
public:
	CaptureNode(Node* innerNode,string tag) {
		_innerNode = innerNode;
		astNodeRef = nullptr;
		this->tag = tag;
	};
	virtual bool eat(Ctx* ctx,string* matched,AstNode* astNode) {
		this->astNodeRef = nullptr;
		int iBegin = ctx->offset;
		log_eat_str("[#]","CaptureNode start eat",tag.c_str(),ctx->eat_level);
		AstNode* child = new AstNode();
		child->scope = ctx->scope.curScope;
		ctx->eat_level++;
		bool ret = _innerNode->eat(ctx,matched,child);
		ctx->eat_level--;
		captured = ctx->code.substr(iBegin,ctx->offset - iBegin);
		if (captured != "") {
			astNode->add_child(child);
			astNodeRef = child;
			child->captured = captured;
			//child->add_path(astNode->path, tag);
			child->add_tag(tag);
			log_eat_str("[#]","CaptureNode end eat",(tag + ":" + captured).c_str(),ctx->eat_level);
		}
		else {
			EmptyNode* emptyNode = CAST_TO(_innerNode,EmptyNode);
			if (emptyNode) {
				astNode->add_child(child);
				astNodeRef = child;
				child->captured = captured;
				child->add_tag(tag);
				log_eat_str("[#]","CaptureNode end eat[EmptyNode]",tag.c_str(),ctx->eat_level);
			}
			else {
				log_eat_str("[!]","CaptureNode end eat Failed!",tag.c_str(),ctx->eat_level);
			}
		}
		return ret;
	};
	string captured;
private:
	string tag;
};
class CapturedNode : public Node
{
public:
	CapturedNode(string tag) {
		astNodeRef = nullptr;
		this->tag = tag;
	};
	virtual bool eat(Ctx* ctx,string* matched,AstNode* astNode) {
		this->astNodeRef = nullptr;
		log_eat_str("[#]","CaptureNode start eat",tag.c_str(),ctx->eat_level);
		AstNode* child = new AstNode();
		child->scope = ctx->scope.curScope;
		ctx->eat_level++;
		ctx->eat_level--;
		{
			captured = tag;
			astNode->add_child(child);
			astNodeRef = child;
			child->captured = captured;
			//child->add_path(astNode->path, tag);
			child->add_tag(tag);
			log_eat_str("[#]","CaptureNode end eat",(tag + ":" + captured).c_str(),ctx->eat_level);
		}
		return true;
	};
	string captured;
private:
	string tag;
};

enum FILTER_TYPE {
	FILTER_NONE = 0,
	FILTER_IF_ONE_NODE,
};
class FilterNode : public InnerNode
{
public:
	FilterNode(Node* innerNode,FILTER_TYPE filterType = FILTER_NONE) {
		_innerNode = innerNode;
		_filterType = filterType;
	};
	virtual bool eat(Ctx* ctx, string* matched, AstNode* astNode) {
		this->astNodeRef = nullptr;
		bool ret = _innerNode->eat(ctx,matched,astNode);
		CaptureNode* capNode = CAST_TO(_innerNode,CaptureNode);
		if (capNode) {
			switch (_filterType)
			{
				case FILTER_NONE:
					break;
				case FILTER_IF_ONE_NODE: {
					AstNode* ast = capNode->astNodeRef;
					if (ast && ast->children.size() == 1) {
						AstNode* astParent = ast->parent;
						if (astParent && ast->children.size() == 1) {
							vector<AstNode*>::iterator result = find(astParent->children.begin(), astParent->children.end(),ast);
							if (result != astParent->children.end()) {
								*result = ast->children[0];
							}
						}
					}
					break;
				}
			}
		}
		return ret;
	};
	FILTER_TYPE _filterType;
};

class ScopeNode : public InnerNode
{
public:
	ScopeNode(Node* innerNode) {
		_innerNode = innerNode;
	};
	virtual bool eat(Ctx* ctx,string* matched,AstNode* astNode) {
		this->astNodeRef = nullptr;
		log_eat_str("[#]","CaptureNode start eat","ScopeStart",ctx->eat_level);
		ctx->eat_level++;
		ctx->scope.newChild();
		bool ret = _innerNode->eat(ctx,matched,astNode);
		if (ret) {
			ctx->scope.revert();
		}
		else {
			ctx->scope.revertAndRemove();
		}
		ctx->eat_level--;
		log_eat_str("[#]","CaptureNode end eat","ScopeEnd",ctx->eat_level);
		return ret;
	};
};
enum SCOPE_VAR_CAPTURE_TYPE {
	CAP_SCOPE_VAR_NAME = 0,
	CAP_SCOPE_VAR_TYPE = 1,
};
class ScopeVarCaptureNode : public InnerNode
{
public:
	ScopeVarCaptureNode(Node* innerNode,SCOPE_VAR_CAPTURE_TYPE type) {
		capType = type;
		CaptureNode* capNode = CAST_TO(innerNode,CaptureNode);
		StringNode* strNode = CAST_TO(innerNode,StringNode);
		CapturedNode* capedNode = CAST_TO(innerNode,CapturedNode);
		if (!capNode && !strNode && !capedNode) {
			printf("ScopeCaptureNode's innerNode must be CaptureNode\n");
			_innerNode = nullptr;
			assert(0);
		}
		else {
			_innerNode = innerNode;
		}
	};
	virtual bool eat(Ctx* ctx,string* matched,AstNode* astNode) {
		this->astNodeRef = nullptr;
		log_eat_str("[#]","ScopeCaptureNode start eat","ScopeStart",ctx->eat_level);
		ctx->eat_level++;
		bool ret = _innerNode->eat(ctx,matched,astNode);
		/*
		CaptureNode* capNode = CAST_TO(_innerNode,CaptureNode);
		if (capNode) {
			if (capType == CAP_SCOPE_VAR_NAME) {
				ctx->scope.setCurVarName(capNode->captured);
			}
			else if (capType == CAP_SCOPE_VAR_TYPE) {
				ctx->scope.setCurVarType(capNode->captured);
			}
		}
		else {
			StringNode* strNode = CAST_TO(_innerNode,StringNode);
			if (strNode) {
				if (capType == CAP_SCOPE_VAR_NAME) {
					ctx->scope.setCurVarName(strNode->str);
				}
				else if (capType == CAP_SCOPE_VAR_TYPE) {
					ctx->scope.setCurVarType(strNode->str);
				}
			}
			else {
				CapturedNode* capedNode = CAST_TO(_innerNode,CapturedNode);
				if (capedNode) {
					if (capType == CAP_SCOPE_VAR_NAME) {
						ctx->scope.setCurVarName(capedNode->captured);
					}
					else if (capType == CAP_SCOPE_VAR_TYPE) {
						ctx->scope.setCurVarType(capedNode->captured);
					}
				}
				else {
					assert(0);
				}
			}
		}*/
		ctx->eat_level--;
		log_eat_str("[#]","ScopeCaptureNode end eat","ScopeEnd",ctx->eat_level);
		return ret;
	};
	SCOPE_VAR_CAPTURE_TYPE capType;
};

class CbNode : public Node
{
public:
	CbNode() {
	};
	virtual bool eat(string code, int& offset, string * matched) {
		return true;
	};
};
