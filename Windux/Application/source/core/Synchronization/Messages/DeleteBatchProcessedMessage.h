#ifndef DELETEBATCHPROCESSEDMESSAGE_H
#define DELETEBATCHPROCESSEDMESSAGE_H



#include "ProcessedMessage.h"



namespace synchronization
{


	class DeleteBatchProcessedMessage : public ProcessedMessage
	{
	public:
		/* --- Types and subtypes --- */
		typedef enum
		{
			DeleteChangesOnly,
			DeleteChangesAndBatch
		} DeleteBatchMethod;


		/* --- Constructors and destructor --- */
		DeleteBatchProcessedMessage(int batchId, DeleteBatchMethod deleteMethod);
		DeleteBatchProcessedMessage(const char* buffer);
		virtual ~DeleteBatchProcessedMessage();


		/* --- Getters and setters --- */
		int getBatchId() const { return batchId_; }
		void setBatchId(int batchId) { batchId_ = batchId; }

		DeleteBatchMethod getDeleteMethod() const { return deleteMethod_; }
		void setDeleteMethod(DeleteBatchMethod deleteMethod) { deleteMethod_ = deleteMethod; }


		/* --- Function members --- */
		/* Operators*/
		DeleteBatchProcessedMessage& operator = (const DeleteBatchProcessedMessage& rhs);

		const char* serialize() const;


		/* --- Class function members --- */
		static const std::string& getDeleteMethodName(DeleteBatchMethod method);


	private:
		/* --- Instance Data members --- */
		int batchId_;
		DeleteBatchMethod deleteMethod_;


		/* --- Constructors and destructor --- */


		/* --- Function members --- */


		/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif


		static std::string DeleteChangesOnlyName;
		static std::string DeleteChangesAndBatchName;
		static std::string DeleteInvalidName;
	};


}



#endif /* DELETEBATCHPROCESSEDMESSAGE_H */
