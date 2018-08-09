#include <iostream>
#include "stdafx.h"

#include "ComputerState.h"
#include "RegisterSet.h"
#include "Tests.h"

const int BaseSize           = 4;
const int InternalMemorySize = 32;
const int RamMemorySize      = 64;

using namespace std;

using Computer = ComputerState<BaseSize, InternalMemorySize, RamMemorySize>;

void run_tests() {
	cerr << "Run tests:" << endl;
	Tests::test_all();
	cerr << endl;
}

template<int Size>
void print_memory(const bitset<Size>& mem, int bits_per_space, int bits_per_line) {
	int sbytes = 0;
	int lbytes = 0;
	for (int i = 0; i < Size; i++) {
		cout << int(mem[i]) << ' ';
		sbytes++;
		if (sbytes >= bits_per_space) {
			sbytes = 0;
			cout << " ";
		}
		lbytes++;
		if (lbytes >= bits_per_line) {
			lbytes = 0;
			cout << endl;
		}
	}
}

void print_registers(const Computer& state) {
	auto& regs = state.Registers;
	auto& cpu = state.CPU;
	auto flags = cpu.get(regs.Flags);
	auto terminated = cpu.get(regs.Terminated);
	cout << "Flags: " << flags << " (terminated: " << terminated << ")" << endl;
	auto counter = cpu.get(regs.Counter);
	cout << "Counter: " << counter << endl;
	auto ip = cpu.get(regs.IP);
	cout << "IP: " << ip << endl;
	auto ap = cpu.get(regs.AP);
	cout << "AP: " << ap << endl;

	auto cn = regs.get_CN_count();
	for (int i = 0; i < cn; i++) {
		auto addr = bitset<BaseSize>(i);
		auto ci = state.CPU.get(regs.get_CN(addr));
		cout << "C" << i << ": " << ci << endl;
	}
}

void print_state(const Computer& state) {
	cout << "Registers:" << endl;
	print_registers(state);

	cout << endl << "Internal Memory:" << endl;
	print_memory(state.CPU.get_all(), BaseSize, BaseSize * 4);

	cout << endl << "RAM Memory:" << endl;
	print_memory(state.RAM.get_all(), BaseSize, BaseSize * 4);
}

int main(int argc, char* argv[]) {
	auto is_test_only_mode = false;
	if (argc > 1) {
		cout << "'" << argv[1] << "'" << endl;
		string arg = argv[1];
		is_test_only_mode = (arg == "test_only_mode");
		cout << is_test_only_mode << endl;
	}

	cout << "=== CppProc ===" << endl;
	if (is_test_only_mode) {
		cout << "Test Only Mode" << endl;
	}
	cout << endl;

	run_tests();

	if (is_test_only_mode) {
		return 0;
	}

	auto ram_mem = bitset<RamMemorySize>();
	auto comp = Computer(ram_mem);

	cout << "Start execution..." << endl;
	cout << endl;

	auto running = true;
	while (running) {
		print_state(comp);
		running = comp.tick();
		cout << endl;
		cin.get();
	}

	cout << endl << "Execution done." << endl;
	cin.get();
	return 0;
}