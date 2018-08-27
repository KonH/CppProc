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
	enum Command {
		NOOP = 0x00,
		RST  = 0x01,
		CLR  = 0x02,
		INC  = 0x03,
		SUM  = 0x04,
		MOV  = 0x05,
		RSTA = 0x06,
		INCA = 0x07,
		ADDA = 0x08,
		LD   = 0x09,
		ST   = 0x0A,
		SUB  = 0x0B,
		SUBA = 0x0C,
		DEC  = 0x0D,
		DECA = 0x0E,
		JMP  = 0x0F,
	};
	
	template<size_t IMS>
	class CpuCommands {
		using Regs     = const RegisterSet<IMS>&;
		using CpuMem   = MemoryState<IMS>&;
		using CpuLogic = CpuLogics<IMS>&;
		
	public:
		using HandlerFunc =
			function<bool(CpuCommands, const int step, const Word&, const Word&)>;

		class Handler {
		public:
			const int         Arguments;
			const HandlerFunc Func;

			Handler(int args, HandlerFunc func) : Arguments(args), Func(func) {}
		};

		CpuCommands(Regs regs, CpuMem cpu, CpuLogic logics): _regs(regs), _cpu(cpu), _logics(logics) {}
	
		tuple<bool, Handler> get_handler(const Word& command_code) {
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
			{ Command::NOOP, HANDLER_0 (NOOP) }, // NOOP _ _ => no operation, just bump IP & inc Counter
			{ Command::RST,  HANDLER_0 (RST)  }, // RST  _ _ => set Terminated flag
			{ Command::CLR,  HANDLER_1 (CLR)  }, // CLR  x _ => clear given common register (r[x])
			{ Command::INC,  HANDLER_1 (INC)  }, // INC  x _ => increment given common register
			{ Command::SUM,  HANDLER_2 (SUM)  }, // SUM  x y => r[y] + r[x] will be saved to AC
			{ Command::MOV,  HANDLER_2 (MOV)  }, // MOV  x y => move r[x] value to r[y]
			{ Command::RSTA, HANDLER_0 (RSTA) }, // RSTA _ _ => clear AR register
			{ Command::INCA, HANDLER_0 (INCA) }, // INCA _ _ => increment AR register
			{ Command::ADDA, HANDLER_1 (ADDA) }, // ADDA x _ => add r[x] to AR register
			{ Command::LD,   HANDLER_2N(LD)   }, // LD   x y => load data from ram by address at r[x] to r[y]
			{ Command::ST,   HANDLER_2 (ST)   }, // ST   x y => store data from r[x] to ram by address r[y]
			{ Command::SUB,  HANDLER_2 (SUB)  }, // SUB  x y => acc = r[x] - r[y]
			{ Command::SUBA, HANDLER_1 (SUBA) }, // SUBA x _ => acc = acc - r[x]
			{ Command::DEC,  HANDLER_1 (DEC)  }, // DEC  x _ => r[x] = r[x] - 1
			{ Command::DECA, HANDLER_0 (DECA) }, // DECA _ _ => AR = AR - 1
			{ Command::JMP,  HANDLER_1 (JMP)  }, // JMP  x _ => set IP to x
		};
        
        using CmdArg = const Word&;

		void set_next_op(size_t args) {
			_logics.set_next_operation(args);
		}
		
		void NOOP() {
			Utils::log_line("CpuCommands.NOOP");
			set_next_op(0);
		}

		void RST() {
			Utils::log_line("CpuCommands.RST");
			_cpu.set_bits(_regs.Terminated, bitset<1>(0b1));
			set_next_op(0);
		}

		void CLR(CmdArg x) {
			Utils::log_line("CpuCommands.CLR(x = ", x, ")");
			_cpu.set_bits(_regs.get_CN(x), BitUtils::get_zero());
			set_next_op(1);
		}

		void INC(CmdArg x) {
			Utils::log_line("CpuCommands.INC(x = ", x, ")");
			_logics.inc_register(_regs.get_CN(x));
			set_next_op(1);
		}

		void SUM(CmdArg x, CmdArg y) {
			Utils::log_line("CpuCommands.SUM(x = ", x, ", y = ", y, ")");
			auto x_value = _cpu[_regs.get_CN(x)];
			auto y_value = _cpu[_regs.get_CN(y)];
			auto[result, overflow] = BitUtils::plus(x_value, y_value);
			_logics.set_overflow(overflow);
			_cpu.set_bits(_regs.AR, result);
			set_next_op(2);
		}

		void MOV(CmdArg x, CmdArg y) {
			Utils::log_line("CpuCommands.MOV(x = ", x, ", y = ", y, ")");
			auto x_addr = _regs.get_CN(x);
			auto x_value = _cpu[x_addr];
			auto y_addr = _regs.get_CN(y);
			_cpu.set_bits(y_addr, x_value);
			set_next_op(2);
		}

		void RSTA() {
			Utils::log_line("CpuCommands.RSTA");
			_cpu.set_zero(_regs.AR);
			set_next_op(0);
		}
		
		void INCA() {
			Utils::log_line("CpuCommands.INCA");
			_logics.inc_register(_regs.AR);
			set_next_op(0);
		}
		
		void ADDA(CmdArg x) {
			Utils::log_line("CpuCommands.ADDA(", x, ")");
			_logics.add_to_register(_regs.AR, x);
			set_next_op(1);
		}
		
		bool LD(int step, CmdArg x, CmdArg y) {
			switch (step) {
				case 0:
					Utils::log_line("CpuCommands.LD_0(", x, ", ", y, ")");
					_logics.request_ram_read(WReference(x.to_ulong()));
					return false;
					
				case 1:
					Utils::log_line("CpuCommands.LD_1(", x, ", ", y, ")");
					auto value = _logics.read_data_bus();
					Utils::log_line("CpuCommands.LD_1: readed value: ", value);
					_cpu.set_bits(_regs.get_CN(y), value);
					set_next_op(2);
					return true;
			}
			return true;
		}
		
		void ST(CmdArg x, CmdArg y) {
			Utils::log_line("CpuCommands.ST(", x, ", ", y, ")");
			auto value = _cpu[_regs.get_CN(x)];
			auto addr = WReference(y.to_ulong());
			_logics.request_ram_write(addr, value);
			set_next_op(2);
		}
		
		void SUB(CmdArg x, CmdArg y) {
			Utils::log_line("CpuCommands.SUB(", x, ", ", y, ")");
			auto xref = _regs.get_CN(x);
			auto y_value = _cpu[_regs.get_CN(y)];
			_logics.sub_register(xref, y_value);
			set_next_op(2);
		}
		
		void SUBA(CmdArg x) {
			Utils::log_line("CpuCommands.SUBA(", x, ")");
			auto xref = _regs.get_CN(x);
			auto x_value = _cpu[xref];
			_logics.sub_register(_regs.AR, x_value);
			set_next_op(1);
		}
		
		void DEC(CmdArg x) {
			Utils::log_line("CpuCommands.DEC(", x, ")");
			_logics.dec_register(_regs.get_CN(x));
			set_next_op(1);
		}
		
		void DECA() {
			Utils::log_line("CpuCommands.DECA");
			_logics.dec_register(_regs.AR);
			set_next_op(0);
		}
		
		void JMP(CmdArg x) {
			Utils::log_line("CpuCommands.JMP(", x, ")");
			_logics.inc_counter();
			_cpu.set_bits(_regs.IP, x);
		}

	private:
		Regs     _regs;
		CpuMem   _cpu;
		CpuLogic _logics;
	};
}
