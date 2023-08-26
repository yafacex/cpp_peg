#include "Instruction.h"
#include "Vm.h"
#include "Log.h"

using namespace yfpeg;
void yfpeg::Vm::run() {
	pc = pc_entrance;
	int finalAddr = instructions.size() - 1;
	bpStack.push_back(0);
	while (pc != finalAddr) {
		cout << "RUN_PC:"<< pc << endl;
		if (pc == 11) {
			cout << "DEBUG_RUN_PC:"<< pc << endl;
		}
		instructions[pc]->exec(this);
		pc++;
	};
	cout << "RUN_PC:"<< pc << endl;
}
void yfpeg::Vm::dumpInstructions() {
	cout<<"**INSTRUCTIONS**"<<endl;
	for (int i = 0; i < instructions.size(); ++i) {
		Instruction* cmd = instructions[i];
		cout << ("[" + int2str(i) + "]")<< cmd->description() << endl;
	}
}
void yfpeg::Vm::addInstruction(yfpeg::Instruction* cmd) {
	instructions.push_back(cmd);
}
