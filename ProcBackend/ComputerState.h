#pragma once

#include <bitset>

#include "MemoryState.h"

using std::bitset;

namespace State {
	template<int BS, int IMS, int RMS>
	class ComputerState {
	public:
		MemoryState<IMS> CPU = { 0 };
		MemoryState<RMS> RAM;

		ComputerState(bitset<RMS> ram_memory) :RAM(ram_memory) {}
	};
}