#ifndef DELETEBATCHMESSAGE_H
#define DELETEBATCHMESSAGE_H



#include "EvolMessage.h"


#include "Messages/DeleteBatchProcessedMessage.h"



namespace synchronization
{


	class DeleteBatchMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		DeleteBatchMessage(int id, DeleteBatchProcessedMessage::DeleteBatchMethod deleteMethod);
		DeleteBatchMessage(const char* buffer);
		DeleteBatchMessage(const DeleteBatchMessage& sm);
		virtual ~DeleteBatchMessage();


		/* --- Getters and setters --- */
		int getBatchId() const { return batchId_; }
		void setBatchId(int batchId) { batchId_ = batchId; }

		DeleteBatchProcessedMessage::DeleteBatchMethod getDeletionMethod() const { return deleteMethod_; }
		void setDeletionType(DeleteBatchProcessedMessage::DeleteBatchMethod deleteMethod) { deleteMethod_ = deleteMethod; }


		/* --- Function members --- */
		/* Operators*/
		DeleteBatchMessage& operator = (const DeleteBatchMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Data members --- */
		int batchId_;
		DeleteBatchProcessedMessage::DeleteBatchMethod deleteMethod_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
		/* --- Constructors and destructor --- */


		/* --- Function members --- */
	};


}



#endif /* DELETEBATCHMESSAGE_H */
