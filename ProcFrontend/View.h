#pragma once

#include <bitset>
#include <iomanip>
#include <string_view>

#include "Computer.h"
#include "Architecture.h"
#include "ComputerState.h"

using std::bitset;

using Core::Computer;
using Architecture::Word;
using State::ComputerState;

namespace View {
	template<size_t TS>
	void print_memory(const bitset<TS>& mem, int sizes_per_line) {
		int sizes = 0;
		for (size_t i = 0; i < TS; i += Architecture::WORD_SIZE) {
			auto bits = BitUtils::get_bits(mem, i);
			cout << bits << " (" << std::setfill('0') << std::setw(2) << bits.to_ulong() << ")" << " ";
			sizes++;
			if (sizes >= sizes_per_line) {
				sizes = 0;
				cout << endl;
			}
		}
	}

	template<size_t SZ>
	void print_register(const string_view& name, const bitset<SZ>& reg) {
		cout << name << ": " << reg << " (" << std::setfill('0') << std::setw(2) << reg.to_ulong() << ")" << endl;
	}

	template<size_t IMS, size_t RMS>
	void print_registers(const RegisterSet<IMS>& regs, const ComputerState<IMS, RMS>& state) {
		auto& cpu = state.CPU;
		
		cout << "Ss: " << cpu[regs.System];
		cout << " (pipeline state: " << cpu[regs.PipelineState];
		cout << ", argument mode: " << cpu[regs.ArgumentMode] << ")" << endl;

		print_register("CC", cpu[regs.CommandCode]);
		print_register("A1", cpu[regs.Arg1]);
		print_register("A2", cpu[regs.Arg2]);

		cout << "Fs: " << cpu[regs.Flags];
		cout << " (terminated: " << cpu[regs.Terminated];
		cout << ", overflow: " << cpu[regs.Overflow];
		cout << ", fatal: " << cpu[regs.Fatal] << ")" << endl;
		
		print_register("Cr", cpu[regs.Counter]);
		print_register("IP", cpu[regs.IP]);
		print_register("AP", cpu[regs.AR]);

		auto cn = regs.get_CN_count();
		for (int i = 0; i < cn; i++) {
			auto addr = Word(i);
			print_register("C" + std::to_string(i), state.CPU[regs.get_CN(addr)]);
		}
	}

	template<size_t IMS, size_t RMS>
	void print_buses(const ComputerState<IMS, RMS>& state) {
		auto& control = state.ControlBus;
		auto& address = state.AddressBus;
		auto& data    = state.DataBus;

		print_register("CL", control.get_all());
		print_register("AD", address.get_all());
		print_register("DT", data.get_all());
	}

	template<size_t IMS, size_t RMS>
	void print_state(const Computer<IMS, RMS>& cmp) {
		auto& state = cmp.State;
		cout << "Registers:" << endl;
		print_registers(cmp.Registers, state);
		cout << endl;

		cout << "Buses:" << endl;
		print_buses(state);

		cout << endl << "Internal Memory:" << endl;
		print_memory(state.CPU.get_all(), 4);
		cout << endl;

		cout << endl << "RAM Memory:" << endl;
		print_memory(state.RAM.get_all(), 4);
	}
}
