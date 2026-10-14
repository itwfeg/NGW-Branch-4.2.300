#ifndef REGISTERSECONDARYSCALE_H
#define REGISTERSECONDARYSCALE_H



#include "HfcObject.h"



namespace hfc
{


	class RegisterSecondaryScale : public HfcObject
	{
		public:
			/* --- Types and subtypes --- */


			/* --- Constructors and destructor --- */
			RegisterSecondaryScale();
//			RegisterSecondaryScale(const char * data, int len, int formatType);
			~RegisterSecondaryScale();


			/* --- Getters and setters --- */


			/* --- Instance function members --- */
			const char * serialize(int format) const;


			/* --- Class function members --- */


		private:
			/* --- Data members --- */
//			std::string secondaryScale_;


			/* --- Constructors --- */


			/* --- Instance function members --- */


			/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


}



#endif /* REGISTERSECONDARYSCALE_H */
