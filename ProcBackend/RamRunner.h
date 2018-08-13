#pragma once

#include <bitset>

#include "Logger.h"
#include "MemoryState.h"

using std::bitset;

using State::MemoryState;

namespace Logics {
	template<int BS, int RMS>
	class RamRunner {
	public:
		RamRunner(const MemoryState<2>& control_bus, const MemoryState<BS>& address_bus, MemoryState<BS>& data_bus, MemoryState<RMS>& ram):
			_control_bus(control_bus), _address_bus(address_bus), _data_bus(data_bus), _ram(ram) { }

		bool tick() {
			// 00 - [ 1 - enabled, 0 - disabled ]
			// 01 - [ 0 - read,    1 - write    ]
			Utils::log_line(
				"RamRunner.tick(control: ",
				_control_bus.get_all(),
				", address: ", _address_bus.get_all(),
				", data: ", _data_bus.get_all(), ")"
			);
			auto enabled = _control_bus.get(Reference<1>(0)).test(0);
			Utils::log_line("RamRunner.tick: enabled(", enabled, ")");
			if (enabled) {
				auto is_write = _control_bus.get(Reference<1>(1)).test(0);
				auto addr = _address_bus.get_all();
				if (is_write) {
					auto data = _data_bus.get_all();
					process_write(addr, data);
				} else {
					process_read(addr);
				}
			}
			return true;
		}

	private:
		const MemoryState<2>&  _control_bus;
		const MemoryState<BS>& _address_bus;
		MemoryState<BS>&       _data_bus;
		MemoryState<RMS>&      _ram;

		void process_read(bitset<BS> address) {
			Utils::log_line("RamRunner.process_read(", address, ")");
			auto value = _ram.get(Reference<BS>(address.to_ulong()));
			_data_bus.set(Reference<BS>(0), value);
		}

		void process_write(bitset<BS> address, bitset<BS> data) {
			Utils::log_line("RamRunner.process_write(", address, ", ", data, ")");
			_ram.set(Reference<BS>(address.to_ulong()), data);
		}
	};
}