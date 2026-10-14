#ifndef APPLYBATCHMESSAGE_H
#define APPLYBATCHMESSAGE_H



#include "EvolMessage.h"



namespace synchronization
{


	class ApplyBatchMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		ApplyBatchMessage(int id);
		ApplyBatchMessage(const char* buffer);
		ApplyBatchMessage(const ApplyBatchMessage& sm);
		virtual ~ApplyBatchMessage();


		/* --- Getters and setters --- */
		int getBatchId() const { return batchId_; }
		void setBatchId(int batchId) { batchId_ = batchId; }


		/* --- Function members --- */
		/* Operators*/
		ApplyBatchMessage& operator = (const ApplyBatchMessage& rhs);

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



#endif /* APPLYBATCHMESSAGE_H */
