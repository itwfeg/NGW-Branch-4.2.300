#ifndef STARTSYNCMANAGERPROCESSEDMESSAGE_H
#define STARTSYNCMANAGERPROCESSEDMESSAGE_H



#include "ProcessedMessage.h"



namespace synchronization
{


	class StartSyncManagerProcessedMessage : public ProcessedMessage
	{
	public:
		/* --- Constructors and destructor --- */
		StartSyncManagerProcessedMessage(int status);
		StartSyncManagerProcessedMessage(const char* buffer);
		StartSyncManagerProcessedMessage(const StartSyncManagerProcessedMessage& sm);
		virtual ~StartSyncManagerProcessedMessage();


		/* --- Getters and setters --- */


		/* --- Function members --- */
		/* Operators*/
		StartSyncManagerProcessedMessage& operator = (const StartSyncManagerProcessedMessage& rhs);


	private:
		/* --- Data members --- */

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
		/* --- Constructors and destructor --- */


		/* --- Function members --- */
	};


}



#endif /* STARTSYNCMANAGERPROCESSEDMESSAGE_H */
