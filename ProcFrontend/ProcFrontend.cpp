#include <bitset>
#include <fstream>
#include "stdafx.h"
#include <iostream>

#include "Tests.h"
#include "RegisterSet.h"
#include "ComputerState.h"

#include "View.h"

const int BaseSize           = 4;
const int InternalMemorySize = 32;
const int RamMemorySize      = 64;

using std::cin;
using std::cout;
using std::bitset;
using std::ifstream;

using Core::Computer;

bitset<RamMemorySize> read_ram() {
	bitset<RamMemorySize> set;

	auto path = "../raw_mem.txt";
	cout << "Try to read memory from file: " << path << endl;
	auto f = ifstream(path, std::ios::binary | std::ios::in);
	if (f.is_open()) {
		cout << "File is opened." << endl;
		int i = 0;
		char c;
		while (f.get(c)) {
			if (i >= RamMemorySize) {
				break;
			}
			if (c == '1') {
				set.set(i);
				i++;
			} else if (c == '0') {
				i++;
			}
		}
		cout << "End of file." << endl;
		cout << endl;
		cout << "Readed memory: " << endl;
		View::print_memory<4, RamMemorySize>(set, 4);
		cout << endl;
		f.close();
	} else {
		cout << "Can't open file." << endl;
	}
	return set;
}

void run_tests() {
	cerr << "Run tests:" << endl;
	Tests::test_all();
	cerr << endl;
}

int main(int argc, char* argv[]) {
	auto is_test_only_mode = false;
	if (argc > 1) {
		string arg = argv[1];
		is_test_only_mode = (arg == "test_only_mode");
	}

	cout << "=== CppProc ===" << endl;
	if (is_test_only_mode) {
		cout << "Test Only Mode" << endl;
		Utils::enable_log();
	}
	cout << endl;

	run_tests();

	if (is_test_only_mode) {
		return 0;
	}

	auto ram_mem = read_ram();
	auto comp = Computer<BaseSize, InternalMemorySize, RamMemorySize>(ram_mem);

	cout << "Start execution..." << endl;
	cout << endl;

	auto running = true;
	while (running) {
		cout << "State:" << endl;
		View::print_state(comp);

		cout << endl;
		cout << "Operations:" << endl;
		Utils::enable_log();
		running = comp.tick();
		Utils::disable_log();
		cout << endl;

		if (!running) {
			cout << endl << "Execution done." << endl;
		}
		cin.get();
	}
	return 0;
}