/*
 * NGWLabelReady.h
 *
 *  Created on: Jul 27, 2015
 *      Author: aaron
 */

#ifndef NGWLABELREADY_H_
#define NGWLABELREADY_H_
#include "NGWApplier.h"
#include "EvolMessage.h"

class NGWLabelReady : public EvolMessage
{
	public:
	NGWLabelReady(Hobart::CEvolMsgReceiver *pHobThread = NULL, UINT16 pkgid = 0, NGWApplierStation Idstation = _UnknownStation);
	NGWLabelReady(const char *buffer);
	~NGWLabelReady();

	UINT16 getPackageId() const {return packageId;}
	NGWApplierStation getStationId() const {return stationId;}

	const char *serialize()const;
	private:
	UINT16 	packageId;
	NGWApplierStation stationId;



};



#endif /* NGWLABELREADY_H_ */
