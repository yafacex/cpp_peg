#include "BuiltinFuncs.h"
#include "Vm.h"
#include "Ast.h"
#include <vector>
#include <iostream>

using namespace std;
using namespace yfpeg;

#define FOR_EACH_NODE(arr) for(vector<AstNode*>::iterator it = arr.begin(); it != arr.end(); ++it) 
int yfpeg::BuiltinFuncs::print(Vm* vm,int argCount)
{
	cout << "BuiltinFuncs::print BEGIN" << endl;
	for (int i = 0; i < argCount; ++i) {
		cout << vm->runStack[vm->bp + i]->data->description() << endl;
	}
	cout << "BuiltinFuncs::print END" << endl;
	return 0;
}
