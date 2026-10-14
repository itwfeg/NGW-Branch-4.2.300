/**
  * @file printPrompt.h
  * @brief Widget to allow entry of printing selection prompt.
  *
  * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
  * @author Job E. Crew
  * @addtogroup printPromptCsu
  * @{
  */

#ifndef PRINTPROMPT_H
#define PRINTPROMPT_H

#include <QWidget>

#include "../screens/screenwidget.h"

class SessionMainWindow;
class QCombBox;

namespace Ui {
class printPrompt;
}

/// Hobart namespace.
namespace hobart {

/**
 * @class printPrompt
 * @brief Widget to allow the user to select a printer indicating what printer to print to for the selected field.
 */

class printPrompt : public QWidget
{
    Q_OBJECT
    
    typedef printPrompt _Myt;
    typedef QWidget _Mybase;

public:

    /**
     * Constructor.
     *
     * @param[in] parent  parent @c ModeWidget widget
     * @param[in] mainWindow  the @c SessionMainWindow object
     */
    explicit printPrompt(QWidget *parent, QStringList numsList, QStringList nameList, QStringList typesList, QStringList driverList, SessionMainWindow *mainWindow );

    /**
     * Destructor.
     */
    virtual ~printPrompt();

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

signals:
    /**
     * SIGNAL broadcast to anyone listening when the Done button is pressed.
     *
     * @param[in] msg  session message
     */
    void printPromptDoneClicked(int);

public slots:

    /**
    * SLOT method called when the a printer is selected to print on.
    *
    * @param[in] state, where 0=Primary, 1=External Printer 1, and 2=External Printer 2
    * @returns  none
    */
    void changedPrintPrompt(int state);

    /**
    * SLOT method called when the Done button is selected. Returns the user back to pluInfo
    *
    * @param[in] none
    * @returns  none
    */
    void updateDone();

private:
    Ui::printPrompt *ui;
    Session *session_;
    SessionMainWindow *mainWindow_;
    QString printerLocationNumbers_;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize(QStringList printerNumList, QStringList printerNameList, QStringList printerTypeList, QStringList printerDriverList);

    /**
     * Method emits the printPromptDoneClicked signal when the Done button is pressed.
     *
     * @param[in] none
     * @returns  none
     */
    void emitDoneClicked();

    // Enum sections

    enum PrinterModel
    {
        NoPrinter,                          // 0
        EppPrinter,                         // USED FOR All EPPS NOW - not used, no parts for this EPP - EOL
        EssPrinter,                         // not used
        EscPrinter,                         // not used
        ApcPrinter,                         // not used
        Epp2Printer,                        // 5 This is the EPP 2nd printer will use
        UsbPrinter,                         // 6
        NetworkPrinter                      // 7
    };
};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // printPrompt_H
