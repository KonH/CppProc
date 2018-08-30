#include <bitset>
#include <fstream>
#include <iostream>

#include "Tests.h"
#include "RegisterSet.h"
#include "ComputerState.h"

#include "View.h"

const size_t InternalMemorySize = 10;
const size_t RamMemorySize      = 16;

using std::cin;
using std::cout;
using std::bitset;
using std::ifstream;

using Core::Computer;
using Architecture::WordSet;

namespace ProcFrontend {
	auto read_ram() {
		WordSet<RamMemorySize> result;
		
		auto path = "../raw_mem.txt";
		cout << "Try to read memory from file: " << path << endl;
		auto f = ifstream(path, std::ios::binary | std::ios::in);
		if (f.is_open()) {
			cout << "File is opened." << endl;
			size_t i = 0;
			size_t j = 0;
			char c;
			auto set = Word { 0 };
			while (f.get(c)) {
				if (i >= RamMemorySize) {
					break;
				}
				if (c == '1') {
					set.set(i);
					j++;
				} else if (c == '0') {
					j++;
				}
				if ( j == Architecture::WORD_SIZE ) {
					result[i] = set;
					set = Word { 0 };
					j = 0;
					i++;
				}
			}
			cout << "End of file." << endl;
			cout << endl;
			f.close();
		} else {
			cout << "Can't open file." << endl;
		}
		return result;
	}

	void run_tests() {
		cerr << "Run tests:" << endl;
		Tests::test_all();
		cerr << endl;
	}

	int start(int argc, char* argv[]) {
		auto is_test_only_mode = false;
		if (argc > 1) {
			string arg = argv[1];
			is_test_only_mode = (arg == "test_only_mode");
		}
		
		cout << "=== CppProc ===" << endl;
		if (is_test_only_mode) {
			cout << "Test Only Mode" << endl;
			Utils::enable_all_logs();
		}
		cout << endl;
		
		run_tests();
		
		if (is_test_only_mode) {
			return 0;
		}
		
		auto ram_mem = read_ram();
		auto comp = Computer<InternalMemorySize, RamMemorySize>(ram_mem);
		
		cout << "Start execution..." << endl;
		cout << endl;
		
		auto running = true;
		while (running) {
			cout << "State:" << endl;
			View::print_state(comp);
			
			cout << endl;
			cout << "Operations:" << endl;
			Utils::enable_all_logs();
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
}

