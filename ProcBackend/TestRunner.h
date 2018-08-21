#pragma once

#include <set>
#include <map>
#include <tuple>
#include <vector>
#include <sstream>
#include <iostream>
#include <exception>
#include <string_view>

using std::set;
using std::map;
using std::cerr;
using std::endl;

using std::tuple;
using std::vector;
using std::ostream;
using std::iostream;
using std::exception;
using std::string_view;
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
	
	template<class V1, class V2>
	ostream& operator <<(ostream& os, const tuple<V1, V2>& t) {
		const auto& [v1, v2] = t;
		os << "{" << v1 << ", " << v2 << "}";
		return os;
	}
	
	template<class T, class U>
	void assert_equal(const T& actual, const U& expected, const string_view& hint) {
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
	
	void assert_true(bool b, const string_view& hint = "") {
		assert_equal(b, true, hint);
	}
	
	class TestRunner {
	public:
		TestRunner(const string_view& prefix = "") :
		_prefix(prefix) {}
		
		void write_name(const string_view& name) {
			if (!_prefix.empty()) {
				cerr << _prefix << ".";
			}
			cerr << name;
		}
		
		template<class TestFunc>
		void run_test(TestFunc func, const string_view& name) {
			try {
				write_name(name);
				cerr << endl;
				func();
				write_name(name);
				cerr << " OK" << endl << endl;
			}
			catch (runtime_error& e) {
				++_fail_count;
				write_name(name);
				cerr << " fail: " << e.what() << endl << endl;
			}
		}
		
		~TestRunner() {
			if (_fail_count > 0) {
				cerr << _fail_count << " unit tests failed. Terminate." << endl;
				exit(1);
			}
		}
		
	private:
		const string_view _prefix;
		int               _fail_count = 0;
	};
}

