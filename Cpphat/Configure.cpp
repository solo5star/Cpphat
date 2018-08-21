#include "Configure.hpp"

// const Configure Configure::Ini = Configure("C:\\Users\\solo_\\Desktop\\Cpphat\\Debug\\Cpphat.ini");
const Configure Configure::Ini = Configure("Cpphat.ini");

void Configure::Init() {
	std::ifstream ifs;

	ifs.open(m_file, std::ios::in);

	if (ifs.fail()) {
		return;
	}

	// parse ini
	std::string line;
	while (!ifs.eof()) {
		getline(ifs, line);
		std::string key;
		std::string value;
		bool val = false;

		for (size_t i = 0; i < line.length(); i++) {
			if (line[i] == '=') {
				val = true;
				continue;
			}
			(val ? value : key) += line[i];
		}
		m_properties[key] = value;
	}
	ifs.close();
}

Configure::Configure(const char * _file)
	: m_file(_file)
{
	Init();
}

bool Configure::ContainsKey(std::string key) {
	return m_properties.find(key) != m_properties.end();
}

std::string Configure::GetString(std::string key, std::string defaultValue) const {
	if (m_properties.find(key) == m_properties.end()) {
		return defaultValue;
	}
	return m_properties.at(key);
}

bool Configure::GetBool(std::string key, bool defaultValue) const {
	if (m_properties.find(key) == m_properties.end()) {
		return defaultValue;
	}
	std::string value = m_properties.at(key);
	std::transform(value.begin(), value.end(), value.begin(), ::tolower);
	return value == "true";
}

int Configure::GetInt(std::string key, int defaultValue) const {
	if (m_properties.find(key) == m_properties.end()) {
		return defaultValue;
	}
	return std::stoi(m_properties.at(key));
}

const std::map<std::string, std::string>& Configure::GetAll() const {
	return m_properties;
}
