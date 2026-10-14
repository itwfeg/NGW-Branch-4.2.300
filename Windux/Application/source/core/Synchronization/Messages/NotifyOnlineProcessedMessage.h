#ifndef NOTIFYONLINEPROCESSEDMESSAGE_H
#define NOTIFYONLINEPROCESSEDMESSAGE_H



#include "ProcessedMessage.h"



namespace synchronization
{


	class NotifyOnlineProcessedMessage : public ProcessedMessage
	{
	public:
		/* --- Constructors and destructor --- */
		NotifyOnlineProcessedMessage(int status = 0);
		NotifyOnlineProcessedMessage(const char* buffer);
		NotifyOnlineProcessedMessage(const NotifyOnlineProcessedMessage& sm);
		virtual ~NotifyOnlineProcessedMessage();


		/* --- Getters and setters --- */


		/* --- Function members --- */
		/* Operators*/
		NotifyOnlineProcessedMessage& operator = (const NotifyOnlineProcessedMessage& rhs);


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



#endif /* NOTIFYONLINEPROCESSEDMESSAGE_H */
