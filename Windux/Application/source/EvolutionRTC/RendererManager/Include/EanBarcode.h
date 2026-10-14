#ifndef EANBARCODE_H
#define EANBARCODE_H
#include "GenericBarcode.h"

class EanBarcode : public GenericBarcode 
{

	public:
		EanBarcode(Hobart::Rectangle *bounds, INT16 modSize, INT16 rotation, INT16 charHeight, INT16 leading, INT16 faceMax, INT16 ascent);
		~EanBarcode();

		bool getValueFromEANTable(INT16 row, INT16 column);

private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};
#endif	//EANBARCODE_H
