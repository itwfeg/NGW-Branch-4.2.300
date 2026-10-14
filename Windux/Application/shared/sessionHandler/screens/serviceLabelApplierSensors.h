/**
 * @file ServiceLabelApplierSensors.h
 * @brief Widget to view a label applier's sensors.
  *
  * @copyright Copyright (c) 2014 Hobart Corporation
  * @author Alex Zavala
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/
#ifndef SERVICELABELAPPLIERSENSORS_H
#define SERVICELABELAPPLIERSENSORS_H

#include "screenwidget.h"

class MadpInternalConfig;
class DomCurrentPLU;
class DomLabelApplierSensors;
class QLabel;


namespace Ui {
class serviceLabelApplierSensors;
}

/**
* @class ServiceLabelApplierSensors
* @brief  Changes a product's reporting class
*
*/

class serviceLabelApplierSensors  : public ScreenWidget
{
    Q_OBJECT

    typedef serviceLabelApplierSensors _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit serviceLabelApplierSensors(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceLabelApplierSensors();

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
    //void doSave();
    void setBlockedUnblocked(QLabel* label, int blocked);
    void setPressedReleased(QLabel* label, int pressed);
    void hideWeigher();
    void showWeigher();
    void updateLabelApplierConfig();

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
    Ui::serviceLabelApplierSensors *ui;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * Method to reload widget's fields with new values.
     *
     * @param[in] none
     * @returns  none
     */
    void reloadFields();

    /**
     * Method to update widget's fields with new values.
     *
     * @param[in] none
     * @returns  none
     */
    void updateLabelApplierSensors();

    bool hasChanged;

    QString scaleModel;
    QString isScaleMetric;

    DomLabelApplierSensors *labelApplierSensors;

    //Flag to notify when we have received our first sensor msg
    bool labelApplierSensorMsgReceived;


   ///////////////////

};

#endif // SERVICELABELAPPLIERSENSORS_H
