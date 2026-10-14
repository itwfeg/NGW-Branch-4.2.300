#ifndef PROCESSUSERFILEMESSAGE_H
#define PROCESSUSERFILEMESSAGE_H



#include "EvolMessage.h"


namespace synchronization
{


	class ProcessUserFileMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		ProcessUserFileMessage(int id);
		ProcessUserFileMessage(const char* buffer);
		ProcessUserFileMessage(const ProcessUserFileMessage& sm);
		virtual ~ProcessUserFileMessage();


		/* --- Getters and setters --- */
		int getBatchId() const { return batchId_; }
		void setBatchId(int batchId) { batchId_ = batchId; }


		/* --- Function members --- */
		/* Operators*/
		ProcessUserFileMessage& operator = (const ProcessUserFileMessage& rhs);

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



#endif /* PROCESSUSERFILEMESSAGE_H */
