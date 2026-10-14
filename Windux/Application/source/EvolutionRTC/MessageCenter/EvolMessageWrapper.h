#ifndef EVOLMESSAGEWRAPPER_H
#define EVOLMESSAGEWRAPPER_H



#include "EvolMessage.h"


//namespace Hobart
//{

#if !defined( __linux__ )   // Alamy: this statement causes error in Linux
	class Hobart::CEvolMsgReceiver;
#endif

	class EvolMessageWrapper
	{
		public:
			/* --- Constructors and destructor --- */
			EvolMessageWrapper( const EvolMessage * msg );
#ifdef MIN_WARN_FIX
			EvolMessageWrapper( const char * ptr, size_t length );
#else
			EvolMessageWrapper( const char * ptr, int length );
#endif
			virtual ~EvolMessageWrapper();



			/* --- Getters and setters --- */
			EvolDeviceType getDeviceType();

			UINT8 getEventType();

			Hobart::CEvolMsgReceiver *getSourceObject();
			void setSourceObject(Hobart::CEvolMsgReceiver* source);

			unsigned int getSequenceNumber();
			void setSequenceNumber(unsigned int number);


			/* --- Instance function members --- */
			const char * Data();
			const char * CStr();
			int Length();


		private:
			/* --- Instance data members --- */
			std::string evolData;
			
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif			
	};


//}//end of namespace Hobart



#endif /* EVOLMESSAGEWRAPPER_H */

