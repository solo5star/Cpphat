#include "Logger.hpp"

LogChannel& LogChannel::operator()() {
	if (m_enabled) {
		Logger::GetOutputStream() << "[" << m_logLevel << "] ";
		Logger::GetLogStream() << "[" << m_logLevel << "] ";
	}
	return *this;
}

void Logger::SetOutputStream(std::ostream& _os) {
	os = &_os;
}

std::ostream& Logger::GetOutputStream() {
	return *os;
}

std::ofstream& Logger::GetLogStream() {
	return logos;
}

std::ostream* Logger::os = &std::cout;
// std::ofstream Logger::logos = std::ofstream("C:\\Users\\solo_\\Desktop\\Cpphat\\Debug\\Cpphat.log", std::ios::out | std::ios::ate | std::ios::app);
std::ofstream Logger::logos = std::ofstream("Cpphat.log", std::ios::out | std::ios::ate | std::ios::app);

const char Logger::EOL = '\n';
const char * Logger::BACKSPACE = "\b \b";

LogChannel Logger::Info("INFO");
LogChannel Logger::Warn("WARN");
LogChannel Logger::Error("ERROR");
LogChannel Logger::Debug("DEBUG", Configure::Ini.GetBool("debug", false));
