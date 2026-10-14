/**
 * @file selfservicenumberpad.h
 * Self service number pad.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup selfServeCsu
 * @{
 */

#ifndef SELFSERVICENUMBERPAD_H
#define SELFSERVICENUMBERPAD_H

#include <QMap>
#include <QWidget>
#include <QStackedWidget>

class QIntValidator;
class QLabel;
class QTextEdit;
class QSignalMapper;
class QVBoxLayout;
class QHBoxLayout;
class QSpacerItem;

namespace hobart {
    class numericLineEdit;
    class pushButton;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Self service number pad.
class selfServiceNumberPad : public QWidget
{
    Q_OBJECT

    typedef selfServiceNumberPad _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /**
     * @brief  Emitted when text changed.
     * @param[in] text  text
     */
    void textChanged( const QString& text );

    /// Emitted when auto lookup is triggered.
    void lookup();

    /// Emitted when a bycount value is entered.
    void showBackButton();

    /// Emitted when print is triggered.
    void print();

    /**
     * @brief  Emitted when we should trigger an error like no bc quantity entered.
     * @param[in] text  text
     */
    void printError( int err );


    /**
     * @brief  Emitted when we hit enter on the keypad on the login screen
     * @param[in] text  id
     */
    void selfServiceLogin(QString operId);

public:

    // ************************************************************************
    // CTOR / DTOR
    // ************************************************************************

    /**
     * @brief CTOR.
     * @param[in] parent  parent widget
     */
    selfServiceNumberPad( QWidget *parent = NULL );

    /// DTOR
    virtual ~selfServiceNumberPad();

    // ************************************************************************
    // Properties
    // ************************************************************************

    /**
     * @brief  Retrieve auto lookup number of digits.
     * @return  number of digits
     */
    virtual int autoLookupNumOfDigits() const {return autoLookupNumOfDigits_;}

    /**
     * @brief  Retrieve auto lookup override digit.
     * @return  override digit
     */
    virtual int autoLookupOverrideDigit() const {return autoLookupOverrideDigit_;}

    /**
     * @brief  Retrieve auto lookup enabled or not.
     * @return  @c true if enabled, @c false otherwise
     */
    virtual bool autoLookupProduct() const {return autoLookupOverrideDigit_;}

    /**
     * @brief  Retrieve state of keypad. It is displaying the keypad or the Print/Scan feature
     * @return  @c true if enabled, @c false otherwise
     */
    virtual int numberPadStackState();

    /**
     * @brief  Retrieve number pad style.
     * @return  "123" for telephone style, "789" for keyboard style
     */
    virtual QString style() const {return style_;}

    /**
     * @brief  Retrieve number pad text.
     * @return  text
     */
    virtual QString text() const;

    // ************************************************************************
    // Methods
    // ************************************************************************

    /// Translate to current language.
    virtual void translate();

    /**
     * @brief  Update barcode only label.
     */
    virtual void renderBarcodeOnlyLabel();

    hobart::pushButton *enter() const;

public Q_SLOTS:

    /// Clear text.
    virtual void clear();

    /**
     * @brief  Set auto lookup number of digits.
     * @param[in] value  number of digits
     */
    virtual void setAutoLookupNumOfDigits( int value );

    /**
     * @brief  Set auto lookup override digit.
     * @param[in] value  override digit
     */
    virtual void setAutoLookupOverrideDigit( int value );

    /**
     * @brief  Set auto lookup enabled or not.
     * @param[in] value  @c true if enabled, @c false otherwise
     */
    virtual void setAutoLookupProduct( bool value );

    /**
     * @brief  Set number pad style.
     * @param[in] value  "123" for telephone style, "789" for keyboard style
     */
    virtual void setStyle( const QString& value );

    /**
     * @brief  Set number pad text.
     * @param[in] value  text
     */
    virtual void setText( const QString& value );

    /**
     * @brief  Set text for instructions.
     * @param[in] value  text
     */
    virtual void setInstructions( const QString& value );

    /**
     * @brief make the file name available for use since the numpad does not have access to hooks
     */
    virtual void setBarcodeFileName(const QString& value);

    /**
     * @brief  Set the action for the Enter key to Lookup PLU(0) or Print(1).
     * @param[in] value response
     */
    virtual void setEnterResponse( int value );

    /**
     * @brief  Set the text on the Enter key to be either Enter(0) or Print(1).
     * @param[in] value text
     */
    virtual void setEnterText( int value );

    /**
     * @brief  Set the text of the operator id being entered
     * @param[in] value text
     */
    virtual void setOperatorID( const QString& value );

    /**
     * @brief  Display the number pad or instructions.
     * @param[in] value  item to display
     */
    virtual void displayStackedWidget( int value );

    /**
     * @brief  Display the number pad or print and scan options.
     * @param[in] value  item to display
     */
    virtual void displayStackedNumWidget( int value );

    /**
     * @brief  Retrieve Self Service 2021 Refresh (PJR-383) enabled or not.
     * @return  @c true if enabled, @c false otherwise
     */
    virtual void setDisplayBarcodeOnUserInterface( int value );

    /**
     * @brief  Display the number pad text.
     * @param[in] value  title
     */
    //virtual void displayText( bool value );

    /**
     * @brief  Display instructions.
     * @param[in] value  title
     */
    //virtual void displayInstructions( bool value );

    /**
     * @brief  Set input validator.
     * @param[in] value  validator
     */
    virtual void setValidator( const QIntValidator *value ) {validator_ = value;}

protected:

    // ************************************************************************
    // Events
    // ************************************************************************

    /**
     * @brief Resize event.
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

private Q_SLOTS:

    /// Slot to handle button clicked.
    void onButtonClicked( QWidget *w );

private:

    QString style_;
    QString barcodeFile_;

    int enterAction_;
    bool autoLookupProduct_;
    int autoLookupNumOfDigits_;
    int autoLookupOverrideDigit_;
    int stackIndex_;
    int displayBarcode_;
    QString operatorID_;

    QTextEdit *instructions_;
    hobart::numericLineEdit *lineEdit_;
    QStackedWidget *stacked_;
    QStackedWidget *stackedNums_;

    QWidget *buttonsArea_;
    hobart::pushButton *clear_;
    hobart::pushButton *enter_;

    ///////////////////////////////////////////
    QWidget *scanAndPrint_;
    QHBoxLayout *layBarcodeOnly_;
    QWidget *layoutWidget1_;
    QVBoxLayout *layPrintAndScan_;
    QSpacerItem *topSpacer_;
    QLabel *lblScanBarcode_;
    QWidget *barcodeOnly_;
    QSpacerItem *bottomSpacer_;
    hobart::pushButton *bigPrintKey_;

    QSignalMapper *buttonSignalMapper_;

    hobart::pushButton *printLabel_;

    typedef QMap<QString, hobart::pushButton*> buttonMap;
    buttonMap buttons_;

    const QIntValidator *validator_;

    bool selfServicePreventClicks_;

    // not implemented
    selfServiceNumberPad( const _Myt& rhs );

    // not implemented
    _Myt& operator = ( const _Myt& rhs );

    /// Initialize object.
    void initialize();

    /// find the screen dimensions
    int checkScreenSize();

    /// check 10 inch or 14.
    int checkScreenHeight();

    /**
     * Render label image to display.
     */
    void renderLabelImage();

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SELFSERVICENUMBERPAD_H
