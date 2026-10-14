/* Dominator Back-room Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CRONTASK_H
#define CRONTASK_H

class CronTask
{
	public:

	ActionPlan & getFunction();

	void setFunction(ActionPlan & function);

	private:
	ActionPlan function;
};

#endif //CRONTASK_H
