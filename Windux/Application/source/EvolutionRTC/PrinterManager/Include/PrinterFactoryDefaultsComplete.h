/**
 * @file PrinterFactoryDefaultsComplete.h
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Aaron Swift
 * @brief Message for signaling the completion of saving printer configuration defaults.
 * Created on: March 2, 2018
 */
#ifndef PRINTERFACTORYDEFAULTSCOMPLETE_H
#define PRINTERFACTORYDEFAULTSCOMPLETE_H

#include "EvolMessage.h"
#include "Printer.h"
#include "CEvolMsgReceiver.h"

class PrinterFactoryDefaultsComplete : public EvolMessage
{
	public:
	PrinterFactoryDefaultsComplete(Hobart::CEvolMsgReceiver *pHobThread = NULL, bool saved = false );
	PrinterFactoryDefaultsComplete(const char *buffer);
	~PrinterFactoryDefaultsComplete();

	const char *serialize() const;

	bool getSavedStatus() const { return saved_; }
	void setSavedStatus( bool status_ ) { saved_ = status_; }
	private:
		bool saved_;
};

#endif
