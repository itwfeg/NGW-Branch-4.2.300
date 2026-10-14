#ifndef _PAL_ERRCODE_H_
#define _PAL_ERRCODE_H_

#define PAL_OUT_OF_MEM			0xFFFFFFF1L
#define PAL_NULL_RETURNED		0xFFFFFFF2L
#define PAL_CALL_FAILED			0xFFFFFFF3L
#define PAL_INVALID_VAL			0xFFFFFFF4L
#define PAL_UNKNOWN_ERR			0xFFFFFFF5L
#define PAL_SEM_INC_TOO_LARGE           0xFFFFFFF6L

//
//pthread return codes
//
#define PAL_PTHREAD_SUCCESS             ( 0)
#define PAL_PTHREAD_FAILURE             (-1)

//
// socket related
//
#define PAL_SOCKET_SUCCESS              (0)
#define PAL_SOCKET_ERROR                (-1)
//
// FILE related
// 
#define PAL_INVALID_FILE_SIZE           0xFFFFFFFFL
#define PAL_INVALID_SET_FILE_POINTER    0xFFFFFFFFL

//
// TODO: I'm not sure how to map these, or if
//       we need to map them
#define PAL_CREATE_PROC_FAILED          0x80
#define PAL_SESSION_ID_FAILED           0x81

//
//Generic returns
//
#define PAL_SUCCESS			PAL_PTHREAD_SUCCESS
#define PAL_FAILURE			PAL_PTHREAD_FAILURE


//
// Windows Error code (PlatformSDK/Include/WinError.h)
//
#define PAL_ERR_GENERAL_FAILURE         (31L)
#define PAL_ERROR_BROKEN_PIPE           (109L)
#define PAL_ERROR_OPERATION_ABORTED     (995L)

#endif //_PAL_ERRCODE_H_

