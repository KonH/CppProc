#pragma once

#include <bitset>

#include "BitUtils.h"
#include "Reference.h"
#include "TestRunner.h"
#include "MemoryState.h"
#include "RegisterSet.h"
#include "ComputerState.h"

using std::bitset;

using TestUtils::TestRunner;
using TestUtils::assert_true;
using TestUtils::assert_equal;

using Core::Reference;
using State::MemoryState;
using State::ComputerState;
using Architecture::RegisterSet;

namespace Tests {
	namespace Common {
		void sizes() {
			assert_true(sizeof(size_t) == sizeof(unsigned long));
		}
		
		void assert_check() {
			assert_true(true);
		}
		
		void test() {
			TestRunner tr("common");
			tr.run_test(sizes, "sizes");
			tr.run_test(assert_check, "assert_check");
		}
	}
	
	namespace Bits {
		void bit_order() {
			// rank:  7 6 5 4  3 2 1 0
			// value: 1 0 0 0  0 0 0 1
			bitset<8> set(0b10000001);
			
			assert_equal(BitUtils::get_bits<4, 8>(set, 0), bitset<4>(0b0001));
			assert_equal(BitUtils::get_bits<4, 8>(set, 4), bitset<4>(0b1000));
		}
		
		void set_bits() {
			bitset<8> set(0);
			BitUtils::set_bits(set, 2, bitset<1>(0b1));
			assert_true(set.test(2));
			assert_true(BitUtils::get_bits<1>(set, 2).test(0));
		}
		
		void bit_zero() {
			assert_equal(BitUtils::get_zero<1>(), bitset<1>(0b0));
			assert_equal(BitUtils::get_zero<4>(), bitset<4>(0b0));
		}
		
		void bit_one() {
			assert_equal(BitUtils::get_one<1>(), bitset<1>(0b1));
			assert_equal(BitUtils::get_one<4>(), bitset<4>(0b1));
		}
		
		void bit_plus_ordinary() {
			bitset<1> x;
			bitset<1> y;
			tuple<bitset<1>, bool> actual;
			tuple<bitset<1>, bool> expected;
			  
			x = 0b0; y = 0b0; expected = { 0b0, false };
			assert_equal(BitUtils::plus<1>(x, y), expected, "0 + 0 = 0 (carry = 0)");
			  
			x = 0b0; y = 0b1; expected = { 0b1, false };
			assert_equal(BitUtils::plus<1>(x, y), expected, "0 + 1 = 1 (carry = 0)");
			  
			x = 0b1; y = 0b0; expected = { 0b1, false };
			assert_equal(BitUtils::plus<1>(x, y), expected, "1 + 0 = 1 (carry = 0)");
			  
			x = 0b1; y = 0b1; expected = { 0b0, true  };
			assert_equal(BitUtils::plus<1>(x, y), expected, "1 + 1 = 0 (carry = 1)");
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
		  
		  void bit_inc() {
			  bitset<4> x;
			  tuple<bitset<4>, bool> expected;
			  
			  auto one = BitUtils::get_one<4>();
			  
			  x = 0b0000; expected = { 0b0001, false };
			  assert_equal(BitUtils::plus<4>(x, one), expected, "0000 + 0001 = 0001 (carry = 0)");
			  
			  x = 0b0001; expected = { 0b0010, false };
			  assert_equal(BitUtils::plus<4>(x, one), expected, "0001 + 0001 = 0010 (carry = 0)");
			  
			  x = 0b1111; expected = { 0b0000, true };
			  assert_equal(BitUtils::plus<4>(x, one), expected, "1111 + 0001 = 0000 (carry = 1)");
		  }
		
		void test() {
			TestRunner tr("bit_utils");
			tr.run_test(bit_order, "bit_order");
			tr.run_test(set_bits, "set_bits");
			tr.run_test(bit_zero, "bit_zero");
			tr.run_test(bit_one, "bit_one");
			tr.run_test(bit_plus_ordinary, "bit_plus_ordinary");
			tr.run_test(bit_plus_advanced, "bit_plus_advanced");
			tr.run_test(bit_inc, "bit_inc");
		}
	}
	
	namespace Core {
		void ref() {
			Reference<4> r1;
			assert_equal(r1.Address, 0);

			Reference<4> r2(10);
			assert_equal(r2.Address, 10);
		}
		
		void test() {
			TestRunner tr("core");
			tr.run_test(ref, "ref");
		}
	}
	
	namespace State {
		void memory_state() {
			auto ms1 = MemoryState<4>("");
			assert_equal(ms1.get_all().to_ulong(), 0);
			
			auto ms2 = MemoryState<4>("", 10);
			assert_equal(ms2.get_all().to_ulong(), 10);
			
			auto ms3 = MemoryState<4>("");
			ms3.set_bits(Reference<1>(), bitset<1>(1));
			assert_true(ms3.get_bits(Reference<1>()).test(0));
			assert_true(ms3.get_all().test(0));
			
			auto ms4 = MemoryState<4>("");
			ms4.set_bits(Reference<1>(3), bitset<1>(1));
			assert_true(ms4.get_bits(Reference<1>(3)).test(0));
			assert_true(ms4.get_all().test(3));
		}
		
		void computer_state() {
			auto ram = bitset<4>(0b0101);
			auto state = ComputerState<4, 32, 4>(ram);
			assert_equal(state.RAM.get_all(), ram);
		}
		
		void test() {
			TestRunner tr("state");
			tr.run_test(memory_state, "memory_state");
			tr.run_test(computer_state, "computer_state");
		}
	}
	
	namespace Architecture {
		void register_set() {
			RegisterSet<4, 64> set;
			assert_true(set.System < set.CommandCode < set.Arg1 < set.Arg2 < set.Flags < set.Counter < set.IP < set.AR);
			assert_true(set.get_CN_count() > 1);
			assert_true(set.AR < set.get_CN(0));
			assert_true(set.get_CN(0) < set.get_CN(1));
			assert_equal(set.get_CN(bitset<4>(1)), set.get_CN(1));
		}
		
		void test() {
			TestRunner tr("architecture");
			tr.run_test(register_set, "register_set");
		}
	}
	
	void test_all() {
		Tests::Common::test();
		Tests::Bits::test();
		Tests::Core::test();
		Tests::State::test();
		Tests::Architecture::test();
	}
}
