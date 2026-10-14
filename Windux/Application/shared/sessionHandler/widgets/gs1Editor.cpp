/**
 * @file gs1Editor.cpp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @author Joe E. Crew
 */

#include "ui_gs1Editor.h"

#include "digitEntry.h"
#include "gs1Editor.h"

#include "../sessioninfo.h"

#include "../modes/supervisormode.h"

#include <logmanager.h>

#include <Session.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QDebug>
#include <QIntValidator>


// NOTES:
// - GTIN holds 14 digits
// - UPC is an existing form the GTIN
// - GTIN-12 (UPC-A):      12 digit number used in US
// - GTIN-8: (EAN/UCC-8):  8 digit number used outside the US
// - GTIN-13 (EAN/UCC-13): 13 digit number used outside the US
// - GTIN-14 (EAN/UCC-13): 14 digit number used to id trade items at packaging levels
// - All books and serial publications sold internationally (including those in U.S. stores) have
//   GTIN (GTIN-13) codes. The book codes are either constructed by prefixing the old 10-digit ISBN with 978,
//   and recalculating the trailing check digit, or from 1 January 2007 issued as thirteen digits starting
//   with 978 (eventually 979 as the 978 ranges are used up).
// - The GS1 DataBar can carry all 14 digits of a manufacture’s GTIN and is more than 50% smaller than
//   the currently used UPC and EAN symbols

static const int MAX_13_LEN = 13;
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

// Hobart namespace
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
gs1Editor::gs1Editor( const QString &gs1, bool ai, QWidget *parent ) :
    _Mybase( parent ),
    ui_( new Ui::gs1Editor ),
    gs1_( gs1 ),
    ai_( ai ),
    digitEntry_( NULL )
{
    LOG_CLASS_INFO( "gs1Editor", "gs1Editor" );

    ui_->setupUi( this );

    // hook into session
    sessionInfo info( this );

    if ( !(mainWindow_ = info()) )
        LOG_WARN( "could not retrieve main window!", NULL );

    // initialize
    initialize();
    translate();

    // setup state
    ui_->txtGS1FormatString->setText( gs1_ );

    checkCurrentState();

    // setup message
    if ( ai_ )
    {
        ui_->lblWarningMsg->setText( tr( "GS1 AI Code Entry" ) );
        ui_->lblWarningMsgTwo->setText( tr( "" ) );
    }
    else
    {
        ui_->lblWarningMsg->setText( tr( "GS1 GTIN Code (01) Entry" ) );
        ui_->lblWarningMsgTwo->setText( tr( " You may only enter a MAX of 13 for GTIN14. Other GTIN formats require less." ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
gs1Editor::~gs1Editor()
{
    if ( digitEntry_ )
        delete digitEntry_;

    delete ui_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gs1Editor::translate()
{
    ui_->lblGS1FormatStr->setText( tr( "GS1 format string" ) );  // FT-695
    ui_->butBackspace->setText( tr( "Backspace" ) );
    ui_->butByCountDigit->setText( tr( "Q = By count quantity (2 max)" ) );
    ui_->butCancel->setText( tr( "Cancel" ) );
    ui_->butClear->setText( tr( "Clear" ) );
    ui_->butDone->setText( tr( "Done" ) );
    ui_->butEanCodeDigit->setText( tr( "E = EAN code (13 max)" ) );
    ui_->butKeypadDigits->setText( tr( "Digits (0-9) = Prefix (15 max)" ) );
    ui_->butPrefixDigit->setText( tr( "X = Prefix (2 max)" ) );
    ui_->butPriceCheckDigit->setText( tr( "p = Price check digit (1)" ) );
    ui_->butPriceDigit->setText( tr( "P = Price (13 max)" ) );      // docs say it should be only 6.  Hmmmmm
    ui_->butProductNumberDigit->setText( tr( "N = Product number (6 max)" ) );
    ui_->butVendorNumberDigit->setText( tr( "V = Vendor number (6 max)" ) );
    ui_->butWeightCheckDigit->setText( tr( "w = Weight check digit (1)" ) );
    ui_->butWeightDigit->setText( tr( "W = Weight (6 max)" ) );
    ui_->butBestBeforeDateDigit->setText( tr( "B = Best before date (6 max)" ) );
    ui_->butSellByDateDigit->setText( tr( "C = Sell by date (6 max)" ) );
    ui_->butDateTimeProductionDigit->setText( tr( "D = Date && time of production (12 max)" ) );

    // setup message
    if ( ai_ )
    {
        ui_->lblWarningMsg->setText( tr( "GS1 AI Code Entry" ) );
        ui_->lblWarningMsgTwo->setText( tr( "" ) );
    }
    else
    {
        ui_->lblWarningMsg->setText( tr( "GS1 GTIN Code (01) Entry" ) );
        ui_->lblWarningMsgTwo->setText( tr( " You may only enter a MAX of 13 for GTIN14. Other GTIN formats require less." ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gs1Editor::onButtonClicked()
{
    const QString text( ui_->txtGS1FormatString->text() );

    if ( sender() == ui_->butDone )
    {
        emit doneClicked( text );
    }
    else if ( sender() == ui_->butCancel )
    {
        emit cancelClicked();
    }

    else if ( sender() == ui_->butPrefixDigit )
        ui_->txtGS1FormatString->setText( text + "X" );
    else if ( sender() == ui_->butKeypadDigits )
    {
        if ( !digitEntry_ )
        {
            digitEntry_ = new hobart::digitEntry( this, mainWindow_ );
            digitEntry_->move( (mainWindow_->width() * 231) / SessionMainWindow::scalingDividerX_, (mainWindow_->height() * 71) / SessionMainWindow::scalingDividerY_ );

            // listening for SIGNALS from the GS1 editor
            connect( digitEntry_, SIGNAL(digitEntryNumberClicked(QString)), SLOT(onDigitEntryNumberClicked(QString)) );
            connect( digitEntry_, SIGNAL(digitEntryClearClicked()), SLOT(onDigitEntryClearClicked()) );
            connect( digitEntry_, SIGNAL(digitEntryDoneClicked()), SLOT(onDigitEntryDoneClicked()) );
        }

        digitEntry_->show();
    }
    else if ( sender() == ui_->butWeightDigit )
        ui_->txtGS1FormatString->setText( text + "W" );
    else if ( sender() == ui_->butEanCodeDigit )
        ui_->txtGS1FormatString->setText( text + "E" );
    else if ( sender() == ui_->butWeightCheckDigit )
        ui_->txtGS1FormatString->setText( text + "w" );
    else if ( sender() == ui_->butProductNumberDigit )
        ui_->txtGS1FormatString->setText( text + "N" );
    else if ( sender() == ui_->butPriceDigit )
        ui_->txtGS1FormatString->setText( text + "P" );
    else if ( sender() == ui_->butVendorNumberDigit )
        ui_->txtGS1FormatString->setText( text + "V" );
    else if ( sender() == ui_->butPriceCheckDigit )
        ui_->txtGS1FormatString->setText( text + "p" );
    else if ( sender() == ui_->butByCountDigit )
        ui_->txtGS1FormatString->setText( text + "Q" );
    else if ( sender() == ui_->butBestBeforeDateDigit )
        ui_->txtGS1FormatString->setText( text + "B" );
    else if ( sender() == ui_->butSellByDateDigit )
        ui_->txtGS1FormatString->setText( text + "C" );
    else if ( sender() == ui_->butDateTimeProductionDigit )
        ui_->txtGS1FormatString->setText( text + "D" );

    else if ( sender() == ui_->butBackspace )
    {
        if ( text.length() )
            ui_->txtGS1FormatString->setText( text.left( text.length()-1 ) );
    }
    else if ( sender() == ui_->butClear )
    {
        ui_->txtGS1FormatString->setText( "" );
        ui_->butDone->setEnabled( true );
        ui_->butCancel->setEnabled( true );
    }

    checkCurrentState();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
void gs1Editor::onDigitEntryNumberClicked( const QString &value )
{
    int txtlen = ui_->txtGS1FormatString->text().length();
    if( txtlen < 15 )
    {
        ui_->txtGS1FormatString->setText( ui_->txtGS1FormatString->text() + value );
        checkCurrentState();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void gs1Editor::onDigitEntryClearClicked()
{
    ui_->txtGS1FormatString->setText( "" );
    checkCurrentState();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void gs1Editor::onDigitEntryDoneClicked()
{
    digitEntry_->hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void gs1Editor::initialize()
{
    connect( ui_->butDone, SIGNAL(clicked(bool)), SLOT(onButtonClicked()) );
    connect( ui_->butCancel, SIGNAL(clicked(bool)), SLOT(onButtonClicked()) );

    connect( ui_->butPrefixDigit, SIGNAL(clicked(bool)), SLOT(onButtonClicked()) );
    connect( ui_->butKeypadDigits, SIGNAL(clicked(bool)), SLOT(onButtonClicked()) );
    connect( ui_->butWeightDigit, SIGNAL(clicked(bool)), SLOT(onButtonClicked()) );
    connect( ui_->butEanCodeDigit, SIGNAL(clicked(bool)), SLOT(onButtonClicked()) );
    connect( ui_->butWeightCheckDigit, SIGNAL(clicked(bool)), SLOT(onButtonClicked()) );
    connect( ui_->butProductNumberDigit, SIGNAL(clicked(bool)), SLOT(onButtonClicked()) );
    connect( ui_->butPriceDigit, SIGNAL(clicked(bool)), SLOT(onButtonClicked()) );
    connect( ui_->butVendorNumberDigit, SIGNAL(clicked(bool)), SLOT(onButtonClicked()) );
    connect( ui_->butPriceCheckDigit, SIGNAL(clicked(bool)), SLOT(onButtonClicked()) );
    connect( ui_->butByCountDigit, SIGNAL(clicked(bool)), SLOT(onButtonClicked()) );
    connect( ui_->butBestBeforeDateDigit, SIGNAL(clicked(bool)), SLOT(onButtonClicked()) );
    connect( ui_->butSellByDateDigit, SIGNAL(clicked(bool)), SLOT(onButtonClicked()) );
    connect( ui_->butDateTimeProductionDigit, SIGNAL(clicked(bool)), SLOT(onButtonClicked()) );

    connect( ui_->butBackspace, SIGNAL(clicked(bool)), SLOT(onButtonClicked()) );
    connect( ui_->butClear, SIGNAL(clicked(bool)), SLOT(onButtonClicked()) );

    // for AI codes entry hide non-GTIN fields
    if ( ai_ )
    {
        ui_->butPrefixDigit->hide();
        ui_->butKeypadDigits->hide();
        ui_->butEanCodeDigit->hide();
        ui_->butWeightCheckDigit->hide();
        ui_->butProductNumberDigit->hide();
        ui_->butVendorNumberDigit->hide();
        ui_->butPriceCheckDigit->hide();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void gs1Editor::checkCurrentState()
{
    typedef QMap<QChar, int> charMap;

    charMap count;
    charMap contiguous;
    QMap<QChar, bool> valid;

    // count up characters
    const QString text( ui_->txtGS1FormatString->text() );
    int txtlen = text.length();

    if ( !ai_ )
    {
        ui_->butDone->setEnabled( (( txtlen >= 7 && txtlen <= 13 )) || txtlen ==0 );         // because we can have GS1-8 to GS1-13

        if( txtlen < 15 )
            ui_->butKeypadDigits->setEnabled( true );
        else
            ui_->butKeypadDigits->setEnabled( false );

    }

    // enable or disable controls based on validity
    ui_->butCancel->setEnabled( true );


    if( 13 <= txtlen )
    {
        ui_->butDone->setEnabled( true );
        ui_->butPrefixDigit->setEnabled( false );
        ui_->butWeightDigit->setEnabled( false );
        ui_->butEanCodeDigit->setEnabled( false );
        ui_->butWeightCheckDigit->setEnabled( false );
        ui_->butProductNumberDigit->setEnabled( false );
        ui_->butPriceDigit->setEnabled( false );
        ui_->butVendorNumberDigit->setEnabled( false );
        ui_->butPriceCheckDigit->setEnabled( false );
        ui_->butByCountDigit->setEnabled( false );
        ui_->butBestBeforeDateDigit->setEnabled( false );
        ui_->butSellByDateDigit->setEnabled( false );
        ui_->butDateTimeProductionDigit->setEnabled( false );
    }
    else if ( txtlen )
    {
        //count the number of characters in the string
        for ( int i( 0 ); i < txtlen; ++i )
        {
            const QChar c( text[i] );

            // ignore digits
            if (( '0' <= c ) && ( c <= '9' ))
                continue;

            // init mapping
            if ( !count.contains( c ) )
                count[c] = contiguous[c] = 0;

            // increment mappings
            ++count[c];

            if (( 0 < i ) && ( text[i-1] == c ))
                ++contiguous[c];
        }

        QChar last;
        if( txtlen > 0 )
            last = text[txtlen-1];

        //if max number of character in string then valid = false else valid is true
        valid['p'] = !(( count.contains( 'p' ) ) && ( 1 == count['p']  ));
        valid['w'] = !(( count.contains( 'w' ) ) && ( 1 == count['w']  ));
        valid['Q'] = !(( count.contains( 'Q' ) ) && ( 2 == count['Q']  ));
        valid['X'] = !(( count.contains( 'X' ) ) && ( 2 == count['X']  ));
        valid['B'] = !(( count.contains( 'B' ) ) && ( 6 == count['B']  ));
        valid['C'] = !(( count.contains( 'C' ) ) && ( 6 == count['C']  ));
        valid['W'] = !(( count.contains( 'W' ) ) && ( 6 == count['W']  ));
        valid['V'] = !(( count.contains( 'V' ) ) && ( 6 == count['V']  ));
        valid['N'] = !(( count.contains( 'N' ) ) && ( 6 == count['N']  ));
        valid['D'] = !(( count.contains( 'D' ) ) && ( 12 == count['D']  ));
        valid['E'] = !(( count.contains( 'E' ) ) && ( 13 == count['E']  ));
        valid['P'] = !(( count.contains( 'P' ) ) && ( 13 == count['P']  ));

        //if the string contains the character, but it isnt the last charcater ( to keep all characters contiguous)
        foreach ( const QChar &c, count.keys() )
            if(c != last)
                valid[c] = false;

        // NOTE:
        // according to original code 'E' is xor with 'V' and 'N', must pick one or other...
        if( count.contains('V') || count.contains('N') ){
            valid['E'] = false;
        }
        if( count.contains('E') ){
            valid['V'] = valid['N'] = false;
        }

        ui_->butBestBeforeDateDigit->setEnabled( valid['B'] );
        ui_->butSellByDateDigit->setEnabled( valid['C'] );
        ui_->butDateTimeProductionDigit->setEnabled( valid['D'] );
        ui_->butEanCodeDigit->setEnabled( valid['E'] );
        ui_->butProductNumberDigit->setEnabled( valid['N'] );
        ui_->butPriceCheckDigit->setEnabled( valid['p'] );
        ui_->butPriceDigit->setEnabled( valid['P'] );
        ui_->butByCountDigit->setEnabled( valid['Q'] );
        ui_->butVendorNumberDigit->setEnabled( valid['V'] );
        ui_->butWeightCheckDigit->setEnabled( valid['w'] );
        ui_->butWeightDigit->setEnabled( valid['W'] );
        ui_->butPrefixDigit->setEnabled( valid['X'] );
    }
    else
    {
        ui_->butPrefixDigit->setEnabled( true );
        ui_->butWeightDigit->setEnabled( true );
        ui_->butEanCodeDigit->setEnabled( true );
        ui_->butWeightCheckDigit->setEnabled( true );
        ui_->butProductNumberDigit->setEnabled( true );
        ui_->butPriceDigit->setEnabled( true );
        ui_->butVendorNumberDigit->setEnabled( true );
        ui_->butPriceCheckDigit->setEnabled( true );
        ui_->butByCountDigit->setEnabled( true );
        ui_->butBestBeforeDateDigit->setEnabled( true );
        ui_->butSellByDateDigit->setEnabled( true );
        ui_->butDateTimeProductionDigit->setEnabled( true );
    }

    ui_->butBackspace->setEnabled( !text.isEmpty() );
    ui_->butClear->setEnabled( !text.isEmpty() );
    //ui_->butDone->setEnabled( !text.isEmpty() );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

} // hobart namespace
