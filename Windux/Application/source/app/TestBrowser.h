/* Dominator Back-room Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef TESTBROWSER_H
#define TESTBROWSER_H

#include <fstream>

//#include "HobThread.h"
#include "Messenger.h"
#include "CEvolMsgReceiver.h"

class SessionMessage;
class Product;
class Socket;


class TestBrowser :  public HobThread
{
	public:

		TestBrowser(int conn, int form);
		TestBrowser(int conn, int form, char *fname);
		~TestBrowser();

		static int activeFunction(void *pObject);
		int activeFunctionTask();

		void setDelayAfterSend(int ticks);

		HobString hcn; // Used as the "header" when displaying debug messages.

		void handleMessage(SessionMessage *msg);	//	Manipulate the returning message.

		void handleWeigherMessage(SessionMessage *msg);
		void handleLabelApplierMessage(SessionMessage *msg);
		void handlePrinterMessage(SessionMessage *msg);
		void handleProductDevMessage(SessionMessage *msg);
		void handleTotalTypeDevMessage(SessionMessage *msg);
		void handleGraphicDevMessage(SessionMessage *msg);
		void handleConfigurationDevMessage(SessionMessage *msg);


	private:

		int connNum; // connection number (1 = primary)

		int format; // Browser or DG format.

		char *filename; // name of file to read commands from.
		ifstream *commandFile;

		char *rfilename; // name of file to write responses to.
		ofstream *responseFile;

		//HobString hcn; // Used as the "header" when displaying debug messages.

		Messenger *messenger;

		int scnt;
		int rcnt;

		int delayAfterSend;
		

		// Don't allow anyone to use the default constructor.
		TestBrowser();

		Socket *s; // Socket connection to my Session
		void connectToSessionManager();

		int start();
		int startNoFile();
		int startFile();

};

#endif //TESTBROWSER_H




