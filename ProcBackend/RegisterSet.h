#pragma once

#include <bitset>

#include "Reference.h"

using std::bitset;

using Core::Reference;

namespace Architecture {
	template<size_t BS, size_t IMS>
	class RegisterSet {
	public:
		static constexpr int ServiceRegisters = 8;
		
		static_assert(BS >= 4);
		static_assert(IMS >= (BS * ServiceRegisters));

		RegisterSet() = default;

		// Examples below for BaseSize == 4

		// 00 System 0 Pipeline State (000 - Fetch, 001 - Decode, 010 - Read 1th, 011 - Read 2th, 100 - Execute)
		// 01        1
		// 02        2
		// 03        3 Argument mode (is 2th argument required)
		Reference<BS> System        = { get_address_at(0)     };
		Reference<3>  PipelineState = { get_address_at(0) + 0 };
		Reference<1>  ArgumentMode  = { get_address_at(0) + 3 };

		// 04 Command Code
		// 05
		// 06
		// 07
		Reference<BS> CommandCode = { get_address_at(1) };

		// 08 Argument #1
		// 09
		// 10
		// 11
		Reference<BS> Arg1 = { get_address_at(2) };

		// 12 Argument #2
		// 13
		// 14
		// 15
		Reference<BS> Arg2 = { get_address_at(3) };

		// 16 Flags 0 Terminated       (execution completed)
		// 17       1 Integer Overflow (last operation raised overflow)
		// 18       2 Fatal Error      (IP or Counter is out of range)
		// 19       3 -
		Reference<BS> Flags      = { get_address_at(4)     };
		Reference<1>  Terminated = { get_address_at(4) + 0 };
		Reference<1>  Overflow   = { get_address_at(4) + 1 };
		Reference<1>  Fatal      = { get_address_at(4) + 2 };

		// 20 Counter (how many commands was processed)
		// 21
		// 22
		// 23
		Reference<BS> Counter = { get_address_at(5) };

		// 24 IP (next instruction ram pointer)
		// 25
		// 26
		// 27
		Reference<BS> IP = { get_address_at(6) };

		// 28 AR (accumulator register)
		// 29
		// 30
		// 31
		Reference<BS> AR = { get_address_at(7) };

		// 32 CR1 (Common register #1)
		// 33
		// 34
		// 35
		// ...
		// N  CRN (Common register #2)
		// N+1
		// N+2
		// N+3

		auto get_CN(bitset<BS> index) const {
			auto index_val = index.to_ulong();
			return get_CN(index_val);
		}
		
		auto get_CN(size_t index) const {
			return get_register<BS>(ServiceRegisters + index);
		}

		constexpr size_t get_CN_count() const {
			return (IMS - ServiceRegisters * BS) / BS;
		}

		constexpr size_t get_address_at(size_t index) const {
			return index * BS;
		}

		template<size_t SZ>
		auto get_register(size_t index) const {
			return Reference<SZ>(get_address_at(index));
		}
	};
}
