#ifndef CREATEBATCHPROCESSEDMESSAGE_H
#define CREATEBATCHPROCESSEDMESSAGE_H



#include "ProcessedMessage.h"



namespace synchronization
{


	class CreateBatchProcessedMessage : public ProcessedMessage
	{
	public:
		/* --- Constructors and destructor --- */
		CreateBatchProcessedMessage(int status, int batchId);
		CreateBatchProcessedMessage(const char* buffer);
		CreateBatchProcessedMessage(const CreateBatchProcessedMessage& sm);
		virtual ~CreateBatchProcessedMessage();


		/* --- Getters and setters --- */
		int getBatchId() const { return batchId_; }
		void setBatchId(int batchId) { batchId_ = batchId; }

		int getBatchVersion() const { return batchVersion_; }
		void setBatchVersion(int version) { batchVersion_ = version; }


		/* --- Function members --- */
		/* Operators */
		CreateBatchProcessedMessage& operator = (const CreateBatchProcessedMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Instance Data members --- */
		int batchId_;
		int batchVersion_;


		/* --- Constructors and destructor --- */


		/* --- Function members --- */


		/* --- Class Data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* CREATEBATCHPROCESSEDMESSAGE_H */
