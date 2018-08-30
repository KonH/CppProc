#pragma once

#include <bitset>
#include <string>

#include "Logger.h"
#include "BitUtils.h"
#include "Reference.h"
#include "Architecture.h"

using std::bitset;
using std::string_view;

using Utils::LogType;
using Core::Reference;
using Architecture::WordSet;
using Architecture::WORD_SIZE;

namespace State {
	template<size_t MS>
	class MemoryState {
		static_assert(MS > 0);
	public:
		MemoryState(string name): _name(name) { }

		MemoryState(string name, WordSet<MS> init_memory) :_name(name) {
			for ( size_t i = 0; i < MS; i++ ) {
				for ( size_t j = 0; j < Architecture::WORD_SIZE; j++ ) {
					_memory[i * Architecture::WORD_SIZE + j] = init_memory[i][j];
				}
			}
		}

		auto get_all() const {
			return _memory;
		}

		template<size_t SZ>
		void set_bits(Reference<SZ> ref, const bitset<SZ>& value) {
			static_assert(SZ <= MS * WORD_SIZE);
			BitUtils::set_bits<SZ, MS * WORD_SIZE>(_memory, ref.Address, value);
			Utils::log_line(LogType::MemoryState, _name, ": W > ", ref, " = ", value);
		}
		
		template<size_t SZ>
		void set_zero(Reference<SZ> ref) {
			set_bits(ref, BitUtils::get_zero<SZ>());
		}
		
		template<size_t SZ>
		auto operator[](Reference<SZ> ref) const {
			return get_bits(ref);
		}

	private:
		const string                         _name;
		bitset<MS * Architecture::WORD_SIZE> _memory = { 0 };
		
		template<size_t SZ>
		auto get_bits(Reference<SZ> ref) const {
			static_assert(SZ <= MS * WORD_SIZE);
			auto result = BitUtils::get_bits<SZ, MS * WORD_SIZE>(_memory, ref.Address);
			Utils::log_line(LogType::MemoryState, _name, ": R < ", ref, " = ", result);
			return result;
		}
	};
	
	using ControlBusState = MemoryState<1>;
	using AddressBusState = MemoryState<1>;
	using DataBusState    = MemoryState<1>;
}
