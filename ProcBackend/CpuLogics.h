#pragma once

#include "Logger.h"
#include "BitUtils.h"
#include "MemoryState.h"
#include "RegisterSet.h"

using State::MemoryState;
using Architecture::RegisterSet;

namespace Logics {
	template<int BS, int IMS>
	class CpuLogics {
	public:
		CpuLogics(const RegisterSet<BS, IMS>& regs, MemoryState<IMS>& cpu): _regs(regs), _cpu(cpu) { }

		void set_overflow(bool value) {
			Utils::log_line("CpuLogics.set_overflow(", value, ")");
			_cpu.set<1>(_regs.Overflow, BitUtils::get_set<1>(value));
		}

		template<int Size>
		bool add_to_register(Reference<Size> ref, bitset<Size> value) {
			Utils::log_line("CpuLogics.add_to_register(", ref.Address, ":", ref.Size, ", ", value, ")");
			auto old_value = _cpu.get(ref);
			auto[new_value, overflow] = BitUtils::plus(old_value, value);
			_cpu.set(ref, new_value);
			set_overflow(overflow);
			return overflow;
		}

		template<int Size>
		bool inc_register(Reference<Size> ref) {
			Utils::log_line("CpuLogics.inc_register(", ref.Address, ":", ref.Size, ")");
			return add_to_register(ref, BitUtils::get_one<Size>());
		}

	private:
		const RegisterSet<BS, IMS>& _regs;
		MemoryState<IMS>&           _cpu;
	};
}