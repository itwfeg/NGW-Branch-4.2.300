#ifndef NOTIFYLOCALBATCHESLAZILYMESSAGE_H
#define NOTIFYLOCALBATCHESLAZILYMESSAGE_H



#include "EvolMessage.h"



namespace synchronization
{


	class NotifyLocalBatchesLazilyMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		NotifyLocalBatchesLazilyMessage();
		NotifyLocalBatchesLazilyMessage(const NotifyLocalBatchesLazilyMessage& sm);
		NotifyLocalBatchesLazilyMessage(const char* buffer);
		virtual ~NotifyLocalBatchesLazilyMessage();


		/* --- Function members --- */
		/* Operators*/
		NotifyLocalBatchesLazilyMessage& operator = (const NotifyLocalBatchesLazilyMessage& rhs);


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



#endif /* NOTIFYLOCALBATCHESLAZILYMESSAGE_H */
