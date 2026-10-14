/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef KEYACCESS_H
#define KEYACCESS_H

#include "evolution.h"
#include "HfcObject.h"
#include <list>

typedef enum AccessKeysFieldOffsets
{
	fieldOffsetSupervisorPermanent = 1,
	fieldOffsetSupervisorImmediate,
	fieldOffsetServicePermanent,
	endAccessKeysFieldOffsets
}   AccessKeysFieldOffsets_t;


class AccessKeys : public HfcObject
{
	public:
		enum Keys
		{
			SupervisorPermanent = 0x0001,
			SupervisorImmediate = 0x0002,
			ServicePermanent    = 0x0004,
			SelfServePermanent  = 0x0008,
			QATest              = 0x0010,
			MFGTest				= 0x0020
		};

		AccessKeys(int keys = 0);
		AccessKeys(const char *data, int len, int formatType);
		~AccessKeys();

		int getKeys() const;
		void setKeys(int keys);

		bool isSupervisorFunctionKeyPressed() const;
		bool isSupervisorKeyPresent() const;
		bool isServiceKeyPresent() const;
		bool isSelfServeKeyPresent() const;
		bool isQATestKeyPresent() const;
		bool isMFGTestKeyPresent() const;
		bool isAnyAccessKeyPresent() const;

		void setSupervisorFunctionKeyPressed(bool pressed);
		void setSupervisorKeyPresent(bool present);
		void setServiceKeyPresent(bool present);
		void setQAServiceKeyPresent(bool present);
		void setSelfServeKeyPresent(bool present);

		void operator|=(const int newKeys);
		void operator&=(const int newKeys);

		virtual const char *serialize(int format) const;

	private:
		int keys_;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};



#endif // KEYACCESS_H
