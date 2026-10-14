/* Dominator Back-room Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SUPERVISORFUNCTIONS_H
#define SUPERVISORFUNCTIONS_H

#include "Data.h"

/**
 * See function code list in USM pg 2-5.
 *
 * Keep function codes the same.
 *
 * Id to disable id.
 */
class SupervisorFunctions : Data
{
	private:
	int functionList;
	int accessLevel;
};

#endif //SUPERVISORFUNCTIONS_H
