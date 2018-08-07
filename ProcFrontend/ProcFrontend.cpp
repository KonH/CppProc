#include "stdafx.h"
#include <iostream>
#include "Processor.h"

const int InternalMemorySize = 4;
const int RamMemorySize = 8;

using namespace std;

using CurProcessor = Processor<InternalMemorySize, RamMemorySize>;


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

void print_state(const CurProcessor& proc) {
	cout << endl << "Internal Memory:" << endl;
	print_memory(proc.get_internal_memory(), 4);

	cout << endl << "RAM Memory:" << endl;
	print_memory(proc.get_ram_memory(), 4);
}

int main() {
	auto mem = bitset<RamMemorySize>(0b01010110);
	CurProcessor proc(mem);

	print_state(proc);

	int x;
	cin >> x;

	return 0;
}