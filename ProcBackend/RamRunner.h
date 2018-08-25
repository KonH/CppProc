#pragma once

#include <bitset>

#include "Logger.h"
#include "Reference.h"
#include "MemoryState.h"
#include "Architecture.h"

using std::bitset;

using Core::FReference;
using Core::WReference;
using State::MemoryState;
using Architecture::Word;

namespace Logics {
	template<size_t RMS>
	class RamRunner {
		using ControlBus = const State::ControlBusState&;
		using AddrBus    = const State::AddressBusState&;
		using DataBus    = State::DataBusState&;
		using Ram        = MemoryState<RMS>&;
	public:
		RamRunner(ControlBus control_bus, AddrBus address_bus, DataBus data_bus, Ram ram):
			_control_bus(control_bus), _address_bus(address_bus), _data_bus(data_bus), _ram(ram) { }

		bool tick() {
			// 0000 - [ 1 - enabled, 0 - disabled ]
			// 0001 - [ 0 - read,    1 - write    ]
			
			Utils::log_line(
				"RamRunner.tick(control: ",
				_control_bus.get_all(),
				", address: ", _address_bus.get_all(),
				", data: ", _data_bus.get_all(), ")"
			);
			auto enabled = is_enabled();
			Utils::log_line("RamRunner.tick: enabled(", enabled, ")");
			if (enabled) {
				auto addr = _address_bus.get_all();
				if (is_write()) {
					auto data = _data_bus.get_all();
					process_write(addr, data);
				} else {
					process_read(addr);
				}
			}
			return true;
		}

	private:
		ControlBus  _control_bus;
		AddrBus     _address_bus;
		DataBus     _data_bus;
		Ram         _ram;

		bool is_enabled() {
			return _control_bus[FReference(0)].test(0);
		}
		
		bool is_write() {
			return _control_bus[FReference(1)].test(0);
		}
		
		void process_read(const Word& address) {
			Utils::log_line("RamRunner.process_read(", address, ")");
			auto value = _ram[WReference(address.to_ulong() * Architecture::WORD_SIZE)];
			_data_bus.set_bits(WReference(0), value);
		}

		void process_write(const Word& address, const Word& data) {
			Utils::log_line("RamRunner.process_write(", address, ", ", data, ")");
			_ram.set_bits(WReference(address.to_ulong() * Architecture::WORD_SIZE), data);
		}
	};
}
