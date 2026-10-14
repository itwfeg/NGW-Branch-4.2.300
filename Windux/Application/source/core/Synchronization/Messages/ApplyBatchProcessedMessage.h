#ifndef APPLYBATCHPROCESSEDMESSAGE_H
#define APPLYBATCHPROCESSEDMESSAGE_H



#include "ProcessedMessage.h"



namespace synchronization
{


	class ApplyBatchProcessedMessage : public ProcessedMessage
	{
	public:
		/* --- Constructors and destructor --- */
		ApplyBatchProcessedMessage(int batchId);
		ApplyBatchProcessedMessage(const char* buffer);
		virtual ~ApplyBatchProcessedMessage();


		/* --- Getters and setters --- */
		int getBatchId() const { return batchId_; }
		void setBatchId(int batchId) { batchId_ = batchId; }


		/* --- Function members --- */
		/* Operators*/
		ApplyBatchProcessedMessage& operator = (const ApplyBatchProcessedMessage& rhs);

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



#endif /* APPLYBATCHPROCESSEDMESSAGE_H */
