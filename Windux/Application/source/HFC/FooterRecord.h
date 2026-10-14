#ifndef FOOTERRECORD_H
#define FOOTERRECORD_H



#include "HfcObject.h"


#include "tags/Tags.h"


namespace hfc
{


	class SynchronizationChange;


	class FooterRecord : public HfcObject
	{
		public:
			/* --- Types and subtypes --- */


			/* --- Constructors and destructor --- */
			FooterRecord();
			FooterRecord(const char * data, int len, int formatType);
			~FooterRecord();


			/* --- Getters and setters --- */


			/* --- Instance function members --- */
			const char * serialize(int format) const;


			/* --- Class function members --- */
			static void initFieldList();


		private:
			/* --- Data members --- */


			/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
			static FieldOffsetsList _fieldList_;


			/* --- Constructors --- */


			/* --- Instance function members --- */
			void init();
	};


}



#endif /* FOOTERRECORD_H */
