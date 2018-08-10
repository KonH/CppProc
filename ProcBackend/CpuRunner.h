#pragma once

#include <map>
#include <tuple>
#include <bitset>
#include <functional>

#include "Logger.h"
#include "RegisterSet.h"
#include "MemoryState.h"
#include "RegisterSet.h"
#include "ComputerState.h"

using std::map;
using std::tuple;
using std::bitset;
using std::function;

using State::MemoryState;
using State::ComputerState;
using Architecture::RegisterSet;

namespace Logics {
	template<int BS, int IMS, int RMS>
	class CpuRunner {
	public:
		CpuRunner(const RegisterSet<BS, IMS>& regs, MemoryState<IMS>& cpu, MemoryState<RMS>& ram):_regs(regs), _cpu(cpu), _ram(ram) {}

		bool tick() {
			if (_cpu.get(_regs.Terminated).test(0)) {
				Utils::log_line("Runner.tick: terminated.");
				return false;
			}
			Utils::log_line("Runner.tick: continue execution.");
			auto ip = _cpu.get(_regs.IP);
			Reference<BS> command(ip.to_ulong());
			auto command_code = _ram.get(command);
			perform_command(command_code);
			return true;
		}

	private:
		const RegisterSet<BS, IMS>& _regs;
		MemoryState<IMS>&           _cpu;
		MemoryState<RMS>&           _ram;

		void set_overflow(bool value) {
			Utils::log_line("Runner.set_overflow(", value, ")");
			_cpu.set<1>(_regs.Overflow, BitUtils::get_set<1>(value));
		}

		template<int Size>
		bool add_to_register(Reference<Size> ref, bitset<Size> value) {
			Utils::log_line("Runner.add_to_register(", ref.Address, ":", ref.Size, ", ", value, ")");
			auto old_value = _cpu.get(ref);
			auto[new_value, overflow] = BitUtils::plus(old_value, value);
			_cpu.set(ref, new_value);
			set_overflow(overflow);
			return overflow;
		}

		template<int Size>
		bool inc_register(Reference<Size> ref) {
			Utils::log_line("Runner.inc_register(", ref.Address, ":", ref.Size, ")");
			return add_to_register(ref, BitUtils::get_one<Size>());
		}

		void raise_fatal() {
			Utils::log_line("Runner.raise_fatal");
			_cpu.set<1>(_regs.Fatal, 0b1);
			_cpu.set<1>(_regs.Terminated, 0b1);
		}

		void inc_counter() {
			Utils::log_line("Runner.inc_counter");
			auto overflow = inc_register(_regs.Counter);
			if (overflow) {
				raise_fatal();
			}
		}

		void bump_ip(int size) {
			Utils::log_line("Runner.bump_ip(", size, ")");
			auto overflow = add_to_register<BS>(_regs.IP, BitUtils::get_set<BS>(BS * size));
			if (overflow) {
				raise_fatal();
			}
		}

		void set_next_operation(int size) {
			Utils::log_line("Runner.set_next_operation(", size, ")");
			inc_counter();
			bump_ip(size);
		}

		using CommandHandler =
			function<void(CpuRunner*)>;

		void perform_command(const bitset<BS>& command_code) {
			Utils::log_line("Runner.perform_command(", command_code, ")");

			auto code_value = command_code.to_ulong();
			auto cmd_iter = _commands.find(code_value);
			if (cmd_iter != _commands.end()) {
				CommandHandler cmd = _commands.at(code_value);
				cmd(this);
			} else {
				Utils::log_line("Runner.perform_command: unknown command!");
				raise_fatal();
			}
		}

		bitset<BS> read_arg_N(int index) {
			auto address = _cpu.get(_regs.IP).to_ulong() + BS * (index + 1);
			return _ram.get(Reference<BS>(address));
		}

		bitset<BS> read_arg_0() {
			return read_arg_N(0);
		}

		tuple<bitset<BS>, bitset<BS>> read_args() {
			return std::make_tuple(read_arg_N(0), read_arg_N(1));
		}

		map<unsigned long, CommandHandler> _commands = {
			{ 0b0000, &CpuRunner::NOOP }, // NOOP _ _ => no operation, just bump IP & inc Counter
			{ 0b0001, &CpuRunner::RST  }, // RST  _ _ => set Terminated flag
			{ 0b0010, &CpuRunner::CLR  }, // CLR  x _ => clear given common register (r[x])
			{ 0b0011, &CpuRunner::INC  }, // INC  x _ => inc given common register
			{ 0b0100, &CpuRunner::SUM  }, // SUM  x y => r[x] + r[y] will be saved to AC
			{ 0b0101, &CpuRunner::MOV  }, // MOV  x y => move r[x] value to r[y]
		};

		void NOOP() {
			Utils::log_line("Runner.NOOP");
			set_next_operation(1);
		}

		void RST() {
			Utils::log_line("Runner.RST");
			inc_counter();
			_cpu.set(_regs.Terminated, bitset<1>(0b1));
		}

		void CLR() {
			Utils::log_line("Runner.CLR");
			auto a = read_arg_0();
			_cpu.set(_regs.get_CN(a), BitUtils::get_zero<BS>());
			set_next_operation(2);
		}

		void INC() {
			Utils::log_line("Runner.INC");
			auto a = read_arg_0();
			inc_register<BS>(_regs.get_CN(a));
			set_next_operation(2);
		}

		void SUM() {
			Utils::log_line("Runner.SUM");
			auto [a, b] = read_args();
			auto a_value = _cpu.get<BS>(_regs.get_CN(a));
			auto b_value = _cpu.get<BS>(_regs.get_CN(b));
			auto[result, overflow] = BitUtils::plus(a_value, b_value);
			set_overflow(overflow);
			_cpu.set(_regs.AR, result);
			set_next_operation(3);
		}

		void MOV() {
			Utils::log_line("Runner.MOV");
			auto[a, b] = read_args();
			auto a_value = _cpu.get<BS>(_regs.get_CN(a));
			_cpu.set(_regs.get_CN(b), a_value);
			set_next_operation(3);
		}
	};
}