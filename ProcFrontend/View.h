#pragma once

#include <bitset>

#include "Computer.h"
#include "ComputerState.h"

using std::bitset;

using Core::Computer;
using State::ComputerState;

namespace View {
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

	template<int BS, int IMS, int RMS>
	void print_registers(const RegisterSet<BS, IMS>& regs, const ComputerState<BS, IMS, RMS>& state) {
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

	template<int BS, int IMS, int RMS>
	void print_state(const Computer<BS, IMS, RMS>& cmp) {
		auto& state = cmp.State;
		cout << "Registers:" << endl;
		print_registers(cmp.Registers, state);

		cout << endl << "Internal Memory:" << endl;
		print_memory(state.CPU.get_all(), BS, BS * 4);

		cout << endl << "RAM Memory:" << endl;
		print_memory(state.RAM.get_all(), BS, BS * 4);
	}
}