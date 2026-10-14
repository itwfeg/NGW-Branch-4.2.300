/* Fastrack Scale Project
 * COPYRIGHT (c) 2013.  All Rights Reserved.
 * Hobart, A division of ITW Food Equipment Group LLC. Troy, Ohio USA
 */

#ifndef PYTHONSCRIPTER_H
#define PYTHONSCRIPTER_H

#include "CSmartPtr.h"
#include "CCriticalSection.h"
#include "Configuration.h"


class PythonScripter
{
	public:
		//static std::string buildPythonCall(const std::string &scriptName);
		static std::string buildPythonCall(const std::string &scriptName, int port = -1, const std::string &cmd = string(), const std::string &ipAddressOrHostName = string("localhost"));
		static std::string buildPythonCallUpdateSecondaryScale(const std::string &scriptName, int port = -1, const std::string &cmd = string(), const std::string &server = "localhost");

		//static PythonScripter *getInstance() { if (instance == 0) instance = new PythonScripter(); return instance; }

	private:
		PythonScripter() {}
		//static PythonScripter* instance;

		static Util::CSmartCriticalSection pythonScripterCS;		
};

#endif  //PYTHONSCRIPTER_H
