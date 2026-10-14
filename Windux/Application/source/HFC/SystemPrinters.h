/* HT Series Scale Project
 * COPYRIGHT (c) 2015.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SYSTEMPRINTERS_H
#define SYSTEMPRINTERS_H


#include "evolution.h"
#include "HfcObject.h"

class HobList;


class SystemPrinters : public HfcObject
{
	public:

		static int setupUSBZebraPrinter( const std::string filename );
		static int printToUSBLabelPrinter( const std::string filename, int numOfLabels = 1, int xOffset = -1, int yOffset = -1 );

		static std::string determineGhostscriptDevice( std::string printerSystemName );
		static int printToNetworkReportPrinter( std::string printerUserName, std::string filename );
		static int printPdfFile( std::string printerUserName, std::string printerSystemName, std::string filename );
		static int printDocFile( std::string printerUserName, std::string printerSystemName, std::string filename );

		static int addUSBZebraPrinterToSystem( std::string printerUserName );
		static int addNetworkPrinterToSystem( std::string printerUserName, std::string ipAddress, std::string printerSystemName );
		static int removePrinterFromSystem( std::string printerUserName );
		static int setDefaultPrinter( std::string printerUserName );

		static std::string getUsbPrinterNamesAsList();
		static std::string getUsbPrinterNamesAsString();

		static std::string getSystemPrinterNamesAsList();
		static std::string getSystemPrinterDriverNamesAsList();

		static std::string findSystemPrinterDriverForPrinter( std::string printerName );
		static void findAllSystemPrinters();


	private:

		SystemPrinters();
		~SystemPrinters();

		static HobList *systemPrinterNames_;
		static HobList *systemPrinterDriverNames_;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //SYSTEMPRINTERS_H
