/**
 * @file filesystemconfig.h
 * This file contains filesystem configuration.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup platformCsu
 * @{
 */

#ifndef FILESYSTEMCONFIG_H
#define FILESYSTEMCONFIG_H

#include "platform.h"

#include <QDir>
#include <QFile>
#include <QString>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Platform dependant filesystem configuration.
/**
 * This struct contains platform dependant information on filesystem layout and where required
 * files may be found.
 *
 * Example usage:
 * @code
 *   filesystemConfig<PLATFORM> myConfiguration;
 * @endcode
 *
 * @tparam _Platform  platform type
 */
template <platformType::type _Platform>
struct filesystemConfig;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Specialization for the @c platformType::Windows platform.
template <>
struct filesystemConfig<platformType::Windows>
{
    /// Retrieve config file.
    /**
     * @return  config file name
     */
    static QString configFileName() {
        // try old location before new
        static const QString oldConfigFileName( "c:\\program files\\hobart\\access scale\\config.dat" );
        if ( QFile::exists( oldConfigFileName ) )
            return oldConfigFileName;
        return "c:\\program files\\hobart\\config.dat";
    }

    /// Retrieve root folder.
    /**
     * @return  path to root folder
     */
    static QString rootFolder() {
        // try old location before new
        static const QString oldConfigFileName( "c:\\program files\\hobart\\access scale\\config.dat" );
        if ( QFile::exists( oldConfigFileName ) )
            return "c:\\program files\\hobart\\access scale";
        return "c:\\program files\\hobart";
    }

    /// Retrieve home folder.
    /**
     * @return  path to home folder
     */
    static QString homeFolder() {
        return QDir::homePath() + "\\hobart";
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Specialization for the @c platformType::Linux platform.
template <>
struct filesystemConfig<platformType::Linux>
{
    /// Retrieve config file.
    /**
     * @return  config file name
     */
    static QString configFileName() {
        return "/etc/hobart/config_linux.dat";
    }

    /// Retrieve root folder.
    /**
     * @return  path to root folder
     */
    static QString rootFolder() {
        return "/usr/local/hobart";
    }

    /// Retrieve home folder.
    /**
     * @return  path to home folder
     */
    static QString homeFolder() {
        QDir folder;
        if ( folder.cd( QDir::homePath() + "/.config/hobart" ) )
            return folder.path();
        return "/home/hobart";
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Specialization for the @c platformType::EmbeddedLinux platform.
template <>
struct filesystemConfig<platformType::EmbeddedLinux> : public filesystemConfig<platformType::Linux>
{
    // embedded linux is still linux
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // FILESYSTEMCONFIG_H
