#pragma once

#include <bitset>

#include "Computer.h"
#include "ComputerState.h"

using std::bitset;

using Core::Computer;
using State::ComputerState;

namespace View {
	template<int BaseSize, int TotalSize>
	void print_memory(const bitset<TotalSize>& mem, int sizes_per_line) {
		int sizes = 0;
		for (int i = 0; i < TotalSize; i += BaseSize) {
			cout << BitUtils::get_bits<BaseSize>(mem, i) << ' ';
			sizes++;
			if (sizes >= sizes_per_line) {
				sizes = 0;
				cout << endl;
			}
		}
	}

	template<int Size>
	void print_register(const string& name, const bitset<Size>& reg) {
		cout << name << ": " << reg << " (" << reg.to_ulong() << ")" << endl;
	}

	template<int BS, int IMS, int RMS>
	void print_registers(const RegisterSet<BS, IMS>& regs, const ComputerState<BS, IMS, RMS>& state) {
		auto& cpu = state.CPU;
		cout << "Flags: " << cpu.get(regs.Flags);
		cout << " (terminated: " << cpu.get(regs.Terminated);
		cout << ", overflow: " << cpu.get(regs.Overflow);
		cout << ", fatal: " << cpu.get(regs.Fatal) << ")" << endl;
		
		print_register("Counter", cpu.get(regs.Counter));
		print_register("IP", cpu.get(regs.IP));
		print_register("AP", cpu.get(regs.AR));

		auto cn = regs.get_CN_count();
		for (int i = 0; i < cn; i++) {
			auto addr = bitset<BaseSize>(i);
			print_register("C" + std::to_string(i), state.CPU.get(regs.get_CN(addr)));
		}
	}

	template<int BS, int IMS, int RMS>
	void print_state(const Computer<BS, IMS, RMS>& cmp) {
		auto& state = cmp.State;
		cout << "Registers:" << endl;
		print_registers(cmp.Registers, state);

		cout << endl << "Internal Memory:" << endl;
		print_memory<BS, IMS>(state.CPU.get_all(), 4);

		cout << endl << "RAM Memory:" << endl;
		print_memory<BS, RMS>(state.RAM.get_all(), 4);
	}
}