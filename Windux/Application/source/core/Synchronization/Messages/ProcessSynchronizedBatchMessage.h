#ifndef PROCESSSYNCHRONIZEDBATCHMESSAGE_H
#define PROCESSSYNCHRONIZEDBATCHMESSAGE_H



#include "EvolMessage.h"



namespace synchronization
{


	class ProcessSynchronizedBatchMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		ProcessSynchronizedBatchMessage(int batchId);
		ProcessSynchronizedBatchMessage(const char* buffer);
		ProcessSynchronizedBatchMessage(const ProcessSynchronizedBatchMessage& sm);
		virtual ~ProcessSynchronizedBatchMessage();


		/* --- Getters and setters --- */
		int getBatchId() const { return batchId_; }
		void setBatchId(int batchId) { batchId_ = batchId; }


		/* --- Function members --- */
		/* Operators*/
		ProcessSynchronizedBatchMessage& operator = (const ProcessSynchronizedBatchMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Instance data members --- */
		int batchId_;


		/* --- Constructors and destructor --- */


		/* --- Function members --- */


		/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* PROCESSSYNCHRONIZEDBATCHMESSAGE_H */
