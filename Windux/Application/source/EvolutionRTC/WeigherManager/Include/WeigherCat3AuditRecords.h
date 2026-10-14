/**
 * @file WeigherCat3AuditRecords.h
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief Message weigher response to request for CAT3 statistics.
 * Created on: July 1, 2016
 */
#ifndef WEIGHERCAT3AUDITRECORDS_H_
#define WEIGHERCAT3AUDITRECORDS_H_
#include "EvolMessage.h"
#include "Weigher.h"
#include "CEvolMsgReceiver.h"

class WeigherCat3AuditRecords : public EvolMessage
{
	public:

		WeigherCat3AuditRecords(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		WeigherCat3AuditRecords(const char *buffer);
		~WeigherCat3AuditRecords();

		const char *serialize() const;

		unsigned char getPageNumber() const			{ return pageNumber; }
		void setPageNumber(unsigned char number)	{ pageNumber = number; }

		unsigned char getPageIndex() const			{ return pageIndex; }
		void setPageIndex(unsigned char index)		{ pageIndex = index; }

		bool getRecord(int index, Cat3AuditRecord *pRecord);
		void setRecordBuffer(const unsigned char *pBuffer);
		const unsigned char *getRecordBuffer() const		{ return buffer; }


		void show();
		#define RECORD_SIZE	32
		#define NUMBER_OF_RECORDS 10
		#define MAX_BUFFER_SIZE	320

	private:
		unsigned char pageNumber;
		unsigned char pageIndex;
		unsigned char buffer[MAX_BUFFER_SIZE];

};
#endif
