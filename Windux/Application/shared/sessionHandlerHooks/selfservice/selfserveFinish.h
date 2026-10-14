/**
  * @file selfserveFinish.h
  * @brief Widget for Self-Service finish screen - Label taken.
  *
  * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup selfserveFinishCsu
  * @{
  */

#ifndef SELFSERVEFINISH_H
#define SELFSERVEFINISH_H

#include <QWidget>

namespace Ui {
class selfserveFinish;
}

class ScaleConfiguration;

class selfServiceHook;
class videoPlayer;

class QLabel;
class QTimer;
class QMovie;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class selfserveFinish
 * @brief Widget for Self-Service finish screen - Label taken.
 */
class selfserveFinish : public QWidget
{
    Q_OBJECT

    typedef selfserveFinish _Myt;
    typedef QWidget _Mybase;

public:

    // ************************************************************************
    // CTOR / DTOR
    // ************************************************************************

    /**
     * Constructor.
     *
     * @param[in] h  pointer to hook
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit selfserveFinish( selfServiceHook *h, QWidget *parent = NULL );

    /**
     * Destructor.
     */
    virtual ~selfserveFinish();

    // ************************************************************************
    // Methods
    // ************************************************************************

    /**
     * Refresh widget.
     */
    virtual void refresh();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

protected:

    // ************************************************************************
    // Events
    // ************************************************************************

    /**
     * Hide event.
     *
     * @param[in] e  event
     */
    virtual void hideEvent( QHideEvent *e );

    /**
     * Resize event.
     *
     * @param[in] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

    /**
     * Show event.
     *
     * @param[in] e  event
     */
    virtual void showEvent( QShowEvent *e );

private Q_SLOTS:

    /**
     * Begin video playback.
     */
    // void beginVideo();

    /**
     * Slot to reload images.
     */
    void onReloadImages();

    /**
     * Playback error.
     */
    void playbackError( const QString& desc );

    /**
     * Queue up begin video playback.
     */
    //void queueBeginVideo();

    /**
     * Queue up restart video playback.
     */
    //void queueRestartVideo();

    /**
     * Restart video playback.
     */
    //void restartVideo();

    /**
      * Start over if the user didn't click DONE or press PRINT. Hopefully they have scanned it by now if scanning
      */
    void startOver();

    /**
     * Restart customer timeout for the Option 3 PRINT or SCAN screen.
     */
    void resetStartOverTimer();

private:

    Ui::selfserveFinish *ui;

    selfServiceHook *hook_;
    //videoPlayer *player_;

    QMovie *gifPlayer_;
    QLabel *gifViewer_;

    ScaleConfiguration *scaleConfig_;

    QLabel *storeLogo_;

    bool active_;

    bool waitingForStart_;
    //bool needToDestroyPlayer_;

    int playbacks_;
    QString mediaFile_;
    /*
    QTimer *beginVideoTimer_;
    QTimer *restartVideoTimer_;
    */
    QTimer *beginPrintOrScanTimer_;

    /**
     * Initialize.
     */
    void initialize();

    /**
     * Create video player.
     */
    //void createVideoPlayer(); 

    /**
     * Find the screen dimensions.
     */
    int checkScreenSize();

    /**
     * Destroy video player.
     */
    //void destroyVideoPlayer(); 

    /**
     * Render label image to display.
     */
    void renderLabelImage();

    /**
     * Render pull graphic to display.
     */
    void renderPull();

};


///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // SELFSERVEFINISH_H
