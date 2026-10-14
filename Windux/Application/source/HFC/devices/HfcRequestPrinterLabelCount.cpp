/*
 * HfcRequestPrinterLabelCount.cpp
 *
 *  Created on: Feb 17, 2017
 *      Author: aaron
 */
/**
 * @file HfcRequestPrinterLabelCount.cpp
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief Fastrack personality module manager message to request label count
 * Created on: Feb 17, 2017
 */
#include "HfcRequestPrinterLabelCount.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Debug.h"

HfcRequestPrinterLabelCount::HfcRequestPrinterLabelCount( const char* data, int len, int formatType )
{
	switch( formatType )
	{
		case BrowserFormat:
		{

			short loopCnt = 1;
			HobString temp( data, len ), token;
			HobTokenizerCSV tok( &temp, BrowserFormatSeparator );

			while ( tok.next( &token ) != -1 )
			{
				switch ( loopCnt )
				{
					case fieldOffset_type:
					{
						type_ = (PrinterType)Integer::valueOf(token);
						break;
					}
					default:
						break;
				}
				loopCnt++;
			}
			break;
		}
	}
}

HfcRequestPrinterLabelCount::~HfcRequestPrinterLabelCount()
{

}





