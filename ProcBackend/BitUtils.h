#pragma once

#include <bitset>

using namespace std;

class BitUtils {
public:
	template<int Address, int BitSize, int SetSize>
	static bitset<BitSize> get_bits(const bitset<SetSize>& set) {
		static_assert(Address + BitSize <= SetSize);
		bitset<BitSize> value = { 0 };
		for (int i = Address; i < Address + BitSize; i++) {
			value[i] = set[i];
		}
		return value;
	}

	template<int Address, int BitSize, int SetSize>
	static void set_bits(bitset<SetSize>& set, const bitset<BitSize>& value) {
		static_assert(Address + BitSize <= SetSize);
		for (int i = Address; i < Address + BitSize; i++) {
			set[i] = value[i];
		}
	}
};