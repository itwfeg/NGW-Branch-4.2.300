/**
 * @file NGWUpgradeFinished.h
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Aaron Swift
 * @brief Notifies the UI to close the upgrade page
 * Created on: April 4, 2016
 *
 **/


#ifndef NGWUPGRADEFINISHED_H_
#define NGWUPGRADEFINISHED_H_
#include "EvolMessage.h"
#include "NGWEvent.h"

class NGWUpgradeFinished : public EvolMessage
{
	public:

	NGWUpgradeFinished( Hobart::CEvolMsgReceiver *pHobThread = NULL);
	NGWUpgradeFinished( const char *buffer );
	~NGWUpgradeFinished();

	const char *serialize()const;

	private:
};




#endif /* NGWUPGRADEFINISHED_H_ */
