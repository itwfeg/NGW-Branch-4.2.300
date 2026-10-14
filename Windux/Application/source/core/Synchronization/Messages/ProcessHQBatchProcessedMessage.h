#ifndef PROCESSHQBATCHPROCESSEDMESSAGE_H
#define PROCESSHQBATCHPROCESSEDMESSAGE_H



#include "ProcessedMessage.h"



namespace synchronization
{


	class ProcessHQBatchProcessedMessage : public ProcessedMessage
	{
	public:
		/* --- Constructors and destructor --- */
		ProcessHQBatchProcessedMessage(int status, int batchNumber);
		ProcessHQBatchProcessedMessage(const char* buffer);
		ProcessHQBatchProcessedMessage(const ProcessHQBatchProcessedMessage& sm);
		virtual ~ProcessHQBatchProcessedMessage();


		/* --- Getters and setters --- */
		int getBatchNumber() const { return batchNumber_; }
		void setBatchNumber(int batchNumber) { batchNumber_ = batchNumber; }


		/* --- Function members --- */
		/* Operators*/
		ProcessHQBatchProcessedMessage& operator = (const ProcessHQBatchProcessedMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Instance Data members --- */
		int batchNumber_;


		/* --- Constructors and destructor --- */


		/* --- Function members --- */


		/* --- Class Data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* PROCESSHQBATCHPROCESSEDMESSAGE_H */
