#pragma once

#include "Logger.h"
#include "BitUtils.h"
#include "MemoryState.h"
#include "RegisterSet.h"
#include "Architecture.h"

using Core::Reference;
using Core::WReference;
using Core::CBReference;
using State::MemoryState;
using Architecture::Word;
using Architecture::WORD_SIZE;
using Architecture::RegisterSet;

namespace Logics {
	template<size_t IMS>
	class CpuLogics {
		using RSet       = const RegisterSet<IMS>&;
		using CpuMem     = MemoryState<IMS>&;
		using ControlBus = State::ControlBusState&;
		using DataBus    = State::DataBusState&;
		using AddressBus = State::AddressBusState&;
	public:
		CpuLogics(RSet regs, CpuMem cpu, ControlBus control, DataBus data, AddressBus address):
		_regs(regs), _cpu(cpu), _control(control), _data(data), _address(address) { }

		void set_overflow(bool value) {
			Utils::log_line("CpuLogics.set_overflow(", value, ")");
			_cpu.set_bits(_regs.Overflow, BitUtils::get_flag(value));
		}

		template<size_t SZ = WORD_SIZE>
		bool add_to_register(Reference<SZ> ref, const bitset<SZ>& value) {
			Utils::log_line("CpuLogics.add_to_register(", ref, ", ", value, ")");
			auto old_value = _cpu[ref];
			auto[new_value, overflow] = BitUtils::plus(old_value, value);
			_cpu.set_bits(ref, new_value);
			set_overflow(overflow);
			return overflow;
		}
		
		template<size_t SZ = WORD_SIZE>
		bool sub_register(Reference<SZ> ref, const bitset<SZ>& value) {
			Utils::log_line("CpuLogics.sub_register(", ref, ", ", value, ")");
			auto old_value = _cpu[ref];
			auto[new_value, overflow] = BitUtils::minus(old_value, value);
			_cpu.set_bits(ref, new_value);
			set_overflow(overflow);
			return overflow;
		}

		template<size_t SZ = WORD_SIZE>
		bool inc_register(Reference<SZ> ref) {
			Utils::log_line("CpuLogics.inc_register(", ref, ")");
			return add_to_register(ref, BitUtils::get_one<SZ>());
		}
		
		template<size_t SZ = WORD_SIZE>
		bool dec_register(Reference<SZ> ref) {
			Utils::log_line("CpuLogics.dec_register(", ref, ")");
			return sub_register(ref, BitUtils::get_one<SZ>());
		}
		
		void request_ram_read(WReference address) {
			Utils::log_line("CpuLogics.request_ram_read: ", address);
			_control.set_bits(CBReference(0),  bitset<2>(0b01));
			_address.set_bits(WReference (0), BitUtils::get_set(address.Address));
			_data   .set_bits(WReference (0), BitUtils::get_zero());
			
		}
		
		void request_ram_write(WReference address, const Word& value) {
			Utils::log_line("CpuLogics.request_ram_write: ", address, " = ", value);
			_control.set_bits(CBReference(0),  bitset<2>(0b11));
			_address.set_bits(WReference (0), BitUtils::get_set(address.Address));
			_data   .set_bits(WReference (0), value);
		}
		
		auto read_data_bus() {
			return _data[WReference(0)];
		}

	private:
		RSet       _regs;
		CpuMem     _cpu;
		ControlBus _control;
		DataBus    _data;
		AddressBus _address;
	};
}
