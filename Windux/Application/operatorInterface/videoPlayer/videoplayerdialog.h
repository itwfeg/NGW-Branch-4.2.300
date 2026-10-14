/**
 * @file videoplayerdialog.h
 * Dialog box for playing videos.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup metrologyVideosCsu
 * @{
 */

#ifndef VIDEOPLAYERDIALOG_H
#define VIDEOPLAYERDIALOG_H

#include <videoplayerwidget.h>

#include <QDialog>

namespace Ui {
class videoPlayerDialog;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dialog window for playing multimedia videos.
class videoPlayerDialog : public QDialog
{
    Q_OBJECT

    typedef videoPlayerDialog _Myt;
    typedef QDialog _Mybase;
    
public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] pos  widget position
     * @param[in] parent  parent widget
     */
    videoPlayerDialog( const QPoint& pos, QWidget *parent = 0 );

    /// Destructor.
    virtual ~videoPlayerDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if player active.
    /**
     * @return  @c true if player is active, @c false otherwise
     */
    virtual bool active() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Disable the user interface.
    virtual void disableUserInterface();

    /// Play a video.
    /**
     * @param[in] filename filename of video to play
     * @return  @c true if video starts playing, @c false otherwise
     */
    virtual bool play( const QString& filename );

private Q_SLOTS:

    /// Slot for when media finishes.
    void onFinished();

    /// Slot for when media detects video component.
    void onHasVideoChanged( bool hasVideo );

private:

    Ui::videoPlayerDialog *ui_;

    videoPlayerWidget *videoWidget_;

    // ========================================================================

    // not implemented
    videoPlayerDialog( const _Myt& );

    // not implemented
    _Myt& operator = ( const _Myt& );

    /// Initialize.
    void initialize();

    /// Determine what device to use.
    videoPlayerDevice::type device() const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // VIDEOPLAYERDIALOG_H
