#pragma once

#include "Logger.h"
#include "BitUtils.h"
#include "MemoryState.h"
#include "RegisterSet.h"

using State::MemoryState;
using Architecture::RegisterSet;

namespace Logics {
	template<size_t BS, size_t IMS>
	class CpuLogics {
		using RSet = const RegisterSet<BS, IMS>&;
		using CpuMem = MemoryState<IMS>&;
	public:
		CpuLogics(RSet regs, CpuMem cpu): _regs(regs), _cpu(cpu) { }

		void set_overflow(bool value) {
			Utils::log_line("CpuLogics.set_overflow(", value, ")");
			_cpu.set_bits(_regs.Overflow, BitUtils::get_set<1>(value));
		}

		template<size_t SZ>
		bool add_to_register(Reference<SZ> ref, const bitset<SZ>& value) {
			Utils::log_line("CpuLogics.add_to_register(", ref, ", ", value, ")");
			auto old_value = _cpu[ref];
			auto[new_value, overflow] = BitUtils::plus(old_value, value);
			_cpu.set_bits(ref, new_value);
			set_overflow(overflow);
			return overflow;
		}

		template<size_t SZ>
		bool inc_register(Reference<SZ> ref) {
			Utils::log_line("CpuLogics.inc_register(", ref, ")");
			return add_to_register(ref, BitUtils::get_one<SZ>());
		}

	private:
		RSet   _regs;
		CpuMem _cpu;
	};
}
