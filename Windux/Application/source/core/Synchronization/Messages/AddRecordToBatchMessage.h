#ifndef ADDRECORDTOBATCHMESSAGE_H
#define ADDRECORDTOBATCHMESSAGE_H



#include "EvolMessage.h"


#include "SynchronizationChange.h"


#include "CSmartPtr.h"



namespace synchronization
{


	class AddRecordToBatchMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		AddRecordToBatchMessage(int id, Util::CSmartPtr<hfc::SynchronizationChange> record);
		AddRecordToBatchMessage(const char* buffer);
		AddRecordToBatchMessage(const AddRecordToBatchMessage& sm);
		virtual ~AddRecordToBatchMessage();


		/* --- Getters and setters --- */
		int getBatchId() const { return batchId_; }
		void setBatchId(int batchId) { batchId_ = batchId; }

		Util::CSmartPtr<hfc::SynchronizationChange> getRecord() const { return record_; }
		void setRecord(Util::CSmartPtr<hfc::SynchronizationChange> record) { record_ = record; }


		/* --- Function members --- */
		/* Operators*/
		AddRecordToBatchMessage& operator = (const AddRecordToBatchMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Data members --- */
		int batchId_;
		Util::CSmartPtr<hfc::SynchronizationChange> record_;


		/* --- Constructors and destructor --- */


		/* --- Function members --- */


		/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* ADDRECORDTOBATCHMESSAGE_H */
