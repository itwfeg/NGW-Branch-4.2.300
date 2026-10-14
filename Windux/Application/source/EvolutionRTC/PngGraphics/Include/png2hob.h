# ifndef PNG2HOB_H
# define PNG2HOB_H

//#include "gconver.h"
//#include "resblob.h"
#include "hob2png.h"
#include <stdio.h>
#include <stdlib.h>



#define HOB_BIT_DEPTH			1
#define HOB_INTERLACE_TYPE		0

#define HOB_PNG_BLACK_COLOR	0
#define HOB_PNG_WHITE_COLOR	255
#define HOB_PNG_PLTE_ENTRIES	2

#define PNG_BYTES_TO_CHECK 4

/*	used for calculating the size of the header in the Image_Record struct only otherwise not used */
struct HOB_HEADER
{
	char		res_type[4];
	unsigned short	res_id;
	short		res_loaded;
	short		res_length;
	char		Name[32];
	unsigned long   base;      /* Pointer to bitplane */
	short		pitch;     /* Pitch of bitplane   */
	short           left;
	short		top;
	short		right;
	short		bottom;
	char		dummy[4];
};


#ifdef __cplusplus	//added ats
extern "C" {
#endif
//int is_OK_PNGTOHOB_arc(int argumentCounter);
//void show_PNGTOHOB_header(void);
//void show_PNGTOHOB_help(void);
//void PngToHob_display_status(struct IMAGE_RECORD * image_REC_ptr);
RetStatus PngToHob(struct IMAGE_RECORD * image_REC_ptr);
//RESBLOB *PngToHobAsNewResBlob(const SMBLOB *pngBlob, INT2 id, char *graphicName);
//int check_if_png(char *file_name);
#ifdef __cplusplus
}
#endif
#endif 
