#pragma once

#include <tuple>
#include <bitset>

#include "Architecture.h"

using std::tuple;
using std::bitset;

using Architecture::Word;
using Architecture::WORD_SIZE;

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
	
	template<size_t SS>
	auto get_bits(const bitset<SS>& set, size_t address) {
		return get_bits<WORD_SIZE, SS>(set, address);
	}

	template<size_t BS, size_t SS>
	void set_bits(bitset<SS>& set, size_t address, const bitset<BS>& value) {
		static_assert(BS <= SS);
		for (size_t i = 0; i < BS; i++) {
			set[address + i] = value[i];
		}
	}
	
	template<size_t SS>
	void set_bits(bitset<SS>& set, size_t address, const Word& value) {
		set_bits<WORD_SIZE, SS>(set, address, value);
	}

	template<size_t BS = WORD_SIZE>
	auto get_zero() {
		static_assert(BS > 0);
		return bitset<BS>(0b0);
	}

	template<size_t BS = WORD_SIZE>
	auto get_one() {
		static_assert(BS > 0);
		return bitset<BS>(0b1);
	}

	template<size_t BS = WORD_SIZE>
	auto get_set(size_t value) {
		static_assert(BS > 0);
		return bitset<BS>(value);
	}
	
	auto get_flag(bool value) {
		return get_set<1>(value);
	}

	auto plus(bool a, bool b, bool carry) {
		auto result    = carry ? a == b : a != b;
		auto new_carry = (a && b) || (carry && a) || (carry && b);
		return tuple { result, new_carry };
	}

	template<size_t BS = WORD_SIZE>
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
	
	template<size_t BS = WORD_SIZE>
	auto inverse(const bitset<BS>& value) {
		static_assert(BS > 0);
		bitset<BS> result = { 0 };
		for ( size_t i = 0; i < BS; i++ ) {
			result[i] = !value[i];
		}
		return result;
	}
	
	auto minus(bool a, bool b, bool carry) {
		auto result    = carry ? (a == b) : (a != b);
		auto new_carry = b ? ((a == carry) || carry)  : (!a && carry);
		return tuple { result, new_carry };
	}
	
	template<size_t BS = WORD_SIZE>
	auto minus(const bitset<BS>& a, const bitset<BS>& b) {
		static_assert(BS > 0);
		bitset<BS> result = { 0 };
		auto prev_carry = false;
		for (size_t i = 0; i < BS; i++) {
			auto [res, carry] = minus(a.test(i), b.test(i), prev_carry);
			result[i] = res;
			prev_carry = carry;
		}
		return tuple { result, prev_carry };
	}
}
