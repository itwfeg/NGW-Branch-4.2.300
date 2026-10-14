#ifndef _HOBART_C_
#define HOBART_C_

#include<linux/ioctl.h>

//
//IOCTLs
//
#define HOBART_UNIQUE_CHAR                      'H'
#define HOBART_DEV_RESET                        _IO(HOBART_UNIQUE_CHAR, 0)
#define HOBART_DEV_RESETCPU                     _IO(HOBART_UNIQUE_CHAR, 1)


#endif //_HOBART_C_

