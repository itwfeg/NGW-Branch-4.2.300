#ifndef ADDRECORDTOBATCHPROCESSEDMESSAGE_H
#define ADDRECORDTOBATCHPROCESSEDMESSAGE_H



#include "ProcessedMessage.h"



namespace synchronization
{


	class AddRecordToBatchProcessedMessage : public ProcessedMessage
	{
	public:
		/* --- Constructors and destructor --- */
		AddRecordToBatchProcessedMessage(int status, int batchId);
		AddRecordToBatchProcessedMessage(const char* buffer);
		virtual ~AddRecordToBatchProcessedMessage();


		/* --- Getters and setters --- */
		int getBatchId() const { return batchId_; }
		void setBatchId(int batchId) { batchId_ = batchId; }


		/* --- Function members --- */
		/* Operators*/
		AddRecordToBatchProcessedMessage& operator = (const AddRecordToBatchProcessedMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Instance Data members --- */
		int batchId_;


		/* --- Constructors and destructor --- */


		/* --- Function members --- */


		/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* ADDRECORDTOBATCHPROCESSEDMESSAGE_H */
