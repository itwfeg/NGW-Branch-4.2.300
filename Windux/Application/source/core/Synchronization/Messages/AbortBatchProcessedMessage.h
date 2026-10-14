#ifndef ABORTBATCHPROCESSEDMESSAGE_H
#define ABORTBATCHPROCESSEDMESSAGE_H



#include "ProcessedMessage.h"



namespace synchronization
{


	class AbortBatchProcessedMessage : public ProcessedMessage
	{
	public:
		/* --- Constructors and destructor --- */
		AbortBatchProcessedMessage(int status, int batchId);
		AbortBatchProcessedMessage(const char* buffer);
		AbortBatchProcessedMessage(const AbortBatchProcessedMessage& sm);
		virtual ~AbortBatchProcessedMessage();


		/* --- Getters and setters --- */
		int getBatchId() const { return batchId_; }
		void setBatchId(int batchId) { batchId_ = batchId; }


		/* --- Function members --- */
		/* Operators*/
		AbortBatchProcessedMessage& operator = (const AbortBatchProcessedMessage& rhs);

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



#endif /* ABORTBATCHPROCESSEDMESSAGE_H */
