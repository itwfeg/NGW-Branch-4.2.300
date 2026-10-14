/**
 * @file folderconfig.h
 * This file contains filesystem folder configuration.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup platformCsu
 * @{
 */

#ifndef FOLDERCONFIG_H
#define FOLDERCONFIG_H

#include "foldertype.h"
#include "platform.h"

#include <QMap>
#include <QObject>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Folder configuration class.
/**
 * This class will return folder pathing to where files should reside depending on platform. The
 * default values may be overriden by loading configuration from a file or the environment.
 */
class folderConfig : public QObject
{
    Q_OBJECT

    typedef folderConfig _Myt;
    typedef QObject _Mybase;

public:

    // ========================================================================
    // CTOR/DTOR
    // ========================================================================

    /// Constructor.
    folderConfig();

    /// Destructor.
    virtual ~folderConfig();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve config filename by platform.
    /**
     * @return  config file name
     */
    virtual QString configFileName() const;

    /// Retrieve folder location.
    /**
     * @param[in] type  file type
     * @param[in] system  @c true to get system path instead of user path
     * @return  path to folder of requested type
     */
    virtual QString folder( folderType::type type, bool system = false ) const;

    /// Retrieve platform type.
    /**
     * @return  platform type
     */
    virtual platformType::type platform() const {return platform_;}

    /// Retrieve root folder by platform.
    /**
     * @return  root folder
     */
    virtual QString rootFolder() const;

    /// Set folder location.
    /**
     * This method will override the default folder location.
     *
     * @param[in] value  folder name
     * @param[in] type  folder type
     * @param[in] system  @c true to get system path instead of user path
     */
    virtual void setFolder( const QString& value, folderType::type type, bool system = false );

    /// Set platform type.
    /**
     * @param[in] value  platform type
     */
    virtual void setPlatform( platformType::type value ) {platform_ = value;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Load configuration from environment.
    virtual void loadFromEnvironment();

    /// Load configuration from file.
    /**
     * @param[in] filename  file to load from
     */
    virtual void loadFromFile( const QString& filename );

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Check if filename is fully qualified.
    /**
     * When a filename is qualified you will not need to attach pathing to it since it is already
     * fuly pathed out.
     *
     * @param[in] filename  filename to check
     * @return  @c true if fully qualified, @c false otherwise
     */
    static bool isQualifiedFilename( const QString& filename );

private:

    enum {SYSTEM_PARAMETER_OFFSET = 10000};

    platformType::type platform_;

    typedef QMap<int, QString> parameterMap;
    parameterMap parameters_;

    // ========================================================================

    /// Check parameter.
    void checkParameter( const QString& name, const QString& value );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // FOLDERCONFIG_H
