#pragma once

#include <bitset>

#include "Reference.h"
#include "Architecture.h"

using std::bitset;

using Core::Reference;
using Core::WReference;
using Core::FReference;
using Core::PSReference;
using Architecture::Word;
using Architecture::WORD_SIZE;

namespace Architecture {
	template<size_t IMS>
	class RegisterSet {
	public:
		static_assert(IMS >= MIN_MEMORY_SIZE);

		RegisterSet() = default;

		// Examples below for BaseSize == 4

		// 00 System 0 Pipeline State (000 - Fetch, 001 - Decode, 010 - Read 1th, 011 - Read 2th, 100 - Execute 1, 101 - Execute 2)
		// 01        1
		// 02        2
		// 03        3 Argument mode (is 2th argument required)
		WReference  System        = { get_address_at(0)    , "SS" };
		PSReference PipelineState = { get_address_at(0) + 0, "PS" };
		FReference  ArgumentMode  = { get_address_at(0) + 3, "AM" };

		// 04 Command Code
		// 05
		// 06
		// 07
		WReference CommandCode = { get_address_at(1), "CC" };

		// 08 Argument #1
		// 09
		// 10
		// 11
		WReference Arg1 = { get_address_at(2), "A1" };

		// 12 Argument #2
		// 13
		// 14
		// 15
		WReference Arg2 = { get_address_at(3), "A2" };

		// 16 Flags 0 Terminated       (execution completed)
		// 17       1 Integer Overflow (last operation raised overflow)
		// 18       2 Fatal Error      (IP or Counter is out of range)
		// 19       3 -
		WReference Flags      = { get_address_at(4)    , "FS" };
		FReference Terminated = { get_address_at(4) + 0, "TR" };
		FReference Overflow   = { get_address_at(4) + 1, "OF" };
		FReference Fatal      = { get_address_at(4) + 2, "FT" };

		// 20 Counter (how many commands was processed)
		// 21
		// 22
		// 23
		WReference Counter = { get_address_at(5), "CR" };

		// 24 IP (next instruction ram pointer)
		// 25
		// 26
		// 27
		WReference IP = { get_address_at(6), "IP" };

		// 28 AR (accumulator register)
		// 29
		// 30
		// 31
		WReference AR = { get_address_at(7), "AR" };

		// 32 CR1 (Common register #1)
		// 33
		// 34
		// 35
		// ...
		// N  CRN (Common register #2)
		// N+1
		// N+2
		// N+3

		auto get_CN(const Word& index) const {
			auto index_val = index.to_ulong();
			return get_CN(index_val);
		}
		
		auto get_CN(size_t index) const {
			return get_register(SERVICE_REGISTERS + index * WORD_SIZE, "CR" + std::to_string(index));
		}

		constexpr size_t get_CN_count() const {
			return IMS - SERVICE_REGISTERS;
		}

		constexpr size_t get_address_at(size_t index) const {
			return index * WORD_SIZE;
		}

		auto get_register(size_t index, const string& name = "") const {
			return WReference(get_address_at(index), name);
		}
	};
}
