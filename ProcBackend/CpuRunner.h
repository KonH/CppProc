#pragma once

#include <map>
#include <tuple>
#include <bitset>
#include <functional>

#include "Logger.h"
#include "CpuLogics.h"
#include "RegisterSet.h"
#include "MemoryState.h"
#include "RegisterSet.h"
#include "CpuCommands.h"

using std::map;
using std::tuple;
using std::bitset;
using std::function;

using State::MemoryState;
using Logics::CpuCommands;
using Logics::CpuLogics;
using Architecture::RegisterSet;

namespace Logics {
	template<int BS, int IMS, int RMS>
	class CpuRunner {

		using PipelineStep =
			function<void(CpuRunner*)>;

		using CommandHandlerFunc =
			function<void(CpuRunner*)>;

		class CommandHandler {
		public:
			const int                Arguments;
			const CommandHandlerFunc Func;

			CommandHandler(int args, CommandHandlerFunc func) : Arguments(args), Func(func) {}
		};

	public:
		CpuRunner(const RegisterSet<BS, IMS>& regs, MemoryState<IMS>& cpu, MemoryState<2>& control, MemoryState<BS>& address, MemoryState<BS>& data) :
			_regs(regs), _cpu(cpu), _control(control), _address(address), _data(data), _logics(CpuLogics(regs, cpu)), _commands(CpuCommands(regs, cpu, _logics)) {}

		bool tick() {
			Utils::log_line(
				"CpuRunner.tick(control: ",
				_control.get_all(),
				", address: ", _address.get_all(),
				", data: ", _data.get_all(), ")"
			);

			_control.set(Reference<2>(0), BitUtils::get_zero<2>());

			if (is_terminated()) {
				Utils::log_line("CpuRunner.tick: terminated.");
				return false;
			}
			Utils::log_line("CpuRunner.tick: continue execution.");
			auto state = _cpu.get(_regs.PipelineState);
			auto step = get_step(state);
			step(this);
			return !is_terminated();
		}

	private:
		const RegisterSet<BS, IMS>& _regs;
		MemoryState<IMS>&           _cpu;
		MemoryState<2>&             _control;
		MemoryState<BS>&            _address;
		MemoryState<BS>&            _data;
		CpuLogics<BS, IMS>          _logics;
		CpuCommands<BS, IMS>        _commands;

		map<unsigned long, PipelineStep> _steps = {
			{ 0b000, &CpuRunner::tick_fetch   }, // request command code
			{ 0b001, &CpuRunner::tick_decode  }, // save command code, request arg #1, if required
			{ 0b010, &CpuRunner::tick_read_1  }, // save arg #1, request arg #2, if requred
			{ 0b011, &CpuRunner::tick_read_2  }, // save arg #2
			{ 0b100, &CpuRunner::tick_execute }, // execute with saved code & args
		};

		void tick_fetch() {
			Utils::log_line("CpuRunner.tick_fetch");
			_cpu.set(Reference<3>(_regs.PipelineState), BitUtils::get_zero<3> ());
			_cpu.set(Reference<1>(_regs.ArgumentMode),  BitUtils::get_zero<1> ());
			_cpu.set(Reference<BS>(_regs.CommandCode),  BitUtils::get_zero<BS>());
			_cpu.set(Reference<BS>(_regs.Arg1),         BitUtils::get_zero<BS>());
			_cpu.set(Reference<BS>(_regs.Arg2),         BitUtils::get_zero<BS>());
			auto ip = _cpu.get(_regs.IP);
			Reference<BS> command(ip.to_ulong());
			request_ram_read(command);
			_logics.inc_register(Reference<3>(_regs.PipelineState));
		}

		void tick_decode() {
			Utils::log_line("CpuRunner.tick_decode");
			auto code = _data.get<BS>(Reference<BS>(0));
			_cpu.set(Reference<BS>(_regs.CommandCode), code);
			if (auto handler_opt = get_cur_handler<BS, IMS>()) {
				auto handler = handler_opt.value();
				auto args = handler.Arguments;
				if (args == 0) {
					set_next_step(0b100); // execute
				}
				else {
					set_next_step(0b010, args > 1); // read #1
					auto ip = _cpu.get(_regs.IP);
					request_ram_read(ip.to_ulong() + BS);
				}
			} else {
				raise_fatal();
			}
		}

		void tick_read_1() {
			Utils::log_line("CpuRunner.tick_read_1");
			auto arg1 = _data.get<BS>(Reference<BS>(0));
			_cpu.set(Reference<BS>(_regs.Arg1), arg1);
			if (_cpu.get(_regs.ArgumentMode).test(0)) {
				set_next_step(0b011); // read #2
				auto ip = _cpu.get(_regs.IP);
				request_ram_read(ip.to_ulong() + BS * 2);
			} else {
				set_next_step(0b100); // execute
			}
		}

		void tick_read_2() {
			Utils::log_line("CpuRunner.tick_read_2");
			auto arg2 = _data.get(Reference<BS>(0));
			_cpu.set(Reference<BS>(_regs.Arg2), arg2);
			_logics.inc_register(Reference<3>(_regs.PipelineState));
		}

		void tick_execute() {
			Utils::log_line("CpuRunner.tick_execute");
			if (auto handler_opt = get_cur_handler<BS, IMS>()) {
				auto handler = handler_opt.value();
				auto[y, x] = read_args();
				handler.Func(_commands, x, y);
				if (!is_terminated()) {
					set_next_operation(1 + handler.Arguments);
				}
			} else {
				raise_fatal();
			}
		}

		void tick_empty() {}

		void set_next_step(int step) {
			Utils::log_line("CpuRunner.set_next_step(", step, ")");
			_cpu.set(Reference<3>(_regs.PipelineState), BitUtils::get_set<3>(step));
		}

		void set_next_step(int step, bool two_args) {
			Utils::log_line("CpuRunner.set_next_step(", step, ", ", two_args, ")");
			set_next_step(step);
			_cpu.set(Reference<1>(_regs.ArgumentMode), BitUtils::get_set<1>(two_args));
		}

		PipelineStep get_step(const bitset<3>& pipeline_state) {
			auto state_value = pipeline_state.to_ulong();
			auto step_iter = _steps.find(state_value);
			if (step_iter != _steps.end()) {
				auto step = _steps.at(state_value);
				return step;
			}
			else {
				Utils::log_line("CpuRunner.get_step: unknown step!");
				raise_fatal();
			}
			return &CpuRunner::tick_empty;
		}

		void request_ram_read(Reference<BS> address) {
			Utils::log_line("CpuRunner.request_ram_read: ", address.Address, ":", address.Size);
			_control.set(Reference<2>(0),  bitset<2>(0b01));
			_address.set(Reference<BS>(0), BitUtils::get_set<BS>(address.Address));
			_data.set   (Reference<BS>(0), BitUtils::get_zero<BS>());

		}

		void request_ram_write(Reference<BS> address, bitset<BS> value) {
			Utils::log_line("CpuRunner.request_ram_write: ", address.Address, ":", address.Size, " = ", value);
			_control.set(Reference<2>(0),  bitset<2>(0b11));
			_address.set(Reference<BS>(0), BitUtils::get_set<BS>(address.Address));
			_data.set   (Reference<BS>(0), value);
		}
		template<int BS, int IMS>
		auto get_cur_handler() {
			auto command_code = _cpu.get(_regs.CommandCode);
			auto handler = _commands.get_handler(command_code);
			return handler;
		}

		bool is_terminated() {
			return _cpu.get(_regs.Terminated).test(0);
		}

		void raise_fatal() {
			Utils::log_line("CpuRunner.raise_fatal");
			_cpu.set<1>(_regs.Fatal, 0b1);
			_cpu.set<1>(_regs.Terminated, 0b1);
		}

		void inc_counter() {
			Utils::log_line("CpuRunner.inc_counter");
			_logics.inc_register(_regs.Counter);
		}

		void bump_ip(int size) {
			Utils::log_line("CpuRunner.bump_ip(", size, ")");
			auto overflow = _logics.add_to_register<BS>(_regs.IP, BitUtils::get_set<BS>(BS * size));
			if (overflow) {
				raise_fatal();
			}
		}

		void set_next_operation(int size) {
			Utils::log_line("CpuRunner.set_next_operation(", size, ")");
			inc_counter();
			bump_ip(size);
		}

		tuple<bitset<BS>, bitset<BS>> read_args() {
			return std::make_tuple(_cpu.get(_regs.Arg1), _cpu.get(_regs.Arg2));
		}
	};
}