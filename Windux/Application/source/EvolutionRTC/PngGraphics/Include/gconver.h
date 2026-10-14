


# ifndef GCONVER_H
# define GCONVER_H
//#include "vxWorks.h"


//#include "png.h"
//#include "dgtypes.h" ats


//#include <stdio.h>
//#include <iostream>	//added ats
//#include <fstream>	//added ats


#define MAX_FILE_NAME				20
#define NIBBLIZED_FILE_EXTENSION		".ho_"
#define BINARY_FILE_EXTENSION           	".hob"
#define PNG_FILE_EXTENSION              	".png"
#define SEPARATOR                       	'.'


#define DOS_MAX_FILE_NAME_LEN			8
#define DOS_MAX_FILE_EXT_LEN			3

#define _BLACK 0    /* 1 is BLUE*/
#define _WHITE 15

struct HOB_HEADER{
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

struct IMAGE_RECORD{
	char			*nibblized_record;
	long			nibblized_record_size;

	struct HOB_HEADER	header;
	long			header_size;
	char			*binary_image;
	long			binary_image_size;

	char			file_name[MAX_FILE_NAME];
	char        ho__file_name[MAX_FILE_NAME];
	char			hob_file_name[MAX_FILE_NAME];
	char			png_file_name[MAX_FILE_NAME];
};



#ifdef __cplusplus	//added ats
extern "C" {
#endif
struct IMAGE_RECORD *New_image_REC(void);
//void HobToPNG_set_file_names(struct IMAGE_RECORD * image_REC_ptr, char *nibblized_file_name);
//void free_image_REC(struct IMAGE_RECORD * image_REC_ptr);
//void hob_set_invert(struct IMAGE_RECORD * image_REC_ptr);
void set_binary_image_REC(struct IMAGE_RECORD * image_REC_ptr);
int unnibblize_image_REC(struct IMAGE_RECORD * image_REC_ptr);
void swap_binary_image_REC(struct IMAGE_RECORD * image_REC_ptr);
//RetStatus write_nibblized_image_REC(struct IMAGE_RECORD * image_REC_ptr);
//RetStatus write_binary_image_REC(struct IMAGE_RECORD * image_REC_ptr);
//RetStatus read_binary_image_REC(struct IMAGE_RECORD * image_REC_ptr);
//long filesize(FILE *stream);
int read_nibblized_image_REC(struct IMAGE_RECORD * image_REC_ptr);
//RetStatus set_nibblized_image_REC(struct IMAGE_RECORD * image_REC_ptr);
//void nibblize(char *unib, int size, char *nibb);
//int get_leftNibble(char *byte);
//int get_rightNibble(char *byte);
//char hex(int decimal);
//RetStatus read_png_file(char *fileName, char *pngBuffer);
//RetStatus write_png_file(char *fileName, char *pngBuffer);
//void ReadDataFunction(png_structp png_ptr, png_bytep data, png_size_t length);
//void WriteDataFunction(png_structp png_ptr, png_bytep data, png_size_t length);
//void FlushFunction(png_structp png_ptr);
#ifdef __cplusplus
}
#endif
#endif
