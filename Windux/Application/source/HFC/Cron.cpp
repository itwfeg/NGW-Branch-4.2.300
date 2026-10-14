/* Dominator Back-room Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "Cron.h"
 
void Cron::setTaskNum(NumType& taskNum)
{
	this->property = property;
}
NumType& Cron::getTaskNum()
{
	return property;
}
void Cron::setTaskName(StringType& taskName)
{
	this->taskName = taskName;
}
StringType& Cron::getTaskName()
{
	return taskName;
}

