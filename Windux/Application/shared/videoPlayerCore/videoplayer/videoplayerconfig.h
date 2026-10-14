/**
 * @file videoplayerconfig.h
 * Video player configuration per platform.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup metrologyVideosCsu
 * @{
 */

#ifndef VIDEOPLAYERCONFIG_H
#define VIDEOPLAYERCONFIG_H

#include <platform.h>

#include "external/externalvideoplayer.h"
#include "external/gstpdexternalvideoplayer.h"
#include "external/videoplayerappconfig.h"
#include "multimedia/multimediavideoplayer.h"
#include "phonon/phononvideoplayer.h"
#include "wmf/wmfvideoplayer.h"

#include <QtGlobal>

#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Platform dependant video player configuration.
/**
 * This struct contains platform dependant information on video player instantiation.
 *
 * Example usage:
 * @code
 *   // configuration object usage
 *   videoPlayerConfig<PLATFORM> myConfiguration;
 *   // create video player for platform directly
 *   videoPlayer *player = new videoPlayerConfig<PLATFORM>::videoPlayerType;
 * @endcode
 *
 * @tparam _Platform  platform type
 */
template <platformType::type _Platform>
struct videoPlayerConfig;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Specialization for the @c platformType::Windows platform.
template <>
struct videoPlayerConfig<platformType::Windows>
{
#if ( QT_VERSION >= 0x050000 )
    typedef multimediaVideoPlayer videoPlayerType;              ///< video player type
#else
    typedef mediaFoundationVideoPlayer videoPlayerType;         ///< video player type
#endif

    /// Create video player for device.
    /**
     * @param[in] deviceType  device type
     * @param[in] parent  pointer to parent widget
     * @return  pointer to video player
     */
    static videoPlayer *createVideoPlayer( videoPlayerDevice::type deviceType, QWidget *parent = NULL )
    {

// this memory leak fixed by QTBUG-32481 in 5.4.0
#if (( 0x050000 <= QT_VERSION ) && ( QT_VERSION < 0x050400 ))
        QString appName( QFileInfo( QCoreApplication::applicationFilePath() ).fileName() );

        // QTBUG-36671 [Begin]
        // There is a memory leak in QMultimedia for windows... lets work around the issue for now...

        // for gcd customer display use external application player
        if (( "gcd.exe" == appName ) && ( videoPlayerDevice::Customer == deviceType ))
            return new externalVideoPlayer( new videoPlayerAppConfig(), deviceType, parent );

        // QTBUG-36671 [End]
#endif

        return new videoPlayerType( deviceType, parent );
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Specialization for the @c platformType::Linux platform.
template <>
struct videoPlayerConfig<platformType::Linux>
{
#if ( QT_VERSION >= 0x050000 )
    typedef multimediaVideoPlayer videoPlayerType;              ///< video player type
#else
    typedef phononVideoPlayer videoPlayerType;                  ///< video player type
#endif

    /// Create video player for device.
    /**
     * @param[in] deviceType  device type
     * @param[in] parent  pointer to parent widget
     * @return  pointer to video player
     */
    static videoPlayer *createVideoPlayer( videoPlayerDevice::type deviceType, QWidget *parent = NULL )
    {
        return new videoPlayerType( deviceType, parent );
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Specialization for the @c platformType::EmbeddedLinux platform.
template <>
struct videoPlayerConfig<platformType::EmbeddedLinux>
{
    typedef gstpdExternalVideoPlayer videoPlayerType;             ///< video player type

    /// Create video player for device.
    /**
     * @param[in] deviceType  device type
     * @param[in] parent  pointer to parent widget
     * @return  pointer to video player
     */
    static videoPlayer *createVideoPlayer( videoPlayerDevice::type deviceType, QWidget *parent = NULL )
    {
        return new videoPlayerType( deviceType, parent );
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // VIDEOPLAYERCONFIG_H
