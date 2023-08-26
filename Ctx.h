#pragma once
#include <string>
#include "Ast.h"
#include "Scope.h"
#include "Vm.h"
#include "Translator.h"
namespace yfpeg {
	class Ctx
	{
	public:
		Ctx():offset(0),eat_level(0) {};
		string code;
		int eat_level;
		int offset;
		yfpeg::Ast ast;
		Scope scope;
		Vm vm;
		Translator trans;
	};
}

