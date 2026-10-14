#ifndef PROCESSYNCHRONIZEDBATCHPROCESSEDMESSAGE_H
#define PROCESSYNCHRONIZEDBATCHPROCESSEDMESSAGE_H



#include "ProcessedMessage.h"



namespace synchronization
{


	class ProcessSynchronizedBatchProcessedMessage : public ProcessedMessage
	{
	public:
		/* --- Constructors and destructor --- */
		ProcessSynchronizedBatchProcessedMessage(int status, int batchId);
		ProcessSynchronizedBatchProcessedMessage(const char* buffer);
		ProcessSynchronizedBatchProcessedMessage(const ProcessSynchronizedBatchProcessedMessage& sm);
		virtual ~ProcessSynchronizedBatchProcessedMessage();


		/* --- Getters and setters --- */
		int getBatchId() const { return batchId_; }
		void setBatchId(int batchId) { batchId_ = batchId; }


		/* --- Function members --- */
		/* Operators*/
		ProcessSynchronizedBatchProcessedMessage& operator = (const ProcessSynchronizedBatchProcessedMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Data members --- */
		int batchId_;


		/* --- Constructors and destructor --- */


		/* --- Function members --- */
	};


}



#endif /* PROCESSYNCHRONIZEDBATCHPROCESSEDMESSAGE_H */
