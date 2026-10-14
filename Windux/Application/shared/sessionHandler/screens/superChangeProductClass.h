/**
 * @file superChangeProductClass.h
 * @brief Widget to change a product's reporting class.
  *
  * @copyright Copyright (c) 2013 Hobart Corporation
  * @author Todd
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/

#ifndef SUPERCHANGEPRODUCTCLASS_H
#define SUPERCHANGEPRODUCTCLASS_H

#include "screenwidget.h"

class DomCurrentPLU;
class MadpScaleConfig;
class QIntValidator;
class MadpInternalConfig;


namespace Ui {
class superChangeProductClass;
}

/**
* @class superChangeProductClass
* @brief  Changes a product's reporting class
*  
*/

class superChangeProductClass  : public ScreenWidget
{
    Q_OBJECT

    typedef superChangeProductClass _Myt;
    typedef ScreenWidget _Mybase;
    
public:
    /**
      * Constructor.
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit superChangeProductClass(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~superChangeProductClass();

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

    /**
    * SLOT method called when a New product is chosen to be created.
    *
    * @param[in] none
    * @returns  none
    */
    void createNewProductOK_clicked();

    /**
    * SLOT method called when a New product is not chosen to be created.
    *
    * @param[in] none
    * @returns  none
    */
    void createNewProductCancel_clicked();

    /**
    * SLOT method called when a PLU is entered and needs to be searched for.
    *
    * @param[in] none
    * @returns  none
    */

    void doPluLookup();

    /**
    * SLOT method called when a Reporting Class is entered and needs to be validated.
    *
    * @param[in] none
    * @returns  none
    */

    void doReportingClassLookup();

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
    Ui::superChangeProductClass *ui;
    MadpInternalConfig *internalConfigData;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * Method to Popup Dialog to create new PLU
     *
     * @param[in] none
     * @returns  none
     */
    void popupCreateNewPLUDialog();

    /**
     * Method to relaod widget's fields with new values.
     *
     * @param[in] none
     * @returns  none
     */
    void reloadFields();
    void saveProduct();

    int makeValidatorTop(int digits);

    DomCurrentPLU *currentPLUData;
    MadpScaleConfig *scaleConfigData;

    QIntValidator *productNumberValidator_;
    QIntValidator *reportingClassValidator_;

    QString currentPLU;
    QString newPLUInfo;
    QString itemNumber;
    QString productNumber;
    QString description;
    QString reportingClass;
    QString previousProductNumber;

    bool hasChanged;
    bool pluPassedIn;
    bool cameFromCancel;
    bool popupActive;
    bool switchScreensOnNewPLU;
    bool requestedPlu;

    int reportingClassEmitted;

    QString scaleModel;
    QString isScaleMetric;

    QString newProductNumber;
    QString newClass;

   ///////////////////
    QString productNumberToSave;
//
};

#endif // superChangeProductClass
