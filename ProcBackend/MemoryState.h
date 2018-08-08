#pragma once

#include "BitUtils.h"

template<int MemorySize>
class MemoryState {
public:
	MemoryState(bitset<MemorySize> init_memory) :_memory(init_memory) { }

	bitset<MemorySize> get_all() const {
		return _memory;
	}

	template<int Address, int BitSize>
	bitset<BitSize> get() {
		return BitUtils::get_bits<Address, BitSize, MemorySize>(_memory);
	}

	template<int Address, int BitSize>
	void set(bitset<BitSize> value) {
		BitUtils::set_bits<Address, BitSize, MemorySize>(_memory, value);
	}

private:
	bitset<MemorySize> _memory;
};
