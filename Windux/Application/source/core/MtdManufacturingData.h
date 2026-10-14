/**
 * @file MtdManufacturingData.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief HTi serial flash manufacturing section
 * Created on: Oct 31, 2012
 */

#ifndef MTDMANUFACTURINGDATA_H
#define MTDMANUFACTURINGDATA_H

#include "HobVersion.h"
#include "HobDateTime.h"
#include "HobString.h"


class MtdManufacturingData
{

public:

	MtdManufacturingData();

	MtdManufacturingData(const std::string &s);
	MtdManufacturingData(MtdManufacturingData *pMfgData);
	~MtdManufacturingData();


	typedef enum
	{
		XML_ScaleStyle,
		XML_BoardAssembly,
		XML_BoardRevision,
		XML_DateOfManufacture,
		XML_SerialNumber,
		XML_SystemStyle,
		XML_EndManElements

	}MAN_XML_Elements;

	const HobString getScaleStyle() const;
	void setScaleStyle(const HobString &scaleModel ) {scaleStyle_ = scaleModel;}
	void setScaleStyleHTi() { scaleStyle_= "HTi-Access-AWS3";}

	const HobString getBoardAssembly() const {return boardAssembly_;}
	void setBoardAssembly(const HobString &assembly) { boardAssembly_ = assembly;}

	const std::string getBoardRevision() const {return boardRevision_;}
	void setBoardRevision(const std::string &revision) { boardRevision_ = revision;}

	const std::string getDateOfManufacturing() const {return dateOfManufacture_;}
	void setDateOfManufacturing(const std::string &dateTime) { dateOfManufacture_ = dateTime;}

	const HobString getSerialNumber() const {return serialNumber_;}
	void setSerialNumber(const HobString &serial) {serialNumber_ = serial;}

	const HobString getSystemStyle() const { return systemStyle_; }
	void setSystemStyle(const HobString &s) { systemStyle_ = s; }

	void setManufacturingDataDefaults();

	unsigned long getCrc() {return crc_;}
	unsigned long calculateCrc();

	const std::string serializeXml() const;

	void show();

private:
	HobString scaleStyle_;
	HobString boardAssembly_;

	HobString  boardRevision_;
	std::string dateOfManufacture_;
	HobString serialNumber_;

	/*
	 * System style refers to the components the scale is attached.
	 * Ex:	AWS-3		Enhanced AWS2
	 * 		EWS-3		Enhanced EWS2
	 * 		NGW1-RL		Phase 1 small footprint, single roll, right to left discharge
	 * 		NGW2-RL		Phase 1 small footprint, double roll, right to left discharge
	 * 		NGW1-2RL  	Phase 2 small footprint, single roll, right to left discharge, additional CAN interface
	 *		NGW2-2RL 	Phase 2 small footprint, double roll, right to left discharge, additional CAN interface
	 */
	HobString systemStyle_;

	mutable unsigned long crc_;

	mutable std::string serializedData_;

	void init();

};


#endif
