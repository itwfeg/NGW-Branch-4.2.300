//
// File : PAL_errno.cpp
//
// Implements the linux error number related functions
//
/// @file PAL_errno.cpp
/// Implements the linux error number related functions.

#include"PAL_platformheader.h"
#include "../../3RDPartySoftware/WinDDK/inc/wxp/winerror.h"


typedef struct _ERRNO_MAPPING_STRU_
{
    int     linuxErrno;
    DWORD   win32Errno;
} ERRNO_MAP_TABLE;


static ERRNO_MAP_TABLE g_ErrnoMap_Linux2Win[] =
{
    // /usr/include/asm-generic/errno-base.h
    {EPERM              , ERROR_ACCESS_DENIED},                 /* Operation not permitted */
    {ENOENT             , ERROR_FILE_NOT_FOUND},                /* No such file or directory */
    {ESRCH              , ERROR_INVALID_THREAD_ID},             /* No such process */
    {EINTR              , PAL_UNKNOWN_ERR},                     /* Interrupted system call */
    {EIO                , ERROR_IO_DEVICE},                     /* I/O error */
    {ENXIO              , ERROR_DEV_NOT_EXIST},                 /* No such device or address */
//    {E2BIG              , ERROR_PARAMETER_QUOTA_EXCEEDED},      /* Argument list too long */  // Old winerrno.h doesn't define this value
    {E2BIG              , ERROR_BAD_ARGUMENTS},                 /* Argument list too long */
    {ENOEXEC            , ERROR_BAD_FORMAT},                    /* Exec format error */
    {EBADF              , ERROR_FILE_INVALID},                  /* Bad file number */
    {ECHILD             , ERROR_WAIT_NO_CHILDREN},              /* No child processes */
    {EAGAIN             , WSAEWOULDBLOCK},                         /* Try again (Resource temporarily unavailable) */
    {ENOMEM             , ERROR_OUTOFMEMORY},                   /* Out of memory */
    {EACCES             , ERROR_ACCESS_DENIED},                 /* Permission denied */
    {EFAULT             , ERROR_INVALID_ADDRESS},               /* Bad address */
    {ENOTBLK            , ERROR_DEV_NOT_EXIST},                 /* Block device required */
    {EBUSY              , ERROR_BUSY},                          /* Device or resource busy */
    {EEXIST             , ERROR_FILE_EXISTS},                   /* File exists */
    {EXDEV              , PAL_UNKNOWN_ERR},                     /* Cross-device link */
    {ENODEV             , ERROR_DEV_NOT_EXIST},                 /* No such device */
    {ENOTDIR            , ERROR_DIRECTORY},                     /* Not a directory */
    {EISDIR             , ERROR_DIRECTORY},                     /* Is a directory */
    {EINVAL             , ERROR_INVALID_PARAMETER},             /* Invalid argument */
    {ENFILE             , PAL_UNKNOWN_ERR},                     /* File table overflow */
    {EMFILE             , ERROR_TOO_MANY_OPEN_FILES},           /* Too many open files */
    {ENOTTY             , PAL_UNKNOWN_ERR},                     /* Not a typewriter */
    {ETXTBSY            , ERROR_BUSY},                          /* Text file busy */
    {EFBIG              , ERROR_FILE_INVALID},                  /* File too large */
    {ENOSPC             , ERROR_DISK_FULL},                     /* No space left on device */
    {ESPIPE             , ERROR_SEEK},                          /* Illegal seek */
    {EROFS              , ERROR_FILE_READ_ONLY},                /* Read-only file system */
    {EMLINK             , ERROR_TOO_MANY_LINKS},                /* Too many links */
    {EPIPE              , ERROR_BROKEN_PIPE},                   /* Broken pipe */
    {EDOM               , PAL_UNKNOWN_ERR},                     /* Math argument out of domain of func */
    {ERANGE             , PAL_UNKNOWN_ERR},                     /* Math result not representable */

    // /usr/include/asm-generic/errno.h
    {EDEADLK            , ERROR_POSSIBLE_DEADLOCK},             /* Resource deadlock would occur */
    {ENAMETOOLONG       , ERROR_FILENAME_EXCED_RANGE},          /* File name too long */
    {ENOLCK             , ERROR_ACCESS_DENIED},                 /* No record locks available */
    {ENOSYS             , ERROR_INVALID_FUNCTION},              /* Function not implemented */
    {ENOTEMPTY          , ERROR_DIR_NOT_EMPTY},                 /* Directory not empty */
    {ELOOP              , PAL_UNKNOWN_ERR},                     /* Too many symbolic links encountered */
//    {EWOULDBLOCK     EAGAIN  /* Operation would block */
    {ENOMSG             , ERROR_UNSUPPORTED_TYPE},              /* No message of desired type */
    {EIDRM              , PAL_UNKNOWN_ERR},                     /* Identifier removed */
    {ECHRNG             , PAL_UNKNOWN_ERR},                     /* Channel number out of range */
    {EL2NSYNC           , PAL_UNKNOWN_ERR},                     /* Level 2 not synchronized */
    {EL3HLT             , PAL_UNKNOWN_ERR},                     /* Level 3 halted */
    {EL3RST             , PAL_UNKNOWN_ERR},                     /* Level 3 reset */
    {ELNRNG             , PAL_UNKNOWN_ERR},                     /* Link number out of range */
    {EUNATCH            , PAL_UNKNOWN_ERR},                     /* Protocol driver not attached */
    {ENOCSI             , PAL_UNKNOWN_ERR},                     /* No CSI structure available */
    {EL2HLT             , PAL_UNKNOWN_ERR},                     /* Level 2 halted */
    {EBADE              , PAL_UNKNOWN_ERR},                     /* Invalid exchange */
    {EBADR              , ERROR_DS_SEC_DESC_INVALID},           /* Invalid request descriptor */
    {EXFULL             , PAL_UNKNOWN_ERR},                     /* Exchange full */
    {ENOANO             , PAL_UNKNOWN_ERR},                     /* No anode */
    {EBADRQC            , PAL_UNKNOWN_ERR},                     /* Invalid request code */
    {EBADSLT            , PAL_UNKNOWN_ERR},                     /* Invalid slot */

    {EBFONT             , ERROR_BAD_FORMAT},                    /* Bad font file format */
    {ENOSTR             , ERROR_DEV_NOT_EXIST},                 /* Device not a stream */
    {ENODATA            , ERROR_NO_DATA},                       /* No data available */
    {ETIME              , PAL_UNKNOWN_ERR},                     /* Timer expired */
    {ENOSR              , ERROR_NO_SYSTEM_RESOURCES},           /* Out of streams resources */
    {ENONET             , ERROR_NO_NETWORK},                    /* Machine is not on the network */
    {ENOPKG             , ERROR_HOOK_NOT_INSTALLED},            /* Package not installed */
    {EREMOTE            , PAL_UNKNOWN_ERR},                     /* Object is remote */
    {ENOLINK            , PAL_UNKNOWN_ERR},                     /* Link has been severed */
    {EADV               , PAL_UNKNOWN_ERR},                     /* Advertise error */
    {ESRMNT             , PAL_UNKNOWN_ERR},                     /* Srmount error */
    {ECOMM              , PAL_UNKNOWN_ERR},                     /* Communication error on send */
    {EPROTO             , RPC_S_PROTOCOL_ERROR},                /* Protocol error */
    {EMULTIHOP          , PAL_UNKNOWN_ERR},                     /* Multihop attempted */
    {EDOTDOT            , PAL_UNKNOWN_ERR},                     /* RFS specific error */
    {EBADMSG            , PAL_UNKNOWN_ERR},                     /* Not a data message */
    {EOVERFLOW          , PAL_UNKNOWN_ERR},                     /* Value too large for defined data type */
    {ENOTUNIQ           , DNS_ERROR_NOT_UNIQUE},                /* Name not unique on network */
    {EBADFD             , ERROR_BAD_DESCRIPTOR_FORMAT},         /* File descriptor in bad state */
    {EREMCHG            , ERROR_INVALID_ADDRESS},               /* Remote address changed */
    {ELIBACC            , ERROR_ACCESS_DENIED},                 /* Can not access a needed shared library */
    {ELIBBAD            , ERROR_INVALID_LIBRARY},               /* Accessing a corrupted shared library */
    {ELIBSCN            , ERROR_INVALID_LIBRARY},               /* .lib section in a.out corrupted */
    {ELIBMAX            , ERROR_LIBRARY_FULL},                  /* Attempting to link in too many shared libraries */
    {ELIBEXEC           , ERROR_INVALID_LIBRARY},               /* Cannot exec a shared library directly */
    {EILSEQ             , PAL_UNKNOWN_ERR},                     /* Illegal byte sequence */
    {ERESTART           , PAL_UNKNOWN_ERR},                     /* Interrupted system call should be restarted */

    {ESTRPIPE           , PAL_UNKNOWN_ERR},                     /* Streams pipe error */
    {EUSERS             , PAL_UNKNOWN_ERR},                     /* Too many users */
    {ENOTSOCK           , PAL_UNKNOWN_ERR},                     /* Socket operation on non-socket */
    {EDESTADDRREQ       , ERROR_INVALID_ADDRESS},               /* Destination address required */
    {EMSGSIZE           , WSAEMSGSIZE},                         /* Message too long */
    {EPROTOTYPE         , RPC_S_PROTOCOL_ERROR},                /* Protocol wrong type for socket */
    {ENOPROTOOPT        , ERROR_PROTOCOL_UNREACHABLE},          /* Protocol not available */
    {EPROTONOSUPPORT    , ERROR_PROTOCOL_UNREACHABLE},          /* Protocol not supported */
    {ESOCKTNOSUPPORT    , PAL_UNKNOWN_ERR},                     /* Socket type not supported */
    {EOPNOTSUPP         , PAL_UNKNOWN_ERR},                     /* Operation not supported on transport endpoint */
    {EPFNOSUPPORT       , ERROR_PROTOCOL_UNREACHABLE},          /* Protocol family not supported */
    {EAFNOSUPPORT       , PAL_UNKNOWN_ERR},                     /* Address family not supported by protocol */
    {EADDRINUSE         , ERROR_DHCP_ADDRESS_CONFLICT},         /* Address already in use */
    {EADDRNOTAVAIL      , DNS_ERROR_INVALID_IP_ADDRESS},        /* Cannot assign requested address */
    {ENETDOWN           , ERROR_NO_NETWORK},                    /* Network is down */
    {ENETUNREACH        , ERROR_NETWORK_UNREACHABLE},           /* Network is unreachable */
    {ENETRESET          , ERROR_CONNECTION_ABORTED},            /* Network dropped connection because of reset */
    {ECONNABORTED       , WSAECONNABORTED},                     /* Software caused connection abort */
    {ECONNRESET         , WSAECONNRESET},                       /* Connection reset by peer */
    {ENOBUFS            , WSAENOBUFS},                          /* No buffer space available */
    {EISCONN            , ERROR_ADDRESS_ALREADY_ASSOCIATED},    /* Transport endpoint is already connected */
    {ENOTCONN           , ERROR_ADDRESS_NOT_ASSOCIATED},        /* Transport endpoint is not connected */
    {ESHUTDOWN          , PAL_UNKNOWN_ERR},                     /* Cannot send after transport endpoint shutdown */
    {ETOOMANYREFS       , PAL_UNKNOWN_ERR},                     /* Too many references: cannot splice */
    {ETIMEDOUT          , ERROR_TIMEOUT},                       /* Connection timed out */
    {ECONNREFUSED       , ERROR_CONNECTION_REFUSED},            /* Connection refused */
    {EHOSTDOWN          , ERROR_HOST_DOWN},                     /* Host is down */
    {EHOSTUNREACH       , ERROR_HOST_UNREACHABLE},              /* No route to host */
    {EALREADY           , ERROR_SERVICE_ALREADY_RUNNING},       /* Operation already in progress */
    {EINPROGRESS        , WSAEWOULDBLOCK},                     /* Operation now in progress */
    {ESTALE             , PAL_UNKNOWN_ERR},                     /* Stale NFS file handle */
    {EUCLEAN            , PAL_UNKNOWN_ERR},                     /* Structure needs cleaning */
    {ENOTNAM            , PAL_UNKNOWN_ERR},                     /* Not a XENIX named type file */
    {ENAVAIL            , PAL_UNKNOWN_ERR},                     /* No XENIX semaphores available */
    {EISNAM             , PAL_UNKNOWN_ERR},                     /* Is a named type file */
    {EREMOTEIO          , RPC_S_CALL_FAILED_DNE},               /* Remote I/O error */
    {EDQUOT             , PAL_UNKNOWN_ERR},                     /* Quota exceeded */

    {ENOMEDIUM          , ERROR_INVALID_MEDIA},                 /* No medium found */
    {EMEDIUMTYPE        , ERROR_INVALID_MEDIA},                 /* Wrong medium type */
    {ECANCELED          , ERROR_CANCELLED},                     /* Operation Canceled */
    {ENOKEY             , PAL_UNKNOWN_ERR},                     /* Required key not available */
    {EKEYEXPIRED        , PAL_UNKNOWN_ERR},                     /* Key has expired */
    {EKEYREVOKED        , PAL_UNKNOWN_ERR},                     /* Key has been revoked */
    {EKEYREJECTED       , PAL_UNKNOWN_ERR},                     /* Key was rejected by service */

/* for robust mutexes */
    {EOWNERDEAD         , ERROR_SEM_OWNER_DIED},                /* Owner died */
    {ENOTRECOVERABLE    , PAL_UNKNOWN_ERR},                     /* State not recoverable */

    {ERFKILL            , PAL_UNKNOWN_ERR},                     /* Operation not possible due to RF-kill */
    {NULL               , NULL},
};

//-----------------------------------------------------------------------------

//
// Search Algorithm
//  Current: Sequential search: Time: O(N)
//  Binary search: Because [linuxErrno] is in ascending order (sorted).
//      The function could be rewritten to enahnce lookup speed to Log2(N).
//  It will hit the first 20 in most cases, the Sequential search method is enough.
//
DWORD PAL_mapErrnoLinux2Win32(int linux_errno)
{
    ERRNO_MAP_TABLE *pMapTable;

    pMapTable = g_ErrnoMap_Linux2Win;
    while (pMapTable->linuxErrno)
    {
        if (pMapTable->linuxErrno == linux_errno)
        {
            return (pMapTable->win32Errno);
        }
        pMapTable++;
    }

    return PAL_UNKNOWN_ERR;
}

void PAL_SoftwareException(LPCSTR err_file, unsigned long err_line, int err_errno)
{
    char    out_buf[8192];
    std::string out_strg;

    PAL_TIME_STRUCT pal_system_time;
    char    err_time[8192];

    char    *err_message;
    char    err_buf[8192];

    // Retrieve error message
    err_message = strerror_r(err_errno, err_buf, sizeof(err_buf));

    // format time string
    PAL_absclass::PAL_getLocalTime( &pal_system_time );
    sprintf(err_time, "%04d/%02d/%02d %02d:%02d:%02d"
            , pal_system_time.wYear
            , pal_system_time.wMonth
            , pal_system_time.wDay
            , pal_system_time.wHour
            , pal_system_time.wMinute
            , pal_system_time.wSecond
            );

    // format error message
    const   char out_template[] = "%s : %s, line %d. Code = %d, Message = ";
    sprintf(out_buf, out_template
            , err_time
            , err_file
            , err_line
            , err_errno
            );

    // Convert to std::string so that we could operate string easily
    out_strg = std::string(out_buf);
    out_strg += err_message;

    PAL_absclass::PAL_OutputDbgStr( out_strg );
}
