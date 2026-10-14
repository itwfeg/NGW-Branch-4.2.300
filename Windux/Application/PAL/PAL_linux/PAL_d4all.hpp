//
// File : PAL_d4all.hpp
//
// Codebase d4all.hpp wrapper header file
//  To resolve confliction type definition problem (BOOL & HANDLE)
//
//

#ifndef _PAL_D4ALL_HPP_
#define _PAL_D4ALL_HPP_


// ----------
// Step 1: Reaplce BOOL, HANDLE by DB_BOOL, DB_HANDLE in Codebase scope
//
#undef  DB_BOOL                         // Make sure DB_BOOL type will be defined in Codebase header file
#undef  DB_HANDLE


// ----------
// Step 2: Include Codebase header file
//
// The following macro is defined so that the file I/O redirections will not be visible to
// the compiler when compiling EDB code (they are only meant for Codebase code).
#define C4_NOFILEIOREDIR
#include "d4all.hpp"
#undef C4_NOFILEIOREDIR


// ----------
// Step 3: Restore PAL's definition
//
#undef  BOOL
#undef  HANDLE
// Make sure the types match original definition in linux/PAL_typedef.h
typedef bool BOOLEAN, BOOL, *LPBOOL;
typedef void *HANDLE;


#endif  // _PAL_D4ALL_HPP_
