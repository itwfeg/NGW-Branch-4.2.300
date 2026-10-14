/**
 * @file superChangeProductClass.h
 * @brief Widget to view Wrapper System Info.
  *
  * @copyright Copyright (c) 2014 Hobart Corporation
  * @author Todd
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/

#ifndef SERVICENEWWRAPPERSYSTEMINFO_H
#define SERVICENEWWRAPPERSYSTEMINFO_H

#include "screenwidget.h"

class MadpScaleConfig;
class QIntValidator;
class MadpInternalConfig;
class QTimer;


namespace Ui {
class serviceNewWrapperSystemInfo;
}

/**
* @class serviceNewWrapperSystemInfo
* @brief  Shows wrapper System Info
*
*/

class serviceNewWrapperSystemInfo : public ScreenWidget
{
    Q_OBJECT

    typedef serviceNewWrapperSystemInfo _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit serviceNewWrapperSystemInfo(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceNewWrapperSystemInfo();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:
// Sample Slot Methods for various widgets


    /**
    * SLOT method called when product or class change to signal a save on close of form
    *
    * @param[in] none
    * @returns  none
    */
    void doSave();

    void onSaveButClicked();
    void onEraseButClicked();
    void bunnyComesOut(); // easter egg for erase button
    void onPrintButClicked();

    void truncatingSerialNumber(QString incomingSerial);
    void manuDateChanged(QString);
    void installDateChanged(QString);
    void cycleCountChanged(QString newCycleCount);
    void serialNumberChanged(QString newSerialNumber);
    //void truncatingDate(QString newDate);

    void promptAutoConfig();
    void promptInstallDate();

    // SOF-1915
    /**
    * SLOT method called when the timer for exiting the screen elapses.
    *
    * Need to delay exiting the System Info screen until the wrapper has been
    * updated.
    *
    * @param[in] none
    * @returns  none
    */
    void chkForWaitMessage();

protected:
    /**
     * Handle screen changing.
     *
     * This method is called before the screen is changed to something new. If for some reason the
     * screen should not be unloaded and destroyed then a value of @c false should be returned.
     *
     * @returns  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );


    virtual bool eventFilter( QObject *obj, QEvent *event );

    // SOF-1915
    /**
     * Check for specific error message is handled by screen.
     *
     * Session will call this method upon receiving an error message. The default error handling
     * (i.e. popup an error dialog) shall be invoked unless @c true is returned from
     * this property.
     *
     * @returns  @c true to use custom error handler, @c false otherwise
     */
    virtual bool isErrorMessageHandler( const SessionMessage& errorMsg ) const;

private:
    Ui::serviceNewWrapperSystemInfo *ui;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    int makeValidatorTop(int digits);
    QString truncatingDate(QString newDate);

    MadpScaleConfig *scaleConfigData;
    QIntValidator *serialNumberValidator_;
    QIntValidator *cycleCountValidator_;
    QIntValidator *dateOfInstallationValidator_;
    QIntValidator *dateOfManufactureValidator_;

    bool hasChanged;
    bool acknowledgeSystemInfoMessage;

    QString scaleModel;
    QString scaleSerial;
    QString dateOfManf;
    QString isScaleMetric;

    // SOF-1915
    MadpInternalConfig *internalConfigData_;
    QTimer *startScreenExitTimer_;
    bool okToCheckForWaitMessage;
    bool isExitingSysInfo_;

    void sleepFor( int milliseconds ) const;

    // SOF-2295
    void populateModelNumber( int modelNumber );
    int initModelNumber;
};

#endif // SERVICENEWWRAPPERSYSTEMINFO_H
