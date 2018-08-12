#pragma once

#include <bitset>

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

		bool tick() {
			auto& control = State.ControlBus;
			auto& address = State.AddressBus;
			auto& data    = State.DataBus;
			auto& cpu     = State.CPU;
			auto& ram     = State.RAM;
			auto ram_runner = RamRunner<BaseSize, RamMemorySize>(control, address, data, ram);
			auto cpu_runner = CpuRunner<BaseSize, InternalMemorySize, RamMemorySize>(Registers, cpu, ram);
			return ram_runner.tick() && cpu_runner.tick();
		}
	};
}