/* Dominator Back-room Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CRON_H
#define CRON_H

#include "NumType.h"
#include "StringType.h"

class CronTask;

class Cron
{
	public:

	NumType & getTaskNum();

	void setTaskNum(NumType & taskNum);

	StringType & getTaskName();

	void setTaskName(StringType & taskName);

	private:
	NumType taskNum;
	StringType taskName;

	/**
	 * @supplierCardinality 0..*
	 * @clientCardinality 1
	 * @clientRole consists of
	 */
	CronTask * lnkCronTask;
};

#endif //CRON_H
