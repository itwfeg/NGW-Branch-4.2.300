#ifndef REPORT_H
#define REPORT_H

#include "PAL_platformheader.h"

#include <string>


class Report 
{
	public:
		// --- Constructors and destructor ---
		virtual ~Report() {}


		// --- Accessor methods ---
		virtual const std::string& getName() const = 0;


		// --- Processing methods ---
		// Performs any necessary initialization
		virtual void initialize() {}
		// Processes messages with needed information
		virtual void handleMessage(const char* UNUSED(buffer)) {}
		// Returns true only when the report has completed
		virtual bool doReport() { return true; }
		// Stops the generation of the report
		virtual void cancelReport() {}
};



#endif //REPORT_H

