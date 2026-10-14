#include "MessageLogger.h"
#include "Configuration.h"
#include "util/filesys/filename.h"
#include <PAL_platformheader.h>
#include "time.h"
#include <fstream>
#include <string>

using namespace std;



void MessageLogger::logMessage(LogFileIndicator logIndicator, const string &message)
{
	util_filesystem::Filename logFilename;
	logFilename.setPath(Configuration::getInstance()->toStringCfgI(CfgInternal::MESSAGELOGGINGDIRECTORY));
	if (logIndicator == StartupShutdownLog)
		logFilename.setFileName(Configuration::getInstance()->toStringCfgI(CfgInternal::MESSAGELOGGINGSTARTUPSHUTDOWNFILENAME));
	else if (logIndicator == ScaleEventsLog)
		logFilename.setFileName(Configuration::getInstance()->toStringCfgI(CfgInternal::MESSAGELOGGINGSCALEEVENTSFILENAME));
	else if (logIndicator == ScaleMaintenanceLog)
		logFilename.setFileName(Configuration::getInstance()->toStringCfgI(CfgInternal::MESSAGELOGGINGDATAMAINTENANCEFILENAME));
	else if (logIndicator == ScheduledEventsLog)
		logFilename.setFileName(Configuration::getInstance()->toStringCfgI(CfgInternal::MESSAGELOGGINGSCHEDULEDEVENTSFILENAME));
	else if (logIndicator == InfoLog)
		logFilename.setFileName(Configuration::getInstance()->toStringCfgI(CfgInternal::MESSAGELOGGINGINFOFILENAME));
	else if (logIndicator == WarningsLog)
		logFilename.setFileName(Configuration::getInstance()->toStringCfgI(CfgInternal::MESSAGELOGGINGWARNINGSFILENAME));
	else if (logIndicator == ErrorsLog)
		logFilename.setFileName(Configuration::getInstance()->toStringCfgI(CfgInternal::MESSAGELOGGINGERRORSFILENAME));
	else
		return;

    time_t currTime;
    struct tm currTimeParts;
    time(&currTime);
    currTimeParts = *localtime(&currTime);
    char timeStr[30];
    strcpy(timeStr, asctime(&currTimeParts));
    char *ptr = timeStr + strlen(timeStr) - 1;
    if (*ptr == '\n')
        *ptr = '\0';

	std::ostream *outFile = new std::ofstream(logFilename.getFullPath().c_str(), std::ios_base::app);
	*outFile << timeStr << "  " << ((message.size()) ? message.c_str() : "") << std::endl;
    outFile->flush();
    // we should free up this memory
    delete outFile;
}


