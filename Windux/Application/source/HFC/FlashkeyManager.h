/* Dominator Back-room Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef FLASHKEYMANAGER_H
#define FLASHKEYMANAGER_H

#include "DataManager.h"
class Flashkey;

class FlashkeyManager : DataManager
{
	private:
	Vector flashkeyList;

	/**
	 * @supplierCardinality 0..*
	 * @clientCardinality 1
	 */
	Flashkey * lnkFlashkey;
};

#endif //FLASHKEYMANAGER_H



