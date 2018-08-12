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

		template<int BitSize>
		bitset<BitSize> get(Reference<BitSize> ref) const {
			auto result = BitUtils::get_bits<BitSize, MS>(_memory, ref.Address);
			Utils::log_line(_name, ": R < ", ref.Address, ":", BitSize, " = ", result);
			return result;
		}

		template<int Address, int BitSize>
		void set(bitset<BitSize> value) {
			BitUtils::set_bits<Address, BitSize, MS>(_memory, value);
			Utils::log_line(_name, ": W > ", Address, ":", BitSize, " = ", value);
		}

		template<int BitSize>
		void set(Reference<BitSize> ref, bitset<BitSize> value) {
			BitUtils::set_bits<BitSize, MS>(_memory, ref.Address, value);
			Utils::log_line(_name, ": W > ", ref.Address, ":", BitSize, " = ", value);
		}


	private:
		const string _name;
		bitset<MS>   _memory;
	};
}