
/**
 * @file eanEditor.cpp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @author Joe E. Crew
 */


#include "eanEditor.h"
#include "ui_eanEditor.h"

#include "../sessioninfo.h"

#include "../modes/supervisormode.h"

#include <Session.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QtGui>
#include <QtCore>
#include <QDebug>

#include <QIntValidator>

static const int MAX_13_LEN = 12;
static const int MAX_128_LEN = 128;         // was 17. Should it be 127 or 128? I ask bc EAN13 is 12.
static const int MAX_ACTUAL_LEN = 50;       // so far the most I've seen printed on a label is 31 digits

static const int MAX_5 = 5;
static const int MAX_3 = 3;
static const int MAX_9 = 9;
static const int MAX_6 = 6;
static const int MAX_7 = 7;
static const int MAX_2 = 2;
static const int MAX_1 = 1;
static const int MAX_10 = 10;
static const int MAX_25 = 25;
static const int MAX_30 = 30;                                       // we are right on the limit on this to make it fit on the label. 29-30ish TOPS

static const int TYPE_EAN13_DEFAULTS = 1;                           // product defaults EAN 13
static const int TYPE_EAN128_DEFAULTS = 2;                          // product defaults EAN 128
static const int TYPE_EAN13_ACCUMULATE = 3;                         // from normal scale ops, accumulate items EAN string

// Hobart namespace
namespace hobart {

eanEditor::eanEditor(QWidget *parent, QString initialEan, int initialLength, int initialType, SessionMainWindow *mainWindow ) :
    _Mybase( parent ),
    ui(new Ui::eanEditor)
{
    ui->setupUi(this);

    mainWindow_ = mainWindow;
    digitEntry_ = NULL;
	eanString_ = initialEan;
    eanLength_ = initialLength;
    eanType_ = initialType;

    initialize();
}

eanEditor::~eanEditor()
{
    elementStack.clear();
    if( digitEntry_ != NULL )
        delete digitEntry_;
    delete ui;
}

//-----------------------------------------------
// Only show certain fields on the initial screen
//-----------------------------------------------
void eanEditor::initialize()
{
    // initialize vars
    //ui->eanEditor->setVisible(false);

    // eanFormatString Area
    connect(ui->butDone,                    SIGNAL(clicked()),              this, SLOT(updateDoneEanFormating()));          // clicked Done
    connect(ui->butCancel,                  SIGNAL(clicked()),              this, SLOT(updateCancelEanFormating()));        // clicked Cancel

    connect(ui->butPrefixDigit,             SIGNAL(clicked()),              this, SLOT(updateAdd_X_Digit()));               // clicked X=Prefix
    connect(ui->butKeypadDigits,            SIGNAL(clicked()),              this, SLOT(updateAdd_keypad_Digits()));         // clicked Digits(0-9)
    connect(ui->butWeightDigit,             SIGNAL(clicked()),              this, SLOT(updateAdd_W_Digit()));               // clicked W=Weight
    connect(ui->butEanCodeDigit,            SIGNAL(clicked()),              this, SLOT(updateAdd_E_Digit()));               // clicked E=EAN Code
    connect(ui->butWeightCheckDigit,        SIGNAL(clicked()),              this, SLOT(updateAdd_wc_Digit()));              // clicked w=weight check digit
    connect(ui->butProductNumberDigit,      SIGNAL(clicked()),              this, SLOT(updateAdd_N_Digit()));               // clicked N=Product Number
    connect(ui->butPriceDigit,              SIGNAL(clicked()),              this, SLOT(updateAdd_P_Digit()));               // clicked P=Price
    connect(ui->butVendorNumberDigit,       SIGNAL(clicked()),              this, SLOT(updateAdd_V_Digit()));               // clicked V=Vendor Number
    connect(ui->butPriceCheckDigit,         SIGNAL(clicked()),              this, SLOT(updateAdd_pc_Digit()));              // clicked p=Price Check Digit
    connect(ui->butByCountDigit,            SIGNAL(clicked()),              this, SLOT(updateAdd_Q_Digit()));               // clicked Q=Bycount quantity
    connect(ui->butScaleIdDigit,            SIGNAL(clicked()),              this, SLOT(updateAdd_S_Digit()));               // clicked S=Scale ID
    connect(ui->butTransactionNumberDigit,  SIGNAL(clicked()),              this, SLOT(updateAdd_T_Digit()));               // clicked T=Transaction number
    connect(ui->butBackspace,               SIGNAL(clicked()),              this, SLOT(updateAdd_BS_Digit()));              // clicked Backspace (a single char)
    connect(ui->butClear,                   SIGNAL(clicked()),              this, SLOT(updateAdd_ClearMe()));               // clicked Clear format string

	translate();

    if( eanType_ == TYPE_EAN13_DEFAULTS || eanType_== TYPE_EAN13_ACCUMULATE )
        ui->butKeypadDigits->setText(tr("Digits (0-9) = Prefix (12 max)"));
    else
        ui->butKeypadDigits->setText(tr("Digits (0-9) = Prefix (128 max)"));

    launchEanEditor();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool eanEditor::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::translate()
{
    ui->lblEanFormatStr->setText( tr( "Ean format string" ) ); // FT-695
    ui->butBackspace->setText(tr("Backspace"));
    ui->butByCountDigit->setText(tr("Q = By count quantity (2 max)"));
    ui->butCancel->setText(tr("Cancel"));
    ui->butClear->setText(tr("Clear format string"));
    ui->butDone->setText(tr("Done"));

    if( eanType_ == TYPE_EAN13_DEFAULTS || eanType_== TYPE_EAN13_ACCUMULATE )
    {
        ui->butEanCodeDigit->setText(tr("E = EAN code (11 max)"));
        ui->butKeypadDigits->setText(tr("Digits (0-9) = Prefix (12 max)"));
    }
    else
    {
        ui->butEanCodeDigit->setText(tr("E = EAN code (30 max)"));
        ui->butKeypadDigits->setText(tr("Digits (0-9) = Prefix (128 max)"));
    }

    ui->butPrefixDigit->setText(tr("X = Prefix (2 max)"));
    ui->butPriceCheckDigit->setText(tr("p = Price check digit (1)"));
    ui->butPriceDigit->setText(tr("P = Price (7 max)"));
    ui->butProductNumberDigit->setText(tr("N = Product number (6 max)"));
    ui->butVendorNumberDigit->setText(tr("V = Vendor number (6 max)"));
    ui->butWeightCheckDigit->setText(tr("w = Weight check digit (1)"));
    ui->butWeightDigit->setText(tr("W = Weight (5 max)"));
    ui->butScaleIdDigit->setText(tr("S = Scale ID (2 max)"));
    ui->butTransactionNumberDigit->setText(tr("T = Transaction number (6 max)"));

    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                       PRIVATE METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::disableControls()
{
    // disable all of the controls

    ui->butPrefixDigit->setEnabled(false);
    ui->butKeypadDigits->setEnabled(false);
    ui->butWeightDigit->setEnabled(false);
    ui->butPriceDigit->setEnabled(false);
    ui->butPriceCheckDigit->setEnabled(false);
    ui->butWeightCheckDigit->setEnabled(false);
    ui->butVendorNumberDigit->setEnabled(false);
    ui->butProductNumberDigit->setEnabled(false);
    ui->butEanCodeDigit->setEnabled(false);
    ui->butByCountDigit->setEnabled(false);
    ui->butScaleIdDigit->setEnabled(false);
    ui->butTransactionNumberDigit->setEnabled(false);

    if(hasChanged == false)
        ui->butBackspace->setEnabled(false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::enableControls()
{
    // if a specific button is not set as done being allowed to enter digits, enable it

    if ( Xs_Done == false)                                          // if we havent entered all of the total
        ui->butPrefixDigit->setEnabled(true);                       // number of a digit, we can reenable
    if ( Digits_Done == false )                                     // the key. For example, when entering
        ui->butKeypadDigits->setEnabled(true);                      // a 'W' all the other keys are disabled
    if ( Ws_Done == false )                                         // until 4 'W's are entered. This brings
        ui->butWeightDigit->setEnabled(true);                       // everyone back, AS LONG AS, that specific
    if ( Ps_Done == false)                                          // key has not already been entered. That is
        ui->butPriceDigit->setEnabled(true);                        // the IF check.
    if ( ps_small_Done == false && Ps_Done == false )
        ui->butPriceCheckDigit->setEnabled(true);
    if ( ws_small_Done == false && Ws_Done == false )
        ui->butWeightCheckDigit->setEnabled(true);
    if ( Qs_Done == false )
        ui->butByCountDigit->setEnabled(true);
    if( Ss_Done == false )
        ui->butScaleIdDigit->setEnabled(true);
    if( Ts_Done == false )
        ui->butTransactionNumberDigit->setEnabled( true );

    QString tempStr = ui->txtEanFormatString->toPlainText();

    int index = 0;

    if ( Vs_Done == false  && Es_Done == false )
    {
        index = tempStr.indexOf("E");

        if (index < 0)
            ui->butVendorNumberDigit->setEnabled(true);
    }

    if ( Ns_Done == false  && Es_Done == false && index < 0 )
        ui->butProductNumberDigit->setEnabled(true);

    if( index < 0 && adding_Es )
        adding_Es = false;              // just removed last E

    if ( Ns_Done == false  && Es_Done == false && Vs_Done == false )
    {
        int index1 = tempStr.indexOf("V");
        int index2 = tempStr.indexOf("N");

        if ( index1 < 0 && index2 < 0 )
            ui->butEanCodeDigit->setEnabled(true);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::enableAll()
{
    // bring everything back as enabled

    ui->butPrefixDigit->setEnabled(true);
    ui->butKeypadDigits->setEnabled(true);
    ui->butWeightDigit->setEnabled(true);
    ui->butPriceDigit->setEnabled(true);
    ui->butPriceCheckDigit->setEnabled(true);
    ui->butWeightCheckDigit->setEnabled(true);
    ui->butVendorNumberDigit->setEnabled(true);
    ui->butProductNumberDigit->setEnabled(true);
    ui->butEanCodeDigit->setEnabled(true);
    ui->butByCountDigit->setEnabled(true);
    ui->butScaleIdDigit->setEnabled(true);
    ui->butTransactionNumberDigit->setEnabled(true);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::displayMessage(int msgNumber, QString msgValue)
{
    QString warningStr = "";

    switch(msgNumber)
    {
        case 1:
            warningStr = tr("You must follow the ") + checkDigit + tr(" check digit with a valid field of at least four '") + checkDigit.toUpper() + tr("' digits");
        break;
        case 2:
            warningStr = tr("Press 'Clear format string' to clear the current string and begin creating a new one.");
        break;
        case 3:
            warningStr = tr("Either a fixed digit (0-9) or a prefix character (X) must be the first digits entered.");
        break;
        case 4:
            warningStr = tr("Prefix digit 'X' disabled.  Maximum number of prefix digits (2) entered.");
        break;
        case 5:
            warningStr = "";
        break;
        case 6:
            warningStr = "- " + msgValue + " " + tr("button disabled.  (Can't enter barcode characters non-consecutively.)");
        break;
        case 7:
            warningStr = tr("The maximum number of ") + msgValue + tr("'s has been entered.");
        break;
        case 8:
            warningStr = tr("The string has reached the maximum allowable length (12 characters).");
        break;
        case 9:
            if(msgValue == "E")
                warningStr = tr("-'V' and 'N' disabled. (Format string cannot incorporate both an EAN code 'E' field and either a vendor number 'V' or product number 'N'.)");
            else if(msgValue == "V" || msgValue == "N")
                warningStr = tr("-'E' disabled. (Format string cannot incorporate both an EAN code 'E' field and either a vendor number 'V' or product number 'N'.)");
        break;
        case 10:
            warningStr = "- '" + msgValue + tr("' check digit disabled. (Can't have a check digit past the seventh character, unless it follows its corresponding 'W' or 'P' digit.)");
        break;
        case 11:
            warningStr = tr("Re-enabled '") + msgValue + tr("' check digit.");
        break;
        case 12:
            warningStr = tr("Valid weight field detected, re-enabling valid digits.");
        break;
        case 13:
            warningStr = tr("Valid price field detected, re-enabling valid digits.");
        break;
        case 14:
            warningStr = tr("The string has reached the maximum length allowed (128 characters)");
        break;
    }

    ui->lblWarningMsg->setText(warningStr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::startState()
{
    // when we first come in we want to look like this.

    ui->txtEanFormatString->setEnabled(true);
    ui->butPrefixDigit->setEnabled(true);
    ui->butBackspace->setEnabled(true);
    ui->butKeypadDigits->setEnabled(true);

    ui->butPriceCheckDigit->setEnabled(false);
    ui->butWeightCheckDigit->setEnabled(false);
    ui->butPriceDigit->setEnabled(false);
    ui->butWeightDigit->setEnabled(false);
    ui->butVendorNumberDigit->setEnabled(false);
    ui->butProductNumberDigit->setEnabled(false);
    ui->butEanCodeDigit->setEnabled(false);
    ui->butByCountDigit->setEnabled(false);
    ui->butScaleIdDigit->setEnabled(false);
    ui->butTransactionNumberDigit->setEnabled(false);

    displayMessage(3, "");															//startStateMessage();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::clearMessage()
{
    ui->lblWarningMsg->clear();
    hasChanged = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::validateFields(QString myChar, QString butNameId)
{
    // validate the character entered. Also trigger some events that must occur if certain
    // characters are entered in a specific order or if character limits are reached.

    if (checkForMaxLength() == false)                               // has the max for this character been reached
    {
        if( myChar  == "W" )                                        // if not MAX reached, see if character is valid
        {
            isWeightFieldValid(ui->txtEanFormatString->toPlainText(), butNameId);
        }
        else if( myChar  == "P" )
        {
            isPriceFieldValid(ui->txtEanFormatString->toPlainText(), butNameId);
        }
        else if( myChar  == "X" )
        {
            if (butNameId == "butBackspace")                        // backspacing can bring back a key
            {
                enableControls();
            }
            else
            {
                if (Xs_Done == true )
                    displayMessage(4, "");                          //maxPrefixMessage();
            }
        }
        else if( myChar  == "w" ||  myChar  == "p" ||  myChar  == "E" ||  myChar  == "N" ||  myChar  == "V" || adding_Es)
        {
            if (butNameId == "butBackspace")
                enableControls();
        }
        else if( myChar  == "0")
        {
            if (butNameId == "butBackspace")
                enableControls();
        }
        else if( myChar.toInt() > 0)
        {
            if (butNameId == "butBackspace")
                enableControls();
        }


        int charsLeft = eanLength_ - ui->txtEanFormatString->toPlainText().length();

        if (charsLeft == 4)
        {
            if (butNameId != "butBackspace")
            {
                if ( ps_small_Done == false && ui->butPriceCheckDigit->isEnabled() )
                {
                    //	Disable the Price check button.
                    ui->butPriceCheckDigit->setEnabled(false);
                }
                if ( ws_small_Done == false && ui->butWeightCheckDigit->isEnabled() )
                {
                    //	Disable the Weight check button.
                    ui->butWeightCheckDigit->setEnabled(false);
                }
            }
            else
            {
                if ( (myChar != "W") && ( Ps_Done == false ) && ( ui->butPriceDigit->isEnabled() == false ))
                {
                    //	Enable the Price button.
                    ui->butPriceDigit->setEnabled(true);
                }
                if ((myChar != "P") && ( Ws_Done == false ) && (ui->butWeightDigit->isEnabled() == false ) )
                {
                    //	Enable the Weight button.
                    ui->butWeightDigit->setEnabled(true);
                }
            }
        }

        if ( charsLeft == 5 && butNameId == "butBackspace" )
        {
            QString tempStr = ui->txtEanFormatString->toPlainText();

            if ( (myChar != "P") && ( ps_small_Done == false ) && ( ui->butPriceCheckDigit->isEnabled() == false ) )
            {
                if (myChar == "W")
                {
                    int howManyOfThese = tempStr.count("W");
                    if (howManyOfThese >= 4 )
                    {
                        ui->butPriceCheckDigit->setEnabled(true);   //	Enable the Price check button.
                    }
                }
            }

            if ( (myChar != "W") && ( ws_small_Done == false ) && ( ui->butWeightCheckDigit->isEnabled() == false ) )
            {
                if (myChar == "P")
                {
                    int howManyOfThese = tempStr.count("P");
                    if ( howManyOfThese >= 4 )
                    {
                        ui->butWeightCheckDigit->setEnabled(true);  //	Enable the Weight check button.
                    }
                }
            }
        }

        if (charsLeft < 4)
        {
            if ( (myChar != "P") && ( Ps_Done == false ) && (ui->butPriceDigit->isEnabled() == true ) )
            {
                //	We cannot add the proper number of digits for Price, so disable them.
                ui->butPriceDigit->setEnabled(false);
                ui->butPriceCheckDigit->setEnabled(false);
            }
            if ((myChar != "W") && ( Ws_Done == false ) && (ui->butWeightDigit->isEnabled() == true))
            {
                //	We cannot add the proper number of digits for Weight, so disable them.
                ui->butWeightDigit->setEnabled(false);
                ui->butWeightCheckDigit->setEnabled(false);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::isWeightFieldValid(QString tmpStr, QString butNameId)
{
    // There are some specific rules that need checked if we enter a W

    int howManyOfThese = tmpStr.count("W");
    int tmpLen  = tmpStr.length()  - 1;

    if ( howManyOfThese == 0 )                                      // if we haven't entered a W before
    {
        if(tmpStr[tmpLen] != QChar('w') )                           // if the last character is not  little w
            enableControls();                                       // bring keys back. w can be on front or end of str
    }
    else
    {
        if ( (howManyOfThese == 3) && (butNameId == "butBackspace"))
        {
            //	Disable the other chars until we reach a minimum of 4 W chars.
            disableControls();
            ui->butWeightDigit->setEnabled(true);
            ui->butWeightCheckDigit->setEnabled(false);
        }
        else if (howManyOfThese == 4)
        {
            //	A minimum of 4 W chars before other format chars can be added
            enableControls();
            displayMessage(12, "");
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::isPriceFieldValid(QString tmpStr, QString butNameId)
{
    // There are some specific rules that need checked if we enter a P

    int howManyOfThese = tmpStr.count("P");

    int tmpLen  = tmpStr.length() - 1;

    if ( howManyOfThese == 0 )
    {
        if(tmpStr[tmpLen] != QChar('p') )                  // .charAt(tmpStr.length - 1) != "w")
            enableControls();
    }
    else
    {
        if ( howManyOfThese == 3 && butNameId == "butBackspace" )
        {
            //	Disable the other chars until we reach a minimum of 4 P chars.

            disableControls();
            ui->butPriceDigit->setEnabled(true);
            ui->butPriceCheckDigit->setEnabled(false);              // Can't be enabled until we have 4 P chars.
        }
        else if( howManyOfThese == 4 )
        {
            //	A minimum of 4 P chars before other format chars can be added.
            enableControls();
            displayMessage(13, "");
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::togglePriceOrWeightDigit(QString eanStr)
{
    // need to decide when to turn on and turn off price and weight check digits. They have their own rules

    int eanLen = eanStr.length();


    if ( eanLen >= 9 && eanLen < 12 )                               // only 3 chars left to enter
    {
        if(eanStr[eanLen - 1] != QChar('W') )                       // if the previous char is not a W, then disable the
        {                                                           // W and w keys because there is not enough room to
            ui->butWeightDigit->setEnabled(false);                  // enter the key because there must be enough room
            ui->butWeightCheckDigit->setEnabled(false);             // to enter at least 4 W digits
        }

        if(eanStr[eanLen - 1] != QChar('P') )                       // See the Weight comment just above, same issue
        {
            ui->butPriceDigit->setEnabled(false);
            ui->butPriceCheckDigit->setEnabled(false);
        }
    }
    else if(eanLen < 9)
    {
        // REWORKED - correct now.
        int howManyOfTheseWs = eanStr.count("W");                   // count the number of Ws

        if( howManyOfTheseWs == 0 )
            ui->butWeightDigit->setEnabled(true);                   // still have room so enable the weight digit

        int howManyOfThesePs = eanStr.count("P");                   // count Ps

        if( howManyOfThesePs == 0 && ( howManyOfTheseWs == 0 || howManyOfTheseWs >= 4 ) )
            ui->butPriceDigit->setEnabled(true);                    // still have room so enable Ps as long as not entering Ws
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int eanEditor::getCharMax(QString myChar)
{
    // How many can we enter?

    int theMax;

    if( myChar == "X" )
        theMax = 2;
    else if( myChar == "W" )
        theMax = 5;
    else if( myChar == "P" )
        theMax = 7;
    else if( myChar == "U" )
        theMax = 3;
    else if( myChar == "E" )
        theMax = 30;
    else if( myChar == "N" )
        theMax = 6;
    else if( myChar == "V" )
        theMax = 6;
    else if( myChar == "Q" )
        theMax = 2;
    else if( myChar == "0" )
        theMax = eanLength_;
    else if( myChar.toInt() )
        theMax = eanLength_;
    else if( myChar == "S" )
        theMax = 2;
    else if( myChar == "T" )
        theMax = 6;
    else
        theMax = eanLength_;

    return theMax;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::addFormatChar(QString myChar, int MAX_DIGITS, QPushButton *element)
{
    // This is where the character key pressed is added to the string.

    QString eanStr = ui->txtEanFormatString->toPlainText();                // current EAN format string
    int eanlen = eanStr.length();

    int stackLen = 0;                                               // each button we press is added to a vector and pulled off when removed. How many?
    QPushButton *tmpElem = new QPushButton();

    hasChanged = true;

    int howManyOfThese = eanStr.count(myChar);                      // how many characters like this are in the string

    if ( howManyOfThese == 0 )                                      // This is a new character
    {
        stackLen = elementStack.size();
        if (stackLen > 1)
            tmpElem = elementStack.at(stackLen - 1);                // get the previous character being entered off the stack because
                                                                        // b\c prior character are disabled b\c all character entry is CONTINOUS
        if(tmpElem->objectName().length() > 0 )
        {
            setDigitsDone(tmpElem->objectName(), true );            // set previous character as DONE, no more can be entered
            tmpElem->setEnabled(false);                             // Mark the prior button as done so it does not get enabled again.
        }

        QString tmpStr = ui->txtEanFormatString->toPlainText() + myChar;
        ui->txtEanFormatString->setText(tmpStr);                    // Add the character to the format string.
        elementStack.append(element);                               // push current key on stack

        processButtonsForFirstCharacter(myChar);                    //	Do any other buttons need to be disabled? We only need to do this on the first character.
    }
    else
    {
        if( howManyOfThese <= MAX_DIGITS)                           // if we are not at the MAX for this key
        {
                //	Add the character to the format string.
            QString tmpStr = ui->txtEanFormatString->toPlainText() + myChar; // add it on too
            ui->txtEanFormatString->setText(tmpStr);                // Add the character to the format string.

            elementStack.append(element);                           // push to stack
            stackLen = elementStack.size();                         // how many now?

            if ( howManyOfThese == (MAX_DIGITS - 1) )
            {
                setDigitsDone(element->objectName(), true );
                element->setEnabled(false);                         //	Mark the button as done so it does not get enabled again.
            }
        }

        if( 30 == eanlen )
            disableControls();

    }

    validateFields(myChar, element->objectName() );                 // validate key

    checkDigit = "";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::processButtonsForFirstCharacter(QString myChar)
{
    // when the first character is entered, sometimes other keys are immediately disabled.

    if( myChar == "E" || myChar == "N" || myChar == "V" )
    {
        disableForENVChars(myChar, true);
    }
    else if(myChar == "P")
    {
        disableControls();
        ui->butPriceDigit->setEnabled(true);
    }
    else if(myChar == "W")
    {
        disableControls();
        ui->butWeightDigit->setEnabled(true);
    }
    else if(myChar == "p")
    {
        // Disable the p button.
        ps_small_Done = true;
        ui->butPriceCheckDigit->setEnabled(false);
    }
    else if(myChar == "w")
    {
        ws_small_Done = true;
        ui->butWeightCheckDigit->setEnabled(false);
    }
    else if(myChar == "X")
    {
        if(ui->txtEanFormatString->toPlainText().length() == 2)
        {
            Xs_Done = true;
            ui->butPrefixDigit->setEnabled(false);
            enableControls();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::testLittleDigits(QString myChar, QString myBigChar, QPushButton *element)
{
    // test 'p' and 'w'

    QString tmpStr = ui->txtEanFormatString->toPlainText();

    int howManyOfThese = tmpStr.count( myBigChar );                 // how many of these big character digits, W or P
    int tmpLen = tmpStr.length();


    if ( howManyOfThese > 0 )
    {
        QString lastChar = tmpStr.mid(tmpLen - 1, tmpLen );         // enters here with a trailing check digit
        checkDigit = myChar;

        if ( lastChar == myBigChar || myChar == lastChar )          // if last char was a W/P or w=w or p=p
        {
            addFormatChar(myChar, MAX_1, element);                  // add the p or w
        }
        else
        {
            element->setEnabled(false);                             // then we must disable it since it's already there
        }
    }
    else
    {
        addFormatChar(myChar, MAX_1, element);                      //enters here with a leading check digit

        disableControls();

        if( myChar == "w" && ui->txtEanFormatString->toPlainText().length() < 9 )
        {
            ui->butWeightDigit->setEnabled(true);
            checkDigit = "w";
            displayMessage(1, "");                                  //displayCheckDigitWarning();
        }
        else if(myChar == "p" && ui->txtEanFormatString->toPlainText().length() < 9)
        {
            ui->butPriceDigit->setEnabled(true);
            checkDigit = "p";
            displayMessage(1, "");                                  //displayCheckDigitWarning();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::disableForENVChars(QString myChar, bool showMessage)
{
    // if we enter an E, we are not allowed to have Vs or Ns, or if we enter a V or N, disable Es

    if ( myChar == "E")
    {
        ui->butVendorNumberDigit->setEnabled(false);
        ui->butProductNumberDigit->setEnabled(false);

        if (showMessage == true)
            displayMessage(9, myChar);								//displayEANDetected(myChar);
    }
    else if( myChar == "V" || myChar == "N" )
    {
        ui->butEanCodeDigit->setEnabled(false);

        if (showMessage == true)
            displayMessage(9, myChar);								//displayEANDetected(myChar);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool eanEditor::checkForMaxLength()
{
    int fieldLen = ui->txtEanFormatString->toPlainText().length();
    int adjLength = eanLength_ - 1;

    if ( fieldLen > adjLength  )
    {
        disableControls();
        if( fieldLen == MAX_13_LEN )
            displayMessage(8, "");                                  // maxStringLengthWarning(); ean format string
        else
            displayMessage(14, "");                                 // maxStringLengthWarning(); ean128C format string

        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::setDigitsDone(QString buttonID, bool isDone )
{
    // if isDone == true, that means it is TRUE that the button is NOW disabled
    // from CLICKING any more characters of that type. That button IS DONE.
    // Don't ask, I didn't write it, just ported it.
    // I am only going to use this method when the ID name can be anything on
    // a particular line of code. Otherwise I am going to directly set it.

    if(buttonID == "butPrefixDigit")
        Xs_Done = isDone;
    else if(buttonID == "butKeypadDigits")
        Digits_Done = isDone;
    else if(buttonID == "butWeightDigit")
        Ws_Done = isDone;
    else if(buttonID == "butWeightCheckDigit")
        ws_small_Done = isDone;
    else if(buttonID == "butPriceDigit")
        Ps_Done = isDone;
    else if(buttonID == "butPriceCheckDigit")
        ps_small_Done = isDone;
    else if(buttonID == "butEanCodeDigit")
        Es_Done = isDone;
    else if(buttonID == "butProductNumberDigit")
        Ns_Done = isDone;
    else if(buttonID == "butVendorNumberDigit")
        Vs_Done = isDone;
    else if(buttonID == "butByCountDigit")
        Qs_Done = isDone;
    else if(buttonID == "butScaleIdDigit")
        Ss_Done = isDone;
    else if(buttonID == "butTransactionNumberDigit")
        Ts_Done = isDone;

}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::launchEanEditor()
{
    // clear everything up to launch it

    ui->txtEanFormatString->setText( eanString_ );
    hasChanged = false;
    Digits_Done = false;
    adding_Es = false;
    Xs_Done = false;
    Ws_Done = false;
    ws_small_Done = false;
    Ps_Done = false;
    ps_small_Done = false;
    Es_Done = false;
    Ns_Done = false;
    Vs_Done = false;
    Qs_Done = false;
    Ss_Done = false;
    Ts_Done = false;
    checkDigit = "";
    disableControls();
    displayMessage(2, "");												//displayStartMessage();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::updateDoneEanFormating()
{
    // set the accumulate items or default PLU settings DOM value to the new format string

    sessionInfo info( this );
    SessionMainWindow *mainWindow = info();                                         // retrieve pointer to session

    // lets get some pointers
    QString tmpEanString = ui->txtEanFormatString->toPlainText();
    ScaleConfiguration *config = mainWindow->session()->scaleConfig;
    if( eanType_ == TYPE_EAN13_DEFAULTS )
    {
        DomDefaultCurrentPLU *defaultPLU = &config->defaultCurrentPLU;
        defaultPLU->eanFormatString() = tmpEanString;                               // set the DOM data
    }
    else if( eanType_ == TYPE_EAN128_DEFAULTS )
    {
        DomDefaultCurrentPLU *defaultPLU = &config->defaultCurrentPLU;
        defaultPLU->ean128CFormatString() = tmpEanString;                           // set the DOM data
    }
    else if( eanType_ == TYPE_EAN13_ACCUMULATE)
    {
        MadpAccumItemsConfig *accumItems = &config->accumulateItemsConfig;
        accumItems->accumItemsEANFormatString() = tmpEanString;                     // set the DOM data
    }

    emitDoneClicked( tmpEanString );

    ui->txtEanFormatString->clear();
    setVisible(false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::updateCancelEanFormating()
{
    // reset the EAN format string back to its original value

    ui->txtEanFormatString->clear();

    sessionInfo info( this );
    SessionMainWindow *mainWindow = info();

    // lets get some pointers
    ScaleConfiguration *config = mainWindow->session()->scaleConfig;
    if( eanType_ == TYPE_EAN13_DEFAULTS )
    {
        DomDefaultCurrentPLU *defaultPLU = &config->defaultCurrentPLU;
        defaultPLU->eanFormatString() = eanString_;
    }
    else if( eanType_ == TYPE_EAN128_DEFAULTS )
    {
        DomDefaultCurrentPLU *defaultPLU = &config->defaultCurrentPLU;
        defaultPLU->ean128CFormatString() = eanString_;
    }
    else if( eanType_ == TYPE_EAN13_ACCUMULATE)
    {
        MadpAccumItemsConfig *accumItems = &config->accumulateItemsConfig;
        accumItems->accumItemsEANFormatString() = eanString_;
    }

    emitCancelClicked( eanString_ );

    setVisible(false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::updateAdd_ClearMe()
{
    Digits_Done = false;
    Xs_Done = false;
    Ws_Done = false;
    ws_small_Done = false;
    Ps_Done = false;
    ps_small_Done = false;
    Es_Done = false;
    Ns_Done = false;
    Vs_Done = false;
    Qs_Done = false;
    Ss_Done = false;
    Ts_Done = false;

    ui->txtEanFormatString->clear();
    checkDigit = "";
    clearMessage();
    startState();
    elementStack.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::updateAdd_BS_Digit()
{
    // Backspace key clicked

    QString eanStr = ui->txtEanFormatString->toPlainText();
    int tmpLen = eanStr.length();

    if( tmpLen >= 1 )
    {
        QString remChar = eanStr.at(tmpLen - 1);
        int stackLen = elementStack.size();
        QPushButton *remElem = new QPushButton();
        remElem = elementStack.at(stackLen - 1);
        QString prevChar = "";
        QPushButton *prevElem = new QPushButton();
        bool prevCharIsDifferent = false;

        if(tmpLen >= 2)
        {
            prevChar = eanStr.at(tmpLen - 2);
            prevElem = elementStack.at(stackLen - 2 );
            if ( (prevChar != remChar) && (prevChar != "") )
            {
                prevCharIsDifferent = true;
            }
        }

        setDigitsDone( remElem->objectName(), false );              // clear the DigitDone flag
        remElem->setEnabled(true);
        elementStack.remove(stackLen - 1);

        clearMessage();

        if(tmpLen == 12)
            enableControls();               // not sure what ths is supposed to do and for who

        eanStr = eanStr.mid(0,tmpLen - 1);

        ui->txtEanFormatString->setText(eanStr);

        if (prevCharIsDifferent == true)
        {
            int maxDigits = getCharMax( prevChar );
            int howManyOfThese = eanStr.count( prevChar );

            if( howManyOfThese > 0 )
            {
                if( howManyOfThese == maxDigits )
                {
                    setDigitsDone( prevElem->objectName(), true );
                    prevElem->setEnabled(false);
                }
                else
                {
                    if (prevElem->objectName() == "butKeypadDigits" && ui->txtEanFormatString->toPlainText().length() == 1)
                    {
                        Xs_Done = false;
                        ui->butPrefixDigit->setEnabled(true);
                    }
                    else if( (prevElem->objectName() != "butPrefixDigit") || ( (prevElem->objectName() == "butPrefixDigit") && ( ui->txtEanFormatString->toPlainText().length() == 1)) )
                    {
                        prevElem->setEnabled(true);
                    }
                }
            }
        }
        validateFields(prevChar, ui->butBackspace->objectName() );

        hasChanged = true;

        if( eanStr.length() == 0 )
            startState();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::updateAdd_E_Digit()
{
    if ( ui->butEanCodeDigit->isEnabled() )
    {
        clearMessage();
        adding_Es = true;
        addFormatChar("E", MAX_30, ui->butEanCodeDigit);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::updateAdd_keypad_Digits()
{
    if ( !ui->butKeypadDigits->isEnabled()  )
        return;

    hasChanged = true;

    disableControls();
    ui->butBackspace->setEnabled( false );
    ui->butClear->setEnabled( false );

    ui->butCancel->setEnabled(false);
    ui->butDone->setEnabled(false);

    digitEntry_ = new hobart::digitEntry(this, mainWindow_);
    digitEntry_->move( (mainWindow_->width() * 231) / SessionMainWindow::scalingDividerX_, (mainWindow_->height() * 71) / SessionMainWindow::scalingDividerY_ );
    digitEntry_->show();

    // listening for SIGNALS from the EAN editor
    connect(digitEntry_,    SIGNAL( digitEntryNumberClicked( QString ) ),   SLOT( onDigitEntryNumberClicked( QString )) );
    connect(digitEntry_,    SIGNAL( digitEntryClearClicked(  ) ),           SLOT( onDigitEntryClearClicked(  )) );
    connect(digitEntry_,    SIGNAL( digitEntryDoneClicked(  ) ),            SLOT( onDigitEntryDoneClicked(  )) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::updateAdd_N_Digit()
{
    if ( ui->butProductNumberDigit->isEnabled() )
    {
        clearMessage();
        addFormatChar("N", MAX_6, ui->butProductNumberDigit);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::updateAdd_P_Digit()
{
    if ( ui->butPriceDigit->isEnabled() )
    {
        clearMessage();
        addFormatChar("P", MAX_7, ui->butPriceDigit);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::updateAdd_pc_Digit()
{
    if ( ui->butPriceCheckDigit->isEnabled() )
    {
        clearMessage();
        testLittleDigits("p", "P", ui->butPriceCheckDigit);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::updateAdd_Q_Digit()
{
    if ( ui->butByCountDigit->isEnabled() )
    {
        clearMessage();
        addFormatChar("Q", MAX_2, ui->butByCountDigit );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::updateAdd_V_Digit()
{
    if ( ui->butVendorNumberDigit->isEnabled() )
    {
        clearMessage();
        addFormatChar("V", MAX_6, ui->butVendorNumberDigit );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::updateAdd_W_Digit()
{
    if( ui->butWeightDigit->isEnabled() )
    {
        clearMessage();
        addFormatChar("W", MAX_5, ui->butWeightDigit);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::updateAdd_wc_Digit()
{
    if ( ui->butWeightCheckDigit->isEnabled() )
    {
        clearMessage();
        testLittleDigits("w", "W", ui->butWeightCheckDigit );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::updateAdd_X_Digit()
{
    if( ui->butPrefixDigit->isEnabled() )
    {
        if( ui->txtEanFormatString->toPlainText().length()  == 0)
        {
            enableAll();
            clearMessage();
        }

        addFormatChar("X", MAX_2, ui->butPrefixDigit );
    }
}

void eanEditor::updateAdd_S_Digit()
{
    if ( ui->butScaleIdDigit->isEnabled() )
    {
        clearMessage();

        addFormatChar("S", MAX_2, ui->butScaleIdDigit);
    }

}

void eanEditor::updateAdd_T_Digit()
{
    if ( ui->butTransactionNumberDigit->isEnabled() )
    {
        clearMessage();

        addFormatChar("T", MAX_6, ui->butTransactionNumberDigit);
    }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::onDigitEntryNumberClicked(QString numberDigit)
{
    QString eanStr = ui->txtEanFormatString->toPlainText();
    int eanLen = eanStr.length();

    if(eanLength_ == eanLen)                                           // elementstack will be too long with new character do remove
    {                                                               // the 0th element from the elementStack vector list
        eanStr = eanStr.mid(1);
        elementStack.remove(0);
    }

    eanStr = eanStr + numberDigit;
    ui->txtEanFormatString->setText(eanStr);

    QPushButton *tmpElem = new QPushButton();
    tmpElem = ui->butKeypadDigits;
    elementStack.append(tmpElem);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::onDigitEntryClearClicked()
{
    ui->txtEanFormatString->setText("");
    elementStack.clear();
    hasChanged = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::onDigitEntryDoneClicked()
{
    QPushButton *element = new QPushButton();
    element = ui->butKeypadDigits;                                  // the 0-9 digits button

    QString tmpStr = ui->txtEanFormatString->toPlainText();         // what is the current eanFormatString
    int tmpStrLen = tmpStr.length();                                // length of current eanFormatString length
    int eanFieldLength = tmpStrLen - 1;                             // want to know the length before last number added
    int stackLen = elementStack.size();                             // the size of the stack that keeps an array of button objects pressed

    QPushButton *priorElem = new QPushButton();

    hasChanged = 1;                                                 // we changed something here

    if (stackLen >= 1)                                              // check if we have any previous items in the objects array
        priorElem = elementStack.at(stackLen - 1);                  // we do, so grab it

    if ((priorElem != NULL) && (priorElem->objectName() != "butKeypadDigits") )
    {
        // get the previous character being entered off the stack because
        // b\c prior character are disabled b\c all character entry is CONTINOUS

        setDigitsDone(priorElem->objectName(), true );              // set previous character as DONE, no more can be entered
        priorElem->setEnabled(false);                                // disable it too
    }

    bool atMax = checkForMaxLength();                               // is the eanFormatString field at it's max length?

    if ( (atMax == false) && (element->objectName() == "butKeypadDigits") )
    {
        if (  (tmpStrLen > 1)   ||   ((eanFieldLength == 1) && (tmpStrLen >= 1))  )
        {
            enableControls();                                       // re-enables stuff now that we are done
            setDigitsDone("butPrefixDigit", true );                 // Disable the X (Prefix) button because it can only be the
            ui->butPrefixDigit->setEnabled(false);                  // the first or second digit of the eanFormatString

            clearMessage();
        }
        else if(1 == tmpStrLen )
        {
            // FT-872 EAN Prefix and digits
            // as long as the tmpStrLen is 1, we are allowed to add more digits,
            // or one single X, and more digits so bring back the Xs for one more digit

            ui->butPrefixDigit->setEnabled(true);
            ui->butKeypadDigits->setEnabled(true);
        }
    }

    ui->butBackspace->setEnabled( true );
    ui->butClear->setEnabled( true );
    ui->butCancel->setEnabled(true);
    ui->butDone->setEnabled(true);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::emitDoneClicked(QString eanString)
{
    emit eanEditorDoneClicked( eanString );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void eanEditor::emitCancelClicked(QString eanString)
{
    emit eanEditorCancelClicked( eanString );
}

} // hobart namespace
