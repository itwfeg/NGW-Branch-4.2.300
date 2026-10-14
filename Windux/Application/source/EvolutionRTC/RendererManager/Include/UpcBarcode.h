#ifndef UPCBARCODE_H
#define UPCBARCODE_H
#include "GenericBarcode.h"

class UpcBarcode : public GenericBarcode 
{

	public: 	/*class has no font information ats 06172002 */
		UpcBarcode(Hobart::Rectangle *bounds, INT16 modSize, INT16 rotation, INT16 charHeight, INT16 leading, INT16 faceMax, INT16 ascent); 
		~UpcBarcode();


	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif	//UPCBARCODE_H
