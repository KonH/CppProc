#pragma once

namespace Core {
	template<int TSize>
	class Reference {
	public:
		const unsigned long Address;
		const unsigned long Size = TSize;

		Reference(unsigned long address) :Address(address) {}
	};
}