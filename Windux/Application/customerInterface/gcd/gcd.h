/**
 * @file gcd.h
 * GCD frontend and user interface.
 *
 * This is the GCD frontend user interface.  It must be connected to a backend using the Qt
 * signal/slot paradigm.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup metrologyVideosCsu
 * @{
 */

#ifndef GCD_H
#define GCD_H

#include "gcdsignal.h"
#include "gcdlanguage.h"
#include "gcdlayout.h"
#include "overlay.h"

#include <QMainWindow>
#include <QMutex>

namespace Ui {
class gcd;
}

class playlistWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// GCD frontend class and user interface.
class gcd : public QMainWindow
{
    Q_OBJECT

    Q_PROPERTY( TestImageMode testImage READ testImage WRITE setTestImage )
    Q_PROPERTY( bool outOfService READ outOfService WRITE setOutOfService )
    Q_PROPERTY( bool idle READ idle RESET resetIdle )

    typedef gcd _Myt;
    typedef QMainWindow _Mybase;

Q_SIGNALS:

    /// Update status.
    /**
     * This signal is emitted when the status changes.
     *
     * @param[in] versionMajor  major version of the gcd application
     * @param[in] versionMinor  minor version of the gcd application
     * @param[in] language  language the gcd application is currently using
     * @param[in] weighMode  weigh mode the gcd application is currently in
     */
    void updateStatus(
        unsigned int versionMajor,
        unsigned int versionMinor,
        const QString& language,
        const QString& weighMode );

    /// Playlist error.
    /**
     * This signal is emitted when a playlist error occurs.
     *
     * @param[in] file  file in error
     * @param[in] errorDescription  description of error
     */
    void playlistError(
        const QString& file,
        const QString& errorDescription );

public:

    /// Test image mode.
    enum TestImageMode
    {
        NoTestImage = 0,                                ///< No pattern
        ColorBarPattern,                                ///< Color bar pattern
        WhiteScreen,                                    ///< White screen pattern
        GrayBackground,                                 ///< Plain gray pattern
        BlackScreen,                                    ///< Black screen pattern
        CustomImage,                                     ///< Custom image from file
        BlankScreen,
        BlankScreenOff
    };

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    gcd( QWidget *parent = 0 );

    /// Destructor.
    virtual ~gcd();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if scale is in test state.
    /**
     * @return  mode type
     */
    virtual TestImageMode testImage() const {return testImage_;}

    /// Set test image mode.
    /**
     * @param[in] mode  mode type
     * @param[in] filename  filename
     */
    virtual void setTestImage( TestImageMode mode, const QString &filename = QString() );

    /// Check if scale is out of service.
    /**
     * @return  @c true if out of service, @c false otherwise
     */
    virtual bool outOfService() const {return (( outOfService_ ) || ( outOfServiceForced_ ));}

    /// Set out of service mode.
    /**
     * @param[in] mode  @c true to set out of service, @c false to exit
     */
    virtual void setOutOfService( bool mode );

    /// Check if scale is in idle mode.
    /**
     * When the scale is idle it is playing videos or displaying images.
     *
     * @return  @c true if scale is idle, @c false otherwise
     */
    virtual bool idle() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Reset idle mode.
    virtual void resetIdle();

private Q_SLOTS:

    /// Slot for timer timeout.
    void onTimeout();

    /// Slot for test image timer timeout.
    void onTestImageTimeout();

    /// Slot for layout checking timer.
    void onCheckForLayoutChanged();

    /// Update user interface.
    /**
     * This slot is used to update graphical fields.
     *
     * @param[in] type  ui field type
     * @param[in] value  field value
     */
    void onUpdateUserInterface( gcdUpdateUserInterfaceType::type type, const QString& value );

    /// Control message.
    /**
     * This slot is used to perform specific functionality (i.e. parameter changes, special commands).
     *
     * @param[in] type  control message type
     * @param[in] value  control message data
     */
    void onControlMessage( gcdControlMessageType::type type, const QString& value );

private:

    enum state
    {
        stateOutOfService,
        stateTestImage,
        stateMetrology,
        stateFullscreenAdverts
    };

    enum advertismentMode
    {
        showPlaylist = 0,
        showWebsite
    };

    enum productAdvertismentMode
    {
        productShowNone = 0,
        productShowImage,
        productShowText,
        productShowPlaylist
    };

    enum {VERSION_MAJOR = 3};
    enum {VERSION_MINOR = 3};

    enum {TIMER_RESOLUTION = 1000};
    enum {IDLE_TIMEOUT = 30000};

    enum {LAYOUT_TIMER_RESOLUTION = 60000};

    Ui::gcd *ui_;

    gcdLayout layout_;
    QTimer *layoutTimer_;

    QFont layout_pricePerLbValueLabel_font_;
    QFont layout_fsPricePerLbValueLabel_font_;

    QTimer *timer_;

    unsigned int idleTimer_;
    unsigned int idleTimerTimeout_;
    QString idleFile_;

    bool idleTimerBlockedItem_;
    bool idleTimerBlockedWeight_;

    advertismentMode advertType_;
    QString currentPlaylist_;
    QString currentWebsite_;

    productAdvertismentMode productAdvertType_;
    QString productAdvertParam_;

    overlay *overlay_;
    QMutex overlayMutex_;

    bool outOfService_;
    bool outOfServiceForced_;
    TestImageMode testImage_;
    QTimer *testImageTimer_;

    gcdWeighModeType::type weighMode_;
    gcdCurrencyType::type currency_;
    gcdLanguage::type capacityDualRangeMode_;

    bool weigherAttached_;

    // not implemented
    gcd( const _Myt& );

    // not implemented
    _Myt& operator = ( const _Myt& );

    /// Set capacity string.
    void setCapacityDualRangeMode( gcdLanguage::type newMode, const QString& text );

    /// Set state.
    void setState( state s );

    /// Show frequent shopper UI.
    void showFrequentShopper();

    /// Hide frequent shopper UI.
    void hideFrequentShopper();

    /// Reset metrology text.
    void resetMetrologyPage();

    /// Update idle adverts.
    void updateIdleAdverts();

    /// Update background image.
    void updateBackground();

    /// Setup the user interface as per xml configuration.
    void setupUi( const QString& filename );

    /// Translate labels to proper language.
    void retranslateUi();

    /// Update string with proper unit separator.
    QString updateUnitSeparator( const QString& value );

    /// Destroy overlay.
    void destroyOverlay();

    /// Create product advert.
    void createProductAdvert( productAdvertismentMode type, const QString& value = QString() );

    /// Check if advert pane visible or not.
    bool isAdvertPaneVisible() const;

    /// Retrieve idle advert video feed.
    playlistWidget *idleVideoFeed() const;

    /// Retrieve item (transactional) advert video feed.
    playlistWidget *itemVideoFeed() const;

    /// Adjust text font point size to fit label.
    static void adjustLabelFontPointSize( QLabel *label, const QFont &f );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // GCD_H
