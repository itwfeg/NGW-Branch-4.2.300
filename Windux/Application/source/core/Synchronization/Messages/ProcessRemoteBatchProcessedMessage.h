#ifndef PROCESREMOTEBATCHPROCESSEDMESSAGE_H
#define PROCESREMOTEBATCHPROCESSEDMESSAGE_H



#include "ProcessedMessage.h"



namespace synchronization
{


	class ProcessRemoteBatchProcessedMessage : public ProcessedMessage
	{
	public:
		/* --- Constructors and destructor --- */
		ProcessRemoteBatchProcessedMessage(int status, int batchId);
		ProcessRemoteBatchProcessedMessage(const char* buffer);
		ProcessRemoteBatchProcessedMessage(const ProcessRemoteBatchProcessedMessage& sm);
		virtual ~ProcessRemoteBatchProcessedMessage();


		/* --- Getters and setters --- */
		int getBatchId() const { return batchId_; }
		void setBatchId(int batchId) { batchId_ = batchId; }


		/* --- Function members --- */
		/* Operators*/
		ProcessRemoteBatchProcessedMessage& operator = (const ProcessRemoteBatchProcessedMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Data members --- */
		int batchId_;


		/* --- Constructors and destructor --- */


		/* --- Function members --- */
	};


}



#endif /* PROCESREMOTEBATCHPROCESSEDMESSAGE_H */
