#include "Ast.h"
#include "Ctx.h"
using namespace yfpeg;
void AstNode::walk(Ctx* ctx,int level) {
	LOG("%s%s:%s",getIndent(level).c_str(),tag.c_str(),captured.c_str());
	//if (this->scope) {
	//	ctx->scope.push(this->scope);
	//}
	ctx->trans.exec(tag,this,&ctx->vm,this->scope);
	AstNodeVectorFor(children){
		AstNode * child = *it;
		int new_level = level + 1;
		child->walk(ctx,new_level);
	}
	//if (this->scope) {
	//	ctx->scope.pop();
	//}
}
