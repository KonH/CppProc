#pragma once

#include <ostream>

using std::ostream;

namespace Core {
	template<size_t SZ>
	class Reference {
	public:
		const size_t Address;
		const size_t Size = SZ;

		Reference(size_t address = 0): Address(address) {}
	};
	
	template<size_t SZ>
	ostream& operator <<(ostream& os, const Reference<SZ>& ref) {
		os << ref.Address << ":" << SZ;
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
