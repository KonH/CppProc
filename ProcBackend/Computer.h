#pragma once

#include <bitset>

#include "Logger.h"
#include "CpuRunner.h"
#include "RamRunner.h"
#include "ComputerState.h"

using std::bitset;

using Logics::RamRunner;
using Logics::CpuRunner;
using State::ComputerState;
using Architecture::RegisterSet;

namespace Core {
	template<size_t BaseSize, size_t InternalMemorySize, size_t RamMemorySize>
	class Computer {
		using Regs      = RegisterSet  <BaseSize, InternalMemorySize>;
		using CompState = ComputerState<BaseSize, InternalMemorySize, RamMemorySize>;
	public:
		Regs Registers;
		CompState State;

		Computer(bitset<RamMemorySize> init_ram):State(init_ram) { }

		bool tick(size_t ticks = 1) {
			for (size_t i = 0; i < ticks; i++) {
				Utils::log_line("Computer.tick(", i, ")");
				auto ram = tick_ram();
				auto cpu = tick_cpu();
				Utils::log_line();
				auto is_working = ram && cpu;
				if (!is_working) {
					return false;
				}
			}
			return true;
		}

		bool tick_ram() {
			auto& control = State.ControlBus;
			auto& address = State.AddressBus;
			auto& data    = State.DataBus;
			auto& ram     = State.RAM;
			return RamRunner<BaseSize, RamMemorySize>(control, address, data, ram).tick();
		}

		bool tick_cpu() {
			auto& control = State.ControlBus;
			auto& address = State.AddressBus;
			auto& data    = State.DataBus;
			auto& cpu     = State.CPU;
			return CpuRunner<BaseSize, InternalMemorySize, RamMemorySize>(Registers, cpu, control, address, data).tick();
		}
	};
}
