#pragma once

#include <bitset>
#include <tuple>

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

	template<int BitSize>
	bitset<BitSize> get_zero() {
		return bitset<BitSize>(0b0);
	}

	template<int BitSize>
	bitset<BitSize> get_one() {
		return bitset<BitSize>(0b1);
	}

	template<int BitSize>
	bitset<BitSize> get_set(unsigned long value) {
		return bitset<BitSize>(value);
	}

	auto plus(bool a, bool b, bool carry) {
		auto result    = carry ? a == b : a != b;
		auto new_carry = (a && b) || (carry && a) || (carry && b);
		return std::make_tuple(result, new_carry);
	}

	template<int BitSize>
	auto plus(const bitset<BitSize>& a, const bitset<BitSize>& b) {
		bitset<BitSize> result = { 0 };
		auto prev_carry = false;
		for (int i = 0; i < BitSize; i++) {
			auto [res, carry] = plus(a.test(i), b.test(i), prev_carry);
			result[i] = res;
			prev_carry = carry;
		}
		return std::make_tuple(result, prev_carry);
	}
}