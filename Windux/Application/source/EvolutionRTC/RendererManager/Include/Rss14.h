/*
 * Rss14.h
 *
 *  Created on: Jul 8, 2013
 *      Author: aaron
 */

#ifndef RSS14_H_
#define RSS14_H_
#include "GenericBarcode.h"
#include "BitMap.h"
#include "CSmartFontHandle.h"


class Rss14 : public GenericBarcode
{

	public:
	Rss14(BitMap *bitmap, Hobart::Rectangle *bounds, unsigned char *data, INT16 length, INT16 charHeight, INT16 leading, INT16 ascent, INT16 faceMax, Util::CSmartFontHandle fontId, int modsize, int rotation, BitMap &txt_bmp, bool isGS1DataBarExpanded = false); // RSS14

	private:
	static const int tbl174[30];
	static const unsigned char iswhat[256];
	static const int CCSizes[14];
	static const int parWts[24];

	/* 2-segments, no guard patterns */
	static const int ELMNTS = 21;
	/* 2-segment symbol width in modules, no guard patterns	 */
	static const int SYM_W = 49;

	/* height */
	/*#define SYM_H  	46  	EFP####define SYM_H 60*/
	/* max double segments */
	static const int MAX_DBL_SEGS 	= 16;
	unsigned char dblPattern[Rss14::MAX_DBL_SEGS][Rss14::ELMNTS];
	unsigned char chexPattern[Rss14::MAX_DBL_SEGS * Rss14::SYM_W + 2];
	unsigned char linPattern[Rss14::MAX_DBL_SEGS * Rss14::ELMNTS + 4];
	unsigned char sepPattern[Rss14::MAX_DBL_SEGS *( Rss14::ELMNTS + 12) + 4];
	unsigned char dataBuffer[100];
	static const int MODS_EAN_RSS14 = 124;

	static const int MAX_DATA  		= 75+338;
	static const int MAX_CW 		= 176;
	static const int MAX_ROWS 		= 44;		/* ccb-4 max rows */
	static const int MAX_WIDTH 		= 101; 		/* 4 column cca_b	 */
	static const int MAX_ELMNTS 	= 53; 		/* includes qz	*/
	static const int MAX_BYTES 		= 148; 		/* maximum byte mode capacity for ccb4 */
	static const int MAX_CCASIZE 	= 4; 		/* index to 187 in CCSizes   */

	static const int L_PAD 			= 1; 		/* CC left offset */
	static const int MAX_PIXMULT  	= 12;

	static const int MAX_FNAME 		= 25;
	static const int MAX_KEYDATA 	= 120;

	static const int PARITY_MOD  	= 211;
	static const int PARITY_PWR 	= 3;
	static const int HR_SPACES  	= 2;
	static const int LAST_HR_CHAR_OFFSET 	= 15;

	#define FNC1 '#'

	static const int SYM_SEP 	= 94;

	static const int NUM_MODE 	= 1;
	static const int ALNU_MODE 	= 2;
	static const int ISO_MODE 	= 3;
	static const int ALPH_MODE 	= 4;
	static const int FINI_MODE 	= 5;

	static const int IS_NUM 	= 0x1;
	static const int IS_FNC1 	= 0x2;
	static const int IS_ALNU 	= 0x4;
	static const int IS_ISO 	= 0x8;
	static const int IS_FINI 	= 0x80;


	int linFlag; /* tells pack whether linear or cc is being encoded  */
	int rowWidth;
	int line1;

	bool isGS1DataBarExpanded_;

	typedef struct encode_t {
		unsigned char *str;
		int iStr;
		unsigned char *bitField;
		int iBit;
		int mode;
		int typeAI;
		int diNum;
		int diAlpha;
	} encodeT;

	enum {
		AIx,
		AIdummy, /* not used   */
		AI21,
		AI8004
	};

	typedef struct s_params {
		int pixMult;				/* pixels per X	 */
		int undercut;				/* pixels to undercut */
		int Yundercut;				/* Y pixels to undercut	 */
		int sepHt;					/* separator row height	  */
		int segWidth;
		unsigned char *dataStr;
	} sParams;
	sParams params;
	int encode(unsigned char *string, unsigned char bars[Rss14::MAX_DBL_SEGS][Rss14::ELMNTS]);
	void elements(BitMap *bitmap, int elmCnt, unsigned char pattern[], int height, GenericBarcode *barcode,
		                 	   int whtFirst, int rightShift, int reverse);
	void getRSSwidths(int val, int n, int elements, int maxWidth, int noNarrow);
	int cnvSeparator(int elmCnt, unsigned char pattern[], unsigned char sepPattern[], int whtFirst);
	int symCharPat(unsigned char bars[], int symValue, int parity, int weight,int forwardFlag);
	int getVal12(unsigned char bitString[], int symNdx);

	int combins(int n, int r);

	int pack(unsigned char str[], unsigned char bitField[] );
	int check2DData(unsigned char dataStr[]);
	int procALNU(encodeT *encode);
	int procISO(encodeT *encode);
	int procNUM(encodeT  *encode);
	int doMethods(encodeT *encode);
	int insertPad(encodeT *encode);
	int getUnusedBitCnt(int iBit, int *size);
	void putBits(unsigned char bitField[], int bitPos, int length, size_t bits);
	int doLinMethods(unsigned char str[], int *iStr, unsigned char bitField[], int *iBit);
	size_t yymmdd(unsigned char str[]);
	void cnv13 (unsigned char str[], int *iStr, unsigned char bitField[], int *iBit);
	void cnv12 (unsigned char str[], int *iStr, unsigned char bitField[], int *iBit);

	//extern VOID DrawModule(BARCODE_INFO *barcode_info, MODULE_TYPE type, int width, int height);



};

#endif
