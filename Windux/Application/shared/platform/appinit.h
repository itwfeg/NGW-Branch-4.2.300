/**
 * @file appinit.h
 * Common methods for application initialization.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup platformCsu
 * @{
 */

#ifndef APPINIT_H
#define APPINIT_H

#include <QString>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Common initialization method for all Hobart Qt Applications.
/**
 * @param[in] appName  application name
 */
void appInitialize( const QString& appName );

/// Common finalization method for all Hobart Qt Applications.
void appFinalize();

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // APPINIT_H
