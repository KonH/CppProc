#pragma once

#include "Reference.h"

const int ServiceRegisters = 4;

template<int BaseSize, int InternalMemorySize>
class RegisterSet {
public:
	static_assert(BaseSize >= 4);
	static_assert(InternalMemorySize >= (BaseSize * ServiceRegisters));
	
	RegisterSet() = default;

	// Examples below for BaseSize == 4
	// 00 Flags 0 Terminated (execution completed)
	// 01       1 -
	// 02       2 -
	// 03       3 -
	Reference<BaseSize> Flags      = { get_address_at(0) };
	Reference<1>        Terminated = { get_address_at(0) };
	
	// 04 Counter (how many ticks was made)
	// 05
	// 06
	// 07
	Reference<BaseSize> Counter = { get_address_at(1) };

	// 08 IP (next instruction ram pointer)
	// 09
	// 10
	// 11
	Reference<BaseSize> IP = { get_address_at(2) };

	// 12 AR (accumulator register)
	// 13
	// 14
	// 15
	Reference<BaseSize> AP = { get_address_at(3) };

	// 16 CR1 (Common register #1)
	// 17
	// 18
	// 19
	// ...
	// N  CRN (Common register #2)
	// N+1
	// N+2
	// N+3

	auto get_CN(bitset<BaseSize> index) const {
		auto index_val = index.to_ulong();
		return get_register<BaseSize>(ServiceRegisters + index_val);
	}

	constexpr int get_CN_count() const {
		return (InternalMemorySize - ServiceRegisters * BaseSize) / BaseSize;
	}

	constexpr int get_address_at(int index) const {
		return index * BaseSize;
	}

	template<int Size>
	auto get_register(int index) const {
		return Reference<Size>(get_address_at(index));
	}
};