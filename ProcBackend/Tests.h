#pragma once

#include <tuple>
#include <bitset>

#include "BitUtils.h"
#include "Computer.h"
#include "TestRunner.h"

using std::tuple;
using std::bitset;

using Core::Computer;
using TestUtils::TestRunner;
using TestUtils::assert;
using TestUtils::assert_equal;

namespace Tests {
	void bit_plus_ordinary() {
		bitset<1> x;
		bitset<1> y;
		tuple<bitset<1>, bool> actual;
		tuple<bitset<1>, bool> expected;

		x = 0b0; y = 0b0; expected = { 0b0, false };
		assert_equal(BitUtils::plus(x, y), expected, "0 + 0 = 0 (carry = 0)");
		
		x = 0b0; y = 0b1; expected = { 0b1, false };
		assert_equal(BitUtils::plus(x, y), expected, "0 + 1 = 1 (carry = 0)");
		
		x = 0b1; y = 0b0; expected = { 0b1, false };
		assert_equal(BitUtils::plus(x, y), expected, "1 + 0 = 1 (carry = 0)");
		
		x = 0b1; y = 0b1; expected = { 0b0, true  };
		assert_equal(BitUtils::plus(x, y), expected, "1 + 1 = 0 (carry = 1)");
	}

	void bit_plus_advanced() {
		bitset<4> x;
		bitset<4> y;
		tuple<bitset<4>, bool> expected;

		x = 0b0001; y = 0b0010; expected = { 0b0011, false };
		assert_equal(BitUtils::plus(x, y), expected, "0001 + 0010 = 0011 (carry = 0)");

		x = 0b0001; y = 0b0001; expected = { 0b0010, false };
		assert_equal(BitUtils::plus(x, y), expected, "0001 + 0001 = 0010 (carry = 0)");

		x = 0b0011; y = 0b0011; expected = { 0b0110, false };
		assert_equal(BitUtils::plus(x, y), expected, "0011 + 0011 = 0110 (carry = 0)");

		x = 0b0110; y = 0b0011; expected = { 0b1001, false };
		assert_equal(BitUtils::plus(x, y), expected, "0110 + 0011 = 1001 (carry = 0)");

		x = 0b0010; y = 0b0110; expected = { 0b1000, false };
		assert_equal(BitUtils::plus(x, y), expected, "0010 + 0110 = 1000 (carry = 0)");

		x = 0b1000; y = 0b0111; expected = { 0b1111, false };
		assert_equal(BitUtils::plus(x, y), expected, "1000 + 0111 = 1111 (carry = 0)");

		x = 0b1000; y = 0b1000; expected = { 0b0000, true };
		assert_equal(BitUtils::plus(x, y), expected, "1000 + 1000 = 0000 (carry = 1)");

		x = 0b1000; y = 0b1111; expected = { 0b0111, true };
		assert_equal(BitUtils::plus(x, y), expected, "1000 + 1111 = 0111 (carry = 1)");

		x = 0b0001; y = 0b1111; expected = { 0b0000, true };
		assert_equal(BitUtils::plus(x, y), expected, "0001 + 1111 = 0000 (carry = 1)");
	}

	void bit_zero() {
		assert_equal(BitUtils::get_zero<1>(), bitset<1>(0b0));
		assert_equal(BitUtils::get_zero<4>(), bitset<4>(0b0));
	}

	void bit_one() {
		assert_equal(BitUtils::get_one<1>(), bitset<1>(0b1));
		assert_equal(BitUtils::get_one<4>(), bitset<4>(0b1));
	}

	void bit_inc() {
		bitset<4> x;
		tuple<bitset<4>, bool> expected;

		auto one = BitUtils::get_one<4>();

		x = 0b0000; expected = { 0b0001, false };
		assert_equal(BitUtils::plus(x, one), expected, "0000 + 0001 = 0001 (carry = 0)");

		x = 0b0001; expected = { 0b0010, false };
		assert_equal(BitUtils::plus(x, one), expected, "0001 + 0001 = 0010 (carry = 0)");

		x = 0b1111; expected = { 0b0000, true };
		assert_equal(BitUtils::plus(x, one), expected, "1111 + 0001 = 0000 (carry = 1)");
	}

	void state_init() {
		auto ram = bitset<4>(0b0101);
		auto cmp = Computer<4, 16, 4>(ram);
		assert_equal(cmp.State.RAM.get_all(), ram);
	}

	void state_overflow_always_saved() {
		auto cmp = Computer<4, 16, 12>(0b0000); // NOOP
		cmp.State.CPU.set(cmp.Registers.Overflow, BitUtils::get_one<1>());
		auto before = cmp.State.CPU.get(cmp.Registers.Overflow);
		assert_equal(before, BitUtils::get_one<1>());
		cmp.tick();
		auto after = cmp.State.CPU.get(cmp.Registers.Overflow);
		assert_equal(after, BitUtils::get_zero<1>());
	}

	void command_unknown() {
		auto cmp = Computer<4, 16, 12>(0b1111);
		cmp.tick();
		auto fatal = cmp.State.CPU.get(cmp.Registers.Fatal);
		assert_equal(fatal, 0b1);
	}

	void command_NOOP() {
		auto cmp = Computer<4, 16, 12>(0b0000);
		cmp.tick();
		auto counter = cmp.State.CPU.get(cmp.Registers.Counter);
		assert_equal(counter, 0b1);
		auto ip = cmp.State.CPU.get(cmp.Registers.IP);
		assert_equal(ip, 4);
		auto overflow = cmp.State.CPU.get(cmp.Registers.Overflow);
		assert_equal(overflow, 0b0);
	}

	void command_RST() {
		auto cmp = Computer<4, 16, 12>(0b0001);
		auto t1_performing = cmp.tick();
		assert(t1_performing, "performing");
		auto t2_terminated = !cmp.tick();
		assert(t2_terminated, "terminated");
		assert_equal(cmp.State.RAM.get(cmp.Registers.Terminated), 0b1, "flag is set");
	}

	void command_CLR() {
		auto cmp = Computer<4, 16 + 4, 12>(0b0000'0010);
		auto c0 = cmp.Registers.get_CN(0);
		cmp.State.CPU.set(c0, BitUtils::get_one<4>());
		auto before = cmp.State.CPU.get(c0);
		assert_equal(before, BitUtils::get_one<4>());
		cmp.tick();
		auto after = cmp.State.CPU.get(c0);
		assert_equal(after, BitUtils::get_zero<4>());
	}

	void command_INC() {
		auto cmp = Computer<4, 16 + 4, 12>(0b0000'0011);
		auto c0 = cmp.Registers.get_CN(0);
		cmp.State.CPU.set(c0, BitUtils::get_zero<4>());
		auto before = cmp.State.CPU.get(c0);
		assert_equal(before, BitUtils::get_zero<4>());
		cmp.tick();
		auto after = cmp.State.CPU.get(c0);
		assert_equal(after, BitUtils::get_one<4>());
	}

	void command_SUM() {
		auto cmp = Computer<4, 16 + 4 * 2, 12>(0b0001'0000'0100);
		auto c0 = cmp.Registers.get_CN(0);
		auto c1 = cmp.Registers.get_CN(1);
		cmp.State.CPU.set(c0, BitUtils::get_one<4>());
		cmp.State.CPU.set(c1, BitUtils::get_set<4>(2));
		auto ar_before = cmp.State.CPU.get(cmp.Registers.AR);
		assert_equal(ar_before, BitUtils::get_zero<4>());
		cmp.tick();
		auto ar_after = cmp.State.CPU.get(cmp.Registers.AR);
		assert_equal(ar_after, BitUtils::get_set<4>(3));
	}

	void command_MOV() {
		auto cmp = Computer<4, 16 + 4 * 2, 12>(0b0001'0000'0101);
		auto c0 = cmp.Registers.get_CN(0);
		auto c1 = cmp.Registers.get_CN(1);
		cmp.State.CPU.set(c0, BitUtils::get_one<4>());
		auto c1_before = cmp.State.CPU.get(c1);
		assert_equal(c1_before, BitUtils::get_zero<4>());
		cmp.tick();
		auto c1_after = cmp.State.CPU.get(c1);
		assert_equal(c1_after, BitUtils::get_one<4>());
	}

	void test_utils() {
		TestRunner tr("utils");
		tr.run_test(bit_zero, "bit_zero");
		tr.run_test(bit_one, "bit_one");
		tr.run_test(bit_plus_ordinary, "bit_plus_ordinary");
		tr.run_test(bit_plus_advanced, "bit_plus_advanced");
		tr.run_test(bit_inc, "bit_inc");
	}

	void test_state() {
		TestRunner tr("state");
		tr.run_test(state_init, "init");
		tr.run_test(state_overflow_always_saved, "state_overflow_always_saved");
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