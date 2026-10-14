/**
 * @file superScaleSynchronization.h
 * @brief Super Product Defaults sets up all of the initial settings for each product type, ByCount,
 * Random Weight, FixedWeight, Fluid Ounces. These can include default setting for fields such as Unit Price
 * EAN Format String, Description, or Tare.
 *
 * @copyright Copyright (c) 2012, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 * @addtogroup SupervisorProductDefaultsCsu
 * @{
*/


#ifndef SUPERSCALESYNCHRONIZATION_H
#define SUPERSCALESYNCHRONIZATION_H

#include "screenwidget.h"
#include <Messages.h>

class QIntValidator;

class QPushButton;              // prolly wont need this anymore

namespace Ui {
class SuperScaleSynchronization;
}

/**
* @class SuperScaleSynchronization
* @brief Sets up and initializes any default fields for a product.
*  
*/

class SuperScaleSynchronization  : public ScreenWidget
{
    Q_OBJECT

    typedef SuperScaleSynchronization _Myt;
    typedef ScreenWidget _Mybase;
    
public:
    /**
      * Constructor.
      */
    explicit SuperScaleSynchronization(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~SuperScaleSynchronization();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve screen help text.
     *
     * When help is activated on a screen this property returns the screen title and help text.
     *
     * @param[out] title  screen title
     * @param[out] description  screen help description
     * @returns  @c true if help exists for the screen, @c false otherwise
     */
    virtual bool helpText( QString& title, QString& description ) const;

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();



private slots:

    /**
    * SLOT method called when any checkbox is selected or deselected.
    *
    * @param[in] none
    * @returns  none
    */
    void clickedStandAlone();

    /**
    * SLOT method called when any checkbox is selected or deselected.
    *
    * @param[in] none
    * @returns  none
    */
    void clickedPrimary();

    /**
    * SLOT method called when any checkbox is selected or deselected.
    *
    * @param[in] none
    * @returns  none
    */
    void clickedSecondary();

    /**
    * SLOT method called when any checkbox is selected or deselected.
    *
    * @param[in] none
    * @returns  none
    */
    void checkboxChanged();

    /**
    * SLOT method called when the text is changed in a QLineEdit.
    *
    * @param[in] newText, newText is the new value in the text box
    * @returns  none
    */
    void textboxChanged();

    /**
     * Method to wap between host name and ip address for the primary scale address
     *
     * @param[in] none
     * @returns  none
     */
    void clickFlipIt();

    /**
     * Instantly save any changes
     *
     * @param[in] none
     * @returns  none
     */
    void clickSaveIt();


protected:

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

private:
    Ui::SuperScaleSynchronization *ui;

    DomAutomaticDataTypes *automaticDataTypes_;                     // fields being synchronized
    DomAutomaticUpdate *automaticUpdate_;                           // type of scale being update (primary, secondary, standalone)
    MadpInternalConfig *internalConfigData;

    bool hasChanged_;                                               // has data been modified
    bool ignoreSignals_;                                            // when certain events occur, don't trigger SIGNALS

    int localScaleType_;                                            // primary, seconary, or standalone
    int localHostType_;                                             // hostname=1, ip address=2

    QIntValidator *ipTarget1_;
    QIntValidator *ipTarget2_;
    QIntValidator *ipTarget3_;
    QIntValidator *ipTarget4_;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * Method to save any and all data when leaving a screen.
     *
     * @param[in] none
     * @returns  none
     */
    void saveDataOnExit();

    /**
     * Method to update the status line message.
     *
     * @param[in] statusLineMessage  statusLineMessage is the message number received to display.
     * @returns  none
     */
    void updateStatusLine(int errorNum, int errorEvent);

    /**
     * Method to hide or show the host name and it's label
     *
     * @param[in] displayField  to display it or not
     * @returns  none
     */
    void showHostName(bool displayField);

    /**
     * Method to hide or show the IP address and it's label
     *
     * @param[in] displayField  to display it or not
     * @returns  none
     */
    void showIpAddress(bool displayField);

    /**
     * Method to hide or show the "Use Hostname/IP Address" button
     *
     * @param[in] displayField  to display it or not
     * @returns  none
     */
    void showFlipIt(bool displayField);

    /**
     * Method to hide or show the Host name area information button
     *
     * @param[in] displayField  to display it or not
     * @returns  none
     */
    void showHostNameStuff(bool displayField);

    /**
     * Method to clear the IP address fields
     *
     * @param[in] none
     * @returns  none
     */
    void clearIpAddress();

    /**
     * Method to clear the IP address fields
     *
     * @param[in] none
     * @returns  none
     */
    void clearHostName();

    /**
     * Method to update the Scale Synchronization Type as Primary, Secondary, or Standalone.
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateScaleSynchronizationType(const SessionMessage &msg);

    /**
     * Method to update the Scale Synchronization screen with the chosen items to Synchronize.
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateScaleSynchronizationData(const SessionMessage &msg);

};

#endif // SUPERSCALESYNCHRONIZATION
