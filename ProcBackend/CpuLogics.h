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
		using RSet       = const RegisterSet<BS, IMS>&;
		using CpuMem     = MemoryState<IMS>&;
		using ControlBus = MemoryState<2>&;
		using DataBus    = MemoryState<BS>&;
		using AddressBus = MemoryState<BS>&;
	public:
		CpuLogics(RSet regs, CpuMem cpu, ControlBus control, DataBus data, AddressBus address):
		_regs(regs), _cpu(cpu), _control(control), _data(data), _address(address) { }

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
		bool sub_register(Reference<SZ> ref, const bitset<SZ>& value) {
			Utils::log_line("CpuLogics.sub_register(", ref, ", ", value, ")");
			auto old_value = _cpu[ref];
			auto[new_value, overflow] = BitUtils::minus(old_value, value);
			_cpu.set_bits(ref, new_value);
			set_overflow(overflow);
			return overflow;
		}

		template<size_t SZ>
		bool inc_register(Reference<SZ> ref) {
			Utils::log_line("CpuLogics.inc_register(", ref, ")");
			return add_to_register(ref, BitUtils::get_one<SZ>());
		}
		
		template<size_t SZ>
		bool dec_register(Reference<SZ> ref) {
			Utils::log_line("CpuLogics.dec_register(", ref, ")");
			return sub_register(ref, BitUtils::get_one<SZ>());
		}
		
		void request_ram_read(Reference<BS> address) {
			Utils::log_line("CpuLogics.request_ram_read: ", address);
			_control.set_bits(Reference<2>(0),  bitset<2>(0b01));
			_address.set_bits(Reference<BS>(0), BitUtils::get_set<BS>(address.Address));
			_data   .set_bits(Reference<BS>(0), BitUtils::get_zero<BS>());
			
		}
		
		void request_ram_write(Reference<BS> address, bitset<BS> value) {
			Utils::log_line("CpuLogics.request_ram_write: ", address, " = ", value);
			_control.set_bits(Reference<2>(0),  bitset<2>(0b11));
			_address.set_bits(Reference<BS>(0), BitUtils::get_set<BS>(address.Address));
			_data   .set_bits(Reference<BS>(0), value);
		}
		
		auto read_data_bus() {
			return _data[Reference<BS>(0)];
		}

	private:
		RSet       _regs;
		CpuMem     _cpu;
		ControlBus _control;
		DataBus    _data;
		AddressBus _address;
	};
}
