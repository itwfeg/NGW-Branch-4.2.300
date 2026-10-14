#ifndef UPCD3BARCODE_H
#define UPCD3BARCODE_H

#include "GenericBarcode.h"

class UpcD3Barcode : public GenericBarcode
{
	public:
		UpcD3Barcode(Hobart::Rectangle *bounds, INT16 modSize, INT16 rotation);
		~UpcD3Barcode();

		bool isBarcodeBad();

		void createHiSide(INT16 modSize, INT16 rotation);
		void createLowSide(INT16 modSize, INT16 rotation);
	
	private:
		void setupHiSide(INT16 modSize, INT16 rotation);
		void setupLowSide(INT16 modSize, INT16 rotation);

		Hobart::Rectangle *hiRect; 
		Hobart::Rectangle *lowRect;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif	//UPCD3BARCODE_H
