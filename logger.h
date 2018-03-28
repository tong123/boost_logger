#pragma once

#include <iostream>

class Logger {
public:
	enum Severity_Level
	{
		DEBUG = 0,
		INFO,
		EVENT,
		ALARM,
		ERROR,
	};
	Logger();
	~Logger();
	static void initial();
	static void log_write(Severity_Level level, std::string func_name, std::string os);
};
