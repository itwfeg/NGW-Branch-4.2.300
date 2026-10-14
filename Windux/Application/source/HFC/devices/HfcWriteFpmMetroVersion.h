/**
 * @file HfcWriteFpmMetroVersion.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief Fastrack personality module manager message to write metrology version to serial flash
 * Created on: July 20, 2016
 */

#ifndef HFCWRITEFPMMETROVERSION_H
#define HFCWRITEFPMMETROVERSION_H
#include "EvolMessage.h"
#include "HfcObject.h"
#include "FpmEvent.h"

class HfcWriteFpmMetroVersion : public EvolMessage, public HfcObject
{

	public:

	HfcWriteFpmMetroVersion(const char* data, int len, int formatType);
	HfcWriteFpmMetroVersion(Hobart::CEvolMsgReceiver *pHobThread = NULL);
	const char *serialize(int format = BrowserFormat) const;
	std::string getMetrologyVersion() {return metrologyVersion_;}
	void setMetrologyVersion(std::string version) { metrologyVersion_ = version; }

	private:
	std::string metrologyVersion_;
};


#endif
