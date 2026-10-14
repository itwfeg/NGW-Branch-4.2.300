/**
 * @file devicetype.h
 * @brief Device types.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DEVICETYPE_H
#define DEVICETYPE_H

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Device Types.
/**
 * @note Please update "eventfactory.cpp" with added devices!
 */
enum class DeviceType
{
    Invalid                             = 0,

    // session
    SessionManager                      = 1000,                         // /Hobart/SessionManager
    Session,                                                            // /Hobart/Session/X

    // comm/connection
    CommManager                         = 1010,                         // /Hobart/CommManager
    Connection,                                                         // /Hobart/Connection/X

    // scale resources
    // generally each of these devices corresponds to a file type that resides on the filesystem
    GraphicResource                     = 4000,                         // /Hobart/ResourceManager
    FontResource,

    // scale database
    // generally each of these devices corresponds to one table
    DepartmentDatabase                  = 5000,                         // /Hobart/Database/Scale

    ProductDatabase,
    OperatorDatabase,
    OperatorNotesDatabase,
    CategoryDatabase,

    ProductTextDatabase,
    NutritionDatabase,

};

#define MESSAGE_CENTER_DBUS_SERVICE                 "com.hobartcorp.MessageCenter"
#define MESSAGE_CENTER_DBUS_OBJECT_PATH             "/Hobart/MessageCenter"

#define SESSION_MANAGER_DBUS_SERVICE                "com.hobartcorp.SessionManager"
#define SESSION_MANAGER_DBUS_OBJECT_PATH            "/Hobart/SessionManager"
#define SESSION_DBUS_OBJECT_PATH                    "/Hobart/Session/%1"

#define COMM_MANAGER_DBUS_SERVICE                   "com.hobartcorp.CommManager"
#define COMM_MANAGER_DBUS_OBJECT_PATH               "/Hobart/CommManager"
#define CONNECTION_DBUS_OBJECT_PATH                 "/Hobart/Connection/%1"

#define RESOURCE_MANAGER_DBUS_SERVICE               "com.hobartcorp.ResourceManager"
#define RESOURCE_MANAGER_DBUS_OBJECT_PATH           "/Hobart/ResourceManager"

#define SCALE_DATABASE_DBUS_SERVICE                 "com.hobartcorp.Database.Scale"
#define SCALE_DATABASE_DBUS_OBJECT_PATH             "/Hobart/Database/Scale"

#define DIAGNOSTICS_DATABASE_DBUS_SERVICE           "com.hobartcorp.Database.Diagnostics"
#define DIAGNOSTICS_DATABASE_DBUS_OBJECT_PATH       "/Hobart/Database/Diagnostics"

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DEVICETYPE_H
