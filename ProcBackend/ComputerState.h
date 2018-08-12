#pragma once

#include <bitset>

#include "MemoryState.h"

using std::bitset;

namespace State {
	template<int BS, int IMS, int RMS>
	class ComputerState {
	public:
		MemoryState<IMS> CPU        = { "CPU",     0 };
		MemoryState<2>   ControlBus = { "Control", 0 };
		MemoryState<BS>  AddressBus = { "Address", 0 };
		MemoryState<BS>  DataBus    = { "Data",    0 };
		MemoryState<RMS> RAM;

		ComputerState(bitset<RMS> ram_memory) :RAM("RAM", ram_memory) {}
	};
}