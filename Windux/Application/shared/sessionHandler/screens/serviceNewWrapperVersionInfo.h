/**
 * @file serviceNewWrapperVersionInfo.h
 * @brief Widget to view Wrapper Verion Info.
  *
  * @copyright Copyright (c) 2014 Hobart Corporation
  * @author Todd
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/

#ifndef SERVICENEWWRAPPERVERSIONINFO_H
#define SERVICENEWWRAPPERVERSIONINFO_H

#include "screenwidget.h"

class MadpScaleConfig;
class QIntValidator;
class MadpInternalConfig;

class QLabel;

namespace Ui {
class serviceNewWrapperVersionInfo;
}

/**
* @class serviceNewWrapperVersionInfo
* @brief  View wrapper version info
*
*/

class serviceNewWrapperVersionInfo : public ScreenWidget
{
    Q_OBJECT

    typedef serviceNewWrapperVersionInfo _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit serviceNewWrapperVersionInfo(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceNewWrapperVersionInfo();

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
    Ui::serviceNewWrapperVersionInfo *ui;
    MadpInternalConfig *internalConfigData;

    QList<QLabel*> node;
    QList<QLabel*> application;
    QList<QLabel*> boot;
    QList<QLabel*> checksum;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    int makeValidatorTop(int digits);

    MadpScaleConfig *scaleConfigData;

    bool hasChanged;

    QString scaleModel;
    QString isScaleMetric;

};

#endif // SERVICENEWWRAPPERVERSIONINFO_H
