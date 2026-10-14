/**
 * @file HfcWriteFpmFractional.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief Fastrack personality module manager message to write pounds for setting state to serial flash
 * Created on: July 20, 2016
 */

#ifndef HFCWRITEFPMPOUNDSFOR_H
#define HFCWRITEFPMPOUNDSFOR_H
#include "EvolMessage.h"
#include "HfcObject.h"
#include "FpmEvent.h"

class HfcWriteFpmFractional : public EvolMessage, public HfcObject
{

	public:

	HfcWriteFpmFractional(const char* data, int len, int formatType);
	HfcWriteFpmFractional(Hobart::CEvolMsgReceiver *pHobThread = NULL);
	const char *serialize(int format = BrowserFormat) const;
	std::string getPoundsFor() {return fractionalPricing_;}
	void setPoundsFor(std::string value) { fractionalPricing_ = value; }

	private:
	std::string fractionalPricing_;
};


#endif
