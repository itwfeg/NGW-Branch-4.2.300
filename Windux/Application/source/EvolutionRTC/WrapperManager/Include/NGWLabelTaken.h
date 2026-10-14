/*
 * NGWLabelTaken.h
 *
 *  Created on: Jul 27, 2015
 *      Author: aaron
 */

#ifndef NGWLABELTAKEN_H_
#define NGWLABELTAKEN_H_
#include "NGWApplier.h"
#include "EvolMessage.h"

class NGWLabelTaken : public EvolMessage
{
	public:
	NGWLabelTaken(Hobart::CEvolMsgReceiver *pHobThread = NULL, UINT16 pkgid = 0, NGWApplierStation Idstation = _UnknownStation);
	NGWLabelTaken(const char *buffer);
	~NGWLabelTaken();

	UINT16 getPackageId() const {return packageId;}
	NGWApplierStation getStationId() const {return stationId;}

	const char *serialize()const;

	private:
	UINT16 	packageId;
	NGWApplierStation stationId;


};




#endif /* NGWLABELTAKEN_H_ */
