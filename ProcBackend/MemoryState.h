#pragma once

#include "BitUtils.h"
#include "Reference.h"

template<int MemorySize>
class MemoryState {
public:
	MemoryState(bitset<MemorySize> init_memory) :_memory(init_memory) { }

	bitset<MemorySize> get_all() const {
		return _memory;
	}

	template<int Address, int BitSize>
	bitset<BitSize> get() const {
		return BitUtils::get_bits<Address, BitSize, MemorySize>(_memory);
	}

	template<int BitSize>
	bitset<BitSize> get(Reference<BitSize> reg) const {
		return BitUtils::get_bits<BitSize, MemorySize>(_memory, reg.Address);
	}

	template<int Address, int BitSize>
	void set(bitset<BitSize> value) {
		BitUtils::set_bits<Address, BitSize, MemorySize>(_memory, value);
	}

	template<int BitSize>
	void set(Reference<BitSize> reg, bitset<BitSize> value) {
		BitUtils::set_bits<BitSize, MemorySize>(_memory, reg.Address, value);
	}


private:
	bitset<MemorySize> _memory;
};
