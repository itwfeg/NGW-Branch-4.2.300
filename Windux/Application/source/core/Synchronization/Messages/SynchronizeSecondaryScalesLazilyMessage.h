#ifndef SYNCHRONIZESECONDARYSCALESLAZILYMESSAGE_H
#define SYNCHRONIZESECONDARYSCALESLAZILYMESSAGE_H



#include "EvolMessage.h"



namespace synchronization
{


	class SynchronizeSecondaryScalesLazilyMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		SynchronizeSecondaryScalesLazilyMessage();
		SynchronizeSecondaryScalesLazilyMessage(const char* buffer);
		SynchronizeSecondaryScalesLazilyMessage(const SynchronizeSecondaryScalesLazilyMessage& sm);
		virtual ~SynchronizeSecondaryScalesLazilyMessage();


		/* --- Getters and setters --- */


		/* --- Function members --- */
		/* Operators*/
		SynchronizeSecondaryScalesLazilyMessage& operator = (const SynchronizeSecondaryScalesLazilyMessage& rhs);


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



#endif /* SYNCHRONIZESECONDARYSCALESLAZILYMESSAGE_H */
