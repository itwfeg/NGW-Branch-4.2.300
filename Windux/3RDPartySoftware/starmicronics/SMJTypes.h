#ifndef SMJTYPES_H
#define SMJTYPES_H

#define DWORD unsigned int                  // unsigned 32bit integer
#define WORD  unsigned short                // unsigned 16bit integer
#define BYTE  unsigned char                 // unsigned 8bit integer
#define BOOL  unsigned char                 // boolean
#define VOID           void

typedef DWORD *PDWORD;
typedef WORD  *PWORD;
typedef BYTE  *PBYTE;
typedef BOOL  *PBOOL;
typedef VOID  *PVOID;

#define ON      1                           // Used regularly byt internal SMJ firmware source code
#define OFF     0
#define INVALID 0xffffffffL

#define SMJ_NULL 0

#endif // SMJTYPES_H
