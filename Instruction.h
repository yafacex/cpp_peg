#pragma once
#include <string>
#include <sstream>
#include "Vm.h"
//class Vm;
using namespace std;
namespace yfpeg {
	class Instruction {
	public:
		Instruction():name(""),id(0) {};
		string name;
		int id;
		virtual void exec(Vm* vm) = 0;
		virtual string description() = 0;
	};
	class Addr {
	public:
		Addr(int addr,string addrType) {
			this->addr = addr;
			this->addrType = addrType;
		};
		int addr;
		string addrType;
	};
	template<typename CLS>
	class CallCppFunc : public Instruction {
	public:
		typedef int (CLS::*CppFunc)(Vm* vm,int argCount);
		CallCppFunc(CLS* obj,CppFunc func,int argCount,string name):Instruction() {
			this->obj = obj;
			this->cppFunc = func;
			this->argCount = argCount;
			this->name = name;
		};
		virtual void exec(Vm* vm) {
			(obj->*cppFunc)(vm,argCount);
		};
		virtual string description() {
			return "CallCppFunc:" + name;
		};
		CppFunc cppFunc;
		CLS* obj;
		int argCount;
		string name;
	};
	class SaveStackPointer : public Instruction {
	public:
		SaveStackPointer() :Instruction() {
		};
		virtual void exec(Vm* vm) {
			vm->bpStack.push_back(vm->bp);
			vm->bp = vm->runStack.size();
		};
		virtual string description() {
			return "SaveStackPointer";
		};
	};
	class SavePc: public Instruction {
	public:
		SavePc() :Instruction() {
		};
		virtual void exec(Vm* vm) {
			vm->pcStack.push_back(vm->pc);
		};
		virtual string description() {
			return "SavePc";
		};
	};
	class RevertStackPointerAndPc : public Instruction {
	public:
		RevertStackPointerAndPc() :Instruction() {
		};
		virtual void exec(Vm* vm) {
			vm->bp = vm->bpStack.back();
			vm->bpStack.pop_back();
			vm->pc = vm->pcStack.back() + 1;
			vm->pcStack.pop_back();
		};
		virtual string description() {
			return "RevertStackPointerAndPc";
		};
	};
	template<typename DataType>
	class AllocFuncRet : public Instruction {
	public:
		AllocFuncRet(DataType* value) :Instruction() {
			this->value = value;
		};
		virtual void exec(Vm* vm) {
			vm->pushStackObject(new Object(value));
		};
		virtual string description() {
			return "AllocFuncRet:" + value->description();
		};
		DataType* value;
	};
	class AllocateStack : public Instruction {
	public:
		AllocateStack(int count) :Instruction() {
			this->count = count;
		};
		virtual void exec(Vm* vm) {
			for (int i = vm->runStack.size(); i < vm->bp + count; ++i) {
				vm->pushStackObject(new Object(new Null()));
			}
		};
		virtual string description() {
			return "AllocateStack:" + int2str(count);
		};
		int count;
	};
	template<typename DataType>
	class PushConstArg : public Instruction {
	public:
		PushConstArg(DataType* value) :Instruction() {
			this->value = value;
		};
		virtual void exec(Vm* vm) {
			vm->pushStackObject(new Object(value));
		};
		virtual string description() {
			return "PushConstArg:" + value->description();
		};
		DataType* value;
	};
	class PushVarArg : public Instruction {
	public:
		PushVarArg (int offset) :Instruction() {
			this->offset = offset;
		};
		virtual void exec(Vm* vm) {
			Object* value = vm->getStackObject(offset);
			vm->pushStackObject(value);
		};
		virtual string description() {
			return "PushVarArg:" + int2str(offset);
		};
		int offset;
	};
	class JumpTo : public Instruction {
	public:
		JumpTo(int pc = -1):Instruction() {
			this->pc_addr = pc;
		};
		virtual void exec(Vm* vm) {
			vm->pc = pc_addr;
		};
		virtual string description() {
			return "JumpTo:" + int2addrStr(pc_addr);
		};
		int pc_addr;
	};
	class CallStart : public Instruction {
	public:
		CallStart(string func_name):Instruction() {
			this->func_name = func_name;
		};
		virtual void exec(Vm* vm) {
			cout << "exec CallStart:" << func_name << endl;
		};
		virtual string description() {
			return "CallStart:" + func_name;
		};
		string func_name;
	};
	class CallReturn : public Instruction {
	public:
		CallReturn():Instruction() {
		};
		virtual void exec(Vm* vm) {
		};
		virtual string description() {
			return "CallReturn";
		};
	};
	class AssignStackToStack : public Instruction {
	public:
		AssignStackToStack(int _loffset,int _roffset):Instruction() {
			loffset = _loffset;
			roffset = _roffset;
		};
		virtual void exec(Vm* vm) {
			Object* rvalue = vm->getStackObject(roffset);
			vm->setStackObject(rvalue,loffset);
		};
		virtual string description() {
			return "AssignStackToStack:" + int2addrStr(loffset) + "<-" + int2addrStr(roffset);
		};
		int loffset;
		int roffset;
	};
	class AssignFloatToStack : public Instruction {
	public:
		AssignFloatToStack(float _fvalue,int _loffset):Instruction() {
			fvalue = _fvalue;
			loffset = _loffset;
		};
		virtual void exec(Vm* vm) {
			Object* lvalue = vm->getStackObject(loffset);
			lvalue->data = new NumberFloat(fvalue);
		};
		virtual string description() {
			return "AssignFloatToStack:" + int2addrStr(loffset) + "<-" + float2str(fvalue);
		};
		float fvalue;
		int loffset;
	};
	class AssignIntToStack : public Instruction {
	public:
		AssignIntToStack(int _ivalue,int _loffset):Instruction() {
			ivalue = _ivalue;
			loffset = _loffset;
		};
		virtual void exec(Vm* vm) {
			Object* lvalue = vm->getStackObject(loffset);
			lvalue->data = new NumberFloat(ivalue);
		};
		virtual string description() {
			return "AssignIntToStack:" + int2addrStr(loffset) + "<-" + int2str(ivalue);
		};
		float ivalue;
		int loffset;
	};
	class AssignVec2ToStack : public Instruction {
	public:
		AssignVec2ToStack(Vec2& _vec2,int _loffset):Instruction() {
			vec2 = _vec2;
			loffset = _loffset;
		};
		virtual void exec(Vm* vm) {
			Object* lvalue = vm->getStackObject(loffset);
			lvalue->data = new Vec2(vec2.value_x,vec2.value_y);
		};
		virtual string description() {
			return "AssignVec2ToStack:" + int2addrStr(loffset) + "<-" + vec2_2str(vec2.value_x,vec2.value_y);
		};
		Vec2 vec2;
		int loffset;
	};
	class LoadConst: public Instruction {
	public:
		LoadConst(Data* _value,int _offset):Instruction() {
			offset = _offset;
			value = _value;
		};
		virtual void exec(Vm* vm) {
			Data* newData = value->clone();
			vm->setStackObject(new Object(newData),offset);
		};
		virtual string description() {
			return "LoadConst:" + int2addrStr(offset) + "<-" + value->description();
		};
		Data* value;
		int offset;
	};
	template<typename VecType>
	class VecAdd: public Instruction {
	public:
		VecAdd(VecType* _lvalue,VecType* _rvalue,int _offset):Instruction() {
			lvalue = _lvalue;
			rvalue = _rvalue;
			offset = _offset;
		};
		virtual void exec(Vm* vm) {
			VecType* result = lvalue + rvalue;
			vm->setStackObject(new Object(result),offset);
		};
		virtual string description() {
			return "VecAdd:" + lvalue->description() + "+" + rvalue->description();
		};
		VecType* lvalue;
		VecType* rvalue;
		int offset;
	};
	template<typename VecType>
	class VecSub: public Instruction {
	public:
		VecSub(VecType* _lvalue,VecType* _rvalue,int _offset):Instruction() {
			lvalue = _lvalue;
			rvalue = _rvalue;
			offset = _offset;
		};
		virtual void exec(Vm* vm) {
			VecType* result = lvalue - rvalue;
			vm->setStackObject(new Object(result),offset);
		};
		virtual string description() {
			return "VecSub:" + lvalue->description() + "-" + rvalue->description();
		};
		VecType* lvalue;
		VecType* rvalue;
		int offset;
	};
	template<typename VecType>
	class VecMult: public Instruction {
	public:
		VecMult(VecType* _lvalue,VecType* _rvalue,int _offset):Instruction() {
			lvalue = _lvalue;
			rvalue = _rvalue;
			offset = _offset;
		};
		virtual void exec(Vm* vm) {
			VecType* result = lvalue * rvalue;
			vm->setStackObject(new Object(result),offset);
		};
		virtual string description() {
			return "VecMult:" + lvalue->description() + "*" + rvalue->description();
		};
		VecType* lvalue;
		VecType* rvalue;
		int offset;
	};
	template<typename VecType>
	class VecDiv: public Instruction {
	public:
		VecDiv(VecType* _lvalue,VecType* _rvalue,int _offset):Instruction() {
			lvalue = _lvalue;
			rvalue = _rvalue;
			offset = _offset;
		};
		virtual void exec(Vm* vm) {
			VecType* result = lvalue * rvalue;
			vm->setStackObject(new Object(result),offset);
		};
		virtual string description() {
			return "VecDiv:" + lvalue->description() + "/" + rvalue->description();
		};
		VecType* lvalue;
		VecType* rvalue;
		int offset;
	};
	template<typename VecType>
	class VecDot: public Instruction {
	public:
		VecDot(VecType* _lvalue,VecType* _rvalue,int _offset):Instruction() {
			lvalue = _lvalue;
			rvalue = _rvalue;
			offset = _offset;
		};
		virtual void exec(Vm* vm) {
			VecType* result = lvalue->dot(rvalue);
			vm->setStackObject(new Object(result),offset);
		};
		virtual string description() {
			return "VecDot:" + lvalue->description() + "/" + rvalue->description();
		};
		VecType* lvalue;
		VecType* rvalue;
		int offset;
	};
	template<typename VecType>
	class VecCross: public Instruction {
	public:
		VecCross(VecType* _lvalue,VecType* _rvalue,int _offset):Instruction() {
			lvalue = _lvalue;
			rvalue = _rvalue;
			offset = _offset;
		};
		virtual void exec(Vm* vm) {
			VecType* result = lvalue->cross(rvalue);
			vm->setStackObject(new Object(result),offset);
		};
		virtual string description() {
			return "VecCross:" + lvalue->description() + "/" + rvalue->description();
		};
		VecType* lvalue;
		VecType* rvalue;
		int offset;
	};
}

