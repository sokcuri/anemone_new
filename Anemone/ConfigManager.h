#pragma once
#include <unordered_map>
#include <vector>
struct testy; // forward declaration

class proxy
{
private:
	testy & _t;
	std::string _key;
public:
	proxy(testy& t, std::string key) : _t(t), _key(key) {}
	proxy& operator=(std::string val);
	operator std::string() const;
	std::string data();
};

struct testy {
	std::unordered_map<std::string, std::string> data;
	std::string operator[] (std::string key) const { return data.find(key)->second; }
	proxy operator[](std::string key) { return proxy(*this, key); }
};


class ConfigManager
{
public:
	std::unordered_map<std::string, std::string> data;
	ConfigManager();
	~ConfigManager();
	void Load();
	void operator[](std::string field);
	
};

