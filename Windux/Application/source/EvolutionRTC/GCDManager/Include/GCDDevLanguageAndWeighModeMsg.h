#ifndef GCDLANGUAGEANDWEIGHMODEMSG_H
#define GCDLANGUAGEANDWEIGHMODEMSG_H

#include "EvolMessage.h"
#include "HobString.h"
#include "MessageFormat.h"
#include "Weigher.h"
#include "Configuration.h"
#include <map>

class GCDDevLanguageAndWeighModeMsg :	public EvolMessage
{
public:
	GCDDevLanguageAndWeighModeMsg(GcdLanguage gcdLanguage, WeighMode weighMode);

	GCDDevLanguageAndWeighModeMsg(WeighMode weighMode);

	GCDDevLanguageAndWeighModeMsg(GcdLanguage gcdLanguage);

	UINT32 getSerializedLength() const;

	const char *serialize() const;

private:

	static const std::string& getLanguageStr(GcdLanguage gcdLanguage);

	static const std::string& getWeighModeStr(WeighMode weighMode);

	static std::map< GcdLanguage, std::string > s_gcdLanguageMap;
	static std::map< WeighMode, std::string > s_weighModeMap;
};
#endif