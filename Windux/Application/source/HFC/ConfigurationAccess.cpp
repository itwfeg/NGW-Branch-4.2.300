/* HT Series Scale Project
 * COPYRIGHT (c) 2017.  All Rights Reserved.
 * ITWFEG - Hobart. Troy, Ohio  USA
 *
 *
 *  Provides access to Configuration parameters for classes that historically never accessed Configuration.
 *  This allows those classes (EvolutionRTC, DB for example) easy access without including the world.
 */

#include "ConfigurationAccess.h"
#include "Configuration.h"
#include <string>

ConfigurationAccess *ConfigurationAccess::instance = 0;

std::string ConfigurationAccess::toStringCfg(int fieldCode) const
	{ return cfg->toStringCfg(fieldCode); }
int ConfigurationAccess::toIntCfg(int fieldCode) const
	{ return cfg->toIntCfg(fieldCode); }
bool ConfigurationAccess::toBoolCfg(int fieldCode) const
	{ return cfg->toBoolCfg(fieldCode); }

std::string ConfigurationAccess::toStringCfgI(const std::string& name) const
	{ return cfg->toStringCfgI(name); }
int ConfigurationAccess::toIntCfgI(const std::string name) const
	{ return cfg->toIntCfgI(name); }
bool ConfigurationAccess::toBoolCfgI(const std::string name) const
	{ return cfg->toBoolCfgI(name); }

std::string ConfigurationAccess::toStringCfgI(int fieldCode) const
	{ return cfg->toStringCfgI(fieldCode); }
int ConfigurationAccess::toIntCfgI(int fieldCode) const
	{ return cfg->toIntCfgI(fieldCode); }
bool ConfigurationAccess::toBoolCfgI(int fieldCode) const
	{ return cfg->toBoolCfgI(fieldCode); }

ConfigurationAccess *ConfigurationAccess::getInstance()
{
	if (instance == 0)
		instance = new ConfigurationAccess();
	return instance;
}

ConfigurationAccess::ConfigurationAccess()
{
	cfg = Configuration::getInstance();
}

ConfigurationAccess::~ConfigurationAccess()
{
}	
