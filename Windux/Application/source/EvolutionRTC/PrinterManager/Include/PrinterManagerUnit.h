#ifndef PRINTERMANAGERUNIT_H
#define PRINTERMANAGERUNIT_H
#include "CEvolMsgReceiver.h"
#include "CHobartTimer.h"
#include "PrinterEvent.h"
#include "Printer.h"
#include "PrinterPrinterType.h"
#include "RS485Manager.h"
#include <string>


class PrinterManagerUnit : public Hobart::CEvolMsgReceiver, public Util::CRunnableClass {

	public:

	PrinterManagerUnit( Hobart::CEvolMsgReceiver *pMsgHandler );
	~PrinterManagerUnit();

	virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg );

	private:

	Hobart::CEvolMsgReceiver *pPrinter_;

	typedef enum
	{
		_UNKNOWN,
		_INIT,
		_WAIT,
		_PROCESS_LABEL,
		_WAITXFER,
		_END
	}UnitTestStates;

	UnitTestStates state_;
	Util::CMSGEvent m_ExitEvent;
	Util::CSmartPtr<Util::CTLSThread> unitTestThreadPrinter_;

	virtual DWORD Run();

	void handleMsg( PrEvent event, const char *pMsg, unsigned short size );
	void signalLabelStart( unsigned short size );
	void registerEvents();

	static const std::string labelImagePath_;
	static const std::string testLabel1_;
	static const std::string testLabel2_;
	static const std::string testLabel3_;

	static const size_t MAXBUFFERSIZE = 49056;		//4" of label data size for service counter print head
	static const int BITMAPHEADERSIZE = 54;
	//int fd_;

	RS485Manager 	*pRS485Manager_;

	unsigned char 	*pImage1_;
	unsigned char 	*pImage2_;
	unsigned char 	*pImage3_;

	unsigned int 	imageSize1_;
	unsigned int 	imageSize2_;
	unsigned int 	imageSize3_;


	Util::CSmartCriticalSection uLock_;

};
#endif
