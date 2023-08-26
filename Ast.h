#pragma once
#include<string>
#include<vector>
#include "Log.h"
using namespace std;

#define AstNodeVector vector<AstNode*>
#define AstNodeVectorItr vector<AstNode*>::iterator
#define AstNodeVectorFor(vec) for(AstNodeVectorItr it = vec.begin();it != vec.end(); ++it)

namespace yfpeg {
	class Ctx;
	class OneScope;
	class AstNode {
	public :
		AstNode():parent(nullptr),scope(nullptr) {};
		AstNode* parent;
		OneScope* scope;
		vector<AstNode*> children;
		string path;
		string tag;
		string captured;
		string meta;
		void add_child(AstNode* child) {
			child->parent = this;
			children.push_back(child);
		}
		string& add_path(string parentPath,string tag) {
			path = parentPath + "." + tag;
			return path;
		}
		void add_tag(string tag) {
			this->tag = tag;
		}
		void add_meta(string meta) {
			this->meta= meta;
		}
		//DEBUG
		string getIndent(int level) {
			string ret = "";
			for (int i = 0; i < level; ++i) {
				ret += "  ";
			}
			return ret;
		};
		void walk(Ctx* ctx,int level = 0) ;
	};
	class Ast
	{
	public:
		Ast() {};
		AstNode root;
		string matched;
	};
}

