#ifndef GOINGOFFLINEMESSAGE_H
#define GOINGOFFLINEMESSAGE_H



#include "EvolMessage.h"



namespace synchronization
{


	class GoingOffLineMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		GoingOffLineMessage();
		GoingOffLineMessage(const GoingOffLineMessage& sm);
		GoingOffLineMessage(const char* buffer);
		virtual ~GoingOffLineMessage();


		/* --- Function members --- */
		/* Operators*/
		GoingOffLineMessage& operator = (const GoingOffLineMessage& rhs);


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



#endif /* GOINGOFFLINEMESSAGE_H */
