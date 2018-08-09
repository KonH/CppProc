#pragma once

#include <bitset>

#include "BitUtils.h"
#include "Reference.h"

using std::bitset;

using Core::Reference;

namespace State {
	template<int MS>
	class MemoryState {
	public:
		MemoryState(bitset<MS> init_memory) :_memory(init_memory) {}

		bitset<MS> get_all() const {
			return _memory;
		}

		template<int Address, int BitSize>
		bitset<BitSize> get() const {
			return BitUtils::get_bits<Address, BitSize, MS>(_memory);
		}

		template<int BitSize>
		bitset<BitSize> get(Reference<BitSize> reg) const {
			return BitUtils::get_bits<BitSize, MS>(_memory, reg.Address);
		}

		template<int Address, int BitSize>
		void set(bitset<BitSize> value) {
			BitUtils::set_bits<Address, BitSize, MS>(_memory, value);
		}

		template<int BitSize>
		void set(Reference<BitSize> reg, bitset<BitSize> value) {
			BitUtils::set_bits<BitSize, MS>(_memory, reg.Address, value);
		}


	private:
		bitset<MS> _memory;
	};
}