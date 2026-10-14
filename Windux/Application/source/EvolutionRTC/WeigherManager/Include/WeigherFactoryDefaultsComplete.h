/**
 * @file WeigherFactoryDefaultsComplete.h
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Aaron Swift
 * @brief Message for signaling the completion of saving weigher configuration defaults.
 * Created on: March 1, 2018
 */
#ifndef WEIGHERFACTORYDEFAULTSCOMPLETE_H
#define WEIGHERFACTORYDEFAULTSCOMPLETE_H
#include "EvolMessage.h"
#include "Weigher.h"
#include "CEvolMsgReceiver.h"

class WeigherFactoryDefaultsComplete : public EvolMessage
{
	public:
	WeigherFactoryDefaultsComplete(Hobart::CEvolMsgReceiver *pHobThread = NULL, bool saved = false );
	WeigherFactoryDefaultsComplete(const char *buffer);
	~WeigherFactoryDefaultsComplete();

	const char *serialize() const;

	bool getSavedStatus() const { return saved_; }
	void setSavedStatus( bool status_ ) { saved_ = status_; }
	private:
		bool saved_;
};



#endif
