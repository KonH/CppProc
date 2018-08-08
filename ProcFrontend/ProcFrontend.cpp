#include <iostream>
#include "stdafx.h"

#include "ComputerState.h"

const int InternalMemorySize = 16;
const int RamMemorySize      = 32;

using namespace std;

using Computer = ComputerState<InternalMemorySize, RamMemorySize>;

template<int Size>
void print_memory(const bitset<Size>& mem, int bytes_in_line) {
	int bytes = 0;
	for (int i = 0; i < Size; i++) {
		cout << int(mem[i]) << ' ';
		bytes++;
		if (bytes >= bytes_in_line) {
			bytes = 0;
			cout << endl;
		}
	}
}

void print_state(const Computer& state) {
	cout << endl << "Internal Memory:" << endl;
	print_memory(state.CPU.get_all(), 8);

	cout << endl << "RAM Memory:" << endl;
	print_memory(state.RAM.get_all(), 8);
}

int main() {
	auto ram_mem = bitset<RamMemorySize>(0b01010110);
	auto comp = Computer(ram_mem);

	comp.CPU.set<0, 1>(0b1);

	print_state(comp);

	int x;
	cin >> x;

	return 0;
}