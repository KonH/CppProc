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

using Core::FReference;
using Core::WReference;
using Core::PSReference;
using Core::CBReference;
using State::MemoryState;
using Logics::CpuCommands;
using Logics::CpuLogics;
using Architecture::RegisterSet;

namespace Logics {
	template<size_t IMS, size_t RMS>
	class CpuRunner {
		using Regs       = const RegisterSet<IMS>&;
		using CpuMem     = MemoryState<IMS>&;
		using ControlBus = State::ControlBusState&;
		using AddrBus    = State::AddressBusState&;
		using DataBus    = State::DataBusState&;
		using CpuLogic   = CpuLogics<IMS>;
		using CpuCommand = CpuCommands<IMS>;
		
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
		CpuRunner(Regs regs, CpuMem cpu, ControlBus control, AddrBus address, DataBus data):
		_regs(regs), _cpu(cpu), _control(control), _address(address), _data(data),
		_logics(CpuLogics(regs, cpu, control, data, address)), _commands(CpuCommands(regs, cpu, _logics)) {}

		bool tick() {
			Utils::log_line(
				"CpuRunner.tick(control: ",
				_control.get_all(),
				", address: ", _address.get_all(),
				", data: ", _data.get_all(), ")"
			);

			_control.set_zero(CBReference(0));

			if (is_terminated()) {
				Utils::log_line("CpuRunner.tick: terminated.");
				return false;
			}
			Utils::log_line("CpuRunner.tick: continue execution.");
			auto state = _cpu[_regs.PipelineState];
			auto step = get_step(state);
			step(this);
			return !is_terminated();
		}

	private:
		Regs       _regs;
		CpuMem     _cpu;
		ControlBus _control;
		AddrBus    _address;
		DataBus    _data;
		CpuLogic   _logics;
		CpuCommand _commands;

		map<unsigned long, PipelineStep> _steps = {
			{ 0b000, &CpuRunner::tick_fetch     }, // request command code
			{ 0b001, &CpuRunner::tick_decode    }, // save command code, request arg #1, if required
			{ 0b010, &CpuRunner::tick_read_1    }, // save arg #1, request arg #2, if requred
			{ 0b011, &CpuRunner::tick_read_2    }, // save arg #2
			{ 0b100, &CpuRunner::tick_execute_1 }, // execute part 1 with saved code & args
			{ 0b101, &CpuRunner::tick_execute_2 }, // execute part 2 with saved code & args, if required
		};

		void tick_fetch() {
			Utils::log_line("CpuRunner.tick_fetch");
			_cpu.set_zero(_regs.PipelineState);
			_cpu.set_zero(_regs.ArgumentMode);
			_cpu.set_zero(_regs.CommandCode);
			_cpu.set_zero(_regs.Arg1);
			_cpu.set_zero(_regs.Arg2);
			auto ip = _cpu[_regs.IP];
			WReference command(ip.to_ulong());
			_logics.request_ram_read(command);
			_logics.inc_register(PSReference(_regs.PipelineState));
		}

		void tick_decode() {
			Utils::log_line("CpuRunner.tick_decode");
			auto code = _data[WReference(0)];
			_cpu.set_bits(WReference(_regs.CommandCode), code);
			if (auto [has_handler, handler] = get_cur_handler(); has_handler) {
				auto args = handler.Arguments;
				if (args == 0) {
					set_next_step(0b100); // execute
				}
				else {
					set_next_step(0b010, args > 1); // read #1
					auto ip = _cpu[_regs.IP];
					_logics.request_ram_read(ip.to_ulong() + 1);
				}
			} else {
				raise_fatal();
			}
		}

		void tick_read_1() {
			Utils::log_line("CpuRunner.tick_read_1");
			auto arg1 = _logics.read_data_bus();
			Utils::log_line("CpuRunner.tick_read_1: x = ", arg1);
			_cpu.set_bits(WReference(_regs.Arg1), arg1);
			if (_cpu[_regs.ArgumentMode].test(0)) {
				set_next_step(0b011); // read #2
				auto ip = _cpu[_regs.IP];
				_logics.request_ram_read(ip.to_ulong() + 2);
			} else {
				set_next_step(0b100); // execute
			}
		}

		void tick_read_2() {
			Utils::log_line("CpuRunner.tick_read_2");
			auto arg2 = _logics.read_data_bus();
			Utils::log_line("CpuRunner.tick_read_2: y = ", arg2);
			_cpu.set_bits(WReference(_regs.Arg2), arg2);
			_logics.inc_register(PSReference(_regs.PipelineState)); // execute
		}

		void tick_execute_1() {
			Utils::log_line("CpuRunner.tick_execute_1");
			if (auto [has_handler, handler] = get_cur_handler(); has_handler) {
				auto[x, y] = read_args();
				auto is_done = handler.Func(_commands, 0, x, y);
				if (!is_terminated()) {
					if (is_done) {
						set_next_operation(1 + handler.Arguments);
					} else {
						_logics.inc_register(PSReference(_regs.PipelineState)); // execute 2
					}
				}
			} else {
				raise_fatal();
			}
		}
		
		void tick_execute_2() {
			Utils::log_line("CpuRunner.tick_execute_2");
			if (auto [has_handler, handler] = get_cur_handler(); has_handler) {
				auto[x, y] = read_args();
				handler.Func(_commands, 1, x, y);
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
			_cpu.set_bits(PSReference(_regs.PipelineState), BitUtils::get_set<3>(step));
		}

		void set_next_step(int step, bool two_args) {
			Utils::log_line("CpuRunner.set_next_step(", step, ", ", two_args, ")");
			set_next_step(step);
			_cpu.set_bits(FReference(_regs.ArgumentMode), BitUtils::get_flag(two_args));
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
		
		auto get_cur_handler() {
			auto command_code = _cpu[_regs.CommandCode];
			auto handler = _commands.get_handler(command_code);
			return handler;
		}

		bool is_terminated() {
			return _cpu[_regs.Terminated].test(0);
		}

		void raise_fatal() {
			Utils::log_line("CpuRunner.raise_fatal");
			_cpu.set_bits(_regs.Fatal,      BitUtils::get_flag(true));
			_cpu.set_bits(_regs.Terminated, BitUtils::get_flag(true));
		}

		void inc_counter() {
			Utils::log_line("CpuRunner.inc_counter");
			_logics.inc_register(_regs.Counter);
		}

		void bump_ip(int size) {
			Utils::log_line("CpuRunner.bump_ip(", size, ")");
			auto overflow = _logics.add_to_register(_regs.IP, BitUtils::get_set(Architecture::WORD_SIZE * size));
			if (overflow) {
				raise_fatal();
			}
		}

		void set_next_operation(int size) {
			Utils::log_line("CpuRunner.set_next_operation(", size, ")");
			inc_counter();
			bump_ip(size);
		}

		auto read_args() {
			auto arg1 = _cpu[_regs.Arg1];
			auto arg2 = _cpu[_regs.Arg2];
			return tuple { arg1, arg2 };
		}
	};
}
