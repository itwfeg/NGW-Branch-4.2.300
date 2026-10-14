/**
 * @file wrappersettings.h
 * Screen for adjusting wrapper settings.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef WRAPPERSETTINGS_H
#define WRAPPERSETTINGS_H

#include "pluoperatescreen.h"
#include <QMap>
#include <QPixmap>

namespace hobart {
template <class T> class backgroundWidget;
class labeledNumericLineEdit;
class numericLineEdit;
class labeledPluEdit;
}

class MadpInternalConfig;

class DomEvolWrapperEventCapture;
class DomEvolWrapperPkgDesc;
class DomEvolWrapperSettings;
class DomEvolWrapperStatus;
class DomEvolWrapperSystemInformation;
class MadpPeripheralList;

class QComboBox;
class QLabel;
class QPushButton;
class QSignalMapper;
class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Screen for adjusting wrapper settings.
class WrapperSettingsScreen : public PluOperateScreen
{
    Q_OBJECT

    typedef WrapperSettingsScreen _Myt;
    typedef PluOperateScreen _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit WrapperSettingsScreen( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /// Destructor.
    virtual ~WrapperSettingsScreen();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Disable the interface.
    virtual void disableInterface();

    /// Translate the user interface to current language.
    virtual void translate();

    /**
     * Check if beltTempSetting_ field has focus.
     *
     * @returns  @c true if beltTempSetting_ field has focus, @c false otherwise
     */
    virtual bool beltTempSettingHasFocus() const;		//	HA-201

protected:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if wrapper is supported.
    /**
     * @return  @c true if wrapper is supported, @c false otherwise
     */
    virtual bool isWrapperSupported() const;

    /// Screen that gets displayed when swiping (gesturing) the screen right.
    /**
     * @return  next screen in sequence, @c NULL if none
     */
    virtual ScreenWidget *swipeScreenRight() const;

    /// Retrieve temperature scale.
    /**
     * @return  temperature scale
     */
    virtual QString temperatureScale() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

    /// Request settings.
    virtual void requestSettings();

    // SOF-835
    virtual void showEvent( QShowEvent *e );

    /// When leaving clean up some stuff
    virtual void hideEvent( QHideEvent *e );


private Q_SLOTS:

    /// Slot for numberpad moved.
    void onNumberPadMoved( Qt::DockWidgetArea where );

    /// Slot for when screen changes.
    void onScreenChanged();

    /// Slot for timer timeout.
    void onTimerTimeout();

    /// Slot to handle widget updates.
    void onWidgetUpdated( QWidget *widget );

    // Slot for when operator starts cleaning mode
    void onStartCleanMode();

    /**
    * Slot for timer timeout.
    */
    void onCleanTimeout();

private:

    typedef hobart::backgroundWidget<QWidget> frameWidget;

    typedef QPair<QWidget*, QWidget*> labeledWidget;

    bool ignoreSignals_;
    bool registered_;

    QTimer *requestSettings_;
    unsigned int settingsMask_;
    unsigned int settings_;

    QMap<int, QPixmap> largeFrames_;
    QMap<int, QPixmap> smallFrames_;

    QSignalMapper *widgetUpdatedSignalMapper_;
    QMap<QWidget*, int> widgetCommands_;

    QList<labeledWidget> allWidgets_;

    MadpPeripheralList *peripheralList_;
    MadpPeripheralList *prevPeripheralList_;

    // package dimensions
    hobart::labeledNumericLineEdit *packLength_;
    hobart::labeledNumericLineEdit *packWidth_;
    hobart::labeledNumericLineEdit *packHeight_;
    hobart::labeledNumericLineEdit *lipHeight_;

    // film
    frameWidget *film_;

    QLabel *filmSelectionLabel_;
    QComboBox *filmSelection_;

    QLabel *upperFilmWidthLabel_;
    QComboBox *upperFilmWidth_;

    QLabel *lowerFilmWidthLabel_;
    QComboBox *lowerFilmWidth_;

    QPushButton *cutFilm_;
    QPushButton *loadUpperRoll_;
    QPushButton *loadLowerRoll_;
    QLabel *spacer_;

    // room
    frameWidget *room_;

    QLabel *roomTempLabel_;
    QLabel *roomTemp_;
    QPushButton *clean_;

    // belt
    frameWidget *belt_;

    QLabel *sealerBeltPowerLabel_;
    QComboBox *sealerBeltPower_;

    QLabel *beltTempSettingLabel_;
    hobart::numericLineEdit *beltTempSetting_;

    QLabel *currentBeltTempLabel_;
    QLabel *currentBeltTemp_;

    // advanced wrapper settings
    frameWidget *advancedSettings_;

    QLabel *trayNameLabel_;
    QLabel *trayName_;

    QLabel *filmUsedLabel_;
    QLabel *filmUsed_;

    QLabel *knifeTimeLabel_;
    QLabel *knifeTime_;

    // intake
    frameWidget *intake_;

    QLabel *intakeModeLabel_;
    QComboBox *intakeMode_;

    // tension
    frameWidget *tension_;

    QLabel *filmTensionLabel_;
    QComboBox *filmTension_;

    QLabel *tensionFrontToBackLabel_;
    QComboBox *tensionFrontToBack_;

    QLabel *tensionLeftToRightLabel_;
    QComboBox *tensionLeftToRight_;

    QTimer *cycleCleanTimer_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout( Qt::DockWidgetArea numPadLocation );

    /// Update configuration.
    void updateConfig( DomEvolWrapperSystemInformation *m );

    /// Update knife time.
    void updateKnifeTime( DomEvolWrapperEventCapture *m );

    /// Update package description.
    void updatePackageDesc( DomEvolWrapperPkgDesc *m );

    /// Update settings.
    void updateSettings( DomEvolWrapperSettings *m );

    /// Update status.
    void updateStatus( DomEvolWrapperStatus *m );

    /// Update net stretch.
    void updateNetStretch( bool wasModified );

    /// Set combo box index from data value.
    static void setComboBoxIndex( QComboBox *comboBox, const QString& data );

    /// Set Film Tension combo box and check the user data against the current dom value to set
    void setFilmTensionComboBoxIndex(QComboBox *widget, QString comboData);

    /// Check for visiblity and enabled.
    static bool isVisibleAndEnabled( QWidget *w );

    MadpInternalConfig *internalConfigData_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // WRAPPERSETTINGS_H
