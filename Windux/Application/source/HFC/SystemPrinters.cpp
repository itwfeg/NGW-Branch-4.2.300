/* HT Series Scale Project
 * COPYRIGHT (c) 2015.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "SystemPrinters.h"
#include "Configuration.h"
#include "PythonScripter.h"
#include "PAL_platformheader.h"
#include "HobList.h"
#include "HobTokenizer.h"
#include "HobTokenizerCSV.h"
#include "HobString.h"
#include "Integer.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int SystemPrinters::dataType = 0;
std::string SystemPrinters::dataTypeName("SystemPrinters"); 
#endif

// initialize statics
HobList *SystemPrinters::systemPrinterNames_ = 0;
HobList *SystemPrinters::systemPrinterDriverNames_ = 0;


SystemPrinters::SystemPrinters()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

SystemPrinters::~SystemPrinters()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


//static
int SystemPrinters::setupUSBZebraPrinter( std::string printerUserName )
{
	if ( Configuration::getInstance()->toBoolCfgI(CfgInternal::PYTHONENABLED) )
	{
		std::string cmd( PythonScripter::buildPythonCall( "setup_zebra.py" ) );
		cmd.append(" ").append(printerUserName);
		Debug::trace( "SystemPrinters", "calling " + cmd );

		std::string out;
		int rc( PAL_absclass::PAL_executeWithResults( cmd, out ) );

		Debug::trace( "SystemPrinters", "output of call to setup_zebra.py: " + Integer::toString( rc ) + string( " - " ) +  out );
		return rc;
	}
	return -1;
}


//static
int SystemPrinters::printToUSBLabelPrinter( const std::string filename, int numOfLabels, int xOffset, int yOffset )
{
	Configuration *cfg = Configuration::getInstance();
	if (!cfg)
		return -1;

	// Printing to the Zebra printer, use the python script.
	if ( !filename.empty() && cfg->toBoolCfgI(CfgInternal::PYTHONENABLED) )
	{
		std::string xOffsetStr, yOffsetStr;
		if ( xOffset == -1 )
			xOffsetStr = cfg->toStringCfgI(CfgInternal::USBPRINTERXCOORDINATE);
		else
			xOffsetStr = Integer::toString(xOffset);

		if ( yOffset == -1 )
			yOffsetStr = cfg->toStringCfgI(CfgInternal::USBPRINTERYCOORDINATE);
		else
			yOffsetStr = Integer::toString(yOffset);

		std::string cmd = PythonScripter::buildPythonCall("printZebraLabel.py");
		cmd.append(" ").append(filename);
		cmd.append(" ").append(xOffsetStr);
		cmd.append(" ").append(yOffsetStr);
		cmd.append(" ").append(Integer::toString(numOfLabels));
		Debug::trace( "SystemPrinters", "printToUSBLabelPrinter() calling " + cmd );

		std::string out;
		int rc( PAL_absclass::PAL_executeWithResults(cmd, out) );

		Debug::trace("SystemPrinters", "output from call: " + Integer::toString( rc ) + string( " - " ) +  out );
		return rc;
	}
	return -1;
}


std::string SystemPrinters::determineGhostscriptDevice( std::string printerSystemName )
{
	std::string systemPrinterDevice = Configuration::getInstance()->toStringCfgI(CfgInternal::DEFAULTGHOSTSCRIPTDEVICE);
	if ( systemPrinterDevice.empty() )
		systemPrinterDevice = "laserjet";

	std::string gsDevices = Configuration::getInstance()->toStringCfgI(CfgInternal::GHOSTSCRIPTDEVICES);
	if ( !gsDevices.empty() )
	{
		HobList gsDevicesList;
		gsDevicesList.fillListFromCSVString( gsDevices );

		if ( !systemPrinterNames_ )
			SystemPrinters::findAllSystemPrinters();

		if ( systemPrinterNames_ )
		{
			int position = systemPrinterNames_->getElementPosition(printerSystemName);
			Debug::trace("SystemPrinters", "printPdfFile - printerSystemName: " + printerSystemName + "  listPosition: " + Integer::toString(position) );
			if ( position >= 0 )
			{
				std::string gsDevice = gsDevicesList.getElementAtPosition(position);
				if ( !gsDevice.empty() )
				{
					systemPrinterDevice = gsDevice;
					Debug::trace("SystemPrinters", "printPdfFile - found printerSystemName's gsDevice: " + gsDevice);
				}
			}
		}
	}

	return systemPrinterDevice;
}


int SystemPrinters::printToNetworkReportPrinter( std::string printerUserName, std::string filename )
{
#if defined (__linux__)

	// lp -d printerUserName filename
	Debug::trace( "SystemPrinters", "printToNetworkReportPrinter() - printerUserName:" + printerUserName + "  filename:" + filename );

	if ( !printerUserName.empty() && !filename.empty() )
	{
		std::string cmd( "lp -d " );
		cmd.append(printerUserName).append(" ").append(filename);
		Debug::trace( "SystemPrinters", "printToNetworkReportPrinter() calling " + cmd );

		std::string out;
		int rc( PAL_absclass::PAL_executeWithResults( cmd, out ) );

		Debug::trace( "SystemPrinters", "output from call: " + Integer::toString( rc ) + string( " - " ) +  out );
		return rc;
	}
	return -1;

#else
	;
#endif
	return -1;
}


int SystemPrinters::printPdfFile( std::string printerUserName, std::string printerSystemName, std::string filename )
{
#if defined (__linux__)

	// Printing PDF file using ghostscript:
	// gsc -dSAFER -dNOPAUSE -sDEVICE=laserjet -sOutputFile=\|lpr pdf-test.pdf
	// gsc -dSAFER -dNOPAUSE -sDEVICE=laserjet -sOutputFile=\|'lp -d printerUserName' pdf-test.pdf
	Debug::trace( "SystemPrinters", "printPdfFile() - printerUserName:" + printerUserName + "  printerSystemName:" + printerSystemName + "  filename:" + filename );

	if ( !printerUserName.empty() && !filename.empty() )
	{
		std::string gsDeviceStr = SystemPrinters::determineGhostscriptDevice( printerSystemName );

		std::string cmd("gsc -dSAFER -dNOPAUSE -sDEVICE=");
		cmd.append(gsDeviceStr).append(" -sOutputFile=\\|'lp -d ").append(printerUserName).append("' ").append(filename);
		Debug::trace( "SystemPrinters", "printPdfFile() calling " + cmd );

		std::string out;
		int rc( PAL_absclass::PAL_executeWithResults( cmd, out ) );

		Debug::trace( "SystemPrinters", "output from call: " + Integer::toString( rc ) + string( " - " ) +  out );
		return rc;
	}

#else
	;
#endif
	return -1;
}


int SystemPrinters::printDocFile( std::string printerUserName, std::string printerSystemName, std::string filename )
{
#if defined (__linux__)

	// To print DOCX files:
	// Use libreoffice to convert the doc file to pdf with the following:
	// ex: libreoffice --headless -convert-to pdf Timesys_Factory_Creation.docx Timesys_Factory_Creation.pdf
	Debug::trace( "SystemPrinters", "printDocFile() - printerUserName:" + printerUserName + "  printerSystemName:" + printerSystemName + "  filename:" + filename );

	if ( !printerUserName.empty() && !filename.empty() )
	{
		File sourceFile(filename);
		if ( !sourceFile.exists() )
			return -1;
		File destFile(sourceFile);
		destFile.setPath("/tmp");
		destFile.setFileNameExtension("pdf");

		HobString ext = sourceFile.getFileNameExtension();

		std::string cmd( "libreoffice --headless -convert-to pdf " );
		cmd.append(sourceFile.getFullPath()).append(" ").append(destFile.getFullPath());
		Debug::trace( "SystemPrinters", "printDocFile() calling " + cmd );

		std::string out;
		int rc = PAL_absclass::PAL_executeWithResults( cmd, out );
		Debug::trace( "SystemPrinters", "output from call: " + Integer::toString( rc ) + string( " - " ) +  out );

		if (rc == 0)
		{
			rc = SystemPrinters::printPdfFile( printerUserName, printerSystemName, destFile.getFullPath() );
		}

		Debug::trace( "SystemPrinters", "output from call to printPdfFile: " + Integer::toString( rc ) );
		return rc;
	}
	return -1;

#else
	;
#endif
	return -1;
}


int SystemPrinters::addUSBZebraPrinterToSystem( std::string printerUserName )
{
#if defined (__linux__)

	// lpinfo -v
	// lpadmin -p printerUserName -E -v "socket://ipAddress -P printerDriverName
	Debug::trace( "SystemPrinters", "addUSBZebraPrinterToSystem() - printerUserName:" + printerUserName );

	std::string lpInfoCommand = "lpinfo -v";
	std::string lpInfoOutput;
	PAL_absclass::PAL_executeWithResults(lpInfoCommand, lpInfoOutput);
	Debug::trace("SystemPrinters", "lpinfo results: " + lpInfoOutput);

	HobTokenizer tok(&lpInfoOutput, '\n');
	HobString zebraPrinterName, token;
	while ( tok.next(&token) != -1 )
	{
		Debug::trace("SystemPrinters", "Searching for Zebra printer in: " + token);
		if ( token.find("direct") != std::string::npos && token.find("Zebra") != std::string::npos )
		{
			Debug::trace("SystemPrinters", "Is a direct printer: " + token);
			HobTokenizer tok2(&token, ' ', 1);
			HobString connectionName;
			tok2.next(&connectionName);
			tok2.next(&zebraPrinterName);
			zebraPrinterName.stripWhiteSpace(HobString::LEADING_TRAILING);
			zebraPrinterName.stripChar(',');
			zebraPrinterName.stripChar('\n');
			Debug::trace("SystemPrinters", "printer - driver: " + connectionName + "  printerName: " + zebraPrinterName);
		}
	}

	std::string zebraPrinterDriverName = Configuration::getInstance()->toStringCfgI(CfgInternal::NETWORKPRINTERDRIVERSDIRECTORY);
	zebraPrinterDriverName.append( Configuration::getInstance()->toStringCfgI(CfgInternal::KNOWNUSBPRINTERDRIVERS) );

	if ( !printerUserName.empty() && !zebraPrinterName.empty() && !zebraPrinterDriverName.empty() )
	{
		std::string cmd( "lpadmin -p " );
		cmd.append(" ").append(printerUserName).append(" -E -v \"").append(zebraPrinterName).append("\" -P ").append(zebraPrinterDriverName);
		Debug::trace( "SystemPrinters", "addUSBZebraPrinterToSystem() calling " + cmd );

		std::string out;
		int rc( PAL_absclass::PAL_executeWithResults( cmd, out ) );

		Debug::trace( "SystemPrinters", "output from call: " + Integer::toString( rc ) + string( " - " ) +  out );

		SystemPrinters::setDefaultPrinter( printerUserName );

		return rc;
	}
	return -1;

#else
	;
#endif
	return -1;
}


int SystemPrinters::addNetworkPrinterToSystem( std::string printerUserName, std::string ipAddress, std::string printerSystemName )
{
#if defined (__linux__)

	// lpadmin -p printerUserName -E -v "socket://ipAddress" -P printerDriverName
	Debug::trace( "SystemPrinters", "addNetworkPrinterToSystem() - printerUserName:" + printerUserName + "  ipaddress:" + ipAddress + "  printerSystemName:" + printerSystemName );

	if ( !printerUserName.empty() && !ipAddress.empty() && !printerSystemName.empty() )
	{
		std::string driverName = SystemPrinters::findSystemPrinterDriverForPrinter(printerSystemName);
		Debug::trace( "SystemPrinters", "addNetworkPrinterToSystem() - driverName:" + driverName );
		if ( !driverName.empty() )
		{
			std::string cmd( "lpadmin -p " );
			cmd.append(" ").append(printerUserName).append(" -E -v \"socket://").append(ipAddress).append("\" -P ").append(driverName);
			Debug::trace( "SystemPrinters", "addNetworkPrinterToSystem() calling " + cmd );

			std::string out;
			int rc( PAL_absclass::PAL_executeWithResults( cmd, out ) );

			Debug::trace( "SystemPrinters", "output from call: " + Integer::toString( rc ) + string( " - " ) +  out );

			if ( driverName.find("zebra") != std::string::npos )
				SystemPrinters::setDefaultPrinter( printerUserName );

			return rc;
		}
	}
	return -1;

#else
	;
#endif
	return -1;
}


int SystemPrinters::removePrinterFromSystem( std::string printerUserName )
{
#if defined (__linux__)

	// lpadmin -x printerUserName
	Debug::trace( "SystemPrinters", "removePrinterFromSystem() - printerUserName:" + printerUserName );

	if ( !printerUserName.empty() )
	{
		std::string cmd( "lpadmin -x " );
		cmd.append(printerUserName);
		Debug::trace( "SystemPrinters", "removePrinterFromSystem() calling " + cmd );

		std::string out;
		int rc( PAL_absclass::PAL_executeWithResults( cmd, out ) );

		Debug::trace( "SystemPrinters", "output from call: " + Integer::toString( rc ) + string( " - " ) +  out );
		return rc;
	}
	return -1;

#else
	;
#endif
	return -1;
}


int SystemPrinters::setDefaultPrinter( std::string printerUserName )
{
#if defined (__linux__)

	// lpoptions -d printerUserName
	Debug::trace( "SystemPrinters", "setDefaultPrinter() - printerUserName:" + printerUserName );

	if ( !printerUserName.empty() )
	{
		std::string cmd( "lpoptions -d " );
		cmd.append(printerUserName);
		Debug::trace( "SystemPrinters", "setDefaultPrinter() calling " + cmd );

		std::string out;
		int rc( PAL_absclass::PAL_executeWithResults( cmd, out ) );

		Debug::trace( "SystemPrinters", "output from call: " + Integer::toString( rc ) + string( " - " ) +  out );
		return rc;
	}
	return -1;

#else
	;
#endif
	return -1;
}


std::string SystemPrinters::getUsbPrinterNamesAsList()
{
	// These printers are in internal config, not currently read from the OS

	HobList printerNamesList;
	printerNamesList.fillListFromCSVString( Configuration::getInstance()->toStringCfgI(CfgInternal::KNOWNUSBPRINTERS) );
	return printerNamesList.serialize(BrowserFormat);
}


std::string SystemPrinters::getUsbPrinterNamesAsString()
{
	// These printers are in internal config, not currently read from the OS
	std::string usbPrintersString = Configuration::getInstance()->toStringCfgI(CfgInternal::KNOWNUSBPRINTERS);
	return usbPrintersString;
}


std::string SystemPrinters::getSystemPrinterNamesAsList()
{
	if ( !systemPrinterNames_ )
		SystemPrinters::findAllSystemPrinters();

	return (systemPrinterNames_) ? systemPrinterNames_->serialize(BrowserFormat) : "";
}


std::string SystemPrinters::getSystemPrinterDriverNamesAsList()
{
	if ( !systemPrinterDriverNames_ )
		SystemPrinters::findAllSystemPrinters();

	return (systemPrinterDriverNames_) ? systemPrinterDriverNames_->serialize(BrowserFormat) : "";
}


std::string SystemPrinters::findSystemPrinterDriverForPrinter( std::string printerName )
{
	Debug::trace( "SystemPrinters", "findSystemPrinterDriverForPrinter() - printerName:" + printerName );

	if ( !systemPrinterNames_ || !systemPrinterDriverNames_ )
		SystemPrinters::findAllSystemPrinters();

	std::string printerDriverName = Configuration::getInstance()->toStringCfgI(CfgInternal::NETWORKPRINTERDRIVERSDIRECTORY);

	if ( systemPrinterNames_ && systemPrinterDriverNames_ )
	{
		int position = systemPrinterNames_->getElementPosition(printerName);
		Debug::trace("SystemPrinters", "findSystemPrinterDriverForPrinter - printerName: " + printerName + "  listPosition: " + Integer::toString(position) );
		if ( position >= 0 )
		{
			std::string driverName = systemPrinterDriverNames_->getElementAtPosition(position);
			Debug::trace("SystemPrinters", "findSystemPrinterDriverForPrinter - found printerDriverName: " + driverName);
			printerDriverName.append(driverName);
		}
	}

	Debug::trace("SystemPrinters", "findSystemPrinterDriverForPrinter - driverName: " + printerDriverName);
	return printerDriverName;
}


//static
void SystemPrinters::findAllSystemPrinters()
{
	bool traceOn = Debug::isTracing("SystemPrinters");
	HobString myDebug("findAllSystemPrinters():  ");

	if (systemPrinterNames_)
	{
		delete systemPrinterNames_;
		systemPrinterNames_ = 0;
	}
	if (systemPrinterDriverNames_)
	{
		delete systemPrinterDriverNames_;
		systemPrinterDriverNames_ = 0;
	}

	systemPrinterNames_ = new HobList();
	systemPrinterDriverNames_ = new HobList();


#if defined (__linux__)

	std::string lpInfoCommand = "lpinfo -m";
	std::string lpInfoOutput;
	PAL_absclass::PAL_executeWithResults(lpInfoCommand, lpInfoOutput);
	Debug::trace("SystemPrinters", "lpinfo results: " + lpInfoOutput);
/*
	# lpinfo -m
drv:///sample.drv/dymo.ppd Dymo Label Printer
drv:///sample.drv/epson9.ppd Epson 9-Pin Series
drv:///sample.drv/generpcl.ppd Generic PCL Laser Printer
drv:///sample.drv/generic.ppd Generic PostScript Printer
pxlcolor.ppd HP Color LaserJet Series PCL 6 CUPS
pxlmono.ppd HP LaserJet Series PCL 6 CUPS
raw Raw Queue
Ricoh-MP_C4503-pxlcolor-Ricoh.ppd Ricoh MP C4503 PXL
drv:///sample.drv/zebracpl.ppd Zebra CPCL Label Printer
KR203.ppd Zebra Kiosk Printer, KR203
ttp20x0.ppd Zebra Kiosk Printer, TTP20x0
ttp2130.ppd Zebra Kiosk Printer, TTP2130
ttp7030.ppd Zebra Kiosk Printer, TTP7030
ttp8200.ppd Zebra Kiosk Printer, TTP8200
drv:///sample.drv/zebra.ppd Zebra ZPL Label Printer
zebra.ppd Zebra ZPL Label Printer, 1.4
# ls /usr/share/cups/model
KR203.ppd                          pxlmono.ppd  ttp7030.ppd
Ricoh-MP_C4503-pxlcolor-Ricoh.ppd  ttp20x0.ppd  ttp8200.ppd
pxlcolor.ppd                       ttp2130.ppd  zebra.ppd
*/

	HobTokenizer tok(&lpInfoOutput, '\n');
	HobString token;
	while ( tok.next(&token) != -1 )
	{
		Debug::trace("SystemPrinters", "Searching for printer in: " + token);
		if ( token.find("drv://") == std::string::npos )
		{
			Debug::trace("SystemPrinters", "Is a real printer: " + token);
			HobTokenizer tok2(&token, ' ', 1);
			HobString driverName, printerName;
			tok2.next(&driverName);
			tok2.next(&printerName);
			printerName.stripWhiteSpace(HobString::LEADING_TRAILING);
			printerName.stripChar(',');
			printerName.stripChar('\n');
			Debug::trace("SystemPrinters", "printer - driver: " + driverName + "  printerName: " + printerName);

			systemPrinterDriverNames_->push_back( driverName );
			systemPrinterNames_->push_back( printerName );
		}
	}

#else

	// These printers are in internal config, not currently read from the OS
	systemPrinterNames_->fillListFromCSVString( Configuration::getInstance()->toStringCfgI(CfgInternal::KNOWNLANPRINTERS) );
	systemPrinterDriverNames_->fillListFromCSVString( Configuration::getInstance()->toStringCfgI(CfgInternal::KNOWNLANPRINTERDRIVERS) );

#endif
}
