#pragma once

#include <bitset>

#include "CpuRunner.h"
#include "ComputerState.h"

using std::bitset;

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
			auto& cpu = State.CPU;
			auto& ram = State.RAM;
			auto runner = CpuRunner<BaseSize, InternalMemorySize, RamMemorySize>(Registers, cpu, ram);
			return runner.tick();
		}
	};
}