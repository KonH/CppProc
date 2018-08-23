#pragma once

#include <bitset>

#include "MemoryState.h"

using std::bitset;

namespace State {
	template<size_t IMS, size_t RMS>
	class ComputerState {
	public:
		MemoryState<IMS>       CPU        = { "CPU",     0 };
		State::ControlBusState ControlBus = { "Control", 0 };
		State::AddressBusState AddressBus = { "Address", 0 };
		State::DataBusState    DataBus    = { "Data",    0 };
		MemoryState<RMS>       RAM;

		ComputerState(bitset<RMS> ram_memory): RAM("RAM", ram_memory) {}
	};
}
