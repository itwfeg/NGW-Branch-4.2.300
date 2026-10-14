/**
 * @file superSecurityLabels.h
 * @brief ...describe...
 *
 * @copyright Copyright (c) 2015, ITW/Hobart, Food Equipment Group, U.S.A.
 * @Author: Clare Dickey
 * @{
*/

#ifndef SUPERSECURITYLABELS_H
#define SUPERSECURITYLABELS_H

#include "screenwidget.h"

class DomNormalScaleOperation;
class MadpCurrencySettings;

class QIntValidator;

namespace Ui {
class superSecurityLabels;
}

/**
* @class superSecurityLabels
* @brief Allows for the setup and configuration of all Total Types in the scale.
*/
class superSecurityLabels : public ScreenWidget
{
    Q_OBJECT
    
    typedef superSecurityLabels _Myt;
    typedef ScreenWidget _Mybase;

public:

    /**
      * Constructor.
      */
    explicit superSecurityLabels(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~superSecurityLabels();

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
    * SLOT method named minUnitPriceEditingFinished to perform any updates when the text data changes
    *
    * @param[in] none
    * @returns  none
    */
    void minUnitPriceEditingFinished();

    /**
    * SLOT method named minTotalPriceEditingFinished to perform any updates when the text data changes
    *
    * @param[in] none
    * @returns  none
    */
    void minTotalPriceEditingFinished();

	int makeValidatorTop( int digits );


protected:
    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

    /**
     * Handle screen changing.
     *
     * This method is called before the screen is changed to something new. If for some reason the
     * screen should not be unloaded and destroyed then a value of @c false should be returned.
     *
     * @returns  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

private:
    Ui::superSecurityLabels *ui;

    DomNormalScaleOperation *scaleOpData_;
    MadpCurrencySettings *currencyData;

    QIntValidator *unitPriceValidator_;
    QIntValidator *totalPriceValidator_;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * Method used to update the Security label pricing on the screen
     *
     * @param[in] msg  session message from the scale application
     * @returns[out] none
     */
    void displaySecurityLabelPricing(const SessionMessage& msg);

    /**
     * Method used to save the minimum unit price and total price.
     *
     * @param[in] none
     * @returns[out] none
     */
    void saveSecurityLabelPricing();

};

#endif // SUPERSECURITYLABELS_H
