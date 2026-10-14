#ifndef NOTIFYLOCALBATCHES_H
#define NOTIFYLOCALBATCHES_H



#include "HfcObject.h"



namespace hfc
{


	class NotifyLocalBatches : public HfcObject
	{
		public:
			/* --- Types and subtypes --- */


			/* --- Constructors and destructor --- */
			NotifyLocalBatches();
//			NotifyLocalBatches(const char * data, int len, int formatType);
			~NotifyLocalBatches();


			/* --- Getters and setters --- */


			/* --- Instance function members --- */
			const char * serialize(int format) const;


			/* --- Class function members --- */


		private:
			/* --- Data members --- */


			/* --- Constructors --- */


			/* --- Instance function members --- */


			/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


}



#endif /* NOTIFYLOCALBATCHES_H */
