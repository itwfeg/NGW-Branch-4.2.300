/* Barcode.h
 */

#include "math.h"

#ifndef INDLUDED_BARCODE
#define INDLUDED_BARCODE 1

#if ST_BARCODE_TIFF_SUPPORT | WIN32STATIC
#include "tiffio.h"
#else
typedef unsigned char uint8;
typedef unsigned short uint16;
	typedef unsigned int uint32;
typedef void *tdata_t;
typedef short int16;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define ST_SUPPORT_CODE39	 1
#define ST_SUPPORT_CODE25	 1
#define ST_SUPPORT_CODE25_NI 1
#define ST_SUPPORT_CODE128	 1
#define ST_SUPPORT_CODABAR	 1
#define ST_SUPPORT_EAN8		 1
#define ST_SUPPORT_EAN13	 1
#define ST_SUPPORT_UPCA		 1
#define ST_SUPPORT_UPCE		 1
#define ST_SUPPORT_PATCH	 1
#define ST_SUPPORT_PDF417	 1


#define ST_MAX_BARCODES		1024

#define ST_UPPER_RATIO			1
#define ST_LOWER_RATIO			2
#define ST_LINE_JUMP			3
#define ST_MIN_OCCURRENCE		4
#define ST_TIF_WARNINGS			5
#define ST_MULTIPLE_READ		6
#define ST_ORIENTATION			7
#define ST_PREF_OCCURRENCE		8
#define ST_DESPECKLE			9
#define ST_NOISEREDUCTION		10
#define ST_QUIET_SIZE			11
#define	ST_DEBUG				12
#define ST_PAGE_NO				13
#define ST_TIFF_SPLIT			14
#define	ST_READ_BARCODE_TYPE	15
#define	ST_READ_CODE39			16
#define	ST_READ_EAN13			17
#define	ST_READ_EAN8			18
#define	ST_READ_UPCA			19
#define	ST_READ_UPCE			20
#define	ST_SHOW_CHECK_DIGIT	21
#define	ST_CODE39_NEEDS_ASTERISKS	22
#define	ST_READ_CODE128			23
#define	ST_READ_CODE25			24
#define	ST_CONTRAST				25
#define	ST_BARCODE_ZONE			26
#define	ST_READ_PATCHCODES		27
#define	ST_ORIENTATION_MASK		28
#define	ST_USE_OVERSAMPLING		29
#define	ST_OVERSAMPLING_SEP		30
#define	ST_MIN_LEN				31
#define	ST_MAX_LEN				32
#define	ST_TIFF_SPLIT_MODE		33
#define	ST_READ_CODABAR			34
#define ST_CONVERT_UPCE_TO_UPCA	35
#define	ST_SKEW_SETTING			36
#define	ST_READ_CODE25_NI		37
#define	ST_DUPLICATE_BARCODES	38
#define	ST_MAPPING_MODE			39
#define	ST_MIN_SEPARATION		40
#define	ST_EXTENDED_CODE39		41
#define ST_CODE39_CHECKSUM		42
#define	ST_ERROR_CORRECTION		43
#define	ST_NUMERIC_BARCODE		44
#define	ST_REGEX_MATCH			45
#define	ST_MIN_SPACE_BAR_WIDTH	46
#define	ST_READ_PDF417			47
#define	ST_MEDIAN_FILTER		48
#define	ST_CODE25_MIN_OCCURRENCE_LENGTH		49
#define	ST_WEIGHT_LONG_BARCODES	50
#define	ST_CODE25_CHECKSUM		51
#define	ST_ENCODING				52
#define	ST_GAMMA_CORRECTION		53
#define	ST_SKEW_LINE_JUMP		54
#define	ST_MAX_RECT_OVERLAP		55
#define	ST_READ_SHORT_CODE128		56
#define	ST_READ_MICRO_PDF417		57
/* 58 to 61 are reserved */
#define	ST_SHORT_CODE128_MIN_LENGTH	62
#define ST_PDF417DEBUG			63
#define ST_READ_DATAMATRIX              65
#define ST_SHOW_CODABAR_START_STOP	66
#define ST_TIFF_SPLIT_VALUES            67
#define ST_COLOR_PROCESSING_LEVEL       68
#define ST_READ_DATABAR                 69
#define ST_USE_OLD_CODE128_ALGORITHM    70
#define ST_DATABAR_OPTIONS              71
#define	ST_COLOR_CHUNKS			72
#define ST_SKEW_SPEED			73
#define ST_USE_RUN_CACHE		74
#define ST_CODABAR_MAX_VARIANCE		75
#define ST_READ_CODE93			76
/* 77 to 80 are reserved */
#define ST_FILE_PATH_ENCODING		81
#define ST_USE_SCALED_FAX_COORDS    82
#define ST_READ_QRCODE          83
#define ST_QUOTED_PRINTABLE_CHARSET     84
#define ST_MEDIAN_FILTER_LEVEL  85^


#define	ST_LICENSE	999

#define ST_ERROR_FILE_OPEN		-1
#define ST_ERROR_MULTI_PLANE		-2
#define ST_ERROR_BITS_PER_SAMPLE	-3
#define	ST_ERROR_MALLOC			-4
#define	ST_ERROR_COLOR_TIFF		-5

#define	ST_ORIENTATION_0		1
#define	ST_ORIENTATION_90		2
#define	ST_ORIENTATION_180		4
#define	ST_ORIENTATION_270		8

#define	ST_ORIENTATION_0_SKEW		16
#define	ST_ORIENTATION_90_SKEW		32
#define	ST_ORIENTATION_180_SKEW		64
#define	ST_ORIENTATION_270_SKEW		128

#define	ST_ORIENTATION_PORTRAIT_SKEW	(ST_ORIENTATION_0_SKEW | ST_ORIENTATION_180_SKEW)
#define	ST_ORIENTATION_LANDSCAPE_SKEW	(ST_ORIENTATION_90_SKEW | ST_ORIENTATION_270_SKEW)


#define	ST_ORIENTATION_PORTRAIT		(ST_ORIENTATION_0 | ST_ORIENTATION_180)
#define	ST_ORIENTATION_LANDSCAPE	(ST_ORIENTATION_90 | ST_ORIENTATION_270)

#define ST_MAPPING_MODE_PIXEL		0
#define ST_MAPPING_MODE_PERCENT		1


void *STCreateBarCodeSession();
void STFreeBarCodeSession(void *hBarcode);
int  STReadBarCode(void *hBarcode, char *file, char *type, char ***bar_codes, char ***bar_codes_type) ;
int  STGetBarCodePos(void *hBarcode, int nBarCode, uint32 *TopLeftX, uint32 *TopLeftY, uint32 *BotRightX, uint32 *BotRightY) ;
int  STGetBarCodeConfidence(void *hBarcode, int nBarCode) ;
int  STGetBarCodeDirection (void *hBarcode, int nBarCode) ;
void STSetParameter(void *hBarcode, uint16 param, void *value) ;
void STGetParameter(void *hBarcode, uint16 param, void *value) ;
int  STReadBarCodeFromJPEG(void *hBarcode, char *file, char ***bar_codes, char ***bar_codes_type) ;
int  STReadBarCodeFromPNG(void *hBarcode, char *file, char ***bar_codes, char ***bar_codes_type) ;
int  STReadBarCodeFromGIF(void *hBarcode, char *file, char ***bar_codes, char ***bar_codes_type) ;
int  STReadBarCodeFromTiff(void *hBarcode, char *file, char ***bar_codes, char ***bar_codes_type) ;
int  STConvertCode39Extended(const char *input, char *output, int len) ;
int  STSaveResults(void *hBarcode, char *filePath) ;
int  STExportXMLSettings(void *hBarcode, char *filePath) ;
int  STLoadXMLSettings(void *hBarcode, char *filePath, int silent) ;
int  STProcessXML(void *hBarcode, char *inputFilePath, char *outputFilePath, int silent) ;

#ifndef WIN32
typedef struct tagBITMAP {
    int     bmType;
    int     bmWidth;
    int     bmHeight;
    int     bmWidthBytes;
    unsigned char  bmPlanes;
    unsigned char  bmBitsPixel;
    void*   bmBits;
} BITMAP;
#endif

int		STReadBarCodeFromBitmap(void *hBarcode, BITMAP *pBitmap, float resolution, char ***bar_codes, char ***bar_codes_type, short photometric) ;

#ifdef __cplusplus
}
#endif

#endif
