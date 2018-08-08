#pragma once

template<int TSize>
class Reference {
public:
	const int Address;
	const int Size = TSize;

	Reference(int address):Address(address) { }
};