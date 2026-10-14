/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCERRORGRAPHIC_H
#define HFCERRORGRAPHIC_H

#include "evolution.h"
#include "HfcObject.h"
#include "Graphic.h"
#include "HfcError.h"

typedef enum HfcErrorGraphicOffsets
{
	fieldOffsetHfcErrGraphicIndex = endHfcErrorFieldOffsets,
	fieldOffsetHfcErrGraphicName,
	endHfcErrorGraphicOffsets
}   HfcErrorGraphicOffsets_t;

class HfcErrorGraphic : public HfcError
{
	public:

		HfcErrorGraphic();
		HfcErrorGraphic(long code, short style, Graphic *graphic);

		~HfcErrorGraphic();

		virtual const char *serialize(int format) const;


	private:
		Graphic *graphic;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HFCERRORGRAPHIC_H




