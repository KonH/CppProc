#pragma once

#include <map>
#include <bitset>
#include <functional>

#include "Logger.h"
#include "BitUtils.h"
#include "CpuLogics.h"
#include "RegisterSet.h"
#include "MemoryState.h"

using std::map;
using std::bitset;
using std::function;

using Logics::CpuLogics;
using State::MemoryState;
using Architecture::RegisterSet;

namespace Logics {
	template<size_t BS, size_t IMS>
	class CpuCommands {
		using Regs     = const RegisterSet<BS, IMS>&;
		using CpuMem   = MemoryState<IMS>&;
		using CpuLogic = CpuLogics<BS, IMS>&;
		
	public:
		using HandlerFunc =
			function<void(CpuCommands, const bitset<BS>&, const bitset<BS>&)>;

		class Handler {
		public:
			const int         Arguments;
			const HandlerFunc Func;

			Handler(int args, HandlerFunc func) : Arguments(args), Func(func) {}
		};

		CpuCommands(Regs regs, CpuMem cpu, CpuLogic logics): _regs(regs), _cpu(cpu), _logics(logics) {}
	
		tuple<bool, Handler> get_handler(const bitset<BS>& command_code) {
			auto code_value = command_code.to_ulong();
			auto cmd_iter = _commands.find(code_value);
			if (cmd_iter != _commands.end()) {
				auto handler = _commands.at(code_value);
				return { true, handler };
			}
			else {
				Utils::log_line("CpuCommands.get_handler: unknown command!");
			}
			return { false, Handler(0, [](auto c, const auto& x, const auto& y) { }) };
		}

	private:
        #define HANDLER_0(func) { 0, [](auto c, const auto& x, const auto& y) { c.func();  } }
        #define HANDLER_1(func) { 1, [](auto c, const auto& x, const auto& y) { c.func(x); } }
        #define HANDLER_2(func) { 2, [](auto c, const auto& x, const auto& y) { c.func(x, y); } }
        
		map<unsigned long, Handler> _commands = {
			{ 0b0000, HANDLER_0(NOOP) }, // NOOP _ _ => no operation, just bump IP & inc Counter
			{ 0b0001, HANDLER_0(RST)  }, // RST  _ _ => set Terminated flag
			{ 0b0010, HANDLER_1(CLR)  }, // CLR  x _ => clear given common register (r[x])
			{ 0b0011, HANDLER_1(INC)  }, // INC  x _ => inc given common register
			{ 0b0100, HANDLER_2(SUM)  }, // SUM  y x => r[y] + r[x] will be saved to AC
			{ 0b0101, HANDLER_2(MOV)  }, // MOV  y x => move r[x] value to r[y]
		};

		void NOOP() {
			Utils::log_line("CpuCommands.NOOP");
		}

		void RST() {
			Utils::log_line("CpuCommands.RST");
			_cpu.set_bits(_regs.Terminated, bitset<1>(0b1));
		}

		void CLR(const bitset<BS>& x) {
			Utils::log_line("CpuCommands.CLR(x = ", x, ")");
			_cpu.set_bits(_regs.get_CN(x), BitUtils::get_zero<BS>());
		}

		void INC(const bitset<BS>& x) {
			Utils::log_line("CpuCommands.INC(x = ", x, ")");
			_logics.inc_register(_regs.get_CN(x));
		}

		void SUM(const bitset<BS>& x, const bitset<BS>& y) {
			Utils::log_line("CpuCommands.SUM(x = ", x, ", y = ", y, ")");
			auto x_value = _cpu[_regs.get_CN(x)];
			auto y_value = _cpu[_regs.get_CN(y)];
			auto[result, overflow] = BitUtils::plus(x_value, y_value);
			_logics.set_overflow(overflow);
			_cpu.set_bits(_regs.AR, result);
		}

		void MOV(const bitset<BS>& x, const bitset<BS>& y) {
			Utils::log_line("CpuCommands.MOV(x = ", x, ", y = ", y, ")");
			auto y_addr = _regs.get_CN(y);
			auto y_value = _cpu[y_addr];
			Utils::log_line("CpuCommands.MOV(r[", y_addr, "] = ", y_value, ") => ");
			auto x_addr = _regs.get_CN(x);
			_cpu.set_bits(x_addr, y_value);
			Utils::log_line("CpuCommands.MOV(r[", x_addr, "])");
		}

	private:
		Regs     _regs;
		CpuMem   _cpu;
		CpuLogic _logics;
	};
}
