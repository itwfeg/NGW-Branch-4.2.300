/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CMFILE_H
#define CMFILE_H

#include "evolution.h"

#include <fstream>

#include "CommMechanism.h"


/**
 * This is an abstraction that maps to the native socket lib.
 *
 * All sockets we will be using in the Dominator will be connection-based sockets.
 * Even though they are faster, we will not use any connectionless (datagram)
 * sockets because the packets are not guaranteed to arrive in the appropriate
 * sequence, and they are less reliable.
 */
class CMFile : public CommMechanism
{
	public:

		CMFile(const char *fName, short initialMode = CommMechanism::MODE_NONE);
		~CMFile();

		int cmSend(const char *msg, int length);
		int cmReceive(char *msgbuf);
		int cmReceive(HobString &msgbuf);
		int cmNumMsgs();
		void cmClose();
		void cmDelete();

		const char *toString();

		bool isHandlingLineSeparator();
		void setHandlingLineSeparator(bool handlingLineSeparator);

		void determineReaderWriter();


	protected:

		int fileHandle;

		HobString fileName;
		std::fstream *myFile;


	private:

		HobString toStringData;

		bool receivingBeginningOfRecord;

		bool handlingLineSeparator;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //CMFILE_H
