# ifndef HOB2PNG_H
# define HOB2PNG_H

#include "stdio.h"
#include "ImageRecord.h"
//#include "gconver.h"
//#include "blob.h"
#include "string.h"

#define HOB_BIT_DEPTH                   1
#define HOB_INTERLACE_TYPE              0

#define HOB_PNG_BLACK_COLOR				0
#define HOB_PNG_WHITE_COLOR				255
#define HOB_PNG_PLTE_ENTRIES			2

#define PICT_NIB                        "50494354"
#define PICT_BIN                        "PICT"
#define _BLACK 							0    /* 1 is BLUE*/
#define _WHITE 							15

typedef enum 
{
	STATUS_INVALID,

	STATUS_PARAM,						/* Parameter has wrong value */
	STATUS_NOMEMORY,					/* Not enough memory */
	STATUS_CANTOPENFILE,        	/* Unable to open a file */

	STATUS_TRUE,						/* The result is TRUE */
	STATUS_FALSE,						/* The result is FALSE */
	STATUS_OK,							/* Everything went OK */
	STATUS_DONT,						/* Something went wrong */
  	STATUS_ERROR_CONTINUE,			/* Non-fatal error occurred. */
	STATUS_WARNING,					/* Unexpected but controlled situation */
	STATUS_NOTFOUND,					/* Could not find it */
	STATUS_NOTHINGTODO,				/* Nothing was done: nothing to do */
	STATUS_DONEPROCESSING,			/* Finished doing some task */
	STATUS_ENDOFLINE,					/* End of line reached */
	STATUS_ENDOFPARAGRAPH,			/* End of paragraph reached */
	STATUS_EOF,							/* End of file reached */

	STATUS_SHUTDOWN,					/* The result was to shutdown */
	STATUS_EVENTNOTPROCESSED,		/* An event was not processed */

	STATUS_IO_PENDING,				/* Used for overlapped Win32 comm status */
	STATUS_IO_INCOMPLETE,			/* Used for overlapped Win32 comm status */

	STATUS_WRONGRESOURCETYPE,   	/* The resource was not the type expected */

	STATUS_PNGLIBERROR,       	 	/* There was an error in the PNG library */
	STATUS_PNGINVALIDDATA,      	/* The png data is not valid */

   	STATUS_STRINGMAXSIZETOOLONG,	/* String's max size is too long (>25000) */
   	STATUS_STRINGSIZETOOLONG,		/* String's size is too long (>25000 or >max) */
   
	STATUS_UNKNOWN_MESSAGE,			/* The message type is unknown */
	STATUS_MESSAGE_SENT,				/* The message was sent */

	STATUS_POLLING,					/* The result was not achieved yet; starting
											to poll until result is achieved. */
	STATUS_SERVER_SUSPENDED,		/* The server (serial port) is suspended */

	STATUS_LAST
} RetStatus;




#ifdef __cplusplus	//added ats
extern "C" {
#endif
//int is_OK_HOBTOPNG_arc(int argumentCounter);
//void show_HOBTOPNG_header(void);
//void show_HOBTOPNG_help(void);
//RetStatus check_if_hob_NIB(struct IMAGE_RECORD * image_REC_ptr);
//RetStatus check_if_hob_BIN(struct IMAGE_RECORD * image_REC_ptr);
//void HobToPng_display_status(struct IMAGE_RECORD * image_REC_ptr);
RetStatus HobToPng(struct IMAGE_RECORD * image_REC_ptr);
//SMBLOB *HobToPngAsNewBlob(const SMBLOB *hobBlob, UINT2 resid, char **graphicName);
#ifdef __cplusplus
}
#endif
#endif
