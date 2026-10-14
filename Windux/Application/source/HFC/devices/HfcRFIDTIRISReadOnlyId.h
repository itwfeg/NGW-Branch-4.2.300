/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCRFIDTIRISREADONLYID_H
#define HFCRFIDTIRISREADONLYID_H

#include "evolution.h"
#include "TIRISReadOnlyId.h"
#include "HfcObject.h"

class HfcRFIDTIRISReadOnlyId : public TIRISReadOnlyId, public HfcObject
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		// Added the RFID module sofware version to this message, FHA 3/13
		//HfcRFIDTIRISReadOnlyId(Hobart::CEvolMsgReceiver *pHobThread = NULL, UINT8 *currentId = 0, UINT32 size = 0);
		HfcRFIDTIRISReadOnlyId(Hobart::CEvolMsgReceiver *pHobThread = NULL, 
							   UINT8 *currentId = 0, UINT32 size = 0,
							   UINT8 swVersionRFID = 0);
		HfcRFIDTIRISReadOnlyId(Hobart::CEvolMsgReceiver *pHobThread, std::string currentId);
		HfcRFIDTIRISReadOnlyId(const char *data, int len, int formatType);

		virtual ~HfcRFIDTIRISReadOnlyId();
		 
		virtual const char *serialize(int format) const;

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HFCRFIDTIRISREADONLYID_H
