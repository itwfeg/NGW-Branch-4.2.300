#ifndef CREATEBATCHMESSAGE_H
#define CREATEBATCHMESSAGE_H



#include "EvolMessage.h"

#include "Batch.h"



namespace synchronization
{


	class CreateBatchMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		CreateBatchMessage(hfc::Batch::BatchOrigin origin, hfc::Batch::BatchType type);
		CreateBatchMessage(const char* buffer);
		CreateBatchMessage(const CreateBatchMessage& sm);
		virtual ~CreateBatchMessage();


		/* --- Getters and setters --- */
		hfc::Batch::BatchOrigin getOrigin() const { return origin_; }
		void setOrigin(hfc::Batch::BatchOrigin origin) { origin_ = origin; }

		hfc::Batch::BatchType getType() const { return type_; }
		void setType(hfc::Batch::BatchType type) { type_ = type; }


		/* --- Function members --- */
		/* Operators*/
		CreateBatchMessage& operator = (const CreateBatchMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Data members --- */
		hfc::Batch::BatchOrigin origin_;
		hfc::Batch::BatchType type_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
		/* --- Constructors and destructor --- */


		/* --- Function members --- */
	};


}



#endif /* CREATEBATCHMESSAGE_H */
