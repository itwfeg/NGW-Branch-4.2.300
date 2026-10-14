#include <stdio.h>
#ifdef WIN32
#include "getopt_long.h"
#else
#include <unistd.h>
#include <getopt.h>
#endif

#include <string.h> // for strlen
#include <stdlib.h> // for atoi

#include "barcode.h"

#define LONGOPT_BASE	1000


#ifdef WIN32
extern int getopt( int, char **, char * );
extern int   opterr;
extern int   optind;
extern char *optarg;
#define strncasecmp _st_strncasecmp
#endif

char *file ;
char *xml_input = NULL ;
char *xml_output = NULL ;
int xml_process = 0 ;
FILE *output_fp ;

static int	bShowPageNo = 0 ;
static int	bShowBarcodeType = 0 ;


void set_barcode_types(void *hBarcode, char *arg) ;
int flag(void *hBarcode, int argc, char *argv[]) ;
int usage(void *hBarcode, char *name) ;

typedef struct {
        long TopLeftX ;
        long TopLeftY ;
        long BottomRightX ;
        long BottomRightY ;
} BARCODE_RECT ;


int main(int argc, char *argv[])
{
	char **bar_codes ;
	char **bar_codes_type ;
	int	bar_count ;
	uint16	i ;
	void *hBarcode ;

	hBarcode = STCreateBarCodeSession() ;

	output_fp = stdout ;

	flag(hBarcode, argc, argv) ;
	
	if (xml_input && xml_output && xml_process)
	{
		if (! STProcessXML(hBarcode, xml_input, xml_output, 0))
		{
			fprintf (stderr, "Failed to process XML\n") ;
			exit (1) ;
		}
		exit (0) ;
	}
	else if (xml_input && ! STLoadXMLSettings(hBarcode, xml_input, 0))
	{
		fprintf (stderr, "Failed to load XML Settings from file %s\n", xml_input) ;
		exit (1) ;
	}

	bar_count = STReadBarCode(hBarcode, file, 0, &bar_codes, &bar_codes_type) ;
	
	if (bar_count < 0)
	{
		switch(bar_count)
		{
			case ST_ERROR_FILE_OPEN:
				fprintf (stderr, "Cannot open %s as a TIF File\n", file) ;
				break ;
			case ST_ERROR_MULTI_PLANE:
			case ST_ERROR_BITS_PER_SAMPLE:
				fprintf (stderr, "TIF File format error\n") ;
				break ;
		}
		exit (- bar_count) ;
	}
		
	if (xml_output)
	{
		STSaveResults(hBarcode, xml_output) ;
		exit (0) ;
	}

	for (i = 0; i < bar_count; i++)
	{
		uint32 TopLeftX, TopLeftY, BotRightX, BotRightY ;
		int nPage ;

		if (bShowBarcodeType)
		{
			printf ("%s:", bar_codes_type[i]) ;
		}

		nPage = STGetBarCodePos(hBarcode, i, &TopLeftX, &TopLeftY, &BotRightX, &BotRightY) ;
		if (bShowPageNo)
			fprintf (output_fp, "%d:%s %d, %d, %d, %d\n", nPage, bar_codes[i],
				TopLeftX, TopLeftY, BotRightX, BotRightY) ;
		else
			fprintf (output_fp, "%s\n", bar_codes[i]) ;
	}
		
	exit (0) ;
}


int flag(void *hBarcode, int argc, char *argv[])
{
	int ch ;
	float val ;
	uint16	ival ;
	uint32 lval ;
	int topLeftX, topLeftY, botRightX, botRightY, mode ;
	BARCODE_RECT rect ;
	struct option longopts[] = {
		"LineJump", 1, 0, LONGOPT_BASE + ST_LINE_JUMP,
		"MinOccurrence", 1, 0, LONGOPT_BASE + ST_MIN_OCCURRENCE,
		"MultipleRead", 1, 0, LONGOPT_BASE + ST_MULTIPLE_READ,
		"PrefOccurrence", 1, 0, LONGOPT_BASE + ST_PREF_OCCURRENCE,
		"Despeckle", 1, 0, LONGOPT_BASE + ST_DESPECKLE,
		"NoiseReduction", 1, 0, LONGOPT_BASE + ST_NOISEREDUCTION,
		"QuietZoneSize", 1, 0, LONGOPT_BASE + ST_QUIET_SIZE,
		"Debug", 1, 0,	LONGOPT_BASE + ST_DEBUG,
		"PageNo", 1, 0, LONGOPT_BASE + ST_PAGE_NO,
		"TifSplitPath", 1, 0, LONGOPT_BASE + ST_TIFF_SPLIT,
		"ReadCode39", 1, 0,	LONGOPT_BASE + ST_READ_CODE39,
		"ReadEAN13", 1, 0,	LONGOPT_BASE + ST_READ_EAN13,
		"ReadEAN8", 1, 0,	LONGOPT_BASE + ST_READ_EAN8,
		"ReadUPCA", 1, 0,	LONGOPT_BASE + ST_READ_UPCA,
		"ReadUPCE", 1, 0,	LONGOPT_BASE + ST_READ_UPCE,
		"ShowCheckDigit", 1, 0,	LONGOPT_BASE + ST_SHOW_CHECK_DIGIT,
		"Code39NeedStartStop", 1, 0,	LONGOPT_BASE + ST_CODE39_NEEDS_ASTERISKS,
		"ReadCode128", 1, 0,	LONGOPT_BASE + ST_READ_CODE128,
		"ReadCode25", 1, 0,	LONGOPT_BASE + ST_READ_CODE25,
		"ColorThreshold", 1, 0,	LONGOPT_BASE + ST_CONTRAST,
		"ReadPatchCodes", 1, 0,	LONGOPT_BASE + ST_READ_PATCHCODES,
		"ScanDirection", 1, 0,	LONGOPT_BASE + ST_ORIENTATION_MASK,
		"UseOverSampling", 1, 0,	LONGOPT_BASE + ST_USE_OVERSAMPLING,
		"OverSamplingSeparation", 1, 0,	LONGOPT_BASE + ST_OVERSAMPLING_SEP,
		"MinLength", 1, 0,	LONGOPT_BASE + ST_MIN_LEN,
		"MaxLength", 1, 0,	LONGOPT_BASE + ST_MAX_LEN,
		"TifSplitMode", 1, 0,	LONGOPT_BASE + ST_TIFF_SPLIT_MODE,
		"ReadCodabar", 1, 0,	LONGOPT_BASE + ST_READ_CODABAR,
		"ConvertEAN13ToUPCE", 1, 0, LONGOPT_BASE + ST_CONVERT_UPCE_TO_UPCA,
		"SkewTolerance", 1, 0,	LONGOPT_BASE + ST_SKEW_SETTING,
		"ReadCode25ni", 1, 0,	LONGOPT_BASE + ST_READ_CODE25_NI,
		"AllowDuplicateValues", 1, 0,	LONGOPT_BASE + ST_DUPLICATE_BARCODES,
		"MinSeparation", 1, 0,	LONGOPT_BASE + ST_MIN_SEPARATION,
		"ExtendedCode39", 1, 0,	LONGOPT_BASE + ST_EXTENDED_CODE39,
		"Code39Checksum", 1, 0, LONGOPT_BASE + ST_CODE39_CHECKSUM,
		"ErrorCorrection", 1, 0,	LONGOPT_BASE + ST_ERROR_CORRECTION,
		"ReadNumeric", 1, 0,	LONGOPT_BASE + ST_NUMERIC_BARCODE,
		"Pattern", 1, 0,	LONGOPT_BASE + ST_REGEX_MATCH,
		"MinSpaceBarWidth", 1, 0,	LONGOPT_BASE + ST_MIN_SPACE_BAR_WIDTH,
		"ReadPDF417", 1, 0,	LONGOPT_BASE + ST_READ_PDF417,
		"ReadMicroPDF417", 1, 0,	LONGOPT_BASE + ST_READ_MICRO_PDF417,
		"MedianFilter", 1, 0,	LONGOPT_BASE + ST_MEDIAN_FILTER,
		"Encoding", 1, 0,	LONGOPT_BASE + ST_ENCODING,
		"Code25MinOccurrenceLength", 1, 0,	LONGOPT_BASE + ST_CODE25_MIN_OCCURRENCE_LENGTH,
		"WeightLongerBarcodes", 1, 0,	LONGOPT_BASE + ST_WEIGHT_LONG_BARCODES,
		"Code25Checksum", 1, 0,	LONGOPT_BASE + ST_CODE25_CHECKSUM,
		"GammaCorrection", 1, 0,	LONGOPT_BASE + ST_GAMMA_CORRECTION,
		"SkewLineJump", 1, 0,	LONGOPT_BASE + ST_SKEW_LINE_JUMP,
		"MaxRectOverlap", 1, 0,	LONGOPT_BASE + ST_MAX_RECT_OVERLAP,
		"ReadShortCode128", 1, 0,	LONGOPT_BASE + ST_READ_SHORT_CODE128,
		"ShortCode128MinLength", 1, 0,	LONGOPT_BASE + ST_SHORT_CODE128_MIN_LENGTH,
		"PDF417Debug", 1, 0,	LONGOPT_BASE + ST_PDF417DEBUG,
		"ReadDataMatrix", 1, 0,	LONGOPT_BASE + ST_READ_DATAMATRIX,
		"ShowCodabarStartStop", 1, 0,	LONGOPT_BASE + ST_SHOW_CODABAR_START_STOP,
		"TifSplitValues", 1, 0, LONGOPT_BASE + ST_TIFF_SPLIT_VALUES,
		"ColorProcessingLevel", 1, 0, LONGOPT_BASE + ST_COLOR_PROCESSING_LEVEL,
		"ColorChunks", 1, 0, LONGOPT_BASE + ST_COLOR_CHUNKS,
		"ReadDatabar", 1, 0, LONGOPT_BASE + ST_READ_DATABAR,
		"DatabarOptions", 1, 0, LONGOPT_BASE + ST_DATABAR_OPTIONS,
		"UseOldCode128Algorithm", 1, 0, LONGOPT_BASE + ST_USE_OLD_CODE128_ALGORITHM,
		"ReadQRCode", 1, 0, LONGOPT_BASE + ST_READ_QRCODE,
		0, 0, 0, 0} ;

	file = 0 ;

	while ((ch = getopt_long (argc, argv, "-f:M:K:F:T:k:R:A:c:C:u:i:l:j:o:OL:p:t:r:d:q:S:abwmgPsn:N:x:X:", longopts, 0)) != EOF)
	{
		switch(ch)
		{
		case 1:
			file = optarg ;
			break ;
		case 'F':
			if (! (output_fp = fopen (optarg, "w")))
			{
				fprintf (stderr, "Cannot open file %s for output\n", optarg) ;
				exit (1) ;
			}
			break ;
		case 'r':
			if (sscanf(optarg, "%d,%d,%d,%d,%d", &topLeftX, &topLeftY, &botRightX, &botRightY, &mode) != 5)
			{
				fprintf (stderr, "Error: Rectangle argument must be in format topLeftX,topLeftY,botRightX,botRightY,mappingMode\n") ;
				usage(hBarcode, argv[0]) ;
			}

			rect.TopLeftX = topLeftX ;
			rect.TopLeftY = topLeftY ;
			rect.BottomRightX = botRightX ;
			rect.BottomRightY = botRightY ;
			STSetParameter(hBarcode, ST_MAPPING_MODE, &mode) ;
			STSetParameter(hBarcode, ST_BARCODE_ZONE, &rect) ;
			break ;
		case 'R':
			STSetParameter(hBarcode, ST_REGEX_MATCH, optarg) ;
			break ; 
		case 'T':
			ival = atoi (optarg) ;
			STSetParameter(hBarcode, ST_CONTRAST, &ival) ;
			break ;
		case 't':
			set_barcode_types(hBarcode, optarg) ;
			break ;
		case 'k':
			ival = atoi (optarg) ;
			STSetParameter(hBarcode, ST_SKEW_SETTING, &ival) ;
			break ;
		case 'a':
			ival = 1 ;
			STSetParameter(hBarcode, ST_CODE39_NEEDS_ASTERISKS, &ival) ;
			break ;
		case 'b':
			bShowBarcodeType = 1 ;
			break ;
		case 'A':
			ival = atoi (optarg);
			STSetParameter(hBarcode, ST_SKEW_SETTING, &ival) ;
			break ;
		case 'c':
			ival = atoi (optarg);
			STSetParameter(hBarcode, ST_MIN_LEN, &ival) ;
			break ;
		case 'C':
			ival = atoi (optarg);
			STSetParameter(hBarcode, ST_MAX_LEN, &ival) ;
			break ;
		case 'S':
			STSetParameter(hBarcode, ST_TIFF_SPLIT, optarg) ;
			break ;
		case 'f':
			file = optarg ;
			break ;
		case 'g':
			xml_process = 1 ;
			break ;
		case 'u':
			if (sscanf(optarg, "%f", &val) == 1)
				STSetParameter(hBarcode, ST_UPPER_RATIO, &val) ;
			break ;
		case 'K':
			STSetParameter(hBarcode, ST_LICENSE, optarg) ;
			break ;
		case 'l':
			if (sscanf(optarg, "%f", &val) == 1)
				STSetParameter(hBarcode, ST_LOWER_RATIO, &val) ;
			break ;
		case 'M':
			ival = atoi(optarg) ;
			STSetParameter(hBarcode, ST_MIN_SPACE_BAR_WIDTH, &ival) ;
			break ;
		case 'q':
			ival = atoi(optarg) ;
			STSetParameter(hBarcode, ST_QUIET_SIZE, &ival) ;
			break ;
		case 'o':
			ival = atoi(optarg) ;
			STSetParameter(hBarcode, ST_MIN_OCCURRENCE, &ival) ;
			break ;
		case 'p':
			ival = atoi(optarg) ;
			STSetParameter(hBarcode, ST_PREF_OCCURRENCE, &ival) ;
			break ;
		case 'd':
			ival = atoi(optarg) ;
			STSetParameter(hBarcode, ST_ORIENTATION_MASK, &ival) ;
			break ;
		case 'j':
			lval = atoi(optarg) ;
			STSetParameter(hBarcode, ST_LINE_JUMP, &lval) ;
			break ;
		case 'v':
			_st_version(hBarcode) ;
			break ;
		case 'w':
			ival = 1 ;
			STSetParameter(hBarcode, ST_TIF_WARNINGS, &ival) ;
			break ;
		case 'O':
			ival = 1 ;
			STSetParameter(hBarcode, ST_USE_OVERSAMPLING, &ival) ;
			break ;
		case 'L':
			ival = atoi(optarg) ;
			STSetParameter(hBarcode, ST_OVERSAMPLING_SEP, &ival) ;
			break ;
		case 'm':
			ival = 1 ;
			STSetParameter(hBarcode, ST_MULTIPLE_READ, &ival) ;
			break ;
		case 's':
			ival = 1 ;
			STSetParameter(hBarcode, ST_DESPECKLE, &ival) ;
			break ;
		case 'n':
			ival = atoi(optarg) ;
			STSetParameter(hBarcode, ST_NOISEREDUCTION, &ival) ;
			break ;
		case 'N':
			ival = atoi(optarg) ;
			STSetParameter(hBarcode, ST_NOISEREDUCTION, &ival) ;
			ival = 1 ;
			STSetParameter(hBarcode, ST_DESPECKLE, &ival) ;
			break ;
		case 'i':
			ival = atoi(optarg) ;
			STSetParameter(hBarcode, ST_PAGE_NO, &ival) ;
			break ;
		case 'P':
			bShowPageNo = 1 ;
			break ;
		case 'x':
			xml_input = optarg ;
			break ;
		case 'X':
			xml_output = optarg ;
			break ;
		default:
			if (ch >= LONGOPT_BASE)
			{
				ch -= LONGOPT_BASE ;
				if (ch == ST_LINE_JUMP)
				{
					lval = atoi(optarg) ;
					STSetParameter(hBarcode, ch, &lval) ;
				}
				else if ((ch == ST_DEBUG) || (ch == ST_TIFF_SPLIT) || (ch == ST_REGEX_MATCH) || (ch == ST_TIFF_SPLIT_VALUES))
				{
					STSetParameter(hBarcode, ch, optarg) ;
				}
				else
				{
					ival = atoi(optarg) ;
					STSetParameter(hBarcode, ch, &ival) ;
				}

			}
			else
			{
				usage(hBarcode, argv[0]) ;
			}
			break ;
		}
	}

	if (! file && ! xml_process)
		usage(hBarcode, argv[0]) ;


	return 0 ;
}

int usage(void *hBarcode, char *name)
{
	_st_copyright(hBarcode) ;
	_st_version(hBarcode) ;
	printf ("\n") ;
	printf ("Usage: %s -f image_file [options] [long options]\n", name) ;
	printf ("\n") ;
	printf ("options are flags such as -m, which may or may not take an argument\n") ;
	printf ("long options are property name and value pairs, such as --MinSpaceBarWidth=2\n") ;
	printf ("\n") ;
	printf ("options are:\n") ;
	printf ("\n-C max_length        Maximum length of barcode\n") ;
	printf ("\n-c min_length        Minimum length of barcode\n") ;
	printf ("\n-d scan_direction    Scan direction mask 1 = Left to Right, 2 = Top to Bottom\n") ;
	printf ("                     4 = Right to Left and 8 = Bottom to Top. default is 15.\n") ;
	printf ("                     The scan direction mask controls the directions in which.\n") ;
	printf ("                     the barcode reader will look for a barcode. With this\n") ;
	printf ("                     version it it now possible to scan for barcodes in\n") ;
	printf ("                     any rotation with one call to the reader.\n") ;
	printf ("\n-f image_file        image_file can be of type TIF or JPG. JPG files must use\n") ;
	printf ("                     the extension \".jpg\" or \".jpeg\"\n") ;
	printf ("\n-g                   Process files and folders from the XML file specified with\n") ;
	printf ("                     -x flag. See below for more details.\n") ;
	printf ("\n-i page_index        Page number of image indexed from 1\n") ;
	printf ("\n-j jump              Frequency at which scan lines are sampled, default is 1\n") ;
	printf ("\n-k skew tolerance    Skew tolerance 0 = off to 5 = any angle\n") ;
	printf ("\n-L oversample_sep    Distance between the line samples when over-sampling is\n") ;
	printf ("                     used. Default value is 3.\n") ;
	printf ("\n-M min_space_width   Minimum allowed size in pixels for a space between bars\n") ;
	printf ("\n-m                   Scan for multiple barcodes\n") ;
	printf ("\n-N noise_reduction   Advanced Noise Reduction (0 = off, typical value is 20)\n") ; 
	printf ("\n-n noise_reduction   Noise Reduction level (0 = off, typical value is 20)\n") ; 
	printf ("\n-O                   Sample the scan lines in blocks of 3 with each line\n") ;
	printf ("                     separated by oversample_sep lines (see below).\n") ;
	printf ("\n-o min_occurs        Minimum number of matching scan lines, default 2\n") ;
	printf ("\n-P                   Show page numbers\n") ;
	printf ("\n-p pref_occurs       Preferred number of matching scan lines, default 5\n") ;
	printf ("\n-q quiet_size        Number of pixels wide for the quiet zone around bar code\n") ;
	printf ("\n-R pattern     	Barcodes must match regular expression defined by pattern\n") ;
	printf ("\n-r x1,y1,x2,y2,m     Rectangle for scanning (see below for details)\n") ;
	printf ("\n-S file_template:    Split a multi-page TIF file into smaller TIF files.\n") ;
	printf ("                     The first page of each new file will contain a barcode\n") ;
	printf ("                     The template must include %%d to index the files\n") ;
	printf ("                     e.g. file%%d.tif. Must be used with -m flag.\n") ;
	printf ("\n-T color_threshold   Set the color threshold\n") ;
	printf ("\n-t barcode_types     Specifies the types of barcodes to read, barcode_types is\n") ;
	printf ("                     one or more of \"code39\", \"code128\", \"upca\", \"upce\", \"ean8\",\n") ;
	printf ("                     \"code25\", \"ean13\" and \"codabar\" joined together with\n") ;
	printf ("                     the | character.\n") ;
	printf ("\n-t any               Searches for a barcode of any type\n") ;
	printf ("                     e.g \"code39|code128\"\n") ;
	printf ("\n-u upper_ratio       Defunct flag\n") ;
	printf ("\n-w                   Print TIF Warnings to STDERR\n") ;
	printf ("\n-X xml_output		Output results to specified XML file (see below)\n") ;
	printf ("\n-x xml_input			Load settings from specified XML file (see below)\n") ;
	printf ("\n") ;
	printf ("\n") ;
	printf ("The long options give access to all the features of the toolkit.\n") ;
	printf ("\n") ;
	printf ("Supported long options properties are: AllowDuplicateValues, Code39Checksum,\n") ;
	printf ("Code39NeedStartStop, ColorThreshold, ConvertEAN13ToUPCE, Debug, Despeckle,\n") ;
	printf ("ErrorCorrection, ExtendedCode39, LineJump, MaxLength, MedianFilter, MinLength,\n") ;
	printf ("MinOccurrence, MinSeparation, MinSpaceBarWidth, MultipleRead, NoiseReduction\n") ;
	printf ("OverSamplingSeparation, PrefOccurrence, QuietZoneSize, PageNo, Pattern,\n") ;
	printf ("ReadCodabar, ReadCode128, ReadCode25, ReadCode25ni, ReadCode39,ReadDataMatrix\n") ;
	printf ("ReadEAN8, ReadEAN13, ReadNumeric, ReadPatchCodes, ReadPDF417, ReadUPCA\n") ;
	printf ("ReadUPCE, ScanDirection, ShowCheckDigit, SkewTolerance, TifSplitMode\n") ;
	printf ("TifSplitPath and UseOverSampling.\n") ;
	printf ("\n") ;
	printf ("Please refer to the product documentation for descriptions and appropriate\n") ;
	printf ("values.\n") ;
	printf ("\n") ;
	printf ("Rectangle argument:\n\n") ;
	printf ("The top left hand corner of an image is the origin and (x1,y1) (x2,y2) define the\n") ;
	printf ("top left and bottom right corners of the rectangle to be searched - with the\n") ;
	printf ("measurements taken in either pixels or a percentage of the width/height of the image.\n") ;
	printf ("If m = 0 then the units are pixels and if m = 1 then the units are as a percentage.\n") ;
	printf ("Example: To search top right hand quarter of an image use -m50,0,100,50,1\n") ;
	printf ("\n") ;
	printf ("XML Input and Output\n") ;
	printf ("\n") ;
	printf ("Settings and results can be stored in XML format. Multiple layers of settings\n") ;
	printf ("can be specified, with the toolkit trying each successive layer until a barcode\n") ;
	printf ("is found. For more details please refer to the manual pages for LoadXMLSettings\n") ;
	printf ("and SaveResults. If the -g flag is also used then any files or folders specified\n") ;
	printf ("in the input file are processed, as described in the manual page for ProcessXML.\n") ;
	printf ("\n") ;
	printf ("Examples:\n") ;
	printf ("\n") ;
	printf ("Scan for a Code 39 barcode in a portrait orientation image:\n") ;
	printf ("\n") ;
	printf ("bardecode -f file.tif -t code39 -d 5\n") ;
	printf ("\n") ;
	printf ("Using long options:\n") ;
	printf ("\n") ;
	printf ("bardecode -f file.tif --ReadCode39=1 --ScanDirection=5\n") ;
	printf ("\n") ;
	printf ("Scan for a Code 39 barcode in a landscape orientation image:\n") ;
	printf ("\n") ;
	printf ("bardecode -f file.tif -t code39 -d 10\n") ;
	printf ("\n") ;
	printf ("Scan for a Code 128 or UPC-A barcode in an image of poor quality:\n") ;
	printf ("\n") ;
	printf ("bardecode -n 20 -f file.tif -t \"code128|upca\"\n") ;
	printf ("\n") ;
	printf ("Split a multi-page TIF file into smaller files, each with a barcode on page 1\n") ;
	printf ("\n") ;
	printf ("bardecode -t code39 -m -S output%%d.tif -f multipage.tif\n") ;

	exit (1) ;
}

void set_barcode_types(void *hBarcode, char *arg)
{
	uint16 i ;
	int s ;
	int	bReadCODABAR = 0 ;
	int	bReadCODE39 = 0 ;
	int	bReadCODE128 = 0 ;
	int	bReadCODE25 = 0 ;
	int	bReadEAN13 = 0 ;
	int	bReadEAN8 = 0 ;
	int	bReadUPCA = 0 ;
	int	bReadUPCE = 0 ;
	int	bReadPDF417 = 0 ;

	s = 0 ;
	for (i = 0; i <= strlen (arg); i++)
	{
		if (!arg[i] || (arg[i] == '|'))
		{
			if (! strncasecmp(arg + s, "code39", i - s))
				bReadCODE39 = 1 ;
			else if (! strncasecmp(arg + s, "code128", i - s))
				bReadCODE128 = 1 ;
			else if (! strncasecmp(arg + s, "code25", i - s))
				bReadCODE25 = 1 ;
			else if (! strncasecmp(arg + s, "ean13", i - s))
				bReadEAN13 = 1 ;
			else if (! strncasecmp(arg + s, "ean8", i - s))
				bReadEAN8 = 1 ;
			else if (! strncasecmp(arg + s, "upca", i - s))
				bReadUPCA = 1 ;
			else if (! strncasecmp(arg + s, "upce", i - s))
				bReadUPCE = 1 ;
			else if (! strncasecmp(arg + s, "codabar", i - s))
				bReadCODABAR = 1 ;
			else if (! strncasecmp(arg + s, "pdf417", i - s))
				bReadPDF417 = 1 ;
			else if (! strncasecmp(arg + s, "any", i - s))
				bReadCODE39 = bReadCODE128 = bReadCODE25 = bReadEAN13 = bReadEAN8 = bReadUPCE = bReadCODABAR = bReadPDF417 = bReadEAN13 = 1 ;
			s = i + 1;
		}
	}

	if (!bReadCODE39 && !bReadCODE128 && !bReadCODE25 && !bReadEAN13 && !bReadEAN8 && !bReadUPCA && !bReadUPCE && !bReadCODABAR && !bReadPDF417)
	{
		fprintf (stderr, "Please user the -t flag to specify the types f barcode to read\n") ;
		fprintf (stderr, "e.g. -t \"code39|code128\"\n") ;
		exit (1) ;
	}

	i = 0 ;
	STSetParameter(hBarcode, ST_READ_CODE39, &i) ;
	STSetParameter(hBarcode, ST_READ_CODE128, &i) ;
	STSetParameter(hBarcode, ST_READ_CODE25, &i) ;
	STSetParameter(hBarcode, ST_READ_EAN13, &i) ;
	STSetParameter(hBarcode, ST_READ_EAN8, &i) ;
	STSetParameter(hBarcode, ST_READ_UPCA, &i) ;
	STSetParameter(hBarcode, ST_READ_UPCE, &i) ;
	STSetParameter(hBarcode, ST_READ_CODABAR, &i) ;
	STSetParameter(hBarcode, ST_READ_PDF417, &i) ;

	i = 1 ;
	if (bReadCODE39)
		STSetParameter(hBarcode, ST_READ_CODE39, &i) ;
	if (bReadCODE128)
		STSetParameter(hBarcode, ST_READ_CODE128, &i) ;
	if (bReadCODE25)
		STSetParameter(hBarcode, ST_READ_CODE25, &i) ;
	if (bReadEAN13)
		STSetParameter(hBarcode, ST_READ_EAN13, &i) ;
	if (bReadEAN8)
		STSetParameter(hBarcode, ST_READ_EAN8, &i) ;
	if (bReadUPCA)
		STSetParameter(hBarcode, ST_READ_UPCA, &i) ;
	if (bReadUPCE)
		STSetParameter(hBarcode, ST_READ_UPCE, &i) ;
	if (bReadCODABAR)
		STSetParameter(hBarcode, ST_READ_CODABAR, &i) ;
	if (bReadPDF417)
		STSetParameter(hBarcode, ST_READ_PDF417, &i) ;
}
