/* Dominator Back-room Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CronTask.h"
 
void CronTask::setFunction(ActionPlan& function)
{
	this->function = function;
}
ActionPlan& CronTask::getFunction()
{
	return function;
}

