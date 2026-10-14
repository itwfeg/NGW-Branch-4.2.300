/**
 * @file HfcWriteFpmManData.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief Fastrack personality module manager message to write manufacturing data to serial flash
 * Created on: Feb 26, 2013
 */

#ifndef HFCWRITEFPMMANDATA_H
#define HFCWRITEFPMMANDATA_H
#include "EvolMessage.h"
#include "HfcObject.h"
#include "MtdManufacturingData.h"
#include "FpmEvent.h"

class HfcWriteFpmManData : public EvolMessage, public HfcObject
{

	public:

	typedef enum
	{
		OffsetScaleStyle = 1,
		OffsetBoardAssembly,
		OffsetBoardRevision,
		OffsetDateOfMan,
		OffsetSerialNumb,
		OffsetSystemStyle

	}OffsetMfgData;

	HfcWriteFpmManData(const char* data, int len, int formatType);
	HfcWriteFpmManData(MtdManufacturingData *pMfgData, Hobart::CEvolMsgReceiver *pHobThread = NULL);
	const char *serialize(int format = BrowserFormat) const;
	std::string getScaleStyle() {return scaleStyle_;}
	std::string getBoardAssembly() {return boardAssembly_;}
	std::string getBoardRevision() {return boardRevision_;}
	std::string getDate() {return dateOfManufacture_;}
	std::string getSerial() {return serialNumber_;}
	std::string getSystemStyle() { return systemStyle_; }

	private:
	std::string scaleStyle_;
	std::string boardAssembly_;
	std::string boardRevision_;
	std::string dateOfManufacture_;
	std::string serialNumber_;
	std::string systemStyle_;

};


#endif
