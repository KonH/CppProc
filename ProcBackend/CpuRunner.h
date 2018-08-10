#pragma once

#include <map>
#include <bitset>
#include <functional>

#include "Logger.h"
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
				Utils::log_line("Runner.tick: terminated.");
				return false;
			}
			Utils::log_line("Runner.tick: continue execution.");
			auto ip = cpu.get(regs.IP);
			Reference<BS * 3> command(ip.to_ulong()); // Command code + 2 args
			auto command_body = ram.get(command);
			perform_command(regs, state, command_body);
			return true;
		}

	private:
		template<int Size>
		static bool add_to_register(const RegisterSet<BS, IMS>& regs, ComputerState<BS, IMS, RMS>& state, Reference<Size> ref, bitset<Size> value) {
			Utils::log_line("Runner.add_to_register(", ref.Address, ":", ref.Size, ", ", value, ")");
			auto old_value = state.CPU.get(ref);
			auto[new_value, overflow] = BitUtils::plus(old_value, value);
			state.CPU.set(ref, new_value);
			state.CPU.set<1>(regs.Overflow, BitUtils::get_set<1>(overflow));
			return overflow;
		}

		template<int Size>
		static bool inc_register(const RegisterSet<BS, IMS>& regs, ComputerState<BS, IMS, RMS>& state, Reference<Size> ref) {
			Utils::log_line("Runner.inc_register(", ref.Address, ":", ref.Size, ")");
			return add_to_register(regs, state, ref, BitUtils::get_one<Size>());
		}

		static void raise_fatal(const RegisterSet<BS, IMS>& regs, ComputerState<BS, IMS, RMS>& state) {
			Utils::log_line("Runner.raise_fatal");
			state.CPU.set<1>(regs.Fatal, 0b1);
			state.CPU.set<1>(regs.Terminated, 0b1);
		}

		static void inc_counter(const RegisterSet<BS, IMS>& regs, ComputerState<BS, IMS, RMS>& state) {
			Utils::log_line("Runner.inc_counter");
			auto overflow = inc_register(regs, state, regs.Counter);
			if (overflow) {
				raise_fatal(regs, state);
			}
		}

		static void bump_ip(const RegisterSet<BS, IMS>& regs, ComputerState<BS, IMS, RMS>& state) {
			Utils::log_line("Runner.bump_ip");
			auto overflow = add_to_register<BS>(regs, state, regs.IP, BitUtils::get_set<BS>(BS * 3));
			if (overflow) {
				raise_fatal(regs, state);
			}
		}

		static void set_next_operation(const RegisterSet<BS, IMS>& regs, ComputerState<BS, IMS, RMS>& state) {
			Utils::log_line("Runner.set_next_operation");
			inc_counter(regs, state);
			bump_ip(regs, state);
		}

		using CommandHandler =
			function<void(const RegisterSet<BS, IMS>& regs, ComputerState<BS, IMS, RMS>& state, const bitset<BS>&, const bitset<BS>&)>;

		void perform_command(const RegisterSet<BS, IMS>& regs, ComputerState<BS, IMS, RMS>& state, const bitset<BS * 3>& command) const {
			Utils::log_line("Runner.perform_command(", command, ")");
			
			auto code = BitUtils::get_bits<BS, BS * 3>(command, 0);
			auto arg1 = BitUtils::get_bits<BS, BS * 3>(command, BS);
			auto arg2 = BitUtils::get_bits<BS, BS * 3>(command, BS * 2);

			Utils::log_line("Runner.perform_command(", code, ", ", arg1, ", ", arg2, ")");

			auto code_value = code.to_ulong();
			auto cmd_iter = _commands.find(code_value);
			if (cmd_iter != _commands.end()) {
				CommandHandler cmd = _commands.at(code_value);
				cmd(regs, state, arg1, arg2);
			} else {
				Utils::log_line("Runner.perform_command: unknown command!");
				raise_fatal(regs, state);
			}
		}

		map<unsigned long, CommandHandler> _commands = {
			{ 0b0000, &CpuRunner::NOOP }, // NOOP _ _ - no operation, just bump IP & inc Counter
			{ 0b0001, &CpuRunner::RST  }, // RST _ _ - set Terminated flag
		};

		static void NOOP(const RegisterSet<BS, IMS>& regs, ComputerState<BS, IMS, RMS>& state, const bitset<BS>& a, const bitset<BS>& b) {
			Utils::log_line("Runner.NOOP");
			set_next_operation(regs, state);
		}

		static void RST(const RegisterSet<BS, IMS>& regs, ComputerState<BS, IMS, RMS>& state, const bitset<BS>& a, const bitset<BS>& b) {
			Utils::log_line("Runner.RST");
			state.CPU.set(regs.Terminated, bitset<1>(0b1));
		}
	};
}