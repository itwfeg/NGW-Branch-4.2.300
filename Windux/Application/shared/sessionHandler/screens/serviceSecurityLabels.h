/**
 * @file serviceSecurityLabels.h
 * @brief ...describe...
 *
 * @copyright Copyright (c) 2015, ITW/Hobart, Food Equipment Group, U.S.A.
 * @Author: Clare Dickey
 * @{
*/

#ifndef SERVICESECURITYLABELS_H
#define SERVICESECURITYLABELS_H

#include "screenwidget.h"
#include "Messages.h"

class QIntValidator;
class DomPrinterConfigRead;

namespace Ui {
class serviceSecurityLabels;
}

/**
* @class serviceSecurityLabels
* @brief Allows for the setup of the Security label line offset. This is part of the Printer Configuration.
*/
class serviceSecurityLabels : public ScreenWidget
{
    Q_OBJECT
    
    typedef serviceSecurityLabels _Myt;
    typedef ScreenWidget _Mybase;

public:

    /**
      * Constructor.
      */
    explicit serviceSecurityLabels(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceSecurityLabels();

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
    * SLOT method named lineOffsetEditingFinished to perform any updates when the text data changes
    *
    * @param[in] none
    * @returns  none
    */
    void lineOffsetEditingFinished();

    /**
    * SLOT method named onTestButtonClicked to perform any updates when the text data changes
    *
    * @param[in] none
    * @returns  none
    */
    void onTestButtonClicked();


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

    Ui::serviceSecurityLabels *ui;

    DomPrinterConfigRead printerConfig_;
    int activePrinter_;

    QIntValidator *lineOffsetValidator_;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * Method used to update the SLA line offset on the screen
     *
     * @param[in] msg  session message from the scale application
     * @returns[out] none
     */
    void displaySLALineOffset();

    /**
     * Method used to save the Printer config - containing the SLA line offset.
     *
     * @param[in] none
     * @returns[out] none
     */
    void saveSecurityLabelOffset();

};

#endif // SERVICESECURITYLABELS_H
