#pragma once

#include <bitset>
#include <string>

#include "Logger.h"
#include "BitUtils.h"
#include "Reference.h"

using std::bitset;
using std::string;

using Core::Reference;

namespace State {
	template<int MS>
	class MemoryState {
	public:
		MemoryState(const string& name, bitset<MS> init_memory) :_name(name), _memory(init_memory) {}

		bitset<MS> get_all() const {
			return _memory;
		}

		template<int Address, int BitSize>
		bitset<BitSize> get() const {
			auto result = BitUtils::get_bits<Address, BitSize, MS>(_memory);
			Utils::log_line(_name, ": R < ", Address, ":", BitSize, " = ", result, " str: ");
			return result;
		}

		template<int BitSize>
		bitset<BitSize> get(Reference<BitSize> reg) const {
			auto result = BitUtils::get_bits<BitSize, MS>(_memory, reg.Address);
			Utils::log_line(_name, ": R < ", reg.Address, ":", BitSize, " = ", result);
			return result;
		}

		template<int Address, int BitSize>
		void set(bitset<BitSize> value) {
			BitUtils::set_bits<Address, BitSize, MS>(_memory, value);
			Utils::log_line(_name, ": W > ", Address, ":", BitSize, " = ", value);
		}

		template<int BitSize>
		void set(Reference<BitSize> reg, bitset<BitSize> value) {
			BitUtils::set_bits<BitSize, MS>(_memory, reg.Address, value);
			Utils::log_line(_name, ": W > ", reg.Address, ":", BitSize, " = ", value);
		}


	private:
		const string _name;
		bitset<MS>   _memory;
	};
}