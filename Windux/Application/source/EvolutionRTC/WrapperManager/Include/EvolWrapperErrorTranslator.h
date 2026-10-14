#ifndef EVOLWRAPPERERRORTRANSLATOR
#define EVOLWRAPPERERRORTRANSLATOR
#include "EvolWrapperErrorInfo.h"
#include <string.h>
#include <map>

class EvolWrapperErrorTranslator
{
	public:
		//make it a singleton
		static EvolWrapperErrorTranslator *getInstance();
		
		~EvolWrapperErrorTranslator();

		const std::string translateEvolWrapperError(UINT16 wrapperError);
	private:
		static EvolWrapperErrorTranslator *instance;
		EvolWrapperErrorTranslator();

		std::map<EvolWrapperErrorInfo::EvolWrapperError, const std::string> errorTranslator;

#ifdef USE_ACCOUNTANT
        static int dataType;
        static std::string dataTypeName;
#endif
};

#endif