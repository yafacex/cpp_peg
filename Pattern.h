#pragma once
#include "Node.h"
#include "Ctx.h"
#include "Scope.h"
namespace yfpeg {


	class Pattern
	{
	public:
		Pattern(Node* node) {
			pNode = node;
		}
		~Pattern() {
		}
		//And
		Pattern& operator*(Pattern& rvalue) ;
		//Or
		Pattern& operator+(Pattern& rvalue) ;
		//Not
		Pattern& operator-(Pattern& rvalue) ;
		//Assign
		Pattern& operator=(Pattern& rvalue) ;
		//Capture
		Pattern& operator<<(string name) ;
		//Capture Filter
		Pattern& operator>>(FILTER_TYPE filterType) ;
		//atLeast or atMost
		Pattern& operator^(int times) ;
		//Callback
		Pattern& operator/(Pattern& rvalue) ;

		Ctx parse(string code,Ctx* _ctx = nullptr) {
			int idx = 0;
			if (_ctx == nullptr) {
				Ctx ctx;
				ctx.code = code;
				//pNode->walk_level(0);
				pNode->eat(&ctx,&ctx.ast.matched,&ctx.ast.root);
				return ctx;
			}
			else {
				_ctx->code = code;
				//pNode->walk_level(0);
				pNode->eat(_ctx,&_ctx->ast.matched,&_ctx->ast.root);
				return *_ctx;
			}
		}

		Node* pNode;
	};

	static Pattern& placeholder() {
		return *new Pattern(new ReferenceNode());
	};
	static Pattern& mark(string tag) {
		return *new Pattern(new CaptureNode(new EmptyNode(),tag));
	};
	static Pattern& ch(char c) {
		return *new Pattern(new CharNode(c));
	};
	static Pattern& str(const char* str) {
		Pattern& ret = *new Pattern(new StringNode(str));
		ret.pNode->name = string("string:") + string(str);
		return ret;
	};
	static Pattern& range(char from,char to) {
		return *new Pattern(new RangeNode(from,to));
	};
	static Pattern& repeat_range(Pattern& pattern,int atLeast,int atMost) {
		return *new Pattern(new RepeatRangeNode(pattern.pNode,atLeast,atMost));
	};
	static Pattern& set(const char* sets_str) {
		string sets(sets_str);
		return *new Pattern(new SetNode(sets));
	};
	static Pattern& capture(Pattern& pattern,string tag) {
		return *new Pattern(new CaptureNode(pattern.pNode,tag));
	};
	static Pattern& cond(ConditionCb* _cond ,Pattern& truePat,Pattern& falsePat) {
		return *new Pattern(new ConditionNode(_cond,truePat.pNode,falsePat.pNode));
	};
	static Pattern& scope(Pattern& pattern) {
		return *new Pattern(new ScopeNode(pattern.pNode));
	};
	static Pattern& scope_varname(Pattern& pattern) {
		//return *new Pattern(new ScopeVarCaptureNode(pattern.pNode,CAP_SCOPE_VAR_NAME));
		return *new Pattern(new CaptureMetaNode(pattern.pNode,":varname"));
	};
	static Pattern& scope_vartype(Pattern& pattern) {
		//return *new Pattern(new ScopeVarCaptureNode(pattern.pNode,CAP_SCOPE_VAR_TYPE));
		return *new Pattern(new CaptureMetaNode(pattern.pNode,":vartype"));
	};
	static Pattern& scope_vartype(string tag) {
		return *new Pattern(new ScopeVarCaptureNode(new CapturedNode(tag),CAP_SCOPE_VAR_TYPE));
	};

	//some builtin pattern,general use
	//base
	static Pattern& pat_digit() {
		return *new Pattern(new RangeNode('0','9'));
	}
	static Pattern& pat_alpha() {
		OrNode* orNode = new OrNode();
		orNode->addChild(new RangeNode('a', 'z'));
		orNode->addChild(new RangeNode('A', 'Z'));
		return *new Pattern(orNode);
	}
	static Pattern& pat_alnum() {
		OrNode* orNode = new OrNode();
		orNode->addChild(new RangeNode('a', 'z'));
		orNode->addChild(new RangeNode('A', 'Z'));
		orNode->addChild(new RangeNode('0', '9'));
		return *new Pattern(orNode);
	}
	//math
	static Pattern& pat_int() {
		return (set("+-")^-1) * (pat_digit()^1);
	}
	static Pattern& pat_float() {
		return (
			set("+-") ^ -1) * (
				cond([](char ch)->bool {return ch == '.'; },
				ch('.') * (pat_digit()^1),
				(pat_digit()^1) * ch('.') * (pat_digit()^0)
			)
		);
	}
	//var name or func name
	static Pattern& pat_name() {
		Pattern& ret = (pat_alpha() + ch('_'))*((pat_alnum() + ch('_'))^0);
		ret.pNode->name = "pat_name(builtin)";
		return ret;
	}
	//space
	static Pattern& pat_space() {
		return set(" \t\n\r");
	}
	static Pattern& pat_named(Pattern& pat,string name) {
		pat.pNode->name = name;
		return pat;
	}
}

