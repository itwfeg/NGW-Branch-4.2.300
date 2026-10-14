//
// File : PAL_d4all.hpp
//
// Codebase d4all.hpp wrapper header file
//  To resolve confliction type definition problem (BOOL & HANDLE)
//
//

#ifndef _PAL_D4ALL_HPP_
#define _PAL_D4ALL_HPP_

//
// The tricky file is in PAL_linux/PAL_d4all.hpp.
// For Win32, just simply use the original statements.
//

// The following macro is defined so that the file I/O redirections will not be visible to
// the compiler when compiling EDB code (they are only meant for Codebase code).
#define C4_NOFILEIOREDIR
#include "d4all.hpp"
#undef C4_NOFILEIOREDIR


#endif  // _PAL_D4ALL_HPP_
