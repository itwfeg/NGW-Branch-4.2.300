#ifndef PROCESSUSERCHANGESPROCESSEDMESSAGE_H
#define PROCESSUSERCHANGESPROCESSEDMESSAGE_H



#include "ProcessedMessage.h"



namespace synchronization
{


	class ProcessUserChangesProcessedMessage : public ProcessedMessage
	{
	public:
		/* --- Constructors and destructor --- */
		ProcessUserChangesProcessedMessage(int status);
		ProcessUserChangesProcessedMessage(const char* buffer);
		ProcessUserChangesProcessedMessage(const ProcessUserChangesProcessedMessage& sm);
		virtual ~ProcessUserChangesProcessedMessage();


		/* --- Getters and setters --- */


		/* --- Function members --- */
		/* Operators*/
		ProcessUserChangesProcessedMessage& operator = (const ProcessUserChangesProcessedMessage& rhs);


	private:
		/* --- Instance Data members --- */


		/* --- Constructors and destructor --- */


		/* --- Instance Function members --- */


		/* --- Class Data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* PROCESSUSERCHANGESPROCESSEDMESSAGE_H */
