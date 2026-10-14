/**
 * @file HfcWriteFpmPoundsFor.h
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

class HfcWriteFpmPoundsFor : public EvolMessage, public HfcObject
{

	public:

	HfcWriteFpmPoundsFor(const char* data, int len, int formatType);
	HfcWriteFpmPoundsFor(Hobart::CEvolMsgReceiver *pHobThread = NULL);
	const char *serialize(int format = BrowserFormat) const;
	std::string getPoundsFor() {return poundsFor_;}
	void setPoundsFor(std::string value) { poundsFor_ = value; }

	private:
	std::string poundsFor_;
};


#endif
