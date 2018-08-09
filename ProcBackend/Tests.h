#pragma once

#include <bitset>

#include "Computer.h"
#include "TestRunner.h"

using std::bitset;

using Core::Computer;
using TestUtils::TestRunner;
using TestUtils::assert;
using TestUtils::assert_equal;

namespace Tests {
	void state_init() {
		auto ram = bitset<4>(0b0101);
		auto cmp = Computer<4, 16, 4>(ram);
		assert_equal(cmp.State.RAM.get_all(), ram);
	}

	void command_RST() {
		auto cmp = Computer<4, 16, 12>(0b0001);
		auto t1_performing = cmp.tick();
		assert(t1_performing);
		auto t2_terminated = !cmp.tick();
		assert(t2_terminated);
		assert_equal(cmp.State.RAM.get(cmp.Registers.Terminated), 0b1);
	}

	void test_state() {
		TestRunner tr("state");
		tr.run_test(state_init, "init");
	}

	void test_commands() {
		TestRunner tr("commands");
		tr.run_test(command_RST, "RST");
	}

	void test_all() {
		test_state();
		test_commands();
	};
};