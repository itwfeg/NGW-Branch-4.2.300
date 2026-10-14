#ifndef STOPSYNCHRONIZERMESSAGE_H
#define STOPSYNCHRONIZERMESSAGE_H



#include "EvolMessage.h"



namespace synchronization
{


	class StopSynchronizerMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		StopSynchronizerMessage();
		StopSynchronizerMessage(const char* buffer);
		StopSynchronizerMessage(const StopSynchronizerMessage& sm);
		virtual ~StopSynchronizerMessage();


		/* --- Getters and setters --- */


		/* --- Function members --- */
		/* Operators*/
		StopSynchronizerMessage& operator = (const StopSynchronizerMessage& rhs);


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



#endif /* STOPSYNCHRONIZERMESSAGE_H */
