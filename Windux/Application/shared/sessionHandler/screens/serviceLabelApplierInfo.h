/**
 * @file serviceLabelApplierInfo.h
 * @brief Widget to view Label Applier Info.
  *
  * @copyright Copyright (c) 2014 Hobart Corporation
  * @author Todd
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/

#ifndef SERVICELABELAPPLIERINFO_H
#define SERVICELABELAPPLIERINFO_H

#include "screenwidget.h"

class MadpScaleConfig;
class QIntValidator;
class MadpInternalConfig;
class DomLabelApplierSystemInformation;


namespace Ui {
class serviceLabelApplierInfo;
}

/**
* @class serviceNewLabelApplierInfo
* @brief  Shows Label Applier Info
*
*/

class serviceLabelApplierInfo : public ScreenWidget
{
    Q_OBJECT

    typedef serviceLabelApplierInfo _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit serviceLabelApplierInfo(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceLabelApplierInfo();

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
    void showEraseButton();
    void eraseButClicked();
    void saveButClicked();
    void truncatingSerialNumber(QString);
    void dateOfInstallationChanged(QString);
    void dateOfManufactureChanged(QString);


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
    Ui::serviceLabelApplierInfo *ui;
    MadpInternalConfig *internalConfigData;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();
    void updateCLASystemInfo(DomLabelApplierSystemInformation msg);
    QString prepareDateString(bool forDB, QString incomingDate); // true is remove slashes false is add slashes 12/25/14 or 122514
    void hyphenateSerialNumber(QString newSerial);
    QString prepareSerialNumber(bool forDB, QString incomingSerialNumber); //true is remove hyphens false is add hyphens
    QString truncatingDate(QString);
    int makeValidatorTop(int digits);

    MadpScaleConfig *scaleConfigData;

    bool hasChanged;
    bool wasErased;

    QString scaleModel;
    QString isScaleMetric;

};

#endif // SERVICELABELAPPLIERINFO_H
