#pragma once
#include <map>
#include <stdio.h>
#include <sstream>
#include <string>
#include <assert.h>
#include "Ast.h"
#include "Data.h"
#include "Scope.h"
#include "Vm.h"
#include "Log.h"
#include "Instruction.h"
#include "BuiltinFuncs.h"

//static BuiltinFuncs* builtinFuncs;

using namespace std;
namespace yfpeg {
	#define FOR_EACH_NODE(arr) for(vector<AstNode*>::iterator it = arr.begin(); it != arr.end(); ++it) 
	class Translator
	{
		typedef void (Translator::* TransFunc)(AstNode* ast, Vm* vm, OneScope* curScope);
	public:
		Translator() {
			solvers["stat_var"] = &Translator::_stat_var;
			solvers["stat_assign"] = &Translator::_stat_assign;
			solvers["stat_func_end"] = &Translator::_stat_func_end;
			solvers["stat_func_start"] = &Translator::_stat_func_start;
			solvers["stat_func"] = &Translator::_stat_func;
			solvers["callExp"] = &Translator::_callExp;
			solvers["stats"] = &Translator::_stats;
		};
		void exec(string tag, AstNode* ast, Vm* vm, OneScope* curScope) {
			TransFunc func = solvers[tag];
			if (func) {
				(this->*func)(ast, vm, curScope);
			}
		};
	protected:
		Vec2 _vec2(AstNode* ast, Vm* vm) {
			if (ast->tag == "vec2") {
				string _x = ast->children[0]->captured;
				string _y = ast->children[0]->captured;
				return Vec2(stringToNum<float>(_x), stringToNum<float>(_y));
			}
			else {
				assert(false);
				return Vec2(0.0, 0.0);
			}
		};
		float _int(AstNode* ast, Vm* vm) {
			if (ast->tag == "int") {
				string _i = ast->captured;
				return stringToNum<int>(_i);
			}
			else {
				assert(false);
				return 0.0;
			}
		};
		float _float(AstNode* ast, Vm* vm) {
			if (ast->tag == "float") {
				string _f = ast->captured;
				return stringToNum<float>(_f);
			}
			else {
				assert(false);
				return 0.0;
			}
		};
		void _parseScopeOfStats(AstNode* stats,OneScope* curScope) {
			FOR_EACH_NODE(stats->children) {
				AstNode* stat = *it;
				if (stat->tag == "stat_var" || stat->tag == "stat_func") {
					FOR_EACH_NODE(stat->children) {
						AstNode* varstat = *it;
						if (varstat->meta == ":varname") {
							curScope->setCurVarName(varstat->captured);
						}
						else if (varstat->meta == ":vartype") {
							curScope->setCurVarType(varstat->captured);
						}
					}
				}
			}
		}
	protected:
		void __assign_proc(AstNode* lvalue, AstNode* rvalue, Vm* vm, OneScope* curScope) {
			int l_offset = curScope->getOffsetOfVar(lvalue->captured);
			string rtype = rvalue->tag;
			if (rtype == "int") {
				int i = _int(rvalue, vm);
				vm->addInstruction(new AssignIntToStack(i, l_offset));
			}
			else if (rtype == "float") {
				float f = _float(rvalue, vm);
				vm->addInstruction(new AssignFloatToStack(f, l_offset));
			}
			else if (rtype == "vec2") {
				Vec2 vec2 = _vec2(rvalue, vm);
				vm->addInstruction(new AssignVec2ToStack(vec2, l_offset));
			}
			else if (rtype == "exp_var_name") {
				int r_offset = curScope->getOffsetOfVar(rvalue->captured);
				curScope->checkTypeAssignLegalByOffset(l_offset, r_offset);
				vm->addInstruction(new AssignStackToStack(l_offset, r_offset));
			}
		}
	protected:
		void _callExp(AstNode* ast, Vm* vm, OneScope* curScope) {
			AstNode* func_name = ast->children[0];
			AstNode* call_arg_list = ast->children[1];
			Var* varFunc = curScope->getVar(func_name->captured);
			if (varFunc) {

#define CASE_TYPE(VAR_NAME,TYPE_NAME,TYPE_CLS) else if(VAR_NAME == TYPE_NAME){vm->addInstruction(new AllocFuncRet<TYPE_CLS>(new TYPE_CLS()));}

				string funcRetType = varFunc->funcInfo->funcRet.retType;
				if (funcRetType == "void") {
				}
				CASE_TYPE(funcRetType,"int",NumberInt)
				CASE_TYPE(funcRetType,"float",NumberFloat)
				CASE_TYPE(funcRetType,"vec2",Vec2)
				CASE_TYPE(funcRetType,"vec3",Vec3)
				CASE_TYPE(funcRetType,"vec4", Vec4)
				CASE_TYPE(funcRetType,"ivec2",IVec2)
				CASE_TYPE(funcRetType,"ivec3",IVec3)
				CASE_TYPE(funcRetType,"ivec4", IVec4)

				vm->addInstruction(new SaveStackPointer());

				FOR_EACH_NODE(call_arg_list->children) {
					AstNode* arg = *it;
					//arg = nullptr;
					if (arg->tag == "int") {
						vm->addInstruction(new PushConstArg<NumberInt>(new NumberInt(stringToNum<int>(arg->captured))));
					}
					else if (arg->tag == "float") {
						vm->addInstruction(new PushConstArg<NumberFloat>(new NumberFloat(stringToNum<float>(arg->captured))));
					}
				}
				vm->addInstruction(new SavePc());
				vm->addInstruction(new JumpTo(varFunc->funcInfo->addr));
			}
			else {
				BuiltinFunc* func = builtinFunc.getFuncByName(func_name->captured);
				if (func) {
					string funcRetType = func->retType;
					if (funcRetType == "void") {
					}
					CASE_TYPE(funcRetType, "int", NumberInt)
					CASE_TYPE(funcRetType, "float", NumberFloat)
					CASE_TYPE(funcRetType, "vec2", Vec2)
					CASE_TYPE(funcRetType, "vec3", Vec3)
					CASE_TYPE(funcRetType, "vec4", Vec4)
					CASE_TYPE(funcRetType, "ivec2", IVec2)
					CASE_TYPE(funcRetType, "ivec3", IVec3)
					CASE_TYPE(funcRetType, "ivec4", IVec4)

					FOR_EACH_NODE(call_arg_list->children) {
						AstNode* arg = *it;
						//arg = nullptr;
						if (arg->tag == "int") {
							vm->addInstruction(new PushConstArg<NumberInt>(new NumberInt(stringToNum<int>(arg->captured))));
						}
						else if (arg->tag == "float") {
							vm->addInstruction(new PushConstArg<NumberFloat>(new NumberFloat(stringToNum<float>(arg->captured))));
						}
						else if (arg->tag == "exp_var_name") {
							int offset = curScope->getOffsetOfVar(arg->captured);
							vm->addInstruction(new PushVarArg(offset));
						}
					}
					if (func->argCount != call_arg_list->children.size()) {
						assert(0);
					}
					vm->addInstruction(new SavePc());
					vm->addInstruction(new CallCppFunc<BuiltinFuncs>(func->obj,func->func,func->argCount,func->name));
				}
				else {
					assert(0);
				}
			}
		};
		void _stat_func_start(AstNode* ast, Vm* vm, OneScope* curScope) {
			AstNode* func_name = ast->parent->children[1];
			vm->addInstruction(new CallStart(func_name->captured));
		};
		void _stat_func(AstNode* ast, Vm* vm, OneScope* curScope) {
			AstNode* func_ret_type = ast->children[1];
			AstNode* stat_func_name = ast->children[2];
			AstNode* arg_list = ast->children[4];
			AstNode* stats = ast->children[5];
			FOR_EACH_NODE(arg_list->children) {
				AstNode* argstat = *it;
				if (argstat->meta == ":varname") {
					curScope->setCurVarName(argstat->captured);
				}
				else if (argstat->meta == ":vartype") {
					curScope->setCurVarType(argstat->captured);
				}
			}
			_parseScopeOfStats(stats,curScope);
			Var* var = curScope->getVar(stat_func_name->captured);
			int nArg = arg_list->children.size() / 2;
			int i2;
			for (int i = 0; i < nArg; ++i) {
				i2 = i + i;
				AstNode* arg_type = arg_list->children[i2];
				AstNode* arg_name = arg_list->children[i2 + 1];
				var->addFuncArg(arg_name->captured, arg_type->captured);
			}
			var->setFuncAddr(vm->instructions.size());
			var->setFuncRetType(func_ret_type->captured);
		};
		void _stat_func_end(AstNode* ast, Vm* vm, OneScope* curScope) {
			vm->addInstruction(new RevertStackPointerAndPc());
		}
		void _stat_assign(AstNode* ast, Vm* vm, OneScope* curScope) {
			printf("_stat_assign\n");
			AstNode* lvalue = ast->children[0];
			AstNode* op = ast->children[1];
			AstNode* rvalue = ast->children[2];
			string ltag = lvalue->tag;
			string rtag = rvalue->tag;
			//Data* data = to_Data(lvalue);
			//vm->pushStackData(new Object(data));
			if (ltag == "varname") {
				if (rtag == "rvalue") {
					if (op->captured == "=") {
						__assign_proc(lvalue, rvalue->children[0], vm, curScope);
					}
					else if (op->captured == "+=") {
					}
				}
			}
			printf("end\n");
		};
		void _stat_var(AstNode* ast, Vm* vm, OneScope* curScope) {
			printf("_stat_var\n");
			__assign_proc(ast->children[0], ast->children[1], vm, curScope);
			printf("end\n");
		};
		void _stats(AstNode* ast, Vm* vm, OneScope* curScope) {
			_parseScopeOfStats(ast,curScope);
			vm->addInstruction(new AllocateStack(curScope->vars.size()));
		};
	protected:
		Data* to_Data(AstNode* ast) {
			string tag = ast->tag;
			string captured = ast->captured;
			if (tag == "float") {
				return new NumberFloat(stringToNum<float>(captured));
			}
			else if (tag == "int") {
				return new NumberInt(stringToNum<int>(captured));
			}
			else if (tag == "vec2") {
				vector<AstNode*>* children = &ast->children;
				return new Vec2(stringToNum<float>((*children)[0]->captured), stringToNum<float>((*children)[1]->captured));
			}
			else if (tag == "vec3") {
				vector<AstNode*>* children = &ast->children;
				return new Vec3(
					stringToNum<float>((*children)[0]->captured),
					stringToNum<float>((*children)[1]->captured),
					stringToNum<float>((*children)[2]->captured)
				);
			}
			else if (tag == "vec4") {
				vector<AstNode*>* children = &ast->children;
				return new Vec4(
					stringToNum<float>((*children)[0]->captured),
					stringToNum<float>((*children)[1]->captured),
					stringToNum<float>((*children)[2]->captured),
					stringToNum<float>((*children)[3]->captured)
				);
			}
			else if (tag == "ivec2") {
				vector<AstNode*>* children = &ast->children;
				return new IVec2(stringToNum<int>((*children)[0]->captured), stringToNum<int>((*children)[1]->captured));
			}
			else if (tag == "ivec3") {
				vector<AstNode*>* children = &ast->children;
				return new IVec3(
					stringToNum<int>((*children)[0]->captured),
					stringToNum<int>((*children)[1]->captured),
					stringToNum<int>((*children)[2]->captured)
				);
			}
			else if (tag == "ivec4") {
				vector<AstNode*>* children = &ast->children;
				return new IVec4(
					stringToNum<int>((*children)[0]->captured),
					stringToNum<int>((*children)[1]->captured),
					stringToNum<int>((*children)[2]->captured),
					stringToNum<int>((*children)[3]->captured)
				);
			}
			return nullptr;
		};
		Vec2* to_Float(AstNode* ast) {
			return nullptr;
		};
		Vec2* to_Vec2(AstNode* ast) {
			return nullptr;
		};
	protected:
		template <class Type>
		Type stringToNum(string& str)
		{
			istringstream iss(str);
			Type num;
			iss >> num;
			return num;
		}
		map<string, TransFunc> solvers;
		BuiltinFuncs builtinFunc;
	};

};
