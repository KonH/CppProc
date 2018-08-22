#pragma once

#include <bitset>

#include "TestRunner.h"

#include "BitUtils.h"
#include "Reference.h"
#include "CpuLogics.h"
#include "RamRunner.h"
#include "Computer.h"
#include "MemoryState.h"
#include "RegisterSet.h"
#include "ComputerState.h"

using std::bitset;

using TestUtils::TestRunner;
using TestUtils::assert_true;
using TestUtils::assert_equal;

using Core::Computer;
using Core::Reference;
using Logics::CpuLogics;
using Logics::RamRunner;
using State::MemoryState;
using State::ComputerState;
using Architecture::RegisterSet;

namespace Tests {
	namespace Common {		
		void assert_check() {
			assert_true(true);
		}
		
		void test() {
			TestRunner tr("common");
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
			assert_true(ms3[Reference<1>()].test(0));
			assert_true(ms3.get_all().test(0));
			
			auto ms4 = MemoryState<4>("");
			ms4.set_bits(Reference<1>(3), bitset<1>(1));
			assert_true(ms4[Reference<1>(3)].test(0));
			assert_true(ms4.get_all().test(3));
			
			RegisterSet<4, 64> regs;
			auto ms5 = MemoryState<64>("");
			ms5.set_bits(regs.Overflow, bitset<1>(1));
			assert_true(ms5[regs.Overflow].test(0));
			assert_true(ms5.get_all().test(regs.Overflow.Address));
		}
		
		void computer_state() {
			auto ram = bitset<4>(0b0101);
			auto state = ComputerState<4, 32, 4>(ram);
			assert_equal(state.RAM.get_all(), ram);
		}
		
		void overflow_always_saved() {
			auto cmp = Computer<4, 32, 8>(0b00000000); // NOOP
			cmp.State.CPU.set_bits(cmp.Registers.Overflow, BitUtils::get_one<1>());
			auto before = cmp.State.CPU[cmp.Registers.Overflow];
			assert_equal(before, BitUtils::get_one<1>());
			cmp.tick(3); // fetch, decode, execute
			auto after = cmp.State.CPU[cmp.Registers.Overflow];
			assert_equal(after, BitUtils::get_zero<1>());
		}
		
		void test() {
			TestRunner tr("state");
			tr.run_test(memory_state, "memory_state");
			tr.run_test(computer_state, "computer_state");
			tr.run_test(overflow_always_saved, "overflow_always_saved");
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
	
	namespace Logics {
		void cpu_logics() {
			RegisterSet<4, 64> regs;
			MemoryState<64> cpu("");
			CpuLogics<4, 64> logics(regs, cpu);
			
			assert_true(!cpu[regs.Overflow].test(0));
			logics.set_overflow(true);
			assert_true(cpu[regs.Overflow].test(0));
			
			assert_equal(cpu[regs.get_CN(0)], BitUtils::get_zero<4>());
			logics.add_to_register(regs.get_CN(0), BitUtils::get_one<4>());
			assert_equal(cpu[regs.get_CN(0)], BitUtils::get_one<4>());
			
			assert_equal(cpu[regs.get_CN(1)], BitUtils::get_zero<4>());
			logics.inc_register(regs.get_CN(1));
			assert_equal(cpu[regs.get_CN(1)], BitUtils::get_one<4>());
		}
		
		void ram_runner_read() {
			auto ram = MemoryState<4>("", bitset<4>(0b1111));
			auto db = MemoryState<4>("");
			auto cb = MemoryState<2>("");
			auto ab = MemoryState<4>("");
			RamRunner<4, 4> runner(cb, ab, db, ram);
			auto before = db[Reference<4>(0)];
			assert_equal(before, BitUtils::get_zero<4>());
			cb.set_bits(Reference<2>(0), bitset<2>(0b01));
			runner.tick();
			auto after = db[Reference<4>(0)];
			assert_equal(after, ram[Reference<4>()]);
		}
		
		void ram_runner_write() {
			auto ram = MemoryState<4>("");
			auto db = MemoryState<4>("");
			auto cb = MemoryState<2>("");
			auto ab = MemoryState<4>("");
			RamRunner<4, 4> runner(cb, ab, db, ram);
			auto data = bitset<4>(0b1111);
			auto before = ram[Reference<4>(0)];
			assert_equal(before, BitUtils::get_zero<4>());
			cb.set_bits(Reference<2>(0), bitset<2>(0b11));
			db.set_bits(Reference<4>(0), data);
			runner.tick();
			auto after = ram[Reference<4>(0)];
			assert_equal(after, data);
		}
		
		void test() {
			TestRunner tr("logics");
			tr.run_test(cpu_logics, "cpu_logics");
			tr.run_test(ram_runner_read, "ram_runner_read");
			tr.run_test(ram_runner_write, "ram_runner_write");
		}
	}
	
	namespace Commands {
		void unknown() {
			auto cmp = Computer<4, 32, 4>(0b1111);
			cmp.tick(2); // fetch, decode
			auto fatal = cmp.State.CPU[cmp.Registers.Fatal];
			assert_equal(fatal, 0b1);
		}
		
		void NOOP() {
			auto cmp = Computer<4, 32, 4>(0b0000);
			cmp.tick(3); // fetch, decode, execute
			auto counter = cmp.State.CPU[cmp.Registers.Counter];
			assert_equal(counter, 0b1);
			auto ip = cmp.State.CPU[cmp.Registers.IP];
			assert_equal(ip, 4);
			auto overflow = cmp.State.CPU[cmp.Registers.Overflow];
			assert_equal(overflow, 0b0);
		}
		
		void RST() {
			auto cmp = Computer<4, 32, 8>(0b00000001);
			auto t1_performing = cmp.tick(2); // fetch, decode
			assert_true(t1_performing, "performing");
			auto t2_terminated = !cmp.tick(1); // execute
			assert_true(t2_terminated, "terminated");
			assert_equal(cmp.State.CPU[cmp.Registers.Terminated], 0b1, "flag is set");
		}
		
		void CLR() {
			auto cmp = Computer<4, 32 + 4, 8>(0b00000010);
			auto c0 = cmp.Registers.get_CN(0);
			cmp.State.CPU.set_bits(c0, BitUtils::get_one<4>());
			auto before = cmp.State.CPU[c0];
			assert_equal(before, BitUtils::get_one<4>());
			cmp.tick(4); // fetch, decode, read 1, execute
			auto after = cmp.State.CPU[c0];
			assert_equal(after, BitUtils::get_zero<4>());
		}
		
		void INC() {
			auto cmp = Computer<4, 32 + 4, 8>(0b00000011);
			auto c0 = cmp.Registers.get_CN(0);
			cmp.State.CPU.set_bits(c0, BitUtils::get_zero<4>());
			auto before = cmp.State.CPU[c0];
			assert_equal(before, BitUtils::get_zero<4>());
			cmp.tick(4); // fetch, decode, read 1, execute
			auto after = cmp.State.CPU[c0];
			assert_equal(after, BitUtils::get_one<4>());
		}
		
		void SUM() {
			auto cmp = Computer<4, 32 + 4 * 2, 12>(0b000100000100);
			auto c0 = cmp.Registers.get_CN(0);
			auto c1 = cmp.Registers.get_CN(1);
			cmp.State.CPU.set_bits(c0, BitUtils::get_one<4>());
			cmp.State.CPU.set_bits(c1, BitUtils::get_set<4>(2));
			auto ar_before = cmp.State.CPU[cmp.Registers.AR];
			assert_equal(ar_before, BitUtils::get_zero<4>());
			cmp.tick(5); // fetch, decode, read 1, read 2, execute
			auto ar_after = cmp.State.CPU[cmp.Registers.AR];
			assert_equal(ar_after, BitUtils::get_set<4>(3));
		}
		
		void MOV() {
			// MOV y x => move r[x] value to r[y]
			// x = 0, y = 1
			// MOV 0001 0000
			auto cmp = Computer<4, 32 + 4 * 2, 12>(0b000000010101);
			auto c0 = cmp.Registers.get_CN(0);
			auto c1 = cmp.Registers.get_CN(1);
			cmp.State.CPU.set_bits(c0, BitUtils::get_one<4>());
			auto c1_before = cmp.State.CPU[c1];
			assert_equal(c1_before, BitUtils::get_zero<4>());
			cmp.tick(5); // fetch, decode, read 1, read 2, execute
			auto c1_after = cmp.State.CPU[c1];
			assert_equal(c1_after, BitUtils::get_one<4>());
		}
		
		void test() {
			TestRunner tr("commands");
			tr.run_test(unknown, "unknown");
			tr.run_test(NOOP, "NOOP");
			tr.run_test(RST, "RST");
			tr.run_test(CLR, "CLR");
			tr.run_test(INC, "INC");
			tr.run_test(SUM, "SUM");
			tr.run_test(MOV, "MOV");
		}
	}
	
	void test_all() {
		Tests::Common::test();
		Tests::Bits::test();
		Tests::Core::test();
		Tests::State::test();
		Tests::Architecture::test();
		Tests::Logics::test();
		Tests::Commands::test();
	}
}
