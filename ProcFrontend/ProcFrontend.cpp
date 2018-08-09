#include <iostream>
#include "stdafx.h"
#include <bitset>

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

using Core::Computer;

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
	}
	cout << endl;

	run_tests();

	if (is_test_only_mode) {
		return 0;
	}

	auto ram_mem = bitset<RamMemorySize>();
	auto comp = Computer<BaseSize, InternalMemorySize, RamMemorySize>(ram_mem);

	cout << "Start execution..." << endl;
	cout << endl;

	auto running = true;
	while (running) {
		View::print_state(comp);
		running = comp.tick();
		cout << endl;
		cin.get();
	}

	cout << endl << "Execution done." << endl;
	cin.get();
	return 0;
}