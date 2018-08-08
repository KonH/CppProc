#pragma once

#include "MemoryState.h"
#include "RegisterSet.h"

template<int BaseSize, int InternalMemorySize, int RamMemorySize>
class ComputerState {
public:
	RegisterSet<InternalMemorySize, BaseSize> Registers;
	MemoryState<InternalMemorySize>           CPU = { 0 };
	MemoryState<RamMemorySize>                RAM;

	ComputerState(bitset<RamMemorySize> ram_memory):RAM(ram_memory) { }
};