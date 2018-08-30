#pragma once

#include <set>
#include <string>
#include <iostream>

using std::set;
using std::cout;
using std::endl;
using std::string;

namespace Utils {
	enum class LogType {
		Computer,
		CpuRunner,
		CpuCommands,
		CpuLogics,
		RamRunner,
		MemoryState,
	};
	
	LogType _first_log_type = LogType::Computer;
	LogType _last_log_type  = LogType::MemoryState;
	
	set<LogType> _enabled_types;

	void enable_log(LogType type) {
		_enabled_types.emplace(type);
	}
	
	void enable_all_logs() {
		_enabled_types.clear();
		for (auto i = (int)_first_log_type; i < (int)_last_log_type; i++) {
			enable_log((LogType)i);
		}
	}

	void disable_log() {
		_enabled_types.clear();
	}

	template<class T>
	void log(LogType type, const T& msg) {
		if (_enabled_types.count(type) > 0) {
			cout << " + " << msg;
		}
	}

	template<class ...Args>
	void log_line(LogType type, Args&&... args) {
		if (_enabled_types.count(type) > 0) {
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
