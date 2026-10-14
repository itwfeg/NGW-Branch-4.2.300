//#include "vxWorks.h"
#include "UltimaObjects.h"
#include "Label.h"
/*   P R E P R O C E S S O R   M A C R O S   */
#define PointComp(p1,p2)    memcmp(p1,p2,sizeof(POINT))
#define RectComp(r1,r2)     memcmp(r1,r2,sizeof(RECT))

/**/

#define min_map_pitch(bounds) ((bounds->right - bounds->left - 1)/16 + 1)

#define OFFSET_TABLE_SIZE 8

typedef struct _label_stock_descriptor
{
    UltimaPoint size;
    INT16 orientation;
    INT16 skew;
} LABEL_STOCK_DESCRIPTOR;



#define DYNAMIC_SIZE            1


/*********************
*  Descriptor Flags  *
*********************/
#define TEXT_ENTITY     0x0001
#define BARCODE_ENTITY  0x0002
#define GRAPHIC_ENTITY  0x0004
#define LINE_ENTITY     0x0008

#define PRE_IMAGE       0x0010
#define POST_IMAGE      0x0020

#define FIXED           0x0100
#define FLOATING        0x0200
#define STRETCH         0x0400
#define VARIABLE        0x0800

#define RECT_BORDER     0x1000 

#define INVERTED		0x2000

/****************
*  Option Flags  *
****************/
#define EDIT_IMAGE      0x0001
#define SCRUNCH         0x0002

/***********************
*  Image Status Flags  *
***********************/
#define IMAGEING_ACTIVE     0x0001
#define IMAGING_BARCODE     0x0002
#define IMAGING_TEXT        0x0004
#define IMAGING_GRAPHIC     0x0008
#define ROTATING_90         0x0010
#define ROTATING_180        0x0020
#define ROTATING_270        0x0040
#define INVALID_RECT        0x0080
#define IMVALID_BARCODE     0x0100
#define INVALID_FONT        0x0200
#define INVALID_TEXT        0x0400
#define INVALID_ROTATE      0x0800
#define INVALID_FLOAT       0x1000
#define INVALID_VARIABLE    0x2000
#define IMAGE_LOCKED        0x4000
#define IMAGE_INVALID       0x8000

#define N_FORMS         26

/*     >>>   		W	A	R	N	I	N	G	!	!	!
	   >>>
	   >>>	 The items in the following enumeration table must remain
	   >>>	 in order.  When adding an item, append to the end of the
	   >>>	 table.  Also, the FORM_TYPE enumeration tables for Ultima
	   >>>	 and Quantum must remain in synch.  If an item is appended
	   >>>	 to FORM_TYPE for Quantum, it must be appended to Ultima's
	   >>>	 FORM_TYPE.										*/


 
typedef struct label_form_data LABEL_FORM_DATA;

typedef struct _form_rcrd
{
    UltimaPoint        	origin;
    UltimaPoint       	size;
    UINT16        		flag;
    FieldType    		form_type;
    HorizontalJustify   h_justify;
    UINT16        		v_justify;
    UINT8        		type;
    UINT16        		rotation;
    CharSpacing 		font_usage;
    UINT16        		font_id;
    UINT8        		*data;
} FORM_RECORD;
/*(
typedef struct _form_reference
{
    POINT        	origin;
    POINT        	size;
    UINT16        	pad0;
    FORM_TYPE    	form_type;
    H_JUSTIFY    	pad1;
    UINT16        	form_number;
    UINT8        	pad2;
    UINT16        	pad3;
    CHAR_SPACING 	pad4;
    UINT16        	pad5;
    LABEL_FORM_DATA	*label_form; 
} FORM_REFERENCE;

typedef struct _form_generic
{
    POINT        	origin;
    POINT        	size;
    UINT16        	pad0;
    FORM_TYPE    	form_type;
    H_JUSTIFY    	pad1;
    UINT16        	pad2;
    UINT8        	pad3; 
    UINT16        	pad4;
    CHAR_SPACING 	pad5;
    UINT16        	pad6;
    UINT8        	*pad7; 
} FORM_GENERIC;
*/
/*
typedef union 
{
	FORM_GENERIC	generic;
	FORM_RECORD		record;
	FORM_REFERENCE	reference;
}
FORM_RECORDS;
*/
typedef struct _ldb
{
    FORM_RECORD *form_record[N_FORMS];
} FORM_LIST;
/*
struct label_form_data
{
    LABEL_STOCK_DESCRIPTOR  lsd;
    FORM_LIST               form_list;
    FORM_RECORD             form_record[DYNAMIC_SIZE];
};

typedef struct rom_label_form_table
{
    UWORD   key;
    FORM_LIST *form_list;
    LABEL_STOCK_DESCRIPTOR *lsd;
} ROM_LABEL_FORM_TABLE;
*/

typedef struct
{
	INT8 	res_type[4];				/* "LABL"=a label, "FONT"=a font, "PICT"=a graphic */
	UINT16 	res_id;					/* Resource Id */
	UINT8	res_loaded;				/* Indicates if data follows the header structure */
	UINT8	pad0;						/* Alignment space */
	INT16	res_length;				/* Length of resource's binary form (in bytes) */
	INT8 	name[32];					/* Resource name */
} RESOURCE_HEADER;

//typedef  INT16 SIZE_T;

typedef struct
{
	RESOURCE_HEADER header;
	LABEL_STOCK_DESCRIPTOR lsd;
	UINT32 numberForms;					/* Not a member of the Hobart label structure */
	UINT32 *form_list;					/* Variable size, always NULL terminated */
	FORM_RECORD *form_record;
} HOBARTLABEL;



typedef struct _offset_record
{
    INT16 line;
    INT16 offset;
} OFFSET_RECORD;

typedef struct _imager_control_block
{
    FORM_LIST               	*forms_list;
    FORM_RECORD             	*form;
    LABEL_STOCK_DESCRIPTOR   	label_stock_descriptor;	
    INT16                   	field_index;
    UINT16                    	image_options;
    UINT16                   	image_flags;
    INT16                  		image_lines;
    BOOL                     	image_valid;
    UltimaPen                   pen;
    UltimaBitMap                image_bitmap;
    UltimaBitMap                rotated_bitmap;
    UltimaBitMap                *print_bitmap;
    OFFSET_RECORD            	offset_table[OFFSET_TABLE_SIZE];
    UINT16                   	image_bitmap_size;
    UINT16                   	rotated_bitmap_size;
} IMAGER_CONTROL_BLOCK;


//VOID InitPreImage (IMAGER_CONTROL_BLOCK* icb,FORM_LIST* forms_list,LABEL_STOCK_DESCRIPTOR* label_stock_descriptor,INTEGER label_pitch);
//VOID InitPostImage(IMAGER_CONTROL_BLOCK* icb);
//INTEGER ImageField(IMAGER_CONTROL_BLOCK* icb);
//INTEGER ImageLabel(IMAGER_CONTROL_BLOCK* icb);
//VOID FreeICB(IMAGER_CONTROL_BLOCK *icb);
//VOID ScaleLabel(FORM_LIST *forms_list,INTEGER x_factor, INTEGER y_factor);
