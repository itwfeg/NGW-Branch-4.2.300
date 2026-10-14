/**
 * @file FpmEvent.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief Fastrack personality module manager events
 * Created on: Feb 26, 2013
 */

#ifndef FPMEVENT_H
#define FPMEVENT_H

enum FpmManagerEvent
{
	WriteMfgData,
	WriteOfMfgDataPassed,
	WriteOfMfgDataFailed,
	RequestMfgData,
	ReadMfgData,
	WriteScaleHwConfig,
	WriteOfScaleHwConfigPassed,
	WriteOfScaleHwConfigFailed,
	RequestScaleHwConfig,
	ReadScaleHwConfig,
	WriteScaleSWConfig,

	WriteOfScaleSWConfigPassed,
	WriteOfScaleSWConfigFailed,
	RequestScaleSWConfig,
	ReadScaleSWConfig,
	RequestLoaderVersion,		//u-boot for linux bios for windows
	ReadLoaderVersion,
	WriteOfScaleSWConfigMetrologicalVersion,
	RequestScaleSWMetrologicalVersion,
	WriteOfScaleSWConfigPoundsFor,
	RequestScaleSWPoundsFor,

	WriteOfScaleSWConfigFractionalPrice,
	RequestScaleSWFractionalPrice,
	RequestPrinterLabelCount,
	PrinterLabelCount,
	ClearPrinterLabelCount,
	RequestWrapperCycleCount,
	WrapperCycleCount,
	RequestInstallDate,
	WrapperInstallDate,
	SetInstallDate,
	WriteOfTotalPriceDigits,
	RequestTotalPriceDigits,
	WriteOfUnitPriceDigits,
	RequestUnitPriceDigits,
	WriteOfCurrencyRoundingMethod,
	RequestCurrencyRoundingMethod,
	WriteOfPrimaryPrecisionDigits,
	RequestPrimaryPrecisionDigits,
	WriteOfCurrencySymbolPlacement,
	RequestCurrencySymbolPlacement,
	WriteOfUnitSymbolSeparator,
	RequestUnitSymbolSeparator,
	WriteOfPrimaryMonetarySymbol,
	RequestPrimaryMonetarySymbol

};


#endif
