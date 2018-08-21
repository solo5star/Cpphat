#ifndef CPPHAT_LOGGER_H_
#define CPPHAT_LOGGER_H_

#include <iostream>
#include <fstream>
#include "Configure.hpp"

class LogChannel {
	const char * const m_logLevel;
	bool m_enabled;
public:
	LogChannel(const char * _logLevel, bool _enabled = true)
		: m_logLevel(_logLevel)
		, m_enabled(_enabled)
	{ }

	template <typename T>
	LogChannel& operator<<(T& value);
	template <typename T>
	LogChannel& operator<<(const T& value);
	LogChannel& operator()();
};

template <typename T>
LogChannel& LogChannel::operator<<(T& value) {
	if (m_enabled) {
		Logger::GetOutputStream() << value;
		Logger::GetLogStream() << value;
	}
	return *this;
}

template <typename T>
LogChannel& LogChannel::operator<<(const T& value) {
	if (m_enabled) {
		Logger::GetOutputStream() << value;
		Logger::GetLogStream() << value;
	}
	return *this;
}

class Logger {
	Logger();
	static std::ostream* os;
	static std::ofstream logos;

public:
	static void SetOutputStream(std::ostream& _os);
	static std::ostream& GetOutputStream();
	static std::ofstream& GetLogStream();

	static const char EOL;
	static const char * BACKSPACE;

	static LogChannel Info;
	static LogChannel Warn;
	static LogChannel Error;
	static LogChannel Debug;
};

#endif