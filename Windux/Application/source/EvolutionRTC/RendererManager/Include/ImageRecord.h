#ifndef IMAGERECORD_H
#define IMAGERECORD_H


#define MAX_FILE_NAME				20
struct IMAGE_RECORD
{
	char			*nibblized_record;
	long			nibblized_record_size;

//		struct HOB_HEADER	header;
	char			res_type[4];
	unsigned short	res_id;
	short			res_loaded;
	short			res_length;
	char			Name[32];
	unsigned long   base;      /* Pointer to bitplane */
	short			pitch;     /* Pitch of bitplane   */
	short           left;
	short			top;
	short			right;
	short			bottom;
	char			dummy[4];

	long			header_size;
	char			*binary_image;
	long			binary_image_size;

	char			*file_name;
	char			*ho__file_name;
	char			*hob_file_name;
	char			*png_file_name;
};


#endif	//IMAGERECORD_H
