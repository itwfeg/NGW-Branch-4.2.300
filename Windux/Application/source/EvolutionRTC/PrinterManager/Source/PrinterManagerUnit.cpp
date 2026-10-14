/**
 * @file PrinterManagerUnit.cpp
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Aaron Swift
 * @brief Unit testing of new KPC hardware
 * Created on: Jan 11, 2018
 */
#include "PrinterManagerUnit.h"
#include "PrinterEvent.h"
#include "EvolMessage.h"
#include "PrinterTeachTable.h"
#include "PrinterWakeup.h"
#include "PrinterConfiguration.h"
#include "PrinterInquire.h"
#include "PrinterStatus.h"
#include "PrinterMode.h"
#include "PrinterCommand.h"
#include "PrinterNomSize.h"
#include "PrinterRam.h"
#include "PrinterTest.h"
#include "PrinterTeach.h"
#include "PrinterMask.h"
#include "PrinterRequestSensors.h"
#include "PrinterRequestLabelTransfer.h"
#include "PrinterSensors.h"
#include "PrinterFlush.h"
#include "PrinterHeadWear.h"
#include "PrinterGeneric.h"
#include <iostream>
#include <fstream>
#include "Debug.h"
#include "Hex.h"
#include "Integer.h"
#include <string>


using namespace std;

const std::string PrinterManagerUnit::labelImagePath_ 			= "/usr/local/hobart/labels/";
const std::string PrinterManagerUnit::testLabel1_				= "labelImage3.bmp";
const std::string PrinterManagerUnit::testLabel2_				= "labelImage2.bmp";
const std::string PrinterManagerUnit::testLabel3_				= "labelImage3.bmp";


///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterManagerUnit::PrinterManagerUnit( Hobart::CEvolMsgReceiver *pMsgHandler ) : Hobart::CEvolMsgReceiver("PrinterManagerUnit")
{
	Debug::trace( "PrinterManagerUnit", "PrinterManagerUnit()" );

	pPrinter_ = pMsgHandler;

	state_ = _UNKNOWN;



#if defined ( __linux__ )

	//PrinterGeneric msg( PRREQHEADDOTWEAR, this, FirstPrinter );
	//dispatchEvent( &msg );


	//open label data channel to printer
	pRS485Manager_ = RS485Manager::getInstance();
	if( !pRS485Manager_ ) {
		Debug::trace( "PrinterManagerUnit", "Critical error: unable to open RS485 channel to printer!" );
	} else {

		std::string file = labelImagePath_ + testLabel1_;
		long fileSize = 0;

		//read test label images into image buffers
		ifstream file_( file.data(), ios::binary );
		if( file_.is_open() ) {
			// determine size of file
			file_.seekg( 0, ios::end );
			fileSize = file_.tellg();

			//calculate the size of our transfer
			imageSize1_ = fileSize - BITMAPHEADERSIZE;
			Debug::trace( "PrinterManagerUnit", "Image 1 size: " + Integer::toString( imageSize1_ ));

			if( fileSize > 0 ) {
				//allocate space for our label images
				pImage1_ = new unsigned char[fileSize];
				if( pImage1_ ) {
					//back to the start of file
					file_.seekg( 0, ios::beg );
					// read whole file
					file_.read( (char *)pImage1_ , fileSize );
				}
			} else {
				Debug::trace( "PrinterManagerUnit", "Critical error file size is 0: " + file );
			}
		} else {
			Debug::trace( "PrinterManagerUnit", "Critical error: could not open file " + file );
		}
		file_.close();

		file = labelImagePath_ + testLabel2_;

		ifstream file1_( file.data(), ios::binary );
		if( file1_.is_open() ) {
			// determine size of file
			file1_.seekg( 0, ios::end );
			fileSize = file1_.tellg();

			//calculate the size of our transfer
			imageSize2_ = fileSize - BITMAPHEADERSIZE;
			Debug::trace( "PrinterManagerUnit", "Image 2 size: " + Integer::toString( imageSize2_ ));

			if( fileSize > 0 ) {
				//allocate space for our label images
				pImage2_ = new unsigned char[fileSize];
				if( pImage2_ ) {
					//back to the start of file
					file1_.seekg( 0, ios::beg );
					// read whole file
					file1_.read( (char *)pImage2_ , fileSize );
				}
			} else {
				Debug::trace( "PrinterManagerUnit", "Critical error file size is 0: " + file );
			}
		} else {
			Debug::trace( "PrinterManagerUnit", "Critical error: could not open file " + file );
		}
		file1_.close();

		file = labelImagePath_ + testLabel3_;
		ifstream file2_( file.data(), ios::binary );
		if( file2_.is_open() ) {
			// determine size of file
			file2_.seekg( 0, ios::end );
			fileSize = file2_.tellg();

			//calculate the size of our transfer
			imageSize3_ = fileSize - BITMAPHEADERSIZE;

			Debug::trace( "PrinterManagerUnit", "Image 3 size: " + Integer::toString( imageSize3_ ));

			if( fileSize > 0 ) {
				//allocate space for our label images
				pImage3_ = new unsigned char[fileSize];
				if( pImage3_ ) {
					//back to the start of file
					file2_.seekg( 0, ios::beg );
					// read whole file
					file2_.read( (char *)pImage3_ , fileSize );
				}
			} else {
				Debug::trace( "PrinterManagerUnit", "Critical error file size is 0: " + file );
			}
		} else {
			Debug::trace( "PrinterManagerUnit", "Critical error: could not open file " + file );
		}
		file2_.close();
	}

	registerEvents();

	//int i = BITMAPHEADERSIZE + 8;
	int i = 0;
	std::string str = "";
	while( i < imageSize1_ ) {
		std::string s = "0x";
		s.append( Hex::toString( pImage1_[i++] ) );
		s.append( "," );
		str.append(s);
	}
	//Debug::trace( "PrinterManagerUnit", "file: " +  str );
	unitTestThreadPrinter_  = new Util::CTLSThread( *this, 1, 0, "PrinterManager Thread" );
#endif
	Debug::trace( "PrinterManagerUnit", "PrinterManagerUnit() -- Exit" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterManagerUnit::~PrinterManagerUnit()
{

	if( pRS485Manager_ ) {
		delete pRS485Manager_;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterManagerUnit::registerEvents()
{
	registerEvent(Printer, PrLabelTransferReady);
	registerEvent(Printer, PrWakeup );
	registerEvent(Printer, PrStatus );
	registerEvent(Printer, PrLabelTransferComplete );
	registerEvent(Printer, PrLabelTransferFailed );
	registerEvent(Printer, PrHeadDotWear );

}

///////////////////////////////////////////////////////////////////////////////////////////////////
DWORD PrinterManagerUnit::Run()
{
	state_ = _INIT;
	Debug::trace( "PrinterManagerUnit", "Run() -- Enter" );

	while( !m_ExitEvent.IsEventSet() ) {
		switch( state_ )
		{
			case _INIT: {
				Debug::trace( "PrinterManagerUnit", "Run() -- _INIT" );

				signalLabelStart( imageSize1_ );

				state_ = _WAIT;
				break;
			}
			case _WAIT: {
				break;
			}
			case _PROCESS_LABEL: {
				Debug::trace( "PrinterManagerUnit", "Run() -- _PROCESS_LABEL" );
#if 0
				//skip the header information in the bitmap image.
				//unsigned short *pImage_ = (unsigned short *)(pImage1_ + BITMAPHEADERSIZE + 8 );
				unsigned short *pImage_ = (unsigned short *)(pImage1_ + 4048 );
				Debug::trace( "PrinterManagerUnit", "assignLabelImage() image size: " + Integer::toString( imageSize1_ ) );
				pRS485Manager_->assignLabelImage( FirstPrinter, (unsigned char *)pImage_, ( imageSize1_- 4048 ), 4090 );
#endif
				state_ = _WAITXFER;
				break;
			}
			case _WAITXFER: {
				break;
			}
			case _END: {
				break;
			}
		}
		PAL_absclass::PAL_sleep( 1 );
	}
	Debug::trace( "PrinterManagerUnit", "Run() -- Exit" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterManagerUnit::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{
	Util::CLockCriticalSection lock( uLock_ );

	if ( msg.get() != NULL )
	{
		switch (msg->getDeviceType())
		{
			case Printer:
				handleMsg((PrEvent) msg->getEventType(), msg->Data(), (unsigned short)msg->Length() );
				break;
			case PrinterSystem:
				break;
			case Renderer:
				break;
			default:
				break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterManagerUnit::handleMsg( PrEvent event, const char *pMsg, unsigned short size )
{
	switch( event )
	{
		case PrWakeup: {
			Debug::trace( "PrinterManagerUnit", "handleMsg() PrWakeup" );

			break;
		}
		case PrStatus: {
			Debug::trace( "PrinterManagerUnit", "handleMsg() PrStatus" );
			break;
		}
		case PrLabelTransferReady: {
			Debug::trace( "PrinterManagerUnit", "handleMsg() PrLabelTransferReady" );
			state_ = _PROCESS_LABEL;
			break;
		}
		case PrLabelTransferComplete: {
			Debug::trace( "PrinterManagerUnit", "handleMsg() PrLabelTransferComplete" );
			break;
		}
		case PrLabelTransferFailed: {
			Debug::trace( "PrinterManagerUnit", "handleMsg() PrLabelTransferFailed" );
			break;
		}
		case PrHeadDotWear: {
			Debug::trace( "PrinterManagerUnit", "handleMsg() PrHeadDotWear" );
			PrinterHeadWear wear( pMsg );
			wear.show();
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterManagerUnit::signalLabelStart( unsigned short size  )
{
	Debug::trace( "PrinterManagerUnit", "signalLabelStart() " + Integer::toString( size ) );
	unsigned short temp;
	//////////////////////////////////////////////////////////////////////
	// Note: printer flush is no longer needed since we are using RS485
	// instead request a label transfer and wait for PrLabelTransferReady
	// from printer before image transfer
	//////////////////////////////////////////////////////////////////////
	PrinterRequestLabelTransfer reqTransfer( this, FirstPrinter, size, 0, &temp  );
	pPrinter_->send( &reqTransfer );
}
