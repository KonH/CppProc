#pragma once

#include <map>
#include <bitset>
#include <optional>
#include <functional>

#include "Logger.h"
#include "BitUtils.h"
#include "CpuLogics.h"
#include "RegisterSet.h"
#include "MemoryState.h"

using std::map;
using std::bitset;
using std::function;
using std::optional;

using Logics::CpuLogics;
using State::MemoryState;
using Architecture::RegisterSet;

namespace Logics {
	template<int BS, int IMS>
	class CpuCommands {
	public:
		using HandlerFunc =
			function<void(CpuCommands, const bitset<BS>&, const bitset<BS>&)>;

		class Handler {
		public:
			const int         Arguments;
			const HandlerFunc Func;

			Handler(int args, HandlerFunc func) : Arguments(args), Func(func) {}
		};

		CpuCommands(const RegisterSet<BS, IMS>& regs, MemoryState<IMS>& cpu, CpuLogics<BS, IMS>& logics): _regs(regs), _cpu(cpu), _logics(logics) {}
	
		optional<Handler> get_handler(const bitset<BS>& command_code) {
			auto code_value = command_code.to_ulong();
			auto cmd_iter = _commands.find(code_value);
			if (cmd_iter != _commands.end()) {
				auto handler = _commands.at(code_value);
				return handler;
			}
			else {
				Utils::log_line("CpuCommands.get_handler: unknown command!");
			}
			return {};
		}

	private:
		map<unsigned long, Handler> _commands = {
			{ 0b0000, { 0, [](auto c, const auto& x, const auto& y) { c.NOOP();    } } }, // NOOP _ _ => no operation, just bump IP & inc Counter
			{ 0b0001, { 0, [](auto c, const auto& x, const auto& y) { c.RST();     } } }, // RST  _ _ => set Terminated flag
			{ 0b0010, { 1, [](auto c, const auto& x, const auto& y) { c.CLR(x);    } } }, // CLR  x _ => clear given common register (r[x])
			{ 0b0011, { 1, [](auto c, const auto& x, const auto& y) { c.INC(x);    } } }, // INC  x _ => inc given common register
			{ 0b0100, { 2, [](auto c, const auto& x, const auto& y) { c.SUM(x, y); } } }, // SUM  y x => r[y] + r[x] will be saved to AC
			{ 0b0101, { 2, [](auto c, const auto& x, const auto& y) { c.MOV(x, y); } } }, // MOV  y x => move r[x] value to r[y]
		};

		void NOOP() {
			Utils::log_line("CpuCommands.NOOP");
		}

		void RST() {
			Utils::log_line("CpuCommands.RST");
			_cpu.set(_regs.Terminated, bitset<1>(0b1));
		}

		void CLR(const bitset<BS>& x) {
			Utils::log_line("CpuCommands.CLR(x = ", x, ")");
			_cpu.set(_regs.get_CN(x), BitUtils::get_zero<BS>());
		}

		void INC(const bitset<BS>& x) {
			Utils::log_line("CpuCommands.INC(x = ", x, ")");
			_logics.inc_register<BS>(_regs.get_CN(x));
		}

		void SUM(const bitset<BS>& x, const bitset<BS>& y) {
			Utils::log_line("CpuCommands.SUM(x = ", x, ", y = ", y, ")");
			auto x_value = _cpu.get<BS>(_regs.get_CN(x));
			auto y_value = _cpu.get<BS>(_regs.get_CN(y));
			auto[result, overflow] = BitUtils::plus(x_value, y_value);
			_logics.set_overflow(overflow);
			_cpu.set(_regs.AR, result);
		}

		void MOV(const bitset<BS>& x, const bitset<BS>& y) {
			Utils::log_line("CpuCommands.MOV(x = ", x, ", y = ", y, ")");
			auto y_addr = _regs.get_CN(y);
			auto y_value = _cpu.get<BS>(y_addr);
			Utils::log_line("CpuCommands.MOV(r[", y_addr.Address, ":", y_addr.Size, "] = ", y_value, ") => ");
			auto x_addr = _regs.get_CN(x);
			_cpu.set(x_addr, y_value);
			Utils::log_line("CpuCommands.MOV(r[", x_addr.Address, ":", x_addr.Size, "])");
		}

	private:
		const RegisterSet<BS, IMS>& _regs;
		MemoryState<IMS>&           _cpu;
		CpuLogics<BS, IMS>&         _logics;
	};
}