#include <iostream>
#include "stdafx.h"

#include "ComputerState.h"
#include "RegisterSet.h"

const int BaseSize           = 4;
const int InternalMemorySize = 32;
const int RamMemorySize      = 64;

using namespace std;

using Computer = ComputerState<BaseSize, InternalMemorySize, RamMemorySize>;

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

int main() {
	auto ram_mem = bitset<RamMemorySize>(0b01010110);
	auto comp = Computer(ram_mem);

	print_state(comp);

	int x;
	cin >> x;

	return 0;
}