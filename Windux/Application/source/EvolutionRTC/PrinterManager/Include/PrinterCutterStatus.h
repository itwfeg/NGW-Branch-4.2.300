/**
 * @file PrinterCutterStatus.h
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief status of cutter home switch
 * Created on: Nov 9, 2017
 *
 **/
#ifndef PRINTERCUTTERSTATUS_H
#define PRINTERCUTTERSTATUS_H
#include "PrinterGeneric.h"

class PrinterCutterStatus : public PrinterGeneric
{
	public:
		PrinterCutterStatus( Hobart::CEvolMsgReceiver *pHobThread = NULL, PrinterType printerType = FirstPrinter,
						    bool home_ = false, bool installed = false, bool interlock = false, bool jammed = false);
		PrinterCutterStatus( const char *buffer );
		~PrinterCutterStatus();

		const char *serialize() const;


		bool getHome() const { return home; }
		void setHome( bool home_ ) { home = home_; }

		bool getInstalled() const { return installed; }
		void setInstalled( bool installed_ ) { installed = installed_; }

		bool getInterlock() const { return interlock; }
		void setInterlock( bool interlock_ ) { interlock = interlock_; }

		bool getJammed() const { return jammed; }
		void setJammed( bool jammed_ ) { jammed = jammed_; }
	
		void show() const;

	private:
	    bool  home;
		bool  installed;
		bool  interlock;
		bool  jammed;
};
#endif
