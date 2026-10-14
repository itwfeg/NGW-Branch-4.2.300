#ifndef COMPLETEBATCHPROCESSEDMESSAGE_H
#define COMPLETEBATCHPROCESSEDMESSAGE_H



#include "ProcessedMessage.h"



namespace synchronization
{


	class CompleteBatchProcessedMessage : public ProcessedMessage
	{
	public:
		/* --- Constructors and destructor --- */
		CompleteBatchProcessedMessage(int status, int batchId);
		CompleteBatchProcessedMessage(const char* buffer);
		CompleteBatchProcessedMessage(const CompleteBatchProcessedMessage& sm);
		virtual ~CompleteBatchProcessedMessage();


		/* --- Getters and setters --- */
		int getBatchId() const { return batchId_; }
		void setBatchId(int batchId) { batchId_ = batchId; }


		/* --- Function members --- */
		/* Operators*/
		CompleteBatchProcessedMessage& operator = (const CompleteBatchProcessedMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Instance Data members --- */
		int batchId_;

		/* --- Constructors and destructor --- */


		/* --- Instance Function members --- */


		/* --- Class Data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* COMPLETEBATCHPROCESSEDMESSAGE_H */
