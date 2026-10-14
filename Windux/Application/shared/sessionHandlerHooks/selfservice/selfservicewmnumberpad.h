/**
 * @file selfservicewmnumberpad.h
 * Self service weights and measures number pad.
 * Really a complete copy of selfServiceNumberPad
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Joe Crew (via Randy Blankley)
 *
 * @addtogroup selfServeCsu
 * @{
 */

#ifndef SELFSERVICEWMNUMBERPAD_H
#define SELFSERVICEWMNUMBERPAD_H

#include <QMap>
#include <QWidget>
#include <QStackedWidget>

class QIntValidator;
class QLabel;
class QTextEdit;
class QSignalMapper;

namespace hobart {
    class numericLineEdit;
    class pushButton;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Self service weights and measures number pad.
class selfServiceWmNumberPad : public QWidget
{
    Q_OBJECT

    typedef selfServiceWmNumberPad _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /**
     * @brief  Emitted when text changed.
     * @param[in] text  text
     */
    void textChanged( const QString& text );

    /// Emitted when auto lookup is triggered.
    void lookup();

public:

    // ************************************************************************
    // CTOR / DTOR
    // ************************************************************************

    /**
     * @brief CTOR.
     * @param[in] parent  parent widget
     */
    selfServiceWmNumberPad( QWidget *parent = NULL );

    /// DTOR
    virtual ~selfServiceWmNumberPad();

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
     * @brief  Display the number pad or instructions.
     * @param[in] value  item to display
     */
    virtual void displayStackedWidget( int value );

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

    bool autoLookupProduct_;
    int autoLookupNumOfDigits_;
    int autoLookupOverrideDigit_;

    QTextEdit *instructions_;
    //QLabel *instructions_;
    //hobart::numericLineEdit *instructions_;
    hobart::numericLineEdit *lineEdit_;
    QStackedWidget *stacked_;

    QWidget *buttonsArea_;

    hobart::pushButton *clear_;

    typedef QMap<QString, hobart::pushButton*> buttonMap;
    buttonMap buttons_;

    QSignalMapper *buttonSignalMapper_;

    const QIntValidator *validator_;

    // not implemented
    selfServiceWmNumberPad( const _Myt& rhs );

    // not implemented
    _Myt& operator = ( const _Myt& rhs );

    /// Initialize object.
    void initialize();

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SELFSERVICEWMNUMBERPAD_H
