#ifndef EAN128CBARCODE_H
#define EAN128CBARCODE_H
#include "GenericBarcode.h"

class Ean128CBarcode : public GenericBarcode 
{

	public:
		Ean128CBarcode(Hobart::Rectangle *bounds, INT16 modSize, INT16 rotation, INT16 charHeight, INT16 leading, INT16 faceMax, INT16 ascent);
		~Ean128CBarcode();

		UINT16 getValueFromEANTable(INT16 index);

	private:
		UINT16 * EAN128CModCodes;
	
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};
#endif	//EANBARCODE_H
