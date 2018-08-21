#pragma once

#include <bitset>
#include <string_view>

#include "Logger.h"
#include "BitUtils.h"
#include "Reference.h"

using std::bitset;
using std::string_view;

using Core::Reference;

namespace State {
	template<size_t MS>
	class MemoryState {
		static_assert(MS > 0);
	public:
		MemoryState(const string_view& name, bitset<MS> init_memory = { 0 }) :_name(name), _memory(init_memory) {}

		bitset<MS> get_all() const {
			return _memory;
		}

		template<size_t SZ>
		auto get_bits(Reference<SZ> ref) const {
			static_assert(SZ <= MS);
			auto result = BitUtils::get_bits<SZ, MS>(_memory, ref.Address);
			Utils::log_line(_name, ": R < ", ref, " = ", result);
			return result;
		}

		template<size_t SZ>
		void set_bits(Reference<SZ> ref, const bitset<SZ>& value) {
			static_assert(SZ <= MS);
			BitUtils::set_bits<SZ, MS>(_memory, ref.Address, value);
			Utils::log_line(_name, ": W > ", ref, " = ", value);
		}
		
		template<size_t SZ>
		void set_zero(Reference<SZ> ref) {
			set_bits(ref, BitUtils::get_zero<SZ>());
		}

	private:
		const string_view& _name;
		bitset<MS>         _memory;
	};
}
