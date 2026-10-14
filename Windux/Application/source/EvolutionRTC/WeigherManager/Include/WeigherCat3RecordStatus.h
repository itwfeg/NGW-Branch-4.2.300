/**
 * @file WeigherCat3RecordStatus.h
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief Message provides status on record write
 * Created on: July 1, 2016
 */

#ifndef WEIGHERCAT3RECORDSTATUS_H_
#define WEIGHERCAT3RECORDSTATUS_H_

#include "EvolMessage.h"
#include "Weigher.h"
#include "CEvolMsgReceiver.h"

class WeigherCat3RecordStatus : public EvolMessage
{
	public:
		WeigherCat3RecordStatus(Hobart::CEvolMsgReceiver *pHobThread = NULL,
								bool result = false );
		WeigherCat3RecordStatus(const char *buffer);
		~WeigherCat3RecordStatus();

		void setStatus(bool result) { status = result; }
		bool getStatus() 			{ return status; }

		const char *serialize() const;
	private:
		bool status;
};
#endif
