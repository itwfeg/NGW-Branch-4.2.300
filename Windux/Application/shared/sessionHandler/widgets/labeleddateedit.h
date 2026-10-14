/**
 * @file labeleddateedit.h
 * Date edit widget with custom background, highlighting, and label.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef LABELEDDATEEDIT_H
#define LABELEDDATEEDIT_H

#include "labeledlineedit.h"

#include <QDate>

class SessionMainWindow;

class QTimer;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class labeledDateEdit
 * @brief Date edit widget with custom background, highlighting, and label.
 */
class labeledDateEdit : public labeledLineEdit
{
    Q_OBJECT

    Q_PROPERTY( QDate date READ date WRITE setDate NOTIFY dateChanged )
    Q_PROPERTY( bool doNotPrint READ doNotPrint WRITE setDoNotPrint NOTIFY doNotPrintChanged )
    Q_PROPERTY( int hoursModifier READ hoursModifier WRITE setHoursModifier )

    typedef labeledDateEdit _Myt;
    typedef labeledLineEdit _Mybase;

Q_SIGNALS:

    /**
     * Signal for when date changes.
     */
    void dateChanged();

    /**
     * Signal for operator done editing.
     *
     * @param[in] rc  result code
     */
    void dialogActionComplete( int rc );

    /**
     * Signal for when do not print changes.
     */
    void doNotPrintChanged();

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     * @param[in] image  background image for the widget
     */
    explicit labeledDateEdit(
        QWidget *parent = 0,
        const QPixmap& image = QPixmap() );

    /**
     * Constructor.
     *
     * @param[in] text  label text
     * @param[in] parent  parent widget
     * @param[in] image  background image for the widget
     */
    explicit labeledDateEdit(
        const QString& text,
        QWidget *parent = 0,
        const QPixmap& image = QPixmap() );

    /**
     * Destructor.
     */
    virtual ~labeledDateEdit();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve date.
     *
     * @returns  date value
     */
    virtual QDate date() const {return date_;}

    /**
     * Retrieve do not print.
     *
     * @returns  @c true to when do not print set, @c false otherwise
     */
    virtual bool doNotPrint() const {return doNotPrint_;}

    /**
     * Check if widget has focus.
     *
     * @returns  @c true if focused, @c false otherwise
     */
    virtual bool hasFocus() const {return (( dialogOpen_ ) || ( _Mybase::hasFocus() ));}

    /**
     * Retrieve the hours modifier.
     *
     * @returns  hours
     */
    virtual int hoursModifier() const {return hoursModifier_;}

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Pops input dialog for setting date.
     */
    virtual void promptForInput();

    /**
     * Translate the widget fields.
     */
    virtual void translate();

    ///////////////////////////////////////////////////////////////////////////
    // Static methods

    /**
     * Convert date to string using date style.
     *
     * @param[in] date  date to convert
     * @param[in] dateStyle  date style
     * @returns  date in string format
     */
    static QString convertDateToString( const QDate &date, int dateStyle );

    /**
     * Convert time to string using time style.
     *
     * @param[in] time  time to convert
     * @param[in] timeStyle  time style
     * @returns  time in string format
     */
    static QString convertTimeToString( const QTime &time, int timeStyle );

    /**
     * Convert date and time to string using style.
     *
     * @param[in] datetime  datetime to convert
     * @param[in] dateStyle  date style
     * @param[in] timeStyle  time style
     * @returns  datetime in string format
     */
    static QString convertDateTimeToString( const QDateTime &datetime, int dateStyle, int timeStyle );

public Q_SLOTS:

    /**
     * Clear control.
     */
    virtual void clear();

    /**
     * Set backward dating.
     *
     * @param[in] value  @c true if backward dating, @c false otherwise
     */
    virtual void setBackDating( bool value ) {backward_ = value;}

    /// Set basis date, used for backdating.
    /**
     * @param[in] value  basis date
     */
    virtual void setBasisDate( const QDate& value ) {basisDate_ = value;}

    /**
     * Set date.
     *
     * @param[in] value  date to set
     */
    virtual void setDate( const QDate& value );

    /**
     * Set do not print.
     *
     * @param[in] value  @c true to set do not print, @c false otherwise
     */
    virtual void setDoNotPrint( bool value );

    /**
     * Set do not print enabled.
     *
     * @param[in] value  @c true to enable, @c false otherwise
     */
    virtual void setDoNotPrintEnabled( bool value );

    /**
     * Set the hours modifier. This will be shown to the operator if the widget is edited.
     *
     * @param[in] value  hours
     */
    virtual void setHoursModifier( int value ) {hoursModifier_ = value;}

    /**
     * Set max days.
     *
     * @param[in] value  max days
     */
    virtual void setMaxDays( int value ) {daysMax_ = value;}

protected:

    /**
     * Event filter.
     *
     * @param[in,out] obj  object event filter was attached to
     * @param[in,out] event  event that is occuring
     * @returns  @c true if event @p event should be filtered, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *event );

    /**
     * Mouse press event.
     *
     * @param[in,out] event  mouse event
     */
    virtual void mousePressEvent( QMouseEvent *event );

private Q_SLOTS:

    /**
     * Slot for selection changed.
     */
    void onSelectionChanged();

    /**
     * Slot for timer timeout.
     */
    void onTimeout();

private:

    QDate date_;
    bool dialogOpen_;

    QDate basisDate_;

    bool doNotPrint_;
    bool doNotPrintEnabled_;

    int hoursModifier_;
    int daysMax_;

    bool backward_;

    QTimer *timer_;

    SessionMainWindow *mainWindow_;

    /**
     * Initialize.
     */
    void initialize();

    /**
     * Create string from date in current locale.
     */
    QString toString( const QDate& date );

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // LABELEDDATEEDIT_H
