#pragma once

#include <set>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <exception>

using std::set;
using std::map;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::ostream;
using std::iostream;
using std::exception;
using std::stringstream;
using std::ostringstream;
using std::runtime_error;

namespace TestUtils {
	template<class T>
	ostream& operator <<(ostream& os, const set<T>& s) {
		os << "{";
		bool first = true;
		for (const auto& x : s) {
			if (!first) {
				os << ", ";
			}
			first = false;
			os << x;
		}
		return os << "}";
	}

	template<class T>
	ostream& operator <<(ostream& os, const vector<T>& s) {
		os << "[";
		bool first = true;
		for (const auto& x : s) {
			if (!first) {
				os << ", ";
			}
			first = false;
			os << x;
		}
		return os << "]";
	}

	template<class K, class V>
	ostream& operator <<(ostream& os, const map<K, V>& m) {
		os << "{";
		bool first = true;
		for (const auto& kv : m) {
			if (!first) {
				os << ", ";
			}
			first = false;
			os << kv.first << ": " << kv.second;
		}
		return os << "}";
	}

	template<class T, class U>
	void assert_equal(const T& actual, const U& expected, const string& hint) {
		if (actual != expected) {
			ostringstream os;
			os << "Assertion failed: " << actual << " != " << expected;
			if (!hint.empty()) {
				os << " hint: " << hint;
			}
			throw runtime_error(os.str());
		}
	}

	template<class T, class U>
	void assert_equal(const T& actual, const U& expected) {
		assert_equal(actual, expected, "");
	}

	void assert(bool b, const string& hint = "") {
		assert_equal(b, true, hint);
	}

	class TestRunner {
	public:
		TestRunner(const string& prefix = "") :
			_prefix(prefix) {}

		template<class TestFunc>
		void run_test(TestFunc func, const string& testName) {
			try {
				func();
				if (!_prefix.empty()) {
					cerr << _prefix << ".";
				}
				cerr << testName << " OK" << endl;
			}
			catch (runtime_error& e) {
				++_fail_count;
				cerr << testName << " fail: " << e.what() << endl;
			}
		}

		~TestRunner() {
			if (_fail_count > 0) {
				cerr << _fail_count << " unit tests failed. Terminate." << endl;
				exit(1);
			}
		}

	private:
		const string _prefix;
		int          _fail_count = 0;
	};
}
