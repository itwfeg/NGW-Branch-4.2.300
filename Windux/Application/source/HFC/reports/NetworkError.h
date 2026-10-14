/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */


#ifndef NETWORKERROR_H
#define NETWORKERROR_H



#include "HfcObject.h"



class DBManager;



class NetworkError : public HfcObject
{
	public:
		// --- Constructors and destructor ---
		NetworkError() {}
		~NetworkError() {}


		// --- Processing methods ---


		// Class methods
		static NetworkError* findNetworkError(int number, DBManager* dbm) { return 0; }


	private:
};



#endif // NETWORKERROR_H

