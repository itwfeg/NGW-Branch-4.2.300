#ifndef EMAILMANAGEREVENTS_H
#define EMAILMANAGEREVENTS_H



#include "Event.h"
#include "DataTypeEvent.h"



// EmailManager Handled Events - EmailManagerDev
typedef enum EmailManagerEvent
{
	SendEmail = 20,
	SendErrorEmail,
	SendGcdPlaylistErrorEmail,
	SendGcdFtpErrorEmail
	
} EmailManagerEvent_t;


// for ErrEmailManagerDev
typedef enum EmailManagerDevError
{
	EmailManagerErrUnknownError = EmailManagerDev * DEVICE_ERROR_OFFSET,
	EmailManagerErrUnableToSendEmail
} EmailManagerDevError_t;




#endif // EMAILMANAGEREVENTS_H

