/* Fastrack Scale Project
 * COPYRIGHT (c) 2013.  All Rights Reserved.
 * ITWFEG - Hobart. Troy, Ohio  USA
 */
 /* Modified 9/13/2013 for use on the Windux common code project
 */
#ifndef MESSAGELOGGER_H
#define MESSAGELOGGER_H

#include "../db/util/e_string/utilstring.h"
#include "string.h"


class MessageLogger
{
    public:

		typedef enum LogFileIndicator
		{
			InfoLog,
			WarningsLog,
			ErrorsLog,
			StartupShutdownLog,
			ScaleMaintenanceLog,	// Added for Windux's Remote Scale Management feature
			ScaleEventsLog,			// Added for Windux's Remote Scale Management feature
			ScheduledEventsLog,		// Added for Windux's Remote Scale Management feature
			endLogFileIndicator
		} LogFileIndicator_t;

		MessageLogger() {}
        
		static void logMessage(LogFileIndicator logIndicator, const std::string &message);
            
	private:
};

#endif //MESSAGELOGGER_H
