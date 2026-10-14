#ifndef BARCODE_H
#define BARCODE_H


const int ModsUPCA			= 95;	/* Assuming that margins are not used */
const int ModsUPCD3HI		= 95;	/* Assuming no left or right margins */
const int ModsUPCD3LO		= 112;	/* Assuming no left or right margins */
const int ModsEAN			= 103;
const int ModsEAN128C		= 143;
const int ModsEAN_RSS14     = 124;
const int ModsUPCVmarg		= 7;
const int ModsUPCHmarg		= 7;

const int MaxCode 			= 10;
const int UPCLength			= 10;
const int SystemCode		= 0;
const int CheckSum			= UPCLength + 1;
const int RightMargModsUPC	= 7;	/* These constants is specified in the UPC */ 
const int LeftMargModsUPC	= 11;	/* spec and should not be arbitrarily changed */

typedef enum
{
	None,
	UPCAcodes,
	UPCBcodes,
	UPCCcodes,
	EANcodes
}SymbolTables;

typedef enum
{
    UPCA,
    UPCD3,				// 11-06-2019 comment in Renderer.cpp says not supported since 06262002 @ats but bits are in the code. Type not in Hobart Label Editor
    UPCE,				// not supported (but as of 01012020 I dont know when, where, or why who added this comment). Is in Label Editor but does not print on label
    EAN13,				
	EAN128C,            // Now known publically as GS1-128
    RSS14E,				// GS1-14 (GS1-128/ITF-14): GS1 GTIN - used everywhere                     - Is this the same as Enum=4 here?
    RSS14E_GTIN_13,		// GS1-13 (EAN-13/UCC-13): GS1 GTIN - used primarily outside North America - Is this the same as Enum=3 here?
    RSS14E_GTIN_12,		// GS1-12 (UPC-12 or UPC-A): GS1 GTIN - used primarily inside North America - Is this the same as Enum=0 here?
    RSS14E_GTIN_8,		// GS1-8  (EAN-8/UCC-8):  GS1 GTIN - used primarily outside North America
	QR_LEVELM,
	QR_LEVELL,
	QR_LEVELQ,
	QR_LEVELH,
	DATAMATRIX,

    GS1_DATABAR_EXPANDED,  // Initially supported for Coscto in 4.2.200 - 9/15/23

	USER_SUPPLIED,    // For Walmart Cake Label Project, where barcode string is passed in

	// The following are not barcode types as defined within the definition of a Label type,
	// but I need them for the functionality of the BarCodeNumber class
	GS1GTIN,
	GS1AI,
	QR_CODE,
	GS1UPCBARCODE,  // The upc barcode string for the GS1 barcode does not contain the prefix, nor the final check digit

	MAX_BARCODE_TYPES // not a type!
} BarcodeType;

//The entire family of data structures within the GTIN is :
// GTIN - 12 (UPC)
// GTIN - 13 (EAN - 13)
// GTIN - 14 (EAN / UCC - 128 or ITF - 14)
// GTIN - 8  (EAN - 8)

typedef enum
{
    DarkModule,
    LightModule
} ModuleType;



/*-------------------------------------------------*
*   BitMaped Values for UPC A Module Characters    *
*   Seven Modules are used - lower 7 bits of byte  *
*   bit = 1 -> Draw Dark Module                    *
*   bit = 0 -> Draw Light Module                   *
*-------------------------------------------------*/
//const UINT8 UPCAModCodes[MaxCode] = 
//{
/*     0    1    2    3    4    5    6    7    8    9 */
//    0x0D,0x19,0x13,0x3d,0x23,0x31,0x2f,0x3b,0x37,0x0b
//};

/*-------------------------------------------------*
*   BitMaped Values for UPC B Module Characters    *
*   Seven Modules are used - lower 7 bits of byte  *
*   bit = 1 -> Draw Dark Module                    *
*   bit = 0 -> Draw Light Module                   *
*-------------------------------------------------*/
//const UINT8 UPCBModCodes[MaxCode] = 
//{
/*     0    1    2    3    4    5    6    7    8    9 */
//    0x72,0x66,0x6c,0x42,0x5c,0x4e,0x50,0x44,0x48,0x74
//};

/*-------------------------------------------------*
*   BitMaped Values for UPC C Module Characters    *
*   Seven Modules are used - lower 7 bits of byte  *
*   bit = 1 -> Draw Dark Module                    *
*   bit = 0 -> Draw Light Module                   *
*-------------------------------------------------*/
//const UINT8 UPCCModCodes[MaxCode] = 
//{
/*     0    1    2    3    4    5    6    7    8    9 */
//    0x27,0x33,0x1b,0x21,0x1d,0x39,0x05,0x11,0x09,0x17
//};


/**********************************************************************/
/* 
    REF: EAN ademecum Section D - Part I - Chapter 5 - page 4

    The coding system for values of the 13th digit is as follows:

       Value of      Number Sets used for
      13th digit   coding left half of symbol

                   1    2    3    4    5    6

          0        A    A    A    A    A    A
          1        A    A    B    A    B    B
          2        A    A    B    B    A    B
          3        A    A    B    B    B    A
          4        A    B    A    A    B    B
          5        A    B    B    A    A    B
          6        A    B    B    B    A    A
          7        A    B    A    B    A    B
          8        A    B    A    B    B    A
          9        A    B    B    A    B    A

A - TRUE
B - FALSE
                                                                     
   truth table used to encode left hand digits of european barcode    */
/**********************************************************************/
/*
const BOOL EANModeCodeTable[10][6] = {TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,
                         TRUE,TRUE,FALSE,TRUE,FALSE,FALSE,
                         TRUE,TRUE,FALSE,FALSE,TRUE,FALSE,
                         TRUE,TRUE,FALSE,FALSE,FALSE,TRUE,
                         TRUE,FALSE,TRUE,TRUE,FALSE,FALSE,
                         TRUE,FALSE,FALSE,TRUE,TRUE,FALSE,
                         TRUE,FALSE,FALSE,FALSE,TRUE,TRUE,
                         TRUE,FALSE,TRUE,FALSE,TRUE,FALSE,
                         TRUE,FALSE,TRUE,FALSE,FALSE,TRUE,
                         TRUE,FALSE,FALSE,TRUE,FALSE,TRUE
                         };



*/

/* create UPC Barcode type enumeration for the following 
#define RegularUPC      0
#define RandomWeightUPC 2
#define NonFoodUPC      4
#define CouponUPC       5
*/


#endif	//BARCODE_H
