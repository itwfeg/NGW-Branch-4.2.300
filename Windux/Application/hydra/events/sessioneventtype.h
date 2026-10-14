/**
 * @file sessioneventtype.h
 * @brief Session/SessionManager Event types.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef SESSIONEVENTTYPE_H
#define SESSIONEVENTTYPE_H

#include "event.h"
#include "events_global.h"

#include <QVector>

extern EVENTS_EXPORT const QVector<uint> SIMPLE_EVENT_TYPE;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Session Manager event types.
/**
 * @note Please update "eventfactory.cpp" with added events!
 */
enum class SessionManagerEventType
{
    CreateSession                       = 100,
    CreateNamedSession,
    DestroySession,
    ResumeNamedSession,

    RegisterServiceAlias                = 110,
    UnregisterServiceAlias,

    SessionCreated                      = 200,
    SessionResumed,
};

extern EVENTS_EXPORT const QVector<uint> SESSION_MAN_CREATE_SESSION;
#define SESSION_MAN_CREATE_SESSION_TYPE 0                               // uint

using SessionManCreateSession = EventT<DeviceType::SessionManager, (uint) SessionManagerEventType::CreateSession, SESSION_MAN_CREATE_SESSION>;

extern EVENTS_EXPORT const QVector<uint> SESSION_MAN_CREATE_NAMED_SESSION;
#define SESSION_MAN_CREATE_NAMED_SESSION_TYPE 0                         // uint
#define SESSION_MAN_CREATE_NAMED_SESSION_NAME 1                         // string

using SessionManCreateNamedSession = EventT<DeviceType::SessionManager, (uint) SessionManagerEventType::CreateNamedSession, SESSION_MAN_CREATE_NAMED_SESSION>;

extern EVENTS_EXPORT const QVector<uint> SESSION_MAN_DESTROY_SESSION;
#define SESSION_MAN_DESTROY_SESSION_OBJ_PATH 0                          // string

using SessionManDestroySession = EventT<DeviceType::SessionManager, (uint) SessionManagerEventType::DestroySession, SESSION_MAN_DESTROY_SESSION>;

extern EVENTS_EXPORT const QVector<uint> SESSION_MAN_RESUME_NAMED_SESSION;
#define SESSION_MAN_RESUME_NAMED_SESSION_NAME 0                         // string

using SessionManResumeNamedSession = EventT<DeviceType::SessionManager, (uint) SessionManagerEventType::ResumeNamedSession, SESSION_MAN_RESUME_NAMED_SESSION>;

extern EVENTS_EXPORT const QVector<uint> SESSION_MAN_REG_SERVICE_ALIAS;
#define SESSION_MAN_REG_SERVICE_ALIAS_NAME 0                            // string

using SessionManRegisterServiceAlias = EventT<DeviceType::SessionManager, (uint) SessionManagerEventType::RegisterServiceAlias, SESSION_MAN_REG_SERVICE_ALIAS>;

extern EVENTS_EXPORT const QVector<uint> SESSION_MAN_UNREG_SERVICE_ALIAS;
#define SESSION_MAN_UNREG_SERVICE_ALIAS_NAME 0                          // string

using SessionManUnregisterServiceAlias = EventT<DeviceType::SessionManager, (uint) SessionManagerEventType::UnregisterServiceAlias, SESSION_MAN_UNREG_SERVICE_ALIAS>;

extern EVENTS_EXPORT const QVector<uint> SESSION_MAN_SESSION_CREATED;
#define SESSION_MAN_SESSION_CREATED_OBJ_PATH 0                          // string

using SessionManSessionCreated = EventT<DeviceType::SessionManager, (uint) SessionManagerEventType::SessionCreated, SESSION_MAN_SESSION_CREATED>;

extern EVENTS_EXPORT const QVector<uint> SESSION_MAN_SESSION_RESUMED;
#define SESSION_MAN_SESSION_RESUMED_OBJ_PATH 0                          // string

using SessionManSessionResumed = EventT<DeviceType::SessionManager, (uint) SessionManagerEventType::SessionResumed, SESSION_MAN_SESSION_RESUMED>;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Session event types.
enum class SessionEventType
{
    Register                            = 100,
    Unregister,

    RegisterEvent                       = 110,
    UnregisterEvent,
};

using SessionRegister = EventT<DeviceType::Session, (uint) SessionEventType::Register, SIMPLE_EVENT_TYPE>;

using SessionUnregister = EventT<DeviceType::Session, (uint) SessionEventType::Unregister, SIMPLE_EVENT_TYPE>;

extern EVENTS_EXPORT const QVector<uint> SESSION_REGISTER_EVENT;
#define SESSION_REGISTER_EVENT_DEVICE_TYPE 0                            // uint
#define SESSION_REGISTER_EVENT_EVENT_TYPE 1                             // uint

using SessionRegisterEvent = EventT<DeviceType::Session, (uint) SessionEventType::RegisterEvent, SESSION_REGISTER_EVENT>;

extern EVENTS_EXPORT const QVector<uint> SESSION_UNREGISTER_EVENT;
#define SESSION_UNREGISTER_EVENT_DEVICE_TYPE 0                          // uint
#define SESSION_UNREGISTER_EVENT_EVENT_TYPE 1                           // uint

using SessionUnregisterEvent = EventT<DeviceType::Session, (uint) SessionEventType::UnregisterEvent, SESSION_UNREGISTER_EVENT>;

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SESSIONEVENTTYPE_H
