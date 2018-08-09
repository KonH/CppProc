#pragma once

#include <bitset>

#include "Reference.h"

using std::bitset;

using Core::Reference;

namespace Architecture {
	const int ServiceRegisters = 4;

	template<int BS, int IMS>
	class RegisterSet {
	public:
		static_assert(BS >= 4);
		static_assert(IMS >= (BS * ServiceRegisters));

		RegisterSet() = default;

		// Examples below for BaseSize == 4
		// 00 Flags 0 Terminated       (execution completed)
		// 01       1 Integer Overflow (last operation raised overflow)
		// 02       2 Fatal Error      (IP or Counter is out of range)
		// 03       3 -
		Reference<BS> Flags      = { get_address_at(0) };
		Reference<1>  Terminated = { get_address_at(0) };
		Reference<1>  Overflow   = { get_address_at(1) };
		Reference<1>  Fatal      = { get_address_at(2) };

		// 04 Counter (how many ticks was made)
		// 05
		// 06
		// 07
		Reference<BS> Counter = { get_address_at(1) };

		// 08 IP (next instruction ram pointer)
		// 09
		// 10
		// 11
		Reference<BS> IP = { get_address_at(2) };

		// 12 AR (accumulator register)
		// 13
		// 14
		// 15
		Reference<BS> AP = { get_address_at(3) };

		// 16 CR1 (Common register #1)
		// 17
		// 18
		// 19
		// ...
		// N  CRN (Common register #2)
		// N+1
		// N+2
		// N+3

		auto get_CN(bitset<BS> index) const {
			auto index_val = index.to_ulong();
			return get_register<BS>(ServiceRegisters + index_val);
		}

		constexpr int get_CN_count() const {
			return (IMS - ServiceRegisters * BS) / BS;
		}

		constexpr int get_address_at(int index) const {
			return index * BS;
		}

		template<int Size>
		auto get_register(int index) const {
			return Reference<Size>(get_address_at(index));
		}
	};
}