/**
 * @file platform.h
 * This file contains what platforms are supported and what platform we are being compiled for.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup platformCsu
 * @{
 */

#ifndef PLATFORM_H
#define PLATFORM_H

#include <qglobal.h>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Platforms enumeration.
/**
 * Enumeration of supported platforms.
 */
class platformType
{
public:

    /// Platform types.
    enum type
    {
        Windows         = 1,                            ///< windows platform
        Linux           = 2,                            ///< linux platform (i.e. Ubuntu)
        EmbeddedLinux   = 3                             ///< embedded linux platform (i.e. TimeSys)
    };

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Platform.
/**
 * The @c PLATFORM constant defines what platform the compiler is targeting.
 */
#if defined( Q_OS_WIN )
static const platformType::type PLATFORM = platformType::Windows;
#elif defined( Q_OS_LINUX )
#if defined( QT_ARCH_ARM ) || defined( Q_PROCESSOR_ARM )
static const platformType::type PLATFORM = platformType::EmbeddedLinux;
#else
static const platformType::type PLATFORM = platformType::Linux;
#endif
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // PLATFORM_H
