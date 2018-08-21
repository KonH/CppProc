#pragma once

#include <tuple>
#include <bitset>

using std::tuple;
using std::bitset;

namespace BitUtils {
	template<size_t BS, size_t SS>
	auto get_bits(const bitset<SS>& set, size_t address) {
		static_assert(BS <= SS);
		bitset<BS> value = { 0 };
		for (size_t i = 0; i < BS; i++) {
			value[i] = set[address + i];
		}
		return value;
	}

	template<size_t BS, size_t SS>
	void set_bits(bitset<SS>& set, size_t address, const bitset<BS>& value) {
		static_assert(BS <= SS);
		for (size_t i = 0; i < BS; i++) {
			set[address + i] = value[i];
		}
	}

	template<size_t BS>
	auto get_zero() {
		static_assert(BS > 0);
		return bitset<BS>(0b0);
	}

	template<size_t BS>
	auto get_one() {
		static_assert(BS > 0);
		return bitset<BS>(0b1);
	}

	template<size_t BS>
	auto get_set(size_t value) {
		static_assert(BS > 0);
		return bitset<BS>(value);
	}

	auto plus(bool a, bool b, bool carry) {
		auto result    = carry ? a == b : a != b;
		auto new_carry = (a && b) || (carry && a) || (carry && b);
		return tuple { result, new_carry };
	}

	template<size_t BS>
	auto plus(const bitset<BS>& a, const bitset<BS>& b) {
		static_assert(BS > 0);
		bitset<BS> result = { 0 };
		auto prev_carry = false;
		for (size_t i = 0; i < BS; i++) {
			auto [res, carry] = plus(a.test(i), b.test(i), prev_carry);
			result[i] = res;
			prev_carry = carry;
		}
		return tuple { result, prev_carry };
	}
}
