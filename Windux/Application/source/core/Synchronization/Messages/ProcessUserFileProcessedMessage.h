#ifndef PROCESSUSERFILEPROCESSEDMESSAGE_H
#define PROCESSUSERFILEPROCESSEDMESSAGE_H



#include "ProcessedMessage.h"



namespace synchronization
{


	class ProcessUserFileProcessedMessage : public ProcessedMessage
	{
	public:
		/* --- Constructors and destructor --- */
		ProcessUserFileProcessedMessage(int status, int batchId);
		ProcessUserFileProcessedMessage(const char* buffer);
		ProcessUserFileProcessedMessage(const ProcessUserFileProcessedMessage& sm);
		virtual ~ProcessUserFileProcessedMessage();


		/* --- Getters and setters --- */
		int getBatchId() const { return batchId_; }
		void setBatchId(int batchId) { batchId_ = batchId; }


		/* --- Function members --- */
		/* Operators*/
		ProcessUserFileProcessedMessage& operator = (const ProcessUserFileProcessedMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Instance Data members --- */
		int batchId_;


		/* --- Constructors and destructor --- */


		/* --- Function members --- */


		/* --- Class Data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* PROCESSUSERFILEPROCESSEDMESSAGE_H */
