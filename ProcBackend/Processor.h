#pragma once

#include <bitset>

using namespace std;

template<int InternalMemorySize, int RamMemorySize>
class Processor {
public:
	Processor(bitset<RamMemorySize> init_memory):_ram_memory(init_memory) { }
	
	bitset<InternalMemorySize> get_internal_memory() const {
		return _internal_memory;
	}

	bitset<RamMemorySize> get_ram_memory() const {
		return _ram_memory;
	}

private:
	bitset<InternalMemorySize> _internal_memory = { 0 };
	bitset<RamMemorySize> _ram_memory;
};
