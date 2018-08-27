#pragma once

#include <array>
#include <bitset>

#include "TestRunner.h"

#include "BitUtils.h"
#include "Reference.h"
#include "CpuLogics.h"
#include "RamRunner.h"
#include "Computer.h"
#include "MemoryState.h"
#include "RegisterSet.h"
#include "Architecture.h"
#include "ComputerState.h"

using std::array;
using std::bitset;

using TestUtils::TestRunner;
using TestUtils::assert_true;
using TestUtils::assert_equal;

using Core::Computer;
using Core::Reference;
using Logics::Command;
using Core::WReference;
using Core::FReference;
using Logics::CpuLogics;
using Logics::RamRunner;
using State::MemoryState;
using Architecture::Word;
using Architecture::Flag;
using State::DataBusState;
using State::ComputerState;
using State::ControlBusState;
using State::AddressBusState;
using Architecture::WORD_SIZE;
using Architecture::RegisterSet;
using Architecture::MIN_MEMORY_SIZE;

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

		void bit_inverse() {
			bitset<1> x1;
			bitset<1> y1;
			
			x1 = 0b0; y1 = 0b1;
			assert_equal(BitUtils::inverse<1>(x1), y1, "0 => 1");
			
			x1 = 0b1; y1 = 0b0;
			assert_equal(BitUtils::inverse<1>(x1), y1, "1 => 0");
			
			bitset<4> x2;
			bitset<4> y2;
			
			x2 = 0b0000; y2 = 0b1111;
			assert_equal(BitUtils::inverse<4>(x2), y2, "0000 => 1111");
			
			x2 = 0b1111; y2 = 0b0000;
			assert_equal(BitUtils::inverse<4>(x2), y2, "1111 => 0000");
			
			x2 = 0b1000; y2 = 0b0111;
			assert_equal(BitUtils::inverse<4>(x2), y2, "1000 => 0111");
			
			x2 = 0b0111; y2 = 0b1000;
			assert_equal(BitUtils::inverse<4>(x2), y2, "0111 => 1000");
			
		}
		
		void bit_minus_ordinary() {
			bitset<1> x;
			bitset<1> y;
			tuple<bitset<1>, bool> actual;
			tuple<bitset<1>, bool> expected;
			
			x = 0b0; y = 0b0; expected = { 0b0, false };
			assert_equal(BitUtils::minus<1>(x, y), expected, "0 - 0 = 0 (carry = 0)");
			
			x = 0b0; y = 0b1; expected = { 0b1, true };
			assert_equal(BitUtils::minus<1>(x, y), expected, "0 - 1 = 1 (carry = 1)");
			
			x = 0b1; y = 0b0; expected = { 0b1, false };
			assert_equal(BitUtils::minus<1>(x, y), expected, "1 - 0 = 1 (carry = 0)");
			
			x = 0b1; y = 0b1; expected = { 0b0, false };
			assert_equal(BitUtils::minus<1>(x, y), expected, "1 - 1 = 0 (carry = 0)");
		}
		
		void bit_minus_advanced() {
			bitset<4> x;
			bitset<4> y;
			tuple<bitset<4>, bool> actual;
			tuple<bitset<4>, bool> expected;
			
			x = 0b1110; y = 0b0111; expected = { 0b0111, false };
			assert_equal(BitUtils::minus<4>(x, y), expected, "1110 (14) - 0111 (7) = 0111 (7) (carry = 0)");
			
			for ( size_t i = 0; i <= 15; i++ ) {
				for ( size_t j = 0; j <= i; j++ ) {
					x = BitUtils::get_set<4>(i);
					y = BitUtils::get_set<4>(j);
					auto actual = BitUtils::minus<4>(x, y);
					auto expected = tuple { BitUtils::get_set<4>(i - j), false };
					stringstream ss;
					ss << std::to_string(i) << " (" << x << ") - " << std::to_string(j) << " (" << y << ") = ";
					ss << (i - j) << " (" << std::get<0>(expected) << ")";
					assert_equal(actual, expected, ss.str());
				}
			}
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
			tr.run_test(bit_inverse, "bit_inverse");
			tr.run_test(bit_minus_ordinary, "bit_minus_ordinary");
			tr.run_test(bit_minus_advanced, "bit_minus_advanced");
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
			auto ms1 = MemoryState<1>("");
			assert_equal(ms1.get_all().to_ulong(), 0);
			
			auto ms2 = MemoryState<1>("", { 10 } );
			assert_equal(ms2.get_all().to_ulong(), 10);
			
			auto ms3 = MemoryState<1>("");
			ms3.set_bits(FReference(), Flag(1));
			assert_true(ms3[FReference()].test(0));
			assert_true(ms3.get_all().test(0));
			
			auto ms4 = MemoryState<1>("");
			ms4.set_bits(FReference(3), Flag(1));
			assert_true(ms4[FReference(3)].test(0));
			assert_true(ms4.get_all().test(3));
			
			RegisterSet<MIN_MEMORY_SIZE> regs;
			auto ms5 = MemoryState<MIN_MEMORY_SIZE>("");
			ms5.set_bits(regs.Overflow, Flag(1));
			assert_true(ms5[regs.Overflow].test(0));
			assert_true(ms5.get_all().test(regs.Overflow.Address));
		}
		
		void computer_state() {
			auto ram = WordSet<1> { 0b0101 };
			auto state = ComputerState<MIN_MEMORY_SIZE, 1>(ram);
			assert_equal(state.RAM.get_all(), ram[0]);
		}
		
		void overflow_always_saved() {
			auto cmp = Computer<MIN_MEMORY_SIZE, 2> ( { 0b0, 0b0 } ); // NOOP
			cmp.State.CPU.set_bits(cmp.Registers.Overflow, BitUtils::get_one<1>());
			auto before = cmp.State.CPU[cmp.Registers.Overflow];
			assert_equal(before, BitUtils::get_flag(1));
			cmp.tick(3); // fetch, decode, execute
			auto after = cmp.State.CPU[cmp.Registers.Overflow];
			assert_equal(after, BitUtils::get_flag(0));
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
			RegisterSet<MIN_MEMORY_SIZE + 2> set;
			assert_true(set.System < set.CommandCode < set.Arg1 < set.Arg2 < set.Flags < set.Counter < set.IP < set.AR, "order");
			assert_true(set.get_CN_count() > 1, "cn_count");
			assert_true(set.AR < set.get_CN(0), "ar < cn");
			assert_true(set.get_CN(0) < set.get_CN(1), "cn0 < cn1");
			assert_equal(set.get_CN(Word(1)), set.get_CN(1), "cn1 == cn1");
		}
		
		void test() {
			TestRunner tr("architecture");
			tr.run_test(register_set, "register_set");
		}
	}
	
	namespace Logics {
		void cpu_logics() {
			RegisterSet<MIN_MEMORY_SIZE + 2> regs;
			MemoryState<MIN_MEMORY_SIZE + 2> cpu("");
			ControlBusState                  control("");
			DataBusState                     data("");
			AddressBusState                  address("");
			
			CpuLogics<MIN_MEMORY_SIZE + 2>   logics(regs, cpu, control, data, address);
			
			assert_true(!cpu[regs.Overflow].test(0));
			logics.set_overflow(true);
			assert_true(cpu[regs.Overflow].test(0));
			
			assert_equal(cpu[regs.get_CN(0)], BitUtils::get_zero());
			logics.add_to_register(regs.get_CN(0), BitUtils::get_one());
			assert_equal(cpu[regs.get_CN(0)], BitUtils::get_one());
			
			assert_equal(cpu[regs.get_CN(1)], BitUtils::get_zero());
			logics.inc_register(regs.get_CN(1));
			assert_equal(cpu[regs.get_CN(1)], BitUtils::get_one());
		}
		
		void ram_runner_read() {
			auto ram = MemoryState<1>("", { 0b1111 } );
			auto db = DataBusState("");
			auto cb = ControlBusState("");
			auto ab = AddressBusState("");
			RamRunner<1> runner(cb, ab, db, ram);
			auto before = db[WReference(0)];
			assert_equal(before, BitUtils::get_zero());
			cb.set_bits(WReference(0), Word(0b01));
			runner.tick();
			auto after = db[WReference(0)];
			assert_equal(after, ram[WReference()]);
		}
		
		void ram_runner_write() {
			auto ram = MemoryState<1>("");
			auto db = DataBusState("");
			auto cb = ControlBusState("");
			auto ab = AddressBusState("");
			RamRunner<1> runner(cb, ab, db, ram);
			auto data = Word(0b1111);
			auto before = ram[WReference(0)];
			assert_equal(before, BitUtils::get_zero());
			cb.set_bits(WReference(0), Word(0b11));
			db.set_bits(WReference(0), data);
			runner.tick();
			auto after = ram[WReference(0)];
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
			auto cmp = Computer<MIN_MEMORY_SIZE, 1>( { 0xFF } );
			cmp.tick(2); // fetch, decode
			auto fatal = cmp.State.CPU[cmp.Registers.Fatal];
			assert_equal(fatal, 0b1);
		}
		
		void NOOP() {
			// 0000
			auto cmp = Computer<MIN_MEMORY_SIZE, 1>( { Command::NOOP } );
			cmp.tick(3); // fetch, decode, execute
			auto counter = cmp.State.CPU[cmp.Registers.Counter];
			assert_equal(counter, 0b1);
			auto ip = cmp.State.CPU[cmp.Registers.IP];
			assert_equal(ip, 0b1);
			auto overflow = cmp.State.CPU[cmp.Registers.Overflow];
			assert_equal(overflow, 0b0);
		}
		
		void RST() {
			// 0001
			auto cmp = Computer<MIN_MEMORY_SIZE, 2>( { Command::RST, 0b0 } );
			auto t1_performing = cmp.tick(2); // fetch, decode
			assert_true(t1_performing, "performing");
			auto t2_terminated = !cmp.tick(1); // execute
			assert_true(t2_terminated, "terminated");
			assert_equal(cmp.State.CPU[cmp.Registers.Terminated], 0b1, "flag is set");
		}
		
		void CLR() {
			// 0010
			auto cmp = Computer<MIN_MEMORY_SIZE + 1, 2>( { Command::CLR, 0b0 } );
			auto c0 = cmp.Registers.get_CN(0);
			cmp.State.CPU.set_bits(c0, BitUtils::get_one());
			auto before = cmp.State.CPU[c0];
			assert_equal(before, BitUtils::get_one());
			cmp.tick(4); // fetch, decode, read 1, execute
			auto after = cmp.State.CPU[c0];
			assert_equal(after, BitUtils::get_zero());
		}
		
		void INC() {
			// 0011
			auto cmp = Computer<MIN_MEMORY_SIZE + 1, 2>( { Command::INC, 0b0 } );
			auto c0 = cmp.Registers.get_CN(0);
			cmp.State.CPU.set_bits(c0, BitUtils::get_zero());
			auto before = cmp.State.CPU[c0];
			assert_equal(before, BitUtils::get_zero());
			cmp.tick(4); // fetch, decode, read 1, execute
			auto after = cmp.State.CPU[c0];
			assert_equal(after, BitUtils::get_one());
		}
		
		void SUM() {
			// 0100
			auto cmp = Computer<MIN_MEMORY_SIZE + 2, 3>( { Command::SUM, 0b0, 0b1 } );
			auto c0 = cmp.Registers.get_CN(0);
			auto c1 = cmp.Registers.get_CN(1);
			cmp.State.CPU.set_bits(c0, BitUtils::get_one());
			cmp.State.CPU.set_bits(c1, BitUtils::get_set(2));
			auto ar_before = cmp.State.CPU[cmp.Registers.AR];
			assert_equal(ar_before, BitUtils::get_zero());
			cmp.tick(5); // fetch, decode, read 1, read 2, execute
			auto ar_after = cmp.State.CPU[cmp.Registers.AR];
			assert_equal(ar_after, BitUtils::get_set(3));
		}
		
		void MOV() {
			// MOV  x    y
			// 0101 0001 0000
			// c[x] = c[1]
			// c[y] = c[0]
			// c[1] => c[0]
			// initial: c[0] = 0, c[1] = 1
			// expected: c[0] = 1
			
			auto cmp = Computer<MIN_MEMORY_SIZE + 2, 3>( { Command::MOV, 0b1, 0b0 } );
			auto c0 = cmp.Registers.get_CN(0);
			auto c1 = cmp.Registers.get_CN(1);
			cmp.State.CPU.set_bits(c1, BitUtils::get_one());
			auto c0_before = cmp.State.CPU[c0];
			assert_equal(c0_before, BitUtils::get_zero());
			cmp.tick(5); // fetch, decode, read 1, read 2, execute
			auto c0_after = cmp.State.CPU[c0];
			assert_equal(c0_after, BitUtils::get_one());
		}
		
		void CLRA() {
			// 0110
			auto cmp = Computer<MIN_MEMORY_SIZE, 1>( { Command::CLRA } );
			auto ar = cmp.Registers.AR;
			cmp.State.CPU.set_bits(ar, BitUtils::get_one());
			
			auto before = cmp.State.CPU[ar];
			assert_equal(before, BitUtils::get_one());
			
			cmp.tick(3); // fetch, decode, execute
			
			auto after = cmp.State.CPU[ar];
			assert_equal(after, BitUtils::get_zero());
		}
		
		void INCA() {
			// 0111
			auto cmp = Computer<MIN_MEMORY_SIZE, 1>( { Command::INCA } );
			auto ar = cmp.Registers.AR;
			
			auto before = cmp.State.CPU[ar];
			assert_equal(before, BitUtils::get_zero());
			
			cmp.tick(3); // fetch, decode, execute
			
			auto after = cmp.State.CPU[ar];
			assert_equal(after, BitUtils::get_one());
		}
		
		void ADDA() {
			// 1000
			auto cmp = Computer<MIN_MEMORY_SIZE + 2, 2>( { Command::ADDA, 0b1 } );
			cmp.State.CPU.set_bits(cmp.Registers.get_CN(1), BitUtils::get_one());
			auto ar = cmp.Registers.AR;
			
			auto before = cmp.State.CPU[ar];
			assert_equal(before, BitUtils::get_zero());
			
			cmp.tick(4); // fetch, decode, read 1, execute
			
			auto after = cmp.State.CPU[ar];
			assert_equal(after, BitUtils::get_one());
		}
		
		void LD() {
			// desc: LD   x    y    mem
			// addr: 0000 0001 0010 0011
			// data: 1001 0011 0001 0110
			// x - address in ram
			// y - register to save
			// expected: read mem at 0011 (0110) to r[1]

			auto cmp = Computer<MIN_MEMORY_SIZE + 2, 4>( { Command::LD, 0b11, 0b1, 0b110 } );
			auto c1 = cmp.Registers.get_CN(1);
			
			auto before = cmp.State.CPU[c1];
			assert_equal(before, BitUtils::get_zero(), "before");
			
			cmp.tick(6); // fetch, decode, read 1, read 2, execute 1, execute 2
			
			auto after = cmp.State.CPU[c1];
			assert_equal(after, Word(0b0110), "after");
		}
		
		void ST() {
			// desc: ST   x    y    mem
			// addr: 0000 0001 0010 0011
			// data: 1010 0001 0011 0000
			// c[1] = 0110
			// expected: write from c[1] to mem at y
			auto cmp = Computer<MIN_MEMORY_SIZE + 2, 4>( { Command::ST, 0b1, 0b11, 0b0 } );
			auto c1 = cmp.Registers.get_CN(1);
			cmp.State.CPU.set_bits(c1, Word(0b0110));
			
			auto mem_at_3 = WReference(WORD_SIZE * 3);
			auto before = cmp.State.RAM[mem_at_3];
			assert_equal(before, BitUtils::get_zero());
			
			cmp.tick(5); // fetch, decode, read 1, read 2, execute
			
			auto not_commited = cmp.State.RAM[mem_at_3];
			assert_equal(not_commited, BitUtils::get_zero());
			
			cmp.tick_ram();
			
			auto after = cmp.State.RAM[mem_at_3];
			assert_equal(after, Word(0b0110));
		}
		
		void SUB() {
			// SUB  x    y
			// 1011 0000 0001
			// r[0] = 1, r[1] = 1
			// expected: r[0] = 0
			
			auto cmp = Computer<MIN_MEMORY_SIZE + 2, 3>( { Command::SUB, 0b0, 0b1 } );
			auto c0 = cmp.Registers.get_CN(0);
			auto c1 = cmp.Registers.get_CN(1);
			
			cmp.State.CPU.set_bits(c0, BitUtils::get_one());
			cmp.State.CPU.set_bits(c1, BitUtils::get_one());
			
			assert_equal(cmp.State.CPU[c0], BitUtils::get_one());
			assert_equal(cmp.State.CPU[c1], BitUtils::get_one());
			
			cmp.tick(5); // fetch, decode, read 1, read 2, execute
			
			assert_equal(cmp.State.CPU[c0], BitUtils::get_zero());
		}
		
		void SUBA() {
			// SUBA x
			// 1100 0001
			// ar = 1, r[1] = 1
			// expected: ar = 0
			auto cmp = Computer<MIN_MEMORY_SIZE + 2, 2>( { Command::SUBA, 0b1 } );
			auto ar = cmp.Registers.AR;
			auto c1 = cmp.Registers.get_CN(1);
			
			cmp.State.CPU.set_bits(ar, BitUtils::get_one());
			cmp.State.CPU.set_bits(c1, BitUtils::get_one());
			
			assert_equal(cmp.State.CPU[ar], BitUtils::get_one());
			assert_equal(cmp.State.CPU[c1], BitUtils::get_one());
			
			cmp.tick(4); // fetch, decode, read 1, execute
			
			assert_equal(cmp.State.CPU[ar], BitUtils::get_zero());
		}
		
		void DEC() {
			// DEC  x
			// 1101 0000
			auto cmp = Computer<MIN_MEMORY_SIZE + 1, 2>( { Command::DEC, 0b0 } );
			auto c0 = cmp.Registers.get_CN(0);
			cmp.State.CPU.set_bits(c0, BitUtils::get_one());
			auto before = cmp.State.CPU[c0];
			assert_equal(before, BitUtils::get_one());
			cmp.tick(4); // fetch, decode, read 1, execute
			auto after = cmp.State.CPU[c0];
			assert_equal(after, BitUtils::get_zero());
		}
		
		void DECA() {
			// DECA
			// 1110
			auto cmp = Computer<MIN_MEMORY_SIZE, 1>( { Command::DECA } );
			auto ar = cmp.Registers.AR;
			cmp.State.CPU.set_bits(ar, BitUtils::get_one());
			
			auto before = cmp.State.CPU[ar];
			assert_equal(before, BitUtils::get_one());
			
			cmp.tick(3); // fetch, decode, execute
			
			auto after = cmp.State.CPU[ar];
			assert_equal(after, BitUtils::get_zero());
		}
		
		void JMP() {
			// JMP  addr
			// 0x0F 0x03
			auto cmp = Computer<MIN_MEMORY_SIZE, 4>({
				// 0x00             // 0x01
				Word(Command::JMP), Word(0x03),
				// 0x02
				Word(Command::RST),
				// 0x03
				Word(Command::NOOP),
			});
			
			assert_equal(cmp.State.CPU[cmp.Registers.IP], Word(0x00), "IP before");
			cmp.tick(4); // jmp: fetch, decode, read 1, execute
			assert_equal(cmp.State.CPU[cmp.Registers.IP], Word(0x03), "IP after");
			cmp.tick(3); // noop: fetch, decode, execute
			assert_equal(cmp.State.CPU[cmp.Registers.Fatal], BitUtils::get_flag(false), "non fatal");
			assert_equal(cmp.State.CPU[cmp.Registers.Terminated], BitUtils::get_flag(false), "non terminated");
		}
		
		void LDA() {
			// desc: LDA   x    mem
			// addr: 0000 0001 0010
			// data: 0x10 0011 0110
			// x - address in ram
			// expected: read mem at 0011 (0110) to AR
			
			auto cmp = Computer<MIN_MEMORY_SIZE + 1, 3>( { Command::LD, 0b11, 0b110 } );
			auto ar = cmp.Registers.AR;
			
			auto before = cmp.State.CPU[ar];
			assert_equal(before, BitUtils::get_zero(), "before");
			
			cmp.tick(5); // fetch, decode, read 1, execute 1, execute 2
			
			auto after = cmp.State.CPU[ar];
			assert_equal(after, Word(0b0110), "after");
		}
		
		void STA() {
			// desc: ST   x    mem
			// addr: 0000 0001 0010
			// data: 0x11 0001 0000
			// ar = 0110
			// expected: write from ar to mem at y
			auto cmp = Computer<MIN_MEMORY_SIZE + 1, 3>( { Command::ST, 0b10, 0b0 } );
			auto ar = cmp.Registers.AR;
			cmp.State.CPU.set_bits(ar, Word(0b0110));
			
			auto mem_at_2 = WReference(WORD_SIZE * 2);
			auto before = cmp.State.RAM[mem_at_2];
			assert_equal(before, BitUtils::get_zero());
			
			cmp.tick(4); // fetch, decode, read 1, execute
			
			auto not_commited = cmp.State.RAM[mem_at_2];
			assert_equal(not_commited, BitUtils::get_zero());
			
			cmp.tick_ram();
			
			auto after = cmp.State.RAM[mem_at_2];
			assert_equal(after, Word(0b0110));
		}
		
		void CMP() {
			// CMP  x    y
			// 0x12 0x00 0x01
			{
				auto cmp = Computer<MIN_MEMORY_SIZE + 2, 3>( { Command::CMP, Word(0x00), Word(0x01) } );
				cmp.State.CPU.set_bits(cmp.Registers.get_CN(0), Word(0x01));
				cmp.State.CPU.set_bits(cmp.Registers.get_CN(1), Word(0x02));
				cmp.tick(5); // fetch, decode, read 1, read 2, execute
				assert_equal(cmp.State.CPU[cmp.Registers.Zero], BitUtils::get_flag(false), "#1");
			}
			{
				auto cmp = Computer<MIN_MEMORY_SIZE + 2, 3>( { Command::CMP, Word(0x00), Word(0x01) } );
				cmp.State.CPU.set_bits(cmp.Registers.get_CN(0), Word(0x03));
				cmp.State.CPU.set_bits(cmp.Registers.get_CN(1), Word(0x03));
				cmp.tick(5); // fetch, decode, read 1, read 2, execute
				assert_equal(cmp.State.CPU[cmp.Registers.Zero], BitUtils::get_flag(true), "#2");
			}
		}
		
		void JZ() {
			{
				// JZ  addr
				// 0x13 0x03
				auto cmp = Computer<MIN_MEMORY_SIZE, 4>({
					// 0x00             // 0x01
					Word(Command::JZ), Word(0x03),
					// 0x02
					Word(Command::RST),
					// 0x03
					Word(Command::NOOP),
				});
				
				assert_equal(cmp.State.CPU[cmp.Registers.IP], Word(0x00), "IP before #1");
				cmp.tick(4); // jz: fetch, decode, read 1, execute
				assert_equal(cmp.State.CPU[cmp.Registers.IP], Word(0x02), "IP after #1");
			}
			{
				// JZ  addr
				// 0x13 0x03
				auto cmp = Computer<MIN_MEMORY_SIZE, 4>({
					// 0x00             // 0x01
					Word(Command::JZ), Word(0x03),
					// 0x02
					Word(Command::RST),
					// 0x03
					Word(Command::NOOP),
				});
				
				cmp.State.CPU.set_bits(cmp.Registers.Zero, BitUtils::get_flag(true));
				
				assert_equal(cmp.State.CPU[cmp.Registers.IP], Word(0x00), "IP before #2");
				cmp.tick(4); // jz: fetch, decode, read 1, execute
				assert_equal(cmp.State.CPU[cmp.Registers.IP], Word(0x03), "IP after #2");
			}
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
			tr.run_test(CLRA, "CLRA");
			tr.run_test(INCA, "INCA");
			tr.run_test(ADDA, "ADDA");
			tr.run_test(LD, "LD");
			tr.run_test(ST, "ST");
			tr.run_test(SUB, "SUB");
			tr.run_test(SUBA, "SUBA");
			tr.run_test(DEC, "DEC");
			tr.run_test(DECA, "DECA");
			tr.run_test(JMP, "JMP");
			tr.run_test(JMP, "LDA");
			tr.run_test(JMP, "STA");
			tr.run_test(CMP, "CMP");
			tr.run_test(JZ, "JZ");
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
