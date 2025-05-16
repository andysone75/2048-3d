#pragma once

#include <unordered_map>
#include <string>

class Config {
public:
	void initialize();
	inline const std::string& getOption(std::string key) const { return options.at(key); }

private:
	std::unordered_map<std::string, std::string> options;
};