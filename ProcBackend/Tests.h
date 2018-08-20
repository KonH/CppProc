#pragma once

#include <tuple>
#include <bitset>

#include "BitUtils.h"
#include "Computer.h"
#include "Reference.h"
#include "TestRunner.h"

using std::tuple;
using std::bitset;

using Core::Computer;
using Core::Reference;
using TestUtils::TestRunner;
using TestUtils::assert;
using TestUtils::assert_equal;

namespace Tests {
	
	void state_overflow_always_saved() {
		auto cmp = Computer<4, 32, 8>(0b0000'0000); // NOOP
		cmp.State.CPU.set(cmp.Registers.Overflow, BitUtils::get_one<1>());
		auto before = cmp.State.CPU.get(cmp.Registers.Overflow);
		assert_equal(before, BitUtils::get_one<1>());
		cmp.tick(3); // fetch, decode, execute
		auto after = cmp.State.CPU.get(cmp.Registers.Overflow);
		assert_equal(after, BitUtils::get_zero<1>());
	}

	void state_ram_readed() {
		auto ram = bitset<4>(0b1111);
		auto cmp = Computer<4, 32, 4>(ram);
		auto before = cmp.State.DataBus.get(Reference<4>(0));
		assert_equal(before, BitUtils::get_zero<4>());
		cmp.State.ControlBus.set(Reference<2>(0), bitset<2>(0b01));
		cmp.tick_ram();
		auto after = cmp.State.DataBus.get(Reference<4>(0));
		assert_equal(after, ram);
	}

	void state_ram_writed() {
		auto cmp = Computer<4, 32, 4>(0b0000);
		auto data = bitset<4>(0b1111);
		auto before = cmp.State.RAM.get(Reference<4>(0));
		assert_equal(before, BitUtils::get_zero<4>());
		cmp.State.ControlBus.set(Reference<2>(0), bitset<2>(0b11));
		cmp.State.DataBus.set(Reference<4>(0), data);
		cmp.tick_ram();
		auto after = cmp.State.RAM.get(Reference<4>(0));
		assert_equal(after, data);
	}

	void command_unknown() {
		auto cmp = Computer<4, 32, 4>(0b1111);
		cmp.tick(2); // fetch, decode
		auto fatal = cmp.State.CPU.get(cmp.Registers.Fatal);
		assert_equal(fatal, 0b1);
	}

	void command_NOOP() {
		auto cmp = Computer<4, 32, 4>(0b0000);
		cmp.tick(3); // fetch, decode, execute
		auto counter = cmp.State.CPU.get(cmp.Registers.Counter);
		assert_equal(counter, 0b1);
		auto ip = cmp.State.CPU.get(cmp.Registers.IP);
		assert_equal(ip, 4);
		auto overflow = cmp.State.CPU.get(cmp.Registers.Overflow);
		assert_equal(overflow, 0b0);
	}

	void command_RST() {
		auto cmp = Computer<4, 32, 8>(0b0000'0001);
		auto t1_performing = cmp.tick(2); // fetch, decode
		assert(t1_performing, "performing");
		auto t2_terminated = !cmp.tick(1); // execute
		assert(t2_terminated, "terminated");
		assert_equal(cmp.State.CPU.get(cmp.Registers.Terminated), 0b1, "flag is set");
	}

	void command_CLR() {
		auto cmp = Computer<4, 32 + 4, 8>(0b0000'0010);
		auto c0 = cmp.Registers.get_CN(0);
		cmp.State.CPU.set(c0, BitUtils::get_one<4>());
		auto before = cmp.State.CPU.get(c0);
		assert_equal(before, BitUtils::get_one<4>());
		cmp.tick(4); // fetch, decode, read 1, execute
		auto after = cmp.State.CPU.get(c0);
		assert_equal(after, BitUtils::get_zero<4>());
	}

	void command_INC() {
		auto cmp = Computer<4, 32 + 4, 8>(0b0000'0011);
		auto c0 = cmp.Registers.get_CN(0);
		cmp.State.CPU.set(c0, BitUtils::get_zero<4>());
		auto before = cmp.State.CPU.get(c0);
		assert_equal(before, BitUtils::get_zero<4>());
		cmp.tick(4); // fetch, decode, read 1, execute
		auto after = cmp.State.CPU.get(c0);
		assert_equal(after, BitUtils::get_one<4>());
	}

	void command_SUM() {
		auto cmp = Computer<4, 32 + 4 * 2, 12>(0b0001'0000'0100);
		auto c0 = cmp.Registers.get_CN(0);
		auto c1 = cmp.Registers.get_CN(1);
		cmp.State.CPU.set(c0, BitUtils::get_one<4>());
		cmp.State.CPU.set(c1, BitUtils::get_set<4>(2));
		auto ar_before = cmp.State.CPU.get(cmp.Registers.AR);
		assert_equal(ar_before, BitUtils::get_zero<4>());
		cmp.tick(5); // fetch, decode, read 1, read 2, execute
		auto ar_after = cmp.State.CPU.get(cmp.Registers.AR);
		assert_equal(ar_after, BitUtils::get_set<4>(3));
	}

	void command_MOV() {
		// MOV y x => move r[x] value to r[y]
		// x = 0, y = 1
		// MOV 0001 0000
		auto cmp = Computer<4, 32 + 4 * 2, 12>(0b0001'0000'0101);
		auto c0 = cmp.Registers.get_CN(0);
		auto c1 = cmp.Registers.get_CN(1);
		cmp.State.CPU.set(c0, BitUtils::get_one<4>());
		auto c1_before = cmp.State.CPU.get(c1);
		assert_equal(c1_before, BitUtils::get_zero<4>());
		cmp.tick(5); // fetch, decode, read 1, read 2, execute
		auto c1_after = cmp.State.CPU.get(c1);
		assert_equal(c1_after, BitUtils::get_one<4>());
	}

	void test_state() {
		TestRunner tr("state");
		tr.run_test(state_overflow_always_saved, "state_overflow_always_saved");
		tr.run_test(state_ram_readed, "state_ram_readed");
		tr.run_test(state_ram_writed, "state_ram_writed");
	}

	void test_commands() {
		TestRunner tr("commands");
		tr.run_test(command_unknown, "command_unknown");
		tr.run_test(command_NOOP, "NOOP");
		tr.run_test(command_RST, "RST");
		tr.run_test(command_CLR, "CLR");
		tr.run_test(command_INC, "INC");
		tr.run_test(command_SUM, "SUM");
		tr.run_test(command_MOV, "MOV");
	}

	void test_all() {
		test_utils();
		test_state();
		test_commands();
	};
};
