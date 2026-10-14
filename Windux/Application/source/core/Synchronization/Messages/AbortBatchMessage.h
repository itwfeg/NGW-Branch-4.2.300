#ifndef ABORTBATCHMESSAGE_H
#define ABORTBATCHMESSAGE_H



#include "EvolMessage.h"


//#include "Commands/Command.h"



namespace synchronization
{


	class AbortBatchMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		AbortBatchMessage(int id);
		AbortBatchMessage(const char* buffer);
		AbortBatchMessage(const AbortBatchMessage& sm);
		virtual ~AbortBatchMessage();


		/* --- Getters and setters --- */
		int getBatchId() const { return batchId_; }
		void setBatchId(int batchId) { batchId_ = batchId; }


		/* --- Function members --- */
		/* Operators*/
		AbortBatchMessage& operator = (const AbortBatchMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Data members --- */
		int batchId_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif

		/* --- Constructors and destructor --- */


		/* --- Function members --- */
	};


}



#endif /* ABORTBATCHMESSAGE_H */
