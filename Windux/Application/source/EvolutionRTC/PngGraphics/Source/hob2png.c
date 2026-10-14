
#include "hob2png.h"
//#include "gconver.h"
#include "stdlib.h"
#include "ImageRecord.h"
#include "png.h"
//#include <memory.h>
//#include "resblob.h"
//#include "dg.h"

#if     defined( _WIN32 )
	#pragma warning(disable:4996)
#endif

/********************* PRIVATE DELCARATIONS *****************************/
/*
bool HeaderValid(struct HOB_HEADER header, UINT2 id);


int is_OK_HOBTOPNG_arc(int argumentCounter){

	show_HOBTOPNG_header();

	if ( argumentCounter != 2 ){
		show_HOBTOPNG_help();
		return 0;
	}

	return 1;
}

void show_HOBTOPNG_header(void){
	printf("\nHOBTOPNG 1.0 (beta) Hobart Corporation. Troy, Ohio\n");
	printf("____________________________________________________\n");
	printf("compiled Oct 10 1997 with pnglib (unknown version) and zlib (unknown version)\n");
}


void show_HOBTOPNG_help(void){
	printf("\nPurpose:  converts a Hobart graphic file into        Platform: DOS and WINDOWS\n");
        printf("          a B/W PNG (Portable Network Graphics) file.\n\n");
	printf("Usage:    hobtopng [ file[.ho_] ]\n\n");
	printf("          file.ho_  is the input text file that contains the graphic under\n");
	printf("                    hobart format. You can get this file running the program\n");
	printf("                    GETPICT.\n");
	printf("          file.hob  is the output file that would be created during the\n");
	printf("                    process of conversion from Hobart to PNG format. This is\n");
	printf("                    a binary version of file.ho_. This file is useful for\n");
        printf("                    programs like SHOWPICT.\n");
	printf("          file.png  is the output file that contains the PNG file.\n");
	printf("Other\n");
        printf("programs: PNGTOHOB, GETPICT, SHOWPICT, RESFILE\n\n");
}
*/
/***********************************************************************/

/*RetStatus check_if_hob_NIB(struct IMAGE_RECORD * image_REC_ptr){

	char file_type[8];
*/
	/*copy the first 8 characters from the nibblized_record */
/*
	memcpy(file_type, image_REC_ptr->nibblized_record, strlen(PICT_NIB));

   if ( strnicmp(file_type, PICT_NIB, strlen(PICT_NIB)) )
      return(STATUS_WRONGRESOURCETYPE);

   return(STATUS_OK);
}
*/
/*
RetStatus check_if_hob_BIN(struct IMAGE_RECORD * image_REC_ptr){

	char file_type[4];
*/
	/*copy the first 4 characters from the binary_record */
/*
	memcpy(file_type, image_REC_ptr->binary_image, strlen(PICT_BIN));

   if ( strnicmp(file_type, PICT_BIN, strlen(PICT_BIN)) )
      return(STATUS_WRONGRESOURCETYPE);

   return(STATUS_OK);
}



void HobToPng_display_status(struct IMAGE_RECORD * image_REC_ptr){

	printf("\ninput file name:          %s ", image_REC_ptr->file_name);
	printf("(size: %d bytes)\n",              image_REC_ptr->nibblized_record_size);
	printf("output filename (binary): %s ",   image_REC_ptr->hob_file_name);
	printf("(size: %d bytes)\n",              image_REC_ptr->binary_image_size+image_REC_ptr->header_size);
	printf("output filename (PNG):    %s\n",  image_REC_ptr->png_file_name);

	printf("\nHeader (size: %d bytes)\n",image_REC_ptr->header_size);
	printf("=======================\n");
	printf("type=%c", image_REC_ptr->header.res_type[0]);
	printf("%c",      image_REC_ptr->header.res_type[1]);
	printf("%c",      image_REC_ptr->header.res_type[2]);
	printf("%c; ",    image_REC_ptr->header.res_type[3]);


	printf("id=%d; ",      image_REC_ptr->header.res_id);
	printf("loaded=%d; ",  image_REC_ptr->header.res_loaded);
	printf("length=%d; ",  image_REC_ptr->header.res_length);
	printf("name=%s; ",    image_REC_ptr->header.Name);
	printf("base=%u; ",    image_REC_ptr->header.base);
	printf("pitch=%d;\n",  image_REC_ptr->header.pitch);
	printf("position=(%d,",image_REC_ptr->header.left);
	printf("%d,",          image_REC_ptr->header.top);
	printf("%d,",          image_REC_ptr->header.right);
	printf("%d); ",        image_REC_ptr->header.bottom);
	printf("dummy=%c",     image_REC_ptr->header.dummy[0]);
	printf("%c",           image_REC_ptr->header.dummy[1]);
	printf("%c",           image_REC_ptr->header.dummy[2]);
	printf("%c\n",         image_REC_ptr->header.dummy[3]);

*/
	/*Validate the file entries*/
/*
	if ( (image_REC_ptr->binary_image_size +
			image_REC_ptr->header_size) == (image_REC_ptr->header.res_length) ){
			printf("*** the Hobart graphic file was recovered successfully \n");

	}
	else
	{
			printf("*** The Hobart graphic file appears to be corrupted(image size(%d) + header size(%d) != res length(%d)\n",
						image_REC_ptr->binary_image_size,
						image_REC_ptr->header_size,
						image_REC_ptr->header.res_length);
	}

}
*/


RetStatus HobToPng(struct IMAGE_RECORD * image_REC_ptr)
{

	FILE *fp;
   png_structp png_ptr;
   png_infop info_ptr;

   /*for png_set_IHDR function */
   png_uint_32 width, height;
   int			interlace_type, compression_type, filter_type;
   int         bit_depth, color_type;

   /*for png_set_PLTE function */
   png_colorp palette;


   /*for png_set_text function */
	png_text text_ptr[3];
   int num_text;
   char res_id_str[8];

   /*for png_write_image function */

   /*png_bytep row_buf*/

	int k,j;
	int row_width;
   png_byte **row_pointers;


	/* open the file */
   fp = fopen(image_REC_ptr->png_file_name, "wb");
   if (fp == NULL)
      return(1);

   /* Create and initialize the png_struct with an error handlerfunctions.
    * If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters.  We also check that
    * the library version is compatible with the one used at compile time,
    * in case we are using dynamically linked libraries.  REQUIRED.
    */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL,
      (png_error_ptr)NULL, (png_error_ptr)NULL);

   if (png_ptr == NULL)
   {
      fclose(fp);
      return(1);
   }

   /* Allocate/initialize the image information data.  REQUIRED */
   info_ptr = png_create_info_struct(png_ptr);

   if (info_ptr == NULL){
      png_destroy_write_struct(&png_ptr,  (png_infopp)NULL);
      fclose(fp);
      return(1);
   }

   /* Set error handling.  REQUIRED if you aren't supplying your own
    * error hadnling functions in the png_create_write_struct() call.
    */
   if (setjmp(png_ptr->jmpbuf)){
      /* If we get here, we had a problem reading the file */
//      LOG_Log(_log, LOGLEVEL_ERROR, "libpng write error");
		fclose(fp);
      png_destroy_write_struct(&png_ptr,  (png_infopp)NULL);
      return(STATUS_PNGLIBERROR);
   }

   /* I/O initialization function */
   /* set up the output control if you are using standard C streams */
   png_init_io(png_ptr, fp);


   /* Set the image information here.  Width and height are up to 2^31,
   * bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
   * the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
   * PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
   * or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
	* PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
   * currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED
   */

	width 					= (png_uint_32)  (image_REC_ptr->right - image_REC_ptr->left);
   height               = (png_uint_32)  (image_REC_ptr->bottom - image_REC_ptr->top);
   bit_depth				= (int) HOB_BIT_DEPTH;
   color_type				= (int) PNG_COLOR_TYPE_PALETTE;
   interlace_type			= (int) HOB_INTERLACE_TYPE;
   compression_type		= (int) PNG_COMPRESSION_TYPE_DEFAULT;
   filter_type				= (int) PNG_FILTER_TYPE_DEFAULT;

   png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth,
   color_type, interlace_type, compression_type, filter_type);

   /* set the palette if there is one.  REQUIRED for indexed-color images */

	palette = (png_colorp)png_malloc(png_ptr, HOB_PNG_PLTE_ENTRIES * sizeof (png_color));

   if ( palette == NULL )
      return(STATUS_NOMEMORY);
   
	(palette + 0 )->red   = (png_byte) HOB_PNG_BLACK_COLOR;
	(palette + 0 )->green = (png_byte) HOB_PNG_BLACK_COLOR;
  	(palette + 0 )->blue  = (png_byte) HOB_PNG_BLACK_COLOR;


	(palette + 1 )->red   = (png_byte) HOB_PNG_WHITE_COLOR;
  	(palette + 1 )->green = (png_byte) HOB_PNG_WHITE_COLOR;
  	(palette + 1 )->blue  = (png_byte) HOB_PNG_WHITE_COLOR;

    png_set_PLTE(png_ptr, info_ptr, palette, HOB_PNG_PLTE_ENTRIES);

   /* Optionally write comments into the image */

 	/*itoa(image_REC_ptr->header.res_id, res_id_str, 10);*/
   sprintf(res_id_str, "%d", image_REC_ptr->res_id);

	text_ptr[0].key = "Resource_ID:";
	text_ptr[0].text = res_id_str;
   text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
   text_ptr[1].key = "Name:";
   text_ptr[1].text = image_REC_ptr->Name;
   text_ptr[1].compression = PNG_TEXT_COMPRESSION_NONE;
   text_ptr[2].key = "Author:";
   text_ptr[2].text = "HOBART_CORPORATION";
   text_ptr[2].compression = PNG_TEXT_COMPRESSION_NONE;

   num_text = 3;

   png_set_text(png_ptr, info_ptr, text_ptr, num_text);

   /* other optional chunks like cHRM, bKGD, tRNS, tIME, oFFs, pHYs, */

   /* Write the file header information.  REQUIRED */
   png_write_info(png_ptr, info_ptr);


   /* The easiest way to write the image (you may have a different memory
    * layout, however, so choose what fits your needs best).
    */

	row_pointers = (png_byte **) malloc((image_REC_ptr->bottom - image_REC_ptr->top) * sizeof(png_byte *));

   if ( row_pointers == NULL )
      return(STATUS_NOMEMORY);

   row_width = 2 * image_REC_ptr->pitch;

   k=0;
   for (j = 0; j < (image_REC_ptr->bottom - image_REC_ptr->top) ; j++)
   {
   	*(row_pointers + j) = (png_byte *)(image_REC_ptr->binary_image + k);
      k = k + row_width;
   }

 	/**** write out the entire image data in one call ***/
   png_write_image(png_ptr, row_pointers);

   /* You can write optional chunks like tEXt, zTXt, and tIME at the end
    * as well.
    */
   /* It is REQUIRED to call this to finish writing the rest of the file */
   png_write_end(png_ptr, info_ptr);

   /* clean up after the write, and free any memory allocated */
   png_destroy_write_struct(&png_ptr, &info_ptr);

	/* close the file */
	fclose(fp);
	free(row_pointers);
	/* if you malloced the palette, free it here */
	free(palette);
	return(STATUS_OK);
}
/*
SMBLOB *HobToPngAsNewBlob(const SMBLOB *hobBlob, UINT2 resid, char **graphicName)
{
	png_structp png_ptr;
	png_infop info_ptr;
	struct IMAGE_RECORD * image_REC_ptr;
	SMBLOB *pngBlob;
	RESBLOB *resBlob;
*/
	/*for png_set_IHDR function */
/*	png_uint_32 width, height;
	int			interlace_type, compression_type, filter_type;
	int         bit_depth, color_type;
*/
	/*for png_set_PLTE function */
//	png_colorp palette;


	/*for png_set_text function */
//	png_text text_ptr[3];
//	int num_text;
//	char res_id_str[8];

	/*for png_write_image function */

	/*png_bytep row_buf*/

//	int k,j;
//	int row_width;
//	png_byte **row_pointers;

	/*create the image record*/
/*	image_REC_ptr = New_image_REC();

	if (image_REC_ptr == NULL)
	  return(NULL);

	image_REC_ptr->nibblized_record = (char *) BLOB_GetDataAsNewCharArray(hobBlob);

	image_REC_ptr->nibblized_record_size = BLOB_GetSize(hobBlob);
*/
	/* check if the file is a hobart format graphic file */
	/* check_if_hob_NIB(image_REC_ptr); */

	/*calculate the binary image size*/
/*	image_REC_ptr->header_size = sizeof(image_REC_ptr->header);
	image_REC_ptr->binary_image_size  = BLOB_GetSize(hobBlob) - image_REC_ptr->header_size;

	if((image_REC_ptr->binary_image_size + image_REC_ptr->header_size) > 10000)
	{
   	LOG_Log(_log, LOGLEVEL_ERROR, "IMAGE TOO LARGE....REJECTING...");
		return(NULL);
	}
*/
	/*allocate memory for data */
//	if ((image_REC_ptr->binary_image = (char *) malloc(image_REC_ptr->binary_image_size)) == NULL)
//		return(NULL);

	/*copy header */
//	memcpy(&image_REC_ptr->header, (char *)BLOB_GetData(hobBlob), sizeof(struct HOB_HEADER));

	/* copy data */
//	memcpy(image_REC_ptr->binary_image, ((char *)BLOB_GetData(hobBlob) + image_REC_ptr->header_size), image_REC_ptr->binary_image_size);

	/* swap the data */
//	swap_binary_image_REC(image_REC_ptr);

	/* HobToPng_display_status(image_REC_ptr); */

	/*reverse the bits*/
//	hob_set_invert(image_REC_ptr);

   /* Determine if the header is valid */
   /* If it isn't, clean up and leave. */
//   if (!HeaderValid(image_REC_ptr->header, resid))
// 	 {
//   	free_image_REC(image_REC_ptr);

//   	return NULL;
//   }

	/* Create and initialize the png_struct with an error handlerfunctions.
	 * If you want to use the default stderr and longjump method,
	 * you can supply NULL for the last three parameters.  We also check that
	 * the library version is compatible with the one used at compile time,
	 * in case we are using dynamically linked libraries.  REQUIRED.
	 */
/*	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL,
		(png_error_ptr)NULL, (png_error_ptr)NULL);

	if (png_ptr == NULL){
		return(NULL);
	}
*/
	/* Allocate/initialize the image information data.  REQUIRED */
/*	info_ptr = png_create_info_struct(png_ptr);

	if (info_ptr == NULL){
		png_destroy_write_struct(&png_ptr,  (png_infopp)NULL);
		return(NULL);
	}
*/
	/* Set error handling.  REQUIRED if you aren't supplying your own
	 * error hadnling functions in the png_create_write_struct() call.
	 * i.e: png_set_error_fn(png_ptr, thiss, ErrorFunction, ErrorFunction); */
//	if (setjmp(png_ptr->jmpbuf)){
		/* If we get here, we had a problem reading the file */
/*      LOG_Log(_log, LOGLEVEL_ERROR, "libpng write error");
		png_destroy_write_struct(&png_ptr,  (png_infopp)NULL);
		return(NULL);
	}
*/
	/* Create the output pngBlob. */
//	pngBlob = BLOB_NewSize(512);

	/* Create a RESBLOB so we can use the index in our callback. */
//	resBlob = RESBLOB_NewBlob(pngBlob);

	/* Set callbacks so we can read data directly from the blob. */
	/* png_set_read_fn(png_ptr, thiss, ReadDataFunction); */
//	png_set_write_fn(png_ptr, resBlob, WriteDataFunction, FlushFunction);


	/* Set the image information here.  Width and height are up to 2^31,
	* bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
	* the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
	* PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
	* or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
	* PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
	* currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED
	*/
/*
	width 					= (png_uint_32)  (image_REC_ptr->header.right - image_REC_ptr->header.left);
	height               = (png_uint_32)  (image_REC_ptr->header.bottom - image_REC_ptr->header.top);
	bit_depth				= (int) HOB_BIT_DEPTH;
	color_type				= (int) PNG_COLOR_TYPE_PALETTE;
	interlace_type			= (int) HOB_INTERLACE_TYPE;
	compression_type		= (int) PNG_COMPRESSION_TYPE_DEFAULT;
	filter_type				= (int) PNG_FILTER_TYPE_DEFAULT;

	png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth,
	color_type, interlace_type, compression_type, filter_type);
*/
	/* set the palette if there is one.  REQUIRED for indexed-color images */
/*
	palette = (png_colorp)png_malloc(png_ptr, HOB_PNG_PLTE_ENTRIES * sizeof (png_color));

	if ( palette == NULL )
		return(NULL);

	(palette + 0 )->red   = (png_byte) HOB_PNG_BLACK_COLOR;
	(palette + 0 )->green = (png_byte) HOB_PNG_BLACK_COLOR;
	(palette + 0 )->blue  = (png_byte) HOB_PNG_BLACK_COLOR;


	(palette + 1 )->red   = (png_byte) HOB_PNG_WHITE_COLOR;
	(palette + 1 )->green = (png_byte) HOB_PNG_WHITE_COLOR;
	(palette + 1 )->blue  = (png_byte) HOB_PNG_WHITE_COLOR;

	 png_set_PLTE(png_ptr, info_ptr, palette, HOB_PNG_PLTE_ENTRIES);
*/
	/* Optionally write comments into the image */

	/*itoa(image_REC_ptr->header.res_id, res_id_str, 10);*/
/*   sprintf(res_id_str, "%d", image_REC_ptr->header.res_id);

	text_ptr[0].key = "Resource_ID:";
	text_ptr[0].text = res_id_str;
	text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
	text_ptr[1].key = "Name:";
	text_ptr[1].text = image_REC_ptr->header.Name;
	text_ptr[1].compression = PNG_TEXT_COMPRESSION_NONE;
	text_ptr[2].key = "Author:";
	text_ptr[2].text = "HOBART_CORPORATION";
	text_ptr[2].compression = PNG_TEXT_COMPRESSION_NONE;

	num_text = 3;

	png_set_text(png_ptr, info_ptr, text_ptr, num_text);
*/
   /* copy the graphic name to the passed in string so that the calling function can use it */
//   strncpy(*graphicName, image_REC_ptr->header.Name, 31);

	/* other optional chunks like cHRM, bKGD, tRNS, tIME, oFFs, pHYs, */

	/* Write the file header information.  REQUIRED */
//	png_write_info(png_ptr, info_ptr);


	/* The easiest way to write the image (you may have a different memory
	 * layout, however, so choose what fits your needs best).
	 */
/*
	row_pointers = (png_byte **) malloc((image_REC_ptr->header.bottom - image_REC_ptr->header.top) * sizeof(png_byte *));

	if ( row_pointers == NULL )
		return(NULL);

	row_width = 2 * image_REC_ptr->header.pitch;

	k=0;
	for (j = 0; j < (image_REC_ptr->header.bottom - image_REC_ptr->header.top) ; j++){
		*(row_pointers + j) = (png_byte *)(image_REC_ptr->binary_image + k);
		k = k + row_width;
	}
*/
	/**** write out the entire image data in one call ***/
//	png_write_image(png_ptr, row_pointers);

	/* You can write optional chunks like tEXt, zTXt, and tIME at the end
	 * as well.
	 */
	/* It is REQUIRED to call this to finish writing the rest of the file */
//	png_write_end(png_ptr, info_ptr);

	/* clean up after the write, and free any memory allocated */
//	png_destroy_write_struct(&png_ptr, &info_ptr);

//	free(row_pointers);

	/* if you malloced the palette, free it here */
//	free(palette);

	/*kill the image record*/
//	free_image_REC(image_REC_ptr);

	/* Copy the resBlob back to the pngBlob. */
//	BLOB_Copy(pngBlob, resBlob->blob);

//	RESBLOB_Delete(resBlob);

//	return(pngBlob);
//}
/*
void ReadDataFunction(png_structp png_ptr, png_bytep data, png_size_t length)
{
	RESBLOB *resBlob;

	resBlob = (RESBLOB *)png_ptr->io_ptr;

	if((resBlob->index + length) > resBlob->blob->maxsize)
	{
   	LOG_Log(_log, LOGLEVEL_ERROR, "BLOB Bounds exceeded!");
		return;
	}

	memcpy(data, (((char *)BLOB_GetData(resBlob->blob)) + resBlob->index), length);

	resBlob->index += length;

	return;
}

void WriteDataFunction(png_structp png_ptr, png_bytep data, png_size_t length)
{
	RESBLOB *resBlob;

	resBlob = (RESBLOB *)png_ptr->io_ptr;

	BLOB_AppendBuffer(resBlob->blob, data, length);

	resBlob->index += length;

	return;
}

void FlushFunction(png_structp png_ptr)
{
	return;
}

*/
/******************** PRIVATE IMPLEMENTATIONS ***************************/
/*
bool HeaderValid(struct HOB_HEADER header, UINT2 id)
{	*/
	/* Is the resource type valid? */
//	if (strncmp(header.res_type, "PICT", 4))
//   	return FALSE;

	/* Do the resource ids match? */
//	if (header.res_id != id)
//   	return FALSE;

   /* Are the left and right values valid? */
//	if (header.left < 0 || header.right < 0)
//   	return FALSE;

   /* Are the top and bottom values valid? */
//   if (header.top < 0 || header.bottom < 0)
//   	return FALSE;

   /* Is the length value valid? */
//   if (header.res_length < 0)
//   	return FALSE;

	/* Everything looks OK. */
//   return TRUE;
//}

#if     defined( _WIN32 )
	#pragma warning(default:4996)
#endif
