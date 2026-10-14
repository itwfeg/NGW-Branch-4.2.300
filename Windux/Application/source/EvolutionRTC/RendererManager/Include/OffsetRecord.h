#ifndef OFFSETRECORD_H
#define OFFSETRECORD_H



#include <string>
#include "PAL_platformheader.h"


class OffsetRecord 
{
	public:
		OffsetRecord(const OffsetRecord&);
		OffsetRecord(INT16 line = 0, INT16 offset = 0);    
		~OffsetRecord();    


		INT16 getLine() const;
		void setLine(INT16 labelLines);

		INT16 getOffSet() const;
		void setOffSet(INT16 fieldOffset);


	private:    
		INT16 line;
		INT16 offset;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};



#endif //OFFSETRECORD_H
