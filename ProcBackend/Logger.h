#pragma once

#include <string>
#include <iostream>

using std::cout;
using std::endl;
using std::string;

namespace Utils {
	bool Enabled = false;

	void enable_log() {
		Enabled = true;
	}

	void disable_log() {
		Enabled = false;
	}

	template<class T>
	void log(const T& msg) {
		if (Enabled) {
			cout << " + " << msg;
		}
	}

	template<class ...Args>
	void log_line(Args&&... args) {
		if (Enabled) {
			cout << " + ";
		#ifdef __clang__
			#pragma clang diagnostic push
			#pragma clang diagnostic ignored "-Wunused-value"
		#endif
			(cout << ... << args);
		#ifdef __clang__
			#pragma clang diagnostic pop
		#endif
			cout << endl;
		}
	}
}
