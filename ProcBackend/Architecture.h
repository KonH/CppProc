#pragma once

#include <bitset>

using std::bitset;

namespace Architecture {
	const size_t WORD_SIZE        = 4;
	const size_t CONTROL_BUS_SIZE = 2;
	const size_t ADDR_BUS_SIZE    = WORD_SIZE;
	const size_t DATA_BUS_SIZE    = WORD_SIZE;
	
	static constexpr size_t SERVICE_REGISTERS = 8;
	static constexpr size_t MIN_MEMORY_SIZE   = SERVICE_REGISTERS * WORD_SIZE;
	
	using Flag = bitset<1>;
	using Word = bitset<WORD_SIZE>;
	using CBSet = bitset<CONTROL_BUS_SIZE>; // Control bus set
}
