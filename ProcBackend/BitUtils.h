#pragma once

#include <bitset>

using std::bitset;

namespace BitUtils {
	template<int Address, int BitSize, int SetSize>
	bitset<BitSize> get_bits(const bitset<SetSize>& set) {
		static_assert(Address + BitSize <= SetSize);
		return get_bits(set, Address);
	}

	template<int BitSize, int SetSize>
	bitset<BitSize> get_bits(const bitset<SetSize>& set, int address) {
		bitset<BitSize> value = { 0 };
		for (int i = 0; i < BitSize; i++) {
			value[i] = set[address + i];
		}
		return value;
	}

	template<int Address, int BitSize, int SetSize>
	void set_bits(bitset<SetSize>& set, const bitset<BitSize>& value) {
		static_assert(Address + BitSize <= SetSize);
		set_bits(set, Address, value);
	}

	template<int BitSize, int SetSize>
	void set_bits(bitset<SetSize>& set, int address, const bitset<BitSize>& value) {
		for (int i = 0; i < BitSize; i++) {
			set[address + i] = value[i];
		}
	}
}