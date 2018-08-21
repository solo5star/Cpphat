#ifndef CPPHAT_CONFIGURE_H_
#define CPPHAT_CONFIGURE_H_

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <algorithm>

class Configure {
private:
	const char * const m_file;
	std::map<std::string, std::string> m_properties;
	void Init(); // do once
public:
	static const Configure Ini;

	Configure(const char * _file);

	bool ContainsKey(std::string key);

	std::string GetString(std::string key, std::string defaultValue) const;
	bool GetBool(std::string key, bool defaultValue) const;
	int GetInt(std::string key, int defaultValue) const;

	const std::map<std::string, std::string>& GetAll() const;
};

#endif