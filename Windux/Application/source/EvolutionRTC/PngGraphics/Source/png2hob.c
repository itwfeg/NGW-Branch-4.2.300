//#include "dg.h"
#include "png2hob.h"
//#include "gconver.h"
#include "png.h"
//#include "resblob.h"

#if     defined( _WIN32 )
	#pragma warning(disable:4996)
#endif

/*
int is_OK_PNGTOHOB_arc(int argumentCounter){

	show_PNGTOHOB_header();

	if ( argumentCounter != 2 ){
		show_PNGTOHOB_help();
		return 0;
	}

	return 1;
}

void show_PNGTOHOB_header(void){
	printf("\nPNGTOHOB 1.0 (beta) Hobart Corporation. Troy, Ohio\n");
	printf("____________________________________________________\n");
	printf("compiled Oct 10 1997 with pnglib (unknown version) and zlib (unknown version)\n");
}


void show_PNGTOHOB_help(void){

	printf("\nPurpose:  converts a B/W PNG (Portable Network Graphics) file     Platform: DOS\n");
        printf("          into a Hobart format graphic file.                        and WINDOWS\n\n");
	printf("Usage:    pngtohob  [ file[.png] ]\n\n");
 	printf("          file.png  is the input file that contains the B/W PNG file.\n");
	printf("          file.hob  is the output binary file that would be created during the\n");
	printf("                    process of conversion from PNG to Hobart format.\n");
	printf("          file.ho_  is the output text file that contains the graphic under\n");
	printf("                    hobart format.\n");
	printf("Other\n");
	printf("programs: HOBTOPNG, GETPICT, SHOWPICT, RESFILE\n\n");
}

void PngToHob_display_status(struct IMAGE_RECORD * image_REC_ptr){

	printf("\ninput file name (PNG):    %s\n", image_REC_ptr->png_file_name);
	printf("output filename (binary): %s ", image_REC_ptr->hob_file_name);
	printf("(size: %d bytes)\n",            image_REC_ptr->binary_image_size+image_REC_ptr->header_size);
	printf("output filename (HO_):    %s",  image_REC_ptr->ho__file_name);
	printf("(size: %d bytes)          \n",  image_REC_ptr->nibblized_record_size);

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
			image_REC_ptr->header_size) == (image_REC_ptr->header.res_length) )
	{
			printf("\n *** the Hobart graphic file was created successfully \n");

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
int check_if_png(char *file_name)
{

   FILE *fp;
   char buf[PNG_BYTES_TO_CHECK];

   /* Open the prospective PNG file. */

   if ((fp = fopen(file_name, "rb")) == NULL)
   {
	printf("Cannot open file: %s", file_name);
//   	LOG_Log(_log, LOGLEVEL_ERROR, "Cannot open the file: \"%s\"",file_name);
		return(1);		//exit(1);
   }

   /* Read in the signature bytes */

   if (fread(buf, 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK)
   {
	printf("file is not a PNG file: %s", file_name);
//   	LOG_Log(_log, LOGLEVEL_ERROR, "*** \"%s\" is not a PNG file ***",file_name);
		return(1);		//exit(1);
   }

   /* Compare the first PNG_BYTES_TO_CHECK bytes of the signature. */

   fclose(fp);
	return(png_check_sig((png_bytep)buf, PNG_BYTES_TO_CHECK));
}



RetStatus PngToHob(struct IMAGE_RECORD * image_REC_ptr)
{
   FILE *fp;
   png_structp png_ptr;
   png_infop info_ptr;
   struct HOB_HEADER tempHeader;
   /*for png_set_IHDR function */
   png_uint_32 width, height;
   int			interlace_type, compression_type, filter_type;
   int         bit_depth, color_type;
   long			sizeof_binary_record;

   /*for png_set_PLTE function */
   png_colorp palette;
   int num_palette;

		/*png_bytep row_buf*/
   int k,j;
   int row_width;
   png_byte **row_pointers;


	/* validate if is a PNG file*/
   if (! check_if_png(image_REC_ptr->png_file_name) )
      return(STATUS_WRONGRESOURCETYPE);

   /* open the file */
   fp = fopen(image_REC_ptr->png_file_name, "rb");
   if (fp == NULL)
      return(STATUS_CANTOPENFILE);

   /* Create and initialize the png_struct with an error handlerfunctions.
	 * If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters.  We also check that
    * the library version is compatible with the one used at compile time,
    * in case we are using dynamically linked libraries.  REQUIRED.
    */
   png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL,
      (png_error_ptr)NULL, (png_error_ptr)NULL);


   if (png_ptr == NULL){
      fclose(fp);
      return(STATUS_NOMEMORY);
   }

   /* Allocate/initialize the image information data.  REQUIRED */
   info_ptr = png_create_info_struct(png_ptr);

   if (info_ptr == NULL){
      png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		fclose(fp);
      return(STATUS_NOMEMORY);
	}

   /* Set error handling.  REQUIRED if you aren't supplying your own
    * error hadnling functions in the png_create_read_struct() call.
    */
   if (setjmp(png_ptr->jmpbuf)){
      png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
      fclose(fp);
      return(STATUS_PNGLIBERROR);
   }

   /* I/O initialization function */
   /* set up the output control if you are using standard C streams */
   png_init_io(png_ptr, fp);
   png_read_info(png_ptr, info_ptr);

   png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,&color_type, &interlace_type, &compression_type, &filter_type);

   /*validate the binary header data */

   if (bit_depth != HOB_BIT_DEPTH){
   	/* The png graph, it is not HOB_BIT_DEPTH bit depth */
   	return(STATUS_PNGINVALIDDATA);
   }

   if ( !(color_type == PNG_COLOR_TYPE_PALETTE || color_type == PNG_COLOR_TYPE_GRAY) ){
   	/* The png graph, it is not COLOR_TYPE_PALETTE or COLOR_TYPE_GRAY */
   	return(STATUS_PNGINVALIDDATA);
	}

 	/*copy the binary header data */

   image_REC_ptr->res_type[0]='P';
   image_REC_ptr->res_type[1]='I';
   image_REC_ptr->res_type[2]='C';
   image_REC_ptr->res_type[3]='T';

	image_REC_ptr->res_loaded	= 1;

	image_REC_ptr->Name[0]='N';
   image_REC_ptr->Name[1]='O';
   image_REC_ptr->Name[2]=' ';
   image_REC_ptr->Name[3]='N';
   image_REC_ptr->Name[4]='A';
   image_REC_ptr->Name[5]='M';
   image_REC_ptr->Name[6]='E';

   for (j = 7; j < 32 ; j++) image_REC_ptr->Name[j]= '\0';

   image_REC_ptr->base = (unsigned long) 0;
	image_REC_ptr->pitch = (short) (width / 16);
	if (width % 16)
		image_REC_ptr->pitch = (short) (image_REC_ptr->pitch + 1 );
   image_REC_ptr->left = (short) 0;
	image_REC_ptr->top = (short) 0;
	image_REC_ptr->right = (short) width;
	image_REC_ptr->bottom = (short) height;

   for (j = 0; j< 4 ; j++) image_REC_ptr->dummy[j]= '\0';

	/*calculate the binary image size*/
	image_REC_ptr->binary_image_size  = (image_REC_ptr->bottom - image_REC_ptr->top) * image_REC_ptr->pitch * 2;
	image_REC_ptr->header_size = sizeof(tempHeader);
   	sizeof_binary_record = image_REC_ptr->header_size + image_REC_ptr->binary_image_size;
	image_REC_ptr->res_length = (short) sizeof_binary_record;

	png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);

	if ( num_palette != HOB_PNG_PLTE_ENTRIES )
	{
		/* The png graph, has not HOB_PNG_PLTE_ENTRIES entries */
		return(STATUS_PNGINVALIDDATA);
	}


	/* optional call to gamma correct and add the background to the palette
	 * and update info structure.  REQUIRED if you are expecting libpng to
	 * update the palette for you (ie you selected such a transform above).
	 */
	png_read_update_info(png_ptr, info_ptr);


	/*allocate memory for the binary image*/
	image_REC_ptr->binary_image = (char *) malloc(image_REC_ptr->binary_image_size);
	if ( image_REC_ptr->binary_image== NULL)
		return(STATUS_NOMEMORY);  /* terminate program if out of memory */

	/* allocate memory */

	row_pointers = (png_byte **) malloc( (image_REC_ptr->bottom - image_REC_ptr->top) *  sizeof(png_byte *) );
	if (row_pointers == NULL )
		return(STATUS_NOMEMORY);

	row_width = 2 * image_REC_ptr->pitch;
	k=0;

	for (j = 0; j < (image_REC_ptr->bottom - image_REC_ptr->top) ; j++){
		*(row_pointers + j) = (png_byte *)(image_REC_ptr->binary_image + k);
		k = k + row_width;
	}

	/* Now it's time to read the image.  One of these methods is REQUIRED */
	png_read_image(png_ptr, row_pointers);

	/* read rest of file, and get additional chunks in info_ptr - REQUIRED */
	png_read_end(png_ptr, info_ptr);

	/* clean up after the read, and free any memory allocated - REQUIRED */
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

	/* close the file */
	fclose(fp);
	free(row_pointers);

	return(STATUS_OK);
}

/*
RESBLOB *PngToHobAsNewResBlob(const SMBLOB *pngBlob, INT2 id, char *graphicName)
{
  struct IMAGE_RECORD * image_REC_ptr;
  RESBLOB *inputResBlob, *outputResBlob;
  png_structp png_ptr;
  png_infop info_ptr;
  int resourceLength;

*/
  /*for png_set_IHDR function */
/*
  png_uint_32 width, height;
  int         interlace_type, compression_type, filter_type;
  int         bit_depth, color_type;
  long            sizeof_binary_record;

*/
  /*for png_set_PLTE function */
/*
  png_colorp palette;
  int num_palette;
*/

  /*png_bytep row_buf*/
/*
  int k,j;
  int row_width;
  png_byte **row_pointers;

*/
 /* These variables are used in removing unwanted bits at the end of a line
    of a graphic. */
//  bool eXtraBit;   /* Boolean to say that an "overflow" occured. */
//  INT4  eXtraBits;  /* Amount of bits into next byte the overflow took. */
/*  INT4  totalBytes;
 INT4  lastByte;
*/
 /* -chris: End of additional variables. */


  /* Create a RESBLOB out of pngBlob so we can use it's index in our callback. */
/*   inputResBlob = RESBLOB_NewBlob(pngBlob); */

  /*creat the image record*/
/*   image_REC_ptr = New_image_REC(); */

  /* Set the resource id */
/*   image_REC_ptr->header.res_id = id; */

  /* Create and initialize the png_struct with an error handlerfunctions.
   * If you want to use the default stderr and longjump method,
   * you can supply NULL for the last three parameters.  We also check that
   * the library version is compatible with the one used at compile time,
   * in case we are using dynamically linked libraries.  REQUIRED.
   */
/* 
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL,
            (png_error_ptr)NULL, (png_error_ptr)NULL);


  if (png_ptr == NULL){
        return(NULL);
  }
*/
  /* Allocate/initialize the image information data.  REQUIRED */
/* 
  info_ptr = png_create_info_struct(png_ptr);

  if (info_ptr == NULL){
      png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
      return(NULL);
  }
*/
  /* Set error handling.  REQUIRED if you aren't supplying your own
   * error hadnling functions in the png_create_read_struct() call.
   */
/* 
  if (setjmp(png_ptr->jmpbuf)){
      png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
      return(NULL);
  }
*/
  /* Set callbacks so we can read data directly from the blob. */
/*   png_set_read_fn(png_ptr, inputResBlob, ReadDataFunction);  */

  /* Read header info. */
/*  png_read_info(png_ptr, info_ptr); */
/* 
  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,&color_type, &interlace_type, &compression_type, &filter_type);
*/
  /*validate the binary header data */
/*
  if (bit_depth != HOB_BIT_DEPTH){ */
      /* The png graph, it is not HOB_BIT_DEPTH bit depth */
/*      return(NULL);
  }

  if ( !(color_type == PNG_COLOR_TYPE_PALETTE || color_type == PNG_COLOR_TYPE_GRAY) ){ */
      /* The png graph, it is not COLOR_TYPE_PALETTE or COLOR_TYPE_GRAY */
/*      return(NULL);
  }
*/
  /*copy the binary header data */
/*
  image_REC_ptr->header.res_type[0]='P';
  image_REC_ptr->header.res_type[1]='I';
  image_REC_ptr->header.res_type[2]='C';
  image_REC_ptr->header.res_type[3]='T';

  image_REC_ptr->header.res_loaded    = 1;

  memset(image_REC_ptr->header.Name, '\0', 32);

  strncpy(image_REC_ptr->header.Name, graphicName, 31);

  image_REC_ptr->header.base = (unsigned long) 0;
  image_REC_ptr->header.pitch = (short) (width / 16);

  if (width % 16)
  {
    image_REC_ptr->header.pitch = (short) (image_REC_ptr->header.pitch + 1);
    eXtraBit = TRUE;
  }


  image_REC_ptr->header.left = (short) 0;
  image_REC_ptr->header.top = (short) 0;
  image_REC_ptr->header.right = (short) width ;
  image_REC_ptr->header.bottom = (short) height;

  for (j = 0; j< 4 ; j++) image_REC_ptr->header.dummy[j]= '\0';
*/
  /*calculate the binary image size*/
/*  
  image_REC_ptr->binary_image_size  = height * image_REC_ptr->header.pitch * 2;

  image_REC_ptr->header_size = sizeof(image_REC_ptr->header);
  sizeof_binary_record = image_REC_ptr->header_size + image_REC_ptr->binary_image_size;
  image_REC_ptr->header.res_length = (short) sizeof_binary_record;

  png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);

  if ( num_palette != HOB_PNG_PLTE_ENTRIES ){ */
      /* The png graph, has not HOB_PNG_PLTE_ENTRIES entries */
/*      return(NULL);
  }
*/

  /* optional call to gamma correct and add the background to the palette
   * and update info structure.  REQUIRED if you are expecting libpng to
   * update the palette for you (ie you selected such a transform above).
   */
/*
  png_read_update_info(png_ptr, info_ptr);
*/

  /*allocate memory for the binary image*/
/*
  image_REC_ptr->binary_image = (char *) malloc(image_REC_ptr->binary_image_size);
  if ( image_REC_ptr->binary_image== NULL)
      return(NULL); */ /* terminate program if out of memory */

  /* allocate memory */
/*
  row_pointers = (png_byte **) malloc( height *  sizeof(png_byte *) );
  if (row_pointers == NULL )
      return(NULL);

  row_width = 2 * image_REC_ptr->header.pitch;

  k=0;
  for (j = 0; j < (int)height ; j++){
      *(row_pointers + j) = (png_byte *)(image_REC_ptr->binary_image + k);
  k = k + row_width;
  }
*/
  /* Now it's time to read the image.  One of these methods is REQUIRED */
/*  png_read_image(png_ptr, row_pointers); */

  /* read rest of file, and get additional chunks in info_ptr - REQUIRED */
/*   png_read_end(png_ptr, info_ptr);  */

  /* clean up after the read, and free any memory allocated - REQUIRED */

/*  png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL); 

  free(row_pointers);
*/
  /*reverse the bits*/
/*  hob_set_invert(image_REC_ptr); */


  /* Problems with how the scales displayed a graphic sent to them, dictated the
     next section of code.  The problem was a vertical line displayed on the
     right hand side of an image if the image had an part of an unused byte
     at the end of each row.  To correct this, the following code finds the last
     byte at each row of the image and "cleans it" so that all unused bits in
     the image are turned off.  This keeps the line from displaying on the
     scale. */
/*
  if ( eXtraBit == TRUE )
  {
    INT4 j, eXtraByte;
    INT4 mask;
*/
    /* Number of unused, extra bits, in the last byte of each row of the image. */
/*     eXtraBits = (8 - (width % 8));  */

    /* There could be an extra byte, as well, which is unused at the end of the
       image.  */
/*     if (width % 16 <= 8)
       eXtraByte = width / 8 + (width % 8 ? 1 : 0);
    else
       eXtraByte = 0;
*/
    /* Mask used to clear out unwanted bits of a wanted byte. */
/* 
    mask = ~((0x01 << eXtraBits) - 1 );
*/
    /* Last byte on a line. */
/*     lastByte = (width / 8 - (width % 8 ? 0 : 1));
*/

    /* Totalbytes on the line. */
/*     totalBytes = (width /16 + (width % 16 ? 1 : 0 )) * 2; */

    /* For each row... */
/*     for (j = 0; j < (int)height; j++)
    { */
     /* Clean the last byte of each bit. */
/*      if (eXtraBits != 8)
     {
       *(image_REC_ptr->binary_image + lastByte + j * totalBytes) =
       (char) (*(image_REC_ptr->binary_image + lastByte + j * totalBytes) & mask);
     }
*/
     /* If there's an extra byte, just blank it out. */
/* 
     if (eXtraByte)
        *(image_REC_ptr->binary_image + eXtraByte + j * totalBytes) = 0;
    }
  }
*/

  /* Save the resource length before we swap it. */
/*  resourceLength = image_REC_ptr->header.res_length; */

  /* swap and nibblized the data */
/*   set_nibblized_image_REC(image_REC_ptr);  */

  /* Make a BLOB out of the image_REC_ptr. */
/*   outputResBlob = RESBLOB_New(image_REC_ptr->nibblized_record, image_REC_ptr->nibblized_record_size); */
/* 
  RESBLOB_SetLength(outputResBlob, resourceLength);
*/
  /*kill the image record*/
/* 
  free_image_REC(image_REC_ptr);
*/


  /* Dont need the input resBlob anymore.  Delete it. */
/*
  RESBLOB_Delete(inputResBlob);

  return(outputResBlob);
}

*/

#if     defined( _WIN32 )
	#pragma warning(default:4996)
#endif