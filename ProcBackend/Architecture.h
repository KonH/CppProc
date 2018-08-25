#pragma once

#include <array>
#include <bitset>

using std::array;
using std::bitset;

namespace Architecture {
	const size_t WORD_SIZE        = 8;
	const size_t CONTROL_BUS_SIZE = 2;
	const size_t ADDR_BUS_SIZE    = WORD_SIZE;
	const size_t DATA_BUS_SIZE    = WORD_SIZE;
	
	static constexpr size_t SERVICE_REGISTERS = 8;
	static constexpr size_t MIN_MEMORY_SIZE   = SERVICE_REGISTERS;
	
	using Flag = bitset<1>;
	using Word = bitset<WORD_SIZE>;
	
	template<size_t Size>
	using WordSet = array<Word, Size>;
}
