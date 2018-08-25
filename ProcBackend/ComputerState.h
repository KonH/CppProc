#pragma once

#include <bitset>

#include "MemoryState.h"
#include "Architecture.h"

using Architecture::WordSet;

namespace State {
	template<size_t IMS, size_t RMS>
	class ComputerState {
	public:
		MemoryState<IMS>       CPU        = { "CPU"     };
		State::ControlBusState ControlBus = { "Control" };
		State::AddressBusState AddressBus = { "Address" };
		State::DataBusState    DataBus    = { "Data"    };
		MemoryState<RMS>       RAM;

		ComputerState(WordSet<RMS> ram_memory): RAM("RAM", ram_memory) {}
	};
}
