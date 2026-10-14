#ifndef COMPLETEBATCHMESSAGE_H
#define COMPLETEBATCHMESSAGE_H



#include "EvolMessage.h"



namespace synchronization
{


	class CompleteBatchMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		CompleteBatchMessage(int id);
		CompleteBatchMessage(const char* buffer);
		CompleteBatchMessage(const CompleteBatchMessage& sm);
		virtual ~CompleteBatchMessage();


		/* --- Getters and setters --- */
		int getBatchId() const { return batchId_; }
		void setBatchId(int batchId) { batchId_ = batchId; }


		/* --- Function members --- */
		/* Operators*/
		CompleteBatchMessage& operator = (const CompleteBatchMessage& rhs);

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



#endif /* COMPLETEBATCHMESSAGE_H */
