/**
 * @file WeigherCat3PageRequest.h
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief Message for requesting CAT3 records from the weigher
 * Created on: July 1, 2016
 */
#ifndef WEIGHERCAT3PAGEREQUEST_H_
#define WEIGHERCAT3PAGEREQUEST_H_

#include "EvolMessage.h"
#include "Weigher.h"
#include "CEvolMsgReceiver.h"


class WeigherCat3PageRequest : public EvolMessage
{
	public:
		WeigherCat3PageRequest(Hobart::CEvolMsgReceiver *pHobThread = NULL,
				UINT8 number = 0, UINT8 index = 0);
		WeigherCat3PageRequest(const char *buffer);
		~WeigherCat3PageRequest();

		void setPageNumber(UINT8 number){ pageNumber = number; }
		UINT8 getPageNumber() 			{ return pageNumber;}

		void setPageIndex(UINT8 index) 	{ pageIndex = index; }
		UINT8 getPageIndex()			{ return pageIndex; }

		const char *serialize() const;

		void show();
	private:
		UINT8 pageNumber;
		UINT8 pageIndex;
};





#endif
