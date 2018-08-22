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
			function<bool(CpuCommands, const int step, const bitset<BS>&, const bitset<BS>&)>;

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
			return { false, Handler(0, [](auto c, const int step, const auto& x, const auto& y) { return true; }) };
		}

	private:
		#define HANDLER_0(func)  { 0, [](auto c, const int step, const auto& x, const auto& y) { c.func();     return true; } }
		#define HANDLER_1(func)  { 1, [](auto c, const int step, const auto& x, const auto& y) { c.func(x);    return true; } }
		#define HANDLER_2(func)  { 2, [](auto c, const int step, const auto& x, const auto& y) { c.func(x, y); return true; } }
		
		#define HANDLER_2N(func) { 2, [](auto c, const int step, const auto& x, const auto& y) { return c.func(step, x, y); } }
		
		map<unsigned long, Handler> _commands = {
			{ 0b0000, HANDLER_0 (NOOP) }, // NOOP _ _ => no operation, just bump IP & inc Counter
			{ 0b0001, HANDLER_0 (RST)  }, // RST  _ _ => set Terminated flag
			{ 0b0010, HANDLER_1 (CLR)  }, // CLR  x _ => clear given common register (r[x])
			{ 0b0011, HANDLER_1 (INC)  }, // INC  x _ => increment given common register
			{ 0b0100, HANDLER_2 (SUM)  }, // SUM  x y => r[y] + r[x] will be saved to AC
			{ 0b0101, HANDLER_2 (MOV)  }, // MOV  x y => move r[x] value to r[y]
			{ 0b0110, HANDLER_0 (RSTA) }, // RSTA _ _ => clear AR register
			{ 0b0111, HANDLER_0 (INCA) }, // INCA _ _ => increment AR register
			{ 0b1000, HANDLER_1 (ADDA) }, // ADDA x _ => add r[x] to AR register
			{ 0b1001, HANDLER_2N(LD)   }, // LD   x y => load data from ram by address at r[x] to r[y]
			{ 0b1010, HANDLER_2 (ST)   }, // ST   x y => store data from r[x] to ram by address r[y]
		};
        
        using CmdArg = const bitset<BS>&;

		void NOOP() {
			Utils::log_line("CpuCommands.NOOP");
		}

		void RST() {
			Utils::log_line("CpuCommands.RST");
			_cpu.set_bits(_regs.Terminated, bitset<1>(0b1));
		}

		void CLR(CmdArg x) {
			Utils::log_line("CpuCommands.CLR(x = ", x, ")");
			_cpu.set_bits(_regs.get_CN(x), BitUtils::get_zero<BS>());
		}

		void INC(CmdArg x) {
			Utils::log_line("CpuCommands.INC(x = ", x, ")");
			_logics.inc_register(_regs.get_CN(x));
		}

		void SUM(CmdArg x, CmdArg y) {
			Utils::log_line("CpuCommands.SUM(x = ", x, ", y = ", y, ")");
			auto x_value = _cpu[_regs.get_CN(x)];
			auto y_value = _cpu[_regs.get_CN(y)];
			auto[result, overflow] = BitUtils::plus(x_value, y_value);
			_logics.set_overflow(overflow);
			_cpu.set_bits(_regs.AR, result);
		}

		void MOV(CmdArg x, CmdArg y) {
			Utils::log_line("CpuCommands.MOV(x = ", x, ", y = ", y, ")");
			auto x_addr = _regs.get_CN(x);
			auto x_value = _cpu[x_addr];
			auto y_addr = _regs.get_CN(y);
			_cpu.set_bits(y_addr, x_value);
		}

		void RSTA() {
			Utils::log_line("CpuCommands.RSTA");
			_cpu.set_zero(_regs.AR);
		}
		
		void INCA() {
			Utils::log_line("CpuCommands.INCA");
			_logics.inc_register(_regs.AR);
		}
		
		void ADDA(CmdArg x) {
			Utils::log_line("CpuCommands.ADDA(", x, ")");
			_logics.add_to_register(_regs.AR, x);
		}
		
		bool LD(int step, CmdArg x, CmdArg y) {
			switch (step) {
				case 0:
					Utils::log_line("CpuCommands.LD_0(", x, ", ", y, ")");
					_logics.request_ram_read(Reference<BS>(x.to_ulong()));
					return false;
					
				case 1:
					Utils::log_line("CpuCommands.LD_1(", x, ", ", y, ")");
					auto value = _logics.read_data_bus();
					Utils::log_line("CpuCommands.LD_1: readed value: ", value);
					_cpu.set_bits(_regs.get_CN(y), value);
					return true;
			}
			return true;
		}
		
		void ST(CmdArg x, CmdArg y) {
			Utils::log_line("CpuCommands.ST(", x, ", ", y, ")");
			auto value = _cpu[_regs.get_CN(x)];
			auto addr = Reference<BS>(y.to_ulong());
			_logics.request_ram_write(addr, value);
		}

	private:
		Regs     _regs;
		CpuMem   _cpu;
		CpuLogic _logics;
	};
}
