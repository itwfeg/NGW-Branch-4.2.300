/**
  * @file eanEditor.h
  * @brief Widget to allow editing of the EAN string.
  *
  * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
  * @author Job E. Crew
  * @addtogroup eanEditorCsu
  * @{
  */

#ifndef EANEDITOR_H
#define EANEDITOR_H

#include <QWidget>

#include "../screens/screenwidget.h"
#include "digitEntry.h"

class SessionMainWindow;
class QPushButton;

namespace Ui {
class eanEditor;
}

/// Hobart namespace.
namespace hobart {

/**
 * @class eanEditor
 * @brief Widget to allow the ean string to be created and modified.
 */

class eanEditor : public QWidget
{
    Q_OBJECT
    
    typedef eanEditor _Myt;
    typedef QWidget _Mybase;

public:

    /**
     * Constructor.
     *
     * @param[in] parent  parent @c ModeWidget widget
     * @param[in] mainWindow  the @c SessionMainWindow object
     */
    explicit eanEditor(QWidget *parent, QString initialEan, int initialLength, int initialType, SessionMainWindow *mainWindow );

    /**
     * Destructor.
     */
    virtual ~eanEditor();

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
    void eanEditorDoneClicked(QString);

    /**
     * SIGNAL broadcast to anyone listening when the Cancel button is pressed.
     *
     * @param[in] msg  session message
     */
    void eanEditorCancelClicked( QString );

public slots:

    /**
     * SLOT method called when a DIGIT is pressed in the keypad.
     *
     * @param[in] none
     * @returns  none
     */
    void onDigitEntryNumberClicked(QString numberDigit);

    /**
     * SLOT method called when Clear is pressed in the keypad.
     *
     * @param[in] none
     * @returns  none
     */
    void onDigitEntryClearClicked();

    /**
     * SLOT method called when Done is pressed in the keypad.
     *
     * @param[in] none
     * @returns  none
     */
    void onDigitEntryDoneClicked();

    /**
    * SLOT method called the EAN format field is selected
    *
    * @param[in] none
    * @returns  none
    */
    void launchEanEditor();

    /**
    * SLOT method called when the Done button is selected. Returns the user back to the
    * product defaults page and saves the current EAN Format String
    *
    * @param[in] none
    * @returns  none
    */
    void updateDoneEanFormating();

    /**
    * SLOT method called when the Cancel button is selected. Returns the user back to the
    * product defaults page and does not save the current EAN Format String
    *
    * @param[in] none
    * @returns  none
    */
    void updateCancelEanFormating();

    /**
    * SLOT method called when the X=Prefix is selected. Adds an X preix digit that is store defined	 up to 2 digits
    *
    * @param[in] none
    * @returns  none
    */
    void updateAdd_X_Digit();

    /**
    * SLOT method called when the Digits(0-9) is selected. Allows up to 12 digits entered into the EAN code
    *
    * @param[in] none
    * @returns  none
    */
    void updateAdd_keypad_Digits();

    /**
    * SLOT method called when the W=Weight is selected. Adds an W digit up to a maximum of 5
    *
    * @param[in] none
    * @returns  none
    */
    void updateAdd_W_Digit();

    /**
    * SLOT method called when the E=EAN Code is selected. Adds an E digit up to a maximum of 9
    *
    * @param[in] none
    * @returns  none
    */
    void updateAdd_E_Digit();

    /**
    * SLOT method called when the w=weight check digit is selected. Adds an w digit up to a maximum of 1
    *
    * @param[in] none
    * @returns  none
    */
    void updateAdd_wc_Digit();

    /**
    * SLOT method called when the N=Product Number is selected. Adds an N digit up to a maximum of 6
    *
    * @param[in] none
    * @returns  none
    */
    void updateAdd_N_Digit();

    /**
    * SLOT method called when the P=Price is selected. Adds an P digit up to a maximum of 7
    *
    * @param[in] none
    * @returns  none
    */
    void updateAdd_P_Digit();

    /**
    * SLOT method called when the V=Vendor number is selected. Adds an V digit up to a maximum of 6
    *
    * @param[in] none
    * @returns  none
    */
    void updateAdd_V_Digit();

    /**
    * SLOT method called when the p=price check digit is selected. Adds an p digit up to a maximum of 1
    *
    * @param[in] none
    * @returns  none
    */
    void updateAdd_pc_Digit();

    /**
    * SLOT method called when the Q=By count quantity is selected. Adds an Q digit up to a maximum of 2
    *
    * @param[in] none
    * @returns  none
    */
    void updateAdd_Q_Digit();

    /**
    * SLOT method called when the S=Scale ID is selected. Adds an S digit up to a maximum of 2
    *
    * @param[in] none
    * @returns  none
    */
    void updateAdd_S_Digit();

    /**
    * SLOT method called when the T=Transaction Number is selected. Adds an T digit up to a maximum of 6
    *
    * @param[in] none
    * @returns  none
    */
    void updateAdd_T_Digit();

    /**
    * SLOT method called when the Backspace is selected. Removes a single character
    *
    * @param[in] none
    * @returns  none
    */
    void updateAdd_BS_Digit();

    /**
    * SLOT method called when the Clear key is selected. Clears the entire contents of the list
    *
    * @param[in] none
    * @returns  none
    */
    void updateAdd_ClearMe();

//protected:
// nothing in here right now

private:
    Ui::eanEditor *ui;
    hobart::digitEntry *digitEntry_;

    SessionMainWindow *mainWindow_;
	QString eanString_;
    int eanLength_;
    int eanType_;

    QVector<QPushButton*> elementStack;
    QString checkDigit;
    bool hasChanged;

    bool adding_Es;
    bool Digits_Done;
    bool Xs_Done;
    bool Ws_Done;
    bool ws_small_Done;
    bool Ps_Done;
    bool ps_small_Done;
    bool Es_Done;
    bool Ns_Done;
    bool Vs_Done;
    bool Qs_Done;
    bool Ss_Done;
    bool Ts_Done;


    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();


    /**
      * Method to disable most of the controls on the dialog
      *
      * @param[in] none
      * @returns none
      */
    void disableControls();

    /**
      * Method to enable most of the controls on the dialog
      *
      * @param[in] none
      * @returns none
      */
    void enableControls();

    /**
      * Method to enable everything except the clear and backspace keys
      *
      * @param[in] none
      * @returns none
      */
    void enableAll();

    /**
     * Method used set a flag dependant upon the button name, which indicates if a button can no longer
     * receive characters of a particular type. Or reenables the flag so digits can be received again
     * in case of a backspace.
     *
     * @param[in] buttonID  buttonID is the name of the button to stop or start allowing characters of that type
     * @param[in] isDone    true if the more characters are allowed, false otherwise
     * @returns  none
     */
    void setDigitsDone(QString buttonID, bool isDone );

    /**
     * Method used to set the initial startup state or when the EAN format is cleared
     *
     * @param[in] none
     * @returns  none
     */
    void startState();

    /**
     * Method to clear the instructional messages at the bottom of the screen
     *
     * @param[in] none
     * @returns  none
     */
    void clearMessage();

    /**
     * Method to update the product defaults screen with default data values that
     * all plu numbers of this product type have in common.
     *
     * @param[in] digit  digit is the character being added to the EAN String by pressing the button.
     * @param[in] butNameId butNameId is the name of the widget performing the action.
     * @returns  none
     */
    void validateFields(QString digit, QString butNameId);

    /**
     * Method to examine the any character entered, and determine how many
     * characters that letter is allowed to enter.
     *
     * @param[in] eanChar  eanChar is the character length to be determined.
     * @returns  none
     */
    int getCharMax(QString myChar);


    /**
     * Method to validate the number of weight digits in the EAN format string
     *
     * @param[in] tmpStr  tmpStr is the current EAN format string.
     * @param[in] butNameId  butNameId string name of the button.
     * @returns  none
     */
    void isWeightFieldValid(QString tmpStr, QString butNameId);

    /**
     * Method to validate the number of price digits in the EAN format string
     *
     * @param[in] tmpStr  tmpStr is the current EAN format string.
     * @param[in] butNameId  butNameId string name of the button.
     * @returns  none
     */
    void isPriceFieldValid(QString tmpStr, QString butNameId);

    /**
     * Method to toggle between the Price or Weight input buttons when one of the fields reaches it MAX
     *
     * @param[in] eanStr  eanChar is the current EAN format string.
     * @returns  none
     */
    void togglePriceOrWeightDigit(QString eanStr);

    /**
     * Method used to add and validate a character being added to the EAN Format String
     *
     * @param[in] myChar  myChar is the format character to be added.
     * @param[in] MAX_DIGITS  MAX_DIGITS is the maximum number of a specific format character.
     * @param[in] element  element is the widget pressed to add the character.
     * @returns  none
     */
    void addFormatChar(QString myChar, int MAX_DIGITS, QPushButton *element);

    /**
     * Method used to add a price check digit or a weight check digit to verify either item.
     *
     * @param[in] myChar  myChar is the format character to be added.
     * @param[in] myBigChar  myBigChar is character the small digit is checking against as a check digit.
     * @param[in] element  element is the widget pressed to add the character.
     * @returns  none
     */
    void testLittleDigits(QString myChar, QString myBigChar, QPushButton *element);


    /**
     * Method to enable and disable all of the ean format string buttons on the screen
     * based upon the first character
     *
     * @param[in] myChar  myChar represents the first character entered
     * @returns  none
     */
    void processButtonsForFirstCharacter(QString myChar);

    /**
     * Method to disable either the EAN Digit, or the combined Vendor and Product Number Digits,
     * in a mutually exclusive manner from each other.
     *
     * @param[in] myChar  myChar represents added to the format string
     * @param[in] showMessage  showMessage indicates if we display any messages
     * @returns  none
     */
    void disableForENVChars(QString myChar, bool showMessage);

    /**
     * Method that disables features once the ean format string max length is reached.
     *
     * @param[in] none
     * @returns  bool true if the max length has been reached, otherwise false
     */
    bool checkForMaxLength();

    /**
     * Method to display any current instructions about the current ean format string.
     * The strings give warnings and formatting instructions to the user.
     *
     * @param[in] msgNumber  msgNumber is the message to display.
     *       [in] msgValue   msgValue is any input value that needs displayed in the message
     * @returns  none
     */
    void displayMessage(int msgNumber, QString msgValue);

    /**
     * Method emits the eanEditorDoneClicked signal when the Done button is pressed.
     *
     * @param[in] none
     * @returns  none
     */
    void emitDoneClicked(QString eanString);

    /**
     * Method emits the eanEditorCancelClicked signal when the Cancel button is pressed.
     *
     * @param[in] none
     * @returns  none
     */
    void emitCancelClicked(QString eanString);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // eanEditor_H
