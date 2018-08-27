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

		// System 0 Pipeline State (000 - Fetch, 001 - Decode, 010 - Read 1th, 011 - Read 2th, 100 - Execute 1, 101 - Execute 2)
		//         1
		//         2
		//         3 Argument mode (is 2th argument required)
		WReference  System        = { get_address_at(0)    , "SS" };
		PSReference PipelineState = { get_address_at(0) + 0, "PS" };
		FReference  ArgumentMode  = { get_address_at(0) + 3, "AM" };

		// Command Code
		WReference CommandCode = { get_address_at(1), "CC" };

		// Argument #1
		WReference Arg1 = { get_address_at(2), "A1" };

		// Argument #2
		WReference Arg2 = { get_address_at(3), "A2" };

		// Flags 0 Terminated       (execution completed)
		//       1 Integer Overflow (last operation raised overflow)
		//       2 Fatal Error      (IP or Counter is out of range)
		//       3 Zero Flag        (is last CMP operation succeded)
		WReference Flags      = { get_address_at(4)    , "FS" };
		FReference Terminated = { get_address_at(4) + 0, "TR" };
		FReference Overflow   = { get_address_at(4) + 1, "OF" };
		FReference Fatal      = { get_address_at(4) + 2, "FT" };
		FReference Zero       = { get_address_at(4) + 3, "ZF" };

		// Counter (how many commands was processed)
		WReference Counter = { get_address_at(5), "CR" };

		// IP (next instruction ram pointer)
		WReference IP = { get_address_at(6), "IP" };

		// AR (accumulator register)
		WReference AR = { get_address_at(7), "AR" };

		// CR1 (Common register #1)
		// ...
		// CRN (Common register #N)
		auto get_CN(const Word& index) const {
			auto index_val = index.to_ulong();
			if ( index_val >= get_CN_count() ) {
				throw new std::runtime_error("Invalid C register index");
			}
			return get_CN(index_val);
		}
		
		auto get_CN(size_t index) const {
			return get_register(SERVICE_REGISTERS + index, "CR" + std::to_string(index));
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
