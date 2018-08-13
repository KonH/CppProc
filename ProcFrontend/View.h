#pragma once

#include <bitset>
#include <iomanip>

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
			auto bits = BitUtils::get_bits<BaseSize>(mem, i);
			cout << bits << " (" << std::setfill('0') << std::setw(2) << bits.to_ulong() << ")" << " ";
			sizes++;
			if (sizes >= sizes_per_line) {
				sizes = 0;
				cout << endl;
			}
		}
	}

	template<int Size>
	void print_register(const string& name, const bitset<Size>& reg) {
		cout << name << ": " << reg << " (" << std::setfill('0') << std::setw(2) << reg.to_ulong() << ")" << endl;
	}

	template<int BS, int IMS, int RMS>
	void print_registers(const RegisterSet<BS, IMS>& regs, const ComputerState<BS, IMS, RMS>& state) {
		auto& cpu = state.CPU;
		
		cout << "Ss: " << cpu.get(regs.System);
		cout << " (pipeline state: " << cpu.get(regs.PipelineState);
		cout << ", argument mode: " << cpu.get(regs.ArgumentMode) << ")" << endl;

		print_register("CC", cpu.get(regs.CommandCode));
		print_register("A1", cpu.get(regs.Arg1));
		print_register("A2", cpu.get(regs.Arg2));

		cout << "Fs: " << cpu.get(regs.Flags);
		cout << " (terminated: " << cpu.get(regs.Terminated);
		cout << ", overflow: " << cpu.get(regs.Overflow);
		cout << ", fatal: " << cpu.get(regs.Fatal) << ")" << endl;
		
		print_register("Cr", cpu.get(regs.Counter));
		print_register("IP", cpu.get(regs.IP));
		print_register("AP", cpu.get(regs.AR));

		auto cn = regs.get_CN_count();
		for (int i = 0; i < cn; i++) {
			auto addr = bitset<BaseSize>(i);
			print_register("C" + std::to_string(i), state.CPU.get(regs.get_CN(addr)));
		}
	}

	template<int BS, int IMS, int RMS>
	void print_buses(const ComputerState<BS, IMS, RMS>& state) {
		auto& control = state.ControlBus;
		auto& address = state.AddressBus;
		auto& data    = state.DataBus;

		print_register("CL", control.get_all());
		print_register("AD", address.get_all());
		print_register("DT", data.get_all());
	}

	template<int BS, int IMS, int RMS>
	void print_state(const Computer<BS, IMS, RMS>& cmp) {
		auto& state = cmp.State;
		cout << "Registers:" << endl;
		print_registers(cmp.Registers, state);
		cout << endl;

		cout << "Buses:" << endl;
		print_buses(state);

		cout << endl << "Internal Memory:" << endl;
		print_memory<BS, IMS>(state.CPU.get_all(), 4);
		cout << endl;

		cout << endl << "RAM Memory:" << endl;
		print_memory<BS, RMS>(state.RAM.get_all(), 4);
	}
}