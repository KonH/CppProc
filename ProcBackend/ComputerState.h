#pragma once

#include "MemoryState.h"
#include "RegisterSet.h"
#include "CpuRunner.h"

template<int BaseSize, int InternalMemorySize, int RamMemorySize>
class ComputerState {
public:
	CpuRunner  <BaseSize, InternalMemorySize, RamMemorySize> Runner;
	RegisterSet<BaseSize, InternalMemorySize>                Registers;
	MemoryState<InternalMemorySize>                          CPU = { 0 };
	MemoryState<RamMemorySize>                               RAM;

	ComputerState(bitset<RamMemorySize> ram_memory):RAM(ram_memory) { }

	bool Tick() {
		return Runner.Tick(Registers, CPU, RAM);
	}
};