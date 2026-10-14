/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */


#ifndef NETWORKERRORREPORT_H
#define NETWORKERRORREPORT_H


#include "Report.h"


class Session;
class DBManager;


class NetworkErrorReport : public Report
{
	public:
		// --- Constructors and destructor ---
		NetworkErrorReport(Session* session);
		~NetworkErrorReport();


		// --- Accessor methods ---
		const std::string& getName() const { return name_; }


		// --- Processing methods ---
		// doReport returns true only when the report has completed
		bool doReport();
		void cancelReport();


	private:
		// --- Object data members ---
		string name_;
		int currentRecord_;
		DBManager* database_;
		Session* mySession_;
};



#endif // NETWORKERRORREPORT_H

