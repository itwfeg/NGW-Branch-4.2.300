/* ----------------------------------------------------------------------
	Module: hoblabel.h
	Date: 3/5/98
	Author: SJJC based on Kuang Poi's code

	Purpose:
		This is the module that has the declarations associated with the
	HOBARTLABEL class.

	Public Interface:

	Notes:
	-	The name convention followed here for the different structure and
	enumaration types is closely based on the scale software documentation
	that describes the corresponding structures, enumerations, etc.
----------------------------------------------------------------------- */


# ifndef HOBLABEL_H
# define HOBLABEL_H

//added ats 04092002

#include "UltimaObjects.h"
#include "TempHeader.h"
#include "string.h"
//# include "hobres.h"						/* HOBART resource types */
//# include "blob.h"						/* BLOB class */
//# include "label.h"						/* LABEL class */
//# include "config.h"						/* CONFIG class */
//# include "dgtypes.h"					/* types */

//added ats 04092002
typedef struct
{
	char 	res_type[4];				/* "LABL"=a label, "FONT"=a font, "PICT"=a graphic */
	UINT16 	res_id;					/* Resource Id */
	UINT8	res_loaded;				/* Indicates if data follows the header structure */
	UINT8	pad0;						/* Alignment space */
	INT16	res_length;				/* Length of resource's binary form (in bytes) */
	char 	name[32];					/* Resource name */
} RESOURCE_HEADER;


typedef struct
{
	RESOURCE_HEADER header;
	LABEL_STOCK_DESCRIPTOR lsd;
	SIZE_T numberForms;					/* Not a member of the Hobart label structure */
	UINT32 * form_list;					/* Variable size, always NULL terminated */
	FORM_RECORD * form_record;
} HOBARTLABEL;



#endif /* HOBLABEL_H */

