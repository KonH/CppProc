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
	template<int BaseSize, int InternalMemorySize, int RamMemorySize>
	class Computer {
	public:
		RegisterSet  <BaseSize, InternalMemorySize>                Registers;
		ComputerState<BaseSize, InternalMemorySize, RamMemorySize> State;

		Computer(bitset<RamMemorySize> init_ram):State(init_ram) { }

		bool tick(int ticks = 1) {
			for (int i = 0; i < ticks; i++) {
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