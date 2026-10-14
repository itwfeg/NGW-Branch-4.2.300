/**
 * @file serviceNewWrapperTestCamera.h
 * @brief Widget to view Wrapper Test Camera.
  *
  * @copyright Copyright (c) 2014 Hobart Corporation
  * @author Todd
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/

#ifndef SERVICENEWWRAPPERTESTCAMERA_H
#define SERVICENEWWRAPPERTESTCAMERA_H

#include "screenwidget.h"
#include "deviceTests.h"

class MadpScaleConfig;
class QIntValidator;
class MadpInternalConfig;


namespace Ui {
class serviceNewWrapperTestCamera;
}

/**
* @class serviceNewWrapperTestCamera
* @brief  Shows wrapper test Camera
*
*/

class serviceNewWrapperTestCamera : public ScreenWidget
{
    Q_OBJECT

    typedef serviceNewWrapperTestCamera _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit serviceNewWrapperTestCamera(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceNewWrapperTestCamera();

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
    void butCalibrateLeftSideClicked();
    void butCalibrateRightSideClicked();

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

    virtual bool eventFilter( QObject *obj, QEvent *event );
private:
    Ui::serviceNewWrapperTestCamera *ui;
    MadpInternalConfig *internalConfigData;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    int makeValidatorTop(int digits);
    void updateCameraCalibration(const SessionMessage &msg);

    MadpScaleConfig *scaleConfigData;

    bool hasChanged;
    bool isCalibrating;

    QString scaleModel;
    QString isScaleMetric;

    deviceTests *deviceTest;
    QString leftTestResult;
    QString rightTestResult;

};

#endif // SERVICENEWWRAPPERTESTCAMERA_H
