/* Dominator Back-room Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef OPERATORACCESS_H
#define OPERATORACCESS_H

#include "Data.h"

/**
 * These are notes copied from CRC card:
 *
 * Business Rule: access level of 1 has no access to supervisor functions.
 *
 * what functions allowed(ACCESS_CODE)
 * LANGUAGE
 * Supervisor Functions
 * Service Functions
 *
 * The comments above are from the very initial Dominator design effort.
 *
 * This class is used to maintain what the operator can modify.
 * This data is stored with configuration (somewhere).
 *
 * This class was created so that we can configure what each operator access level can modify,
 * allowing the manager to set up each operator access level individually.
 * This is not a requirement.  The functionality required is the same as provided by the Ultima
 * (DPS-5.0.0.PP).  The data will be stored with configuration, but will still be loaded into this class
 * to allow flexibility in the future to enhance this functionality, by operator access level.
 *
 * Having this separate class will not adversely affect implementing this feature.
 */
static class OperatorAccess : Data
{
	public:

	typedef enum
	{
		PRICE_MODIFIABLE, BYCOUNT_MODIFIABLE, TARE_MODIFIABLE, SHELFLIFE_MODIFIABLE, LABELTYPE_MODIFIABLE,
				FIXEDWEIGHT_MODIFIABLE, PRODUCTLIFE_MODIFIABLE, DATETIME_MODIFIABLE
	}
	ITEM_LIST_ACTION;

	static int isAllowed(int accessCode, enum function);

	private:
	int accessLevel;
	bool priceModifiable;
	bool byCountModifiable;
	bool tareModifiable;
	bool shelfLifeModifiable;
	bool labelTypeModifiable;
	bool fixedWeightModifiable;
	bool productLifeModifiable;
	bool dateTimeModifiable;
};

#endif //OPERATORACCESS_H


