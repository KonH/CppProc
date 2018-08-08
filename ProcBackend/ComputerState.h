#pragma once

#include "MemoryState.h"

template<int InternalMemorySize, int RamMemorySize>
class ComputerState {
public:
	MemoryState<InternalMemorySize> CPU = { 0 };
	MemoryState<RamMemorySize>      RAM;

	ComputerState(bitset<RamMemorySize> ram_memory):RAM(ram_memory) { }
};