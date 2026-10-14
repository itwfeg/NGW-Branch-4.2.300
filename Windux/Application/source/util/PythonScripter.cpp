/* Fastrack Scale Project
 * COPYRIGHT (c) 2013.  All Rights Reserved.
 * Hobart, A division of ITW Food Equipment Group LLC. Troy, Ohio USA
 */

#include "PythonScripter.h"
#include "Configuration.h"
#include "HobString.h"
#include "../db/util/filesys/filename.h"
#include "../db/util/e_string/utilstring.h"


// static
Util::CSmartCriticalSection PythonScripter::pythonScripterCS;

// static
std::string PythonScripter::buildPythonCall(const std::string &scriptName, int port, const std::string &cmd, const std::string &ipAddressOrHostName)
{
	Util::CLockCriticalSection lock( PythonScripter::pythonScripterCS );

	util_filesystem::Filename scriptFilename;
	scriptFilename.setPath(Configuration::getInstance()->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
	scriptFilename.appendPath(Configuration::getInstance()->toStringCfgI(CfgInternal::PYTHONSCRIPTSDIR));
	scriptFilename.setFileName(scriptName);

	HobString command = scriptFilename.getFullPath();
	command.quote();
	
	if (port != -1)
	{
		if ( port == Configuration::getInstance()->toIntCfgI(CfgInternal::PYTHONSFTPUPDATEPORT) ||
		     port == Configuration::getInstance()->toIntCfgI(CfgInternal::SMBACKENDPORT) )
			command.append(" --newline");
		    command.append(" --timeout");
			command.append(" 7200");
			command.append(" ").append(ipAddressOrHostName).append(" ").append(util_string::inttostring(port));
	}
	if (!cmd.empty())
		command.append(" ").append(cmd);

	if (Debug::isTracing("PythonCall"))
		Debug::trace("PythonCall", std::string("PythonScripter::buildPythonCall - command: ") + std::string("python ") + command);
	return ("python " + command);
}

// static
std::string PythonScripter::buildPythonCallUpdateSecondaryScale(const std::string &scriptName, int port, const std::string &cmd, const std::string &server)
{
	Util::CLockCriticalSection lock( PythonScripter::pythonScripterCS );

	util_filesystem::Filename scriptFilename;
	scriptFilename.setPath(Configuration::getInstance()->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
	scriptFilename.appendPath(Configuration::getInstance()->toStringCfgI(CfgInternal::PYTHONSCRIPTSDIR));
	scriptFilename.setFileName(scriptName);

	HobString command = scriptFilename.getFullPath();
	command.quote();
	
	if (port != -1)
	{
		if ( port == Configuration::getInstance()->toIntCfgI(CfgInternal::PYTHONSFTPUPDATEPORT) ||
		     port == Configuration::getInstance()->toIntCfgI(CfgInternal::SMBACKENDPORT) )
			command.append(" --newline");
		    command.append(" --timeout");
			command.append(" 7200");
	     	command.append(" ").append(server).append(" ").append(util_string::inttostring(port));
	}
	if (!cmd.empty())
		command.append(" ").append(cmd);

	if (Debug::isTracing("PythonCall"))
		Debug::trace("PythonCall", std::string("PythonScripter::buildPythonCall - command: ") + std::string("python ") + command);
	return ("python " + command);
}
