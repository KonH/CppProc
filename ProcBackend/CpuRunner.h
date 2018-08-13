#pragma once

#include <map>
#include <tuple>
#include <bitset>
#include <functional>

#include "Logger.h"
#include "RegisterSet.h"
#include "MemoryState.h"
#include "RegisterSet.h"

using std::map;
using std::tuple;
using std::bitset;
using std::function;

using State::MemoryState;
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
			_regs(regs), _cpu(cpu), _control(control), _address(address), _data(data) {}

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

		map<unsigned long, PipelineStep> _steps = {
			{ 0b000, &CpuRunner::tick_fetch   }, // request command code
			{ 0b001, &CpuRunner::tick_decode  }, // save command code, request arg #1, if required
			{ 0b010, &CpuRunner::tick_read_1  }, // save arg #1, request arg #2, if requred
			{ 0b011, &CpuRunner::tick_read_2  }, // save arg #2
			{ 0b100, &CpuRunner::tick_execute }, // execute with saved code & args
		};

		map<unsigned long, CommandHandler> _commands = {
			{ 0b0000, { 0, &CpuRunner::NOOP } }, // NOOP _ _ => no operation, just bump IP & inc Counter
			{ 0b0001, { 0, &CpuRunner::RST  } }, // RST  _ _ => set Terminated flag
			{ 0b0010, { 1, &CpuRunner::CLR  } }, // CLR  x _ => clear given common register (r[x])
			{ 0b0011, { 1, &CpuRunner::INC  } }, // INC  x _ => inc given common register
			{ 0b0100, { 2, &CpuRunner::SUM  } }, // SUM  y x => r[y] + r[x] will be saved to AC
			{ 0b0101, { 2, &CpuRunner::MOV  } }, // MOV  y x => move r[x] value to r[y]
		};

		void tick_fetch() {
			Utils::log_line("CpuRunner.tick_fetch");
			auto ip = _cpu.get(_regs.IP);
			Reference<BS> command(ip.to_ulong());
			request_ram_read(command);
			inc_register(Reference<3>(_regs.PipelineState));
		}

		void tick_decode() {
			Utils::log_line("CpuRunner.tick_decode");
			auto code = _data.get<BS>(Reference<BS>(0));
			_cpu.set(Reference<BS>(_regs.CommandCode), code);
			auto handler = get_cur_handler();
			auto args = handler.Arguments;
			if (args == 0) {
				set_next_step(0b100); // execute
			} else {
				set_next_step(0b010, args > 1); // read #1
				auto ip = _cpu.get(_regs.IP);
				request_ram_read(ip.to_ulong() + BS);
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
			inc_register(Reference<3>(_regs.PipelineState));
		}

		void tick_execute() {
			Utils::log_line("CpuRunner.tick_execute");
			auto handler = get_cur_handler();
			handler.Func(this);
			_cpu.set(Reference<3>(_regs.PipelineState), BitUtils::get_zero<3> ());
			_cpu.set(Reference<1>(_regs.ArgumentMode),  BitUtils::get_zero<1> ());
			_cpu.set(Reference<BS>(_regs.CommandCode),  BitUtils::get_zero<BS>());
			_cpu.set(Reference<BS>(_regs.Arg1),         BitUtils::get_zero<BS>());
			_cpu.set(Reference<BS>(_regs.Arg2),         BitUtils::get_zero<BS>());
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

		CommandHandler get_handler(const bitset<BS>& command_code) {
			auto code_value = command_code.to_ulong();
			auto cmd_iter = _commands.find(code_value);
			if (cmd_iter != _commands.end()) {
				auto handler = _commands.at(code_value);
				return handler;
			}
			else {
				Utils::log_line("CpuRunner.get_handler: unknown command!");
				raise_fatal();
			}
			return CommandHandler(0, &CpuRunner::NOOP);
		}

		CommandHandler get_cur_handler() {
			auto command_code = _cpu.get(_regs.CommandCode);
			auto handler = get_handler(command_code);
			return handler;
		}

		bool is_terminated() {
			return _cpu.get(_regs.Terminated).test(0);
		}

		void set_overflow(bool value) {
			Utils::log_line("CpuRunner.set_overflow(", value, ")");
			_cpu.set<1>(_regs.Overflow, BitUtils::get_set<1>(value));
		}

		template<int Size>
		bool add_to_register(Reference<Size> ref, bitset<Size> value) {
			Utils::log_line("CpuRunner.add_to_register(", ref.Address, ":", ref.Size, ", ", value, ")");
			auto old_value = _cpu.get(ref);
			auto[new_value, overflow] = BitUtils::plus(old_value, value);
			_cpu.set(ref, new_value);
			set_overflow(overflow);
			return overflow;
		}

		template<int Size>
		bool inc_register(Reference<Size> ref) {
			Utils::log_line("CpuRunner.inc_register(", ref.Address, ":", ref.Size, ")");
			return add_to_register(ref, BitUtils::get_one<Size>());
		}

		void raise_fatal() {
			Utils::log_line("CpuRunner.raise_fatal");
			_cpu.set<1>(_regs.Fatal, 0b1);
			_cpu.set<1>(_regs.Terminated, 0b1);
		}

		void inc_counter() {
			Utils::log_line("CpuRunner.inc_counter");
			inc_register(_regs.Counter);
		}

		void bump_ip(int size) {
			Utils::log_line("CpuRunner.bump_ip(", size, ")");
			auto overflow = add_to_register<BS>(_regs.IP, BitUtils::get_set<BS>(BS * size));
			if (overflow) {
				raise_fatal();
			}
		}

		void set_next_operation(int size) {
			Utils::log_line("CpuRunner.set_next_operation(", size, ")");
			inc_counter();
			bump_ip(size);
		}

		bitset<BS> read_arg_1() {
			return _cpu.get(_regs.Arg1);
		}

		bitset<BS> read_arg_2() {
			return _cpu.get(_regs.Arg2);
		}

		tuple<bitset<BS>, bitset<BS>> read_args() {
			return std::make_tuple(read_arg_1(), read_arg_2());
		}

		void NOOP() {
			Utils::log_line("CpuRunner.NOOP");
			set_next_operation(1);
		}

		void RST() {
			Utils::log_line("CpuRunner.RST");
			inc_counter();
			_cpu.set(_regs.Terminated, bitset<1>(0b1));
		}

		void CLR() {
			Utils::log_line("CpuRunner.CLR");
			auto x = read_arg_1();
			_cpu.set(_regs.get_CN(x), BitUtils::get_zero<BS>());
			set_next_operation(2);
		}

		void INC() {
			Utils::log_line("CpuRunner.INC");
			auto x = read_arg_1();
			inc_register<BS>(_regs.get_CN(x));
			set_next_operation(2);
		}

		void SUM() {
			Utils::log_line("CpuRunner.SUM");
			auto [y, x] = read_args();
			auto x_value = _cpu.get<BS>(_regs.get_CN(x));
			auto y_value = _cpu.get<BS>(_regs.get_CN(y));
			auto[result, overflow] = BitUtils::plus(x_value, y_value);
			set_overflow(overflow);
			_cpu.set(_regs.AR, result);
			set_next_operation(3);
		}

		void MOV() {
			Utils::log_line("CpuRunner.MOV");
			auto[y, x] = read_args();
			Utils::log_line("CpuRunner.MOV(x = ", x, ", y = ", y, ")");
			auto y_addr = _regs.get_CN(y);
			auto y_value = _cpu.get<BS>(y_addr);
			Utils::log_line("CpuRunner.MOV(r[", y_addr.Address, ":", y_addr.Size, "] = ", y_value, ") => ");
			auto x_addr = _regs.get_CN(x);
			_cpu.set(x_addr, y_value);
			Utils::log_line("CpuRunner.MOV(r[", x_addr.Address, ":", x_addr.Size, "])");
			set_next_operation(3);
		}
	};
}