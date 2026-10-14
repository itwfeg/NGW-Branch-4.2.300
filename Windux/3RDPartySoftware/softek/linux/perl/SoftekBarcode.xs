#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ppport.h"

#include <SoftekBarcode.h>

#include "const-c.inc"

MODULE = SoftekBarcode		PACKAGE = SoftekBarcode		

INCLUDE: const-xs.inc

void
set_uint16 (handle, property, newVal)
		long		handle
		int		property
		int		newVal
	CODE:
		uint16 val ;
		val = (uint16) newVal ;
		STSetParameter((void *) handle, property, &val) ;  

int
get_uint16 (handle, property)
		long		handle
		int		property
	CODE:
		uint16 val ;
		STGetParameter((void *) handle, property, &val) ;  
		RETVAL = val ;
	OUTPUT:
		RETVAL

void
set_uint32 (handle, property, newVal)
		long		handle
		int		property
		int		newVal
	CODE:
		uint32 val ;
		val = (uint32) newVal ;
		STSetParameter((void *) handle, property, &val) ;  

int
get_uint32 (handle, property)
		long		handle
		int		property
	CODE:
		uint32 val ;
		STGetParameter((void *) handle, property, &val) ;  
		RETVAL = val ;
	OUTPUT:
		RETVAL

void
set_string (handle, property, newVal)
		long		handle
		int		property
		char		*newVal
	CODE:
		STSetParameter((void *) handle, property, newVal) ;  

char *
get_string (handle, property)
		long		handle
		int		property
	CODE:
		char *val ;
		STGetParameter((void *) handle, property, &val) ;  
		RETVAL = val ;
	OUTPUT:
		RETVAL

long
init ()
	CODE:
		RETVAL = (long) STCreateBarCodeSession() ;
	OUTPUT:
		RETVAL

void
destroy (handle)
		long		handle
	CODE:
		STFreeBarCodeSession((void *) handle) ;

int
scanbarcode (handle, filePath, h_bar_codes, h_bar_codes_type)
		long		handle
		char		*filePath
		long		h_bar_codes
		long		h_bar_codes_type
	CODE:
		char **bar_codes ;
		char **bar_codes_type ;
		char *type = "tif" ;
		int l ;

		l = strlen(filePath) ;

		if ((l > 4) && !strcmp(&filePath[l - 4], ".jpg"))	
			type = "jpg" ;

		if ((l > 4) && !strcmp(&filePath[l - 4], ".JPG"))	
			type = "jpg" ;

		if ((l > 5) && !strcmp(&filePath[l - 5], ".jpeg"))	
			type = "jpg" ;

		if ((l > 5) && !strcmp(&filePath[l - 5], ".JPEG"))	
			type = "jpg" ;

		RETVAL = STReadBarCode((void *) handle, filePath, type, &bar_codes, &bar_codes_type) ;
		h_bar_codes = (long) bar_codes ;
		h_bar_codes_type = (long) bar_codes_type ;
	OUTPUT:
		RETVAL
		h_bar_codes
		h_bar_codes_type

char *
getstring_from_array_ptr (handle, index)
		long		handle
		int		index
	CODE:
		char **ptr ;
		ptr = (char **) handle ;
		RETVAL = ptr[index] ;
	OUTPUT:
		RETVAL

