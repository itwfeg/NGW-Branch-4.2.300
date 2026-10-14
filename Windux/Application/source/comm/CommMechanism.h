/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef COMMMECHANISM_H
#define COMMMECHANISM_H

#include "CMReaderWriter.h"
class DataManager;

/* This class is pure virtual, you cannot instantiate one of these;
 * you can extend it to provide a communication mechanism through which components
 * of the Evolution "communicate".
 */
class CommMechanism
{
	public:
		CommMechanism() : myReaderWriter((CMReaderWriter *)NULL), mode(MODE_NONE), messageHandler((DataManager *)NULL), sendEmptyMessage(true) {};
		virtual ~CommMechanism() {};

		/* The CommMechanism API - Any class that is a CommMechanism needs
		 * to provide an implementation for these methods.
		 */
		virtual int cmSend(const char * msg, int length) = 0;
		//virtual int cmSend(const HobString &msg) = 0;
		virtual int cmReceive(char *msgbuf) = 0;
		virtual int cmReceive(HobString &msgbuf) = 0;
		virtual int cmNumMsgs() = 0;
		virtual void cmClose() = 0;

      //virtual int readByte() = 0;

		CMReaderWriter *getMyReaderWriter() {return myReaderWriter;};
		void setMyReaderWriter(CMReaderWriter *myReaderWriter) {this->myReaderWriter = myReaderWriter;};

		static const short MODE_NONE = 0x00;
		static const short MODE_INPUT = 0x01;
		static const short MODE_OUTPUT = 0x02;

		short getMode() {return mode;};
		void  setMode(short mode) {this->mode = mode;};
		bool isInputEnabled() {return (mode & MODE_INPUT) ? true : false;};
		bool isOutputEnabled() {return (mode & MODE_OUTPUT) ? true : false;};

		DataManager *getMessageHandler() {return messageHandler;};
		void setMessageHandler(DataManager *messageHandler) {this->messageHandler = messageHandler;};

		// FT-634: enable/disable the empty message
		bool getSendEmptyMessage() {return sendEmptyMessage;}
		void setSendEmptyMessage( bool value ) {sendEmptyMessage = value;}

	protected:

		CMReaderWriter *myReaderWriter;

		short mode;

		DataManager *messageHandler;

		bool sendEmptyMessage;
};

#endif //COMMMECHANISM_H

