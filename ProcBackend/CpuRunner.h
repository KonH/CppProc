#pragma once

#include <map>
#include <bitset>
#include <functional>

#include "RegisterSet.h"
#include "MemoryState.h"
#include "RegisterSet.h"
#include "ComputerState.h"

using std::map;
using std::bitset;
using std::function;

using State::ComputerState;
using Architecture::RegisterSet;

namespace Logics {
	template<int BS, int IMS, int RMS>
	class CpuRunner {
	public:
		bool tick(const RegisterSet<BS, IMS>& regs, ComputerState<BS, IMS, RMS>& state) const {
			auto& cpu = state.CPU;
			auto& ram = state.RAM;
			if (cpu.get(regs.Terminated).test(0)) {
				return false; // Terminated
			}
			auto ip = cpu.get(regs.IP);
			Reference<BS * 3> command(ip.to_ulong()); // Command code + 2 args
			auto command_body = ram.get(command);
			perform_command(regs, state, command_body);
			// todo: check invalid references
			// todo: increase counter
			return true; // Continue execution
		}

	private:
		using CommandHandler =
			function<void(const RegisterSet<BS, IMS>& regs, ComputerState<BS, IMS, RMS>& state, const bitset<BS>&, const bitset<BS>&)>;

		void perform_command(const RegisterSet<BS, IMS>& regs, ComputerState<BS, IMS, RMS>& state, const bitset<BS * 3>& command) const {
			auto code = BitUtils::get_bits<BS, BS * 3>(command, 0);
			auto arg1 = BitUtils::get_bits<BS, BS * 3>(command, BS);
			auto arg2 = BitUtils::get_bits<BS, BS * 3>(command, BS * 2);

			auto code_value = code.to_ulong();
			auto cmd_iter = _commands.find(code_value);
			if (cmd_iter != _commands.end()) {
				CommandHandler cmd = _commands.at(code_value);
				cmd(regs, state, arg1, arg2);
			}
		}

		map<unsigned long, CommandHandler> _commands = {
			{ 0b0001, &CpuRunner::RST } // RST _ _ - set Terminated flag
		};

		static void RST(const RegisterSet<BS, IMS>& regs, ComputerState<BS, IMS, RMS>& state, const bitset<BS>& a, const bitset<BS>& b) {
			state.CPU.set(regs.Terminated, bitset<1>(0b1));
		}
	};
}