#pragma once
#include<vector>
#include<map>
#include<string>
#include<algorithm>
#include<assert.h>
#include"Log.h"
#include"Ast.h"
using namespace std;
namespace yfpeg {
	class FuncArg {
	public:
		FuncArg(string name,string type)
			:index(-1),argName(name),argType(type) {};
		string argName;
		string argType;
		int index;
	};
	class FuncRet {
	public:
		FuncRet(){};
		string retType;
	};
	class FuncInfo {
	public:
		FuncInfo():addr(-1) {};
		int addr;
		vector<FuncArg*> args;
		FuncRet funcRet;
		void addArg(string name,string type) {
			FuncArg* arg = new FuncArg(name,type);
			arg->index = args.size();
			args.push_back(arg);
		};
	};
	class Var{
	public:
		Var(string name,string type = "") {
			this->name = name;
			this->type = type;
			funcInfo = nullptr;
		};
		void addFuncArg(string name,string type) {
			if (funcInfo == nullptr) {
				funcInfo = new FuncInfo();
			}
			funcInfo->addArg(name,type);
		};
		void setFuncAddr(int addr) {
			if (!funcInfo) {
				funcInfo = new FuncInfo();
			}
			funcInfo->addr = addr;
		};
		void setFuncRetType(string type) {
			if (!funcInfo) {
				funcInfo = new FuncInfo();
			}
			funcInfo->funcRet.retType = type;
		};
		string name;
		string type;
		FuncInfo* funcInfo;
	};
	class OneScope {
	public:
		OneScope():curVarName(""), curVarType("") {};
		Var* addVar(string name,string type = "") {
			Var* var = new Var(name,type);
			vars.push_back(var);
			return var;
		}
		Var* getVar(string name) {
			OneScope* cur = this;
			while (cur) {
				for (int i = cur->vars.size()-1 ; i >= 0; --i) {
					if ((cur->vars[i])->name == name) {
						return cur->vars[i];
					}
				}
				cur = cur->parent;
			}
			return nullptr;
		}
		void addChild(OneScope* child) {
			children.push_back(child);
			child->parent = this;
		}
		int getOffsetOfVar(string name) {
			for (int i = vars.size()-1 ; i >= 0; --i) {
				if ((vars[i])->name == name) {
					return i;
				}
			}
			return -1;
		}
		int checkTypeAssignLegalByOffset(int l_offset,int r_offset) {
			static map<string, string> legalConverts = {
				{"int","float"},
				{"float","int"},
				{"vec2","ivec2"},
				{"vec3","ivec3"},
				{"vec4","ivec4"},
			};
			if (l_offset < vars.size() && r_offset < vars.size()) {
				string& ltype = vars[l_offset]->type;
				string& rtype = vars[r_offset]->type;
				if (ltype == rtype) {
					return true;
				}
				else {
					if (legalConverts[ltype] == rtype) {
						return true;
					}
					else {
						LOG("Error:ilegal type convertion:can't assign %s to %s",rtype.c_str(),ltype.c_str());
						assert(false);
						return false;
					}
				}
			}
			else {
				assert(false);
			}
		}
		void walk(int scope_level = 0) {
			for (vector<Var*>::iterator it = vars.begin(); it != vars.end(); ++it) {
				Var* var = *it;
				LOG("%s%s:%s",getIndent(scope_level).c_str(),var->name.c_str(),var->type.c_str());
			}
			for (vector<OneScope*>::iterator it = children.begin(); it != children.end(); ++it) {
				OneScope* one = *it;
				one->walk(scope_level++);
			}
		}
		void setCurVarName(string name) {
			curVarName = name;
			_checkVar();
		}
		void setCurVarType(string type) {
			curVarType = type;
			_checkVar();
		}
		void _checkVar() {
			if (curVarName != "" && curVarType != "") {
				addVar(curVarName,curVarType);
				curVarName = "";
				curVarType = "";
			}
		}
		string curVarName;
		string curVarType;
		vector<Var*> vars;
		vector<OneScope*> children;
		OneScope* parent;
	};
	class Scope
	{
	public:
		Scope() {};
		OneScope* newChild() {
			if (!curScope) {
				OneScope* one = new OneScope();
				curScope = one;
				root = curScope;
				return one;
			}
			else {
				OneScope* one = new OneScope();
				curScope->addChild(one);
				curScope = one;
				return one;
			}
		}
		void revert() {
			curScope = curScope->parent;
		}
		void revertAndRemove() {
			OneScope* parent = curScope->parent;
			if (parent) {
				vector<OneScope*>::iterator it = find(parent->children.begin(), parent->children.end(), curScope);
				if (it != parent->children.end()) {
					parent->children.erase(it);
				}
				curScope = curScope->parent;
			}
		}
		void addVar(string name,string type = "") {
			curScope->addVar(name,type);
		}
		void push(OneScope* scope) {
			stack.push_back(scope);
			curScope = scope;
		};
		void pop() {
			curScope = stack.back();
			stack.pop_back();
		};
		int findEntrance(string entranceName) {
			Var* entrance = root->getVar(entranceName);
			return entrance->funcInfo->addr;
		};
		OneScope* curScope;
		OneScope* root;
		vector<OneScope*> stack;
	};

}
