#include "stdafx.h"
#include "ConfigManager.h"


ConfigManager::ConfigManager()
{
}


ConfigManager::~ConfigManager()
{
}

void ConfigManager::Load()
{

}

void ConfigManager::operator[](std::string field)
{
}

proxy& proxy::operator=(std::string val)
{
	_t.data[_key] = val;
	return *this;
}

proxy::operator std::string() const
{
	return _t.data[_key];
}

std::string proxy::data()
{
	return _t.data[_key];
}