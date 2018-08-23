#pragma once

#include <string>
#include <ostream>

#include "Architecture.h"

using std::string;
using std::ostream;

namespace Core {
	template<size_t SZ>
	class Reference {
	public:
		const size_t Address;
		const size_t Size = SZ;
		const string FriendlyName;

		Reference(size_t address = 0, const string& name = ""): Address(address), FriendlyName(name) {}
	};
	
	using FReference  = Reference<1>; // Flag reference
	using PSReference = Reference<3>; // Pipeline state reference
	using WReference  = Reference<Architecture::WORD_SIZE>;
	using CBReference = Reference<Architecture::CONTROL_BUS_SIZE>;
	
	template<size_t SZ>
	ostream& operator <<(ostream& os, const Reference<SZ>& ref) {
		os << ref.Address << ":" << SZ;
		if ( !ref.FriendlyName.empty() ) {
			os << " (" << ref.FriendlyName << ")";
		}
		return os;
	}
	
	template<size_t SZ>
	bool operator <(const Reference<SZ>& lhs, const Reference<SZ>& rhs) {
		return lhs.Address < rhs.Address;
	}
	
	template<size_t SZ>
	bool operator <(const size_t lhs, const Reference<SZ>& rhs) {
		return lhs < rhs.Address;
	}
	
	template<size_t SZ>
	bool operator !=(const Reference<SZ>& lhs, const Reference<SZ>& rhs) {
		return lhs.Address != rhs.Address;
	}
}
