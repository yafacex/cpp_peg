#pragma once
#include <string>
#include <vector>
#include <iostream>
//#include "Instruction.h"
#include "Data.h"
using namespace std;
namespace yfpeg {
	class Instruction;
	class Vm
	{
	public:
		Vm() {
			init();
		};
		void init() {
			//init regs
			for (int i = 0; i < 20; ++i) {
				regs.push_back(new Object(new Null()));
			}
			pc = 0;
			bp = 0;
			pc_entrance = -1;
		};
		Object* getStackObject(int offset) {
			Object* object = runStack[bp + offset];
			return object;
		};
		void setStackObject(Object* object, int offset) {
			int addr = bp + offset;
			Object* prev = runStack[addr];
			runStack[addr] = object;
			prev->release();
			object->retain();
		};
		void pushStackObject(Object* data) {
			runStack.push_back(data);
		};
		void allocStack(int size) {
			for (int i = 0; i < size; ++i) {
				runStack.push_back(new Object(new Null()));
			}
		};
		void run();
		void dumpInstructions();;
		void addInstruction(Instruction* cmd);;
		int pc;
		int pc_entrance;
		//(esp)extended stack pointer
		//(ebp)extended base pointer(save temp esp)
		int bp;
		vector<Instruction*> instructions;
		vector<Object*> runStack;
		vector<Object*> regs;
		vector<Object*> globals;
		vector<Object*> consts;
		vector<int> bpStack;//bp stack
		vector<int> pcStack;//bp stack
	};
}
