/*
 * NGWLabelSetup.h
 *
 *  Created on: Jul 28, 2015
 *      Author: aaron
 */

#ifndef NGWLABELSETUP_H_
#define NGWLABELSETUP_H_
#include "NGWApplier.h"
#include "EvolMessage.h"

class NGWLabelSetup : public EvolMessage
{
	public:
		NGWLabelSetup(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		NGWLabelSetup(const char *buffer);
		~NGWLabelSetup();

		void setStationId(NGWApplierStation station) { stationId = station; }
		void setLabelSize(UINT16 size) { labelSize = size; }
		void setLabelRotation(UINT16 rotation) { labelRotation = rotation; }
		void setLabelLocation(UINT16 location) { labelLocation = location; }

		NGWApplierStation getStationId() const { return stationId; }
		UINT16 getLabelSize() const { return labelSize; }
		UINT16 getLabelRotation() const { return labelRotation; }
		UINT16 getLabelLocation() const { return labelLocation; }

		const char *serialize()const;

	private:
		NGWApplierStation stationId;
		UINT16	labelSize;
		UINT16	labelRotation;
		UINT16 	labelLocation;


};



#endif /* NGWLABELSETUP_H_ */
