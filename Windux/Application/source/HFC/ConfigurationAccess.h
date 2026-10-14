#ifndef CONFIGURATIONACCESS_H
#define CONFIGURATIONACCESS_H

#include "CfgBaseFieldOffsets.h"
#include <string>
class Configuration;

class ConfigurationAccess
{
	public:
	
		std::string toStringCfg(int fieldCode) const;
		int toIntCfg(int fieldCode) const;
		bool toBoolCfg(int fieldCode) const;

		std::string toStringCfgI(const std::string& name) const;
		int toIntCfgI(const std::string name) const;
		bool toBoolCfgI(const std::string name) const;

		std::string toStringCfgI(int fieldCode) const;
		int toIntCfgI(int fieldCode) const;
		bool toBoolCfgI(int fieldCode) const;

		static ConfigurationAccess *getInstance();

	private:
	
		ConfigurationAccess();
		~ConfigurationAccess();

		static ConfigurationAccess *instance;
		
		Configuration *cfg;
};

#endif //CONFIGURATIONACCESS_H
