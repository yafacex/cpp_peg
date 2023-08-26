#pragma once
#include <map>
#include <string>
using namespace std;
namespace yfpeg{
class Vm;
class BuiltinFuncs;
class AstNode;

typedef int (BuiltinFuncs::*CppFunc)(Vm* vm,int argCount);

class BuiltinFunc {
public:
	string retType;
	CppFunc func;
	BuiltinFuncs* obj;
	string name;
	int argCount;
};
class BuiltinFuncs
{
public:
	BuiltinFuncs() {
		BuiltinFuncs::registerBuiltinFuncs();
	};
	void registerBuiltinFuncs() {
		funcsMap["print"] = {"void", &BuiltinFuncs::print, this, "print", 1 };
	};
	BuiltinFunc* getFuncByName(string name) {
		map<string, BuiltinFunc>::iterator it = funcsMap.find(name);
		if (it != funcsMap.end()) {
			return &it->second;
		}
		else {
			return nullptr;
		}
	};
	int print(Vm* vm,int argCount);
	map<string, BuiltinFunc> funcsMap;
};
}

