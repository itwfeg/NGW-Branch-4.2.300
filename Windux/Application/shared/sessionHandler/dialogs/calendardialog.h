/**
 * @file calendardialog.h
 * Calendar dialog for choosing a date.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef CALENDARDIALOG_H
#define CALENDARDIALOG_H

#include "dcadialog.h"

#include <QDate>
#include <QMap>

class QCheckBox;
class QIntValidator;
class QLabel;
class QLineEdit;
class QPushButton;
class QSignalMapper;
class QString;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Calendar dialog for choosing a date.
class CalendarDialog : public DynamicContentAreaDialog
{
    Q_OBJECT

    Q_ENUMS( Direction )
    Q_PROPERTY( QDate date READ date WRITE setDate )
    Q_PROPERTY( int days READ days WRITE setDays )
    Q_PROPERTY( Direction daysDirection READ daysDirection WRITE setDaysDirection )
    Q_PROPERTY( int daysMax READ daysMax WRITE setDaysMax )
    Q_PROPERTY( int daysMin READ daysMin WRITE setDaysMin )
    Q_PROPERTY( bool doNotPrint READ doNotPrint WRITE setDoNotPrint )
    Q_PROPERTY( bool doNotPrintVisible READ doNotPrintVisible WRITE setDoNotPrintVisible )
    Q_PROPERTY( QDate rangeMax READ rangeMax WRITE setRangeMax )
    Q_PROPERTY( QDate rangeMin READ rangeMin WRITE setRangeMin )
    Q_PROPERTY( bool required READ isRequired WRITE setRequired )
    Q_PROPERTY( QString title READ title WRITE setTitle )

    typedef CalendarDialog _Myt;
    typedef DynamicContentAreaDialog _Mybase;

public:

    /// Time directions.
    /**
     * When setting days, this represents if days is backward in time or foward.
     */
    enum Direction
    {
        Backward,                                   ///< Adjust days backward from today (i.e. backdating)
        Forward                                     ///< Adjust days forward from today.
    };

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit CalendarDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Constructor.
    /**
     * @param[in] date  date calendar should be set to initially
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit CalendarDialog( const QDate& date, QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Constructor.
    /**
     * @param[in] days  number of days from today calendar should be set to initially
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit CalendarDialog( int days, QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~CalendarDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Whether or not clicking the overlay rejects the dialog.
    /**
     * @return  @c true if allowed, @c false otherwise
     */
    virtual bool allowOverlayClickDismiss() const {return !isRequired();}

    /// Retrieve dialog date.
    /**
     * @return  dialog date
     */
    virtual QDate date() const {return selectedDate_;}

    /// Retrieve number of days selected date is from today.
    /**
     * @return  number of days
     */
    virtual int days() const;

    /// Retrieve which direction days is set to.
    /**
     * @return  direction
     */
    virtual Direction daysDirection() const {return daysDirection_;}

    /// Retrieve max amount days may be set to.
    /**
     * @return  days amount
     */
    virtual int daysMax() const;

    /// Retrieve min amount days may be set to.
    /**
     * @return  days amount
     */
    virtual int daysMin() const;

    /// Retrieve do not print radio button state.
    /**
     * @return  @c true if do not print was selected, @c false otherwise
     */
    virtual bool doNotPrint() const;

    /// Retrieve do not print radio button visibility.
    /**
     * @return  @c true if visible, @c false otherwise
     */
    virtual bool doNotPrintVisible() const;

    /// Retrieve required.
    /**
     * @return  @c true if required, @c false otherwise
     */
    virtual bool isRequired() const {return required_;}

    /// Check if selected date is valid and within range.
    /**
     * @return  @c true if valid, @c false otherwise
     */
    virtual bool isValidDate() const {return isValidDate( selectedDate_ );}

    /// Check if date @p date is valid and within range.
    /**
     * @return  @c true if valid, @c false otherwise
     */
    virtual bool isValidDate( const QDate& date ) const;

    /// Retrieve the highest possible date that may be selected.
    /**
     * @return  date
     */
    virtual QDate rangeMax() const;

    /// Retrieve the lowest possible date that may be selected.
    /**
     * @return  date
     */
    virtual QDate rangeMin() const;

    /// Set basis date, used for backdating.
    /**
     * @param[in] value  basis date
     */
    virtual void setBasisDate( const QDate& value ) {now_ = value;}

    /// Set if the calendar is enabled.
    /**
     * @param[in] value  @c true to enable the calendar, @c false otherwise
     */
    virtual void setCalendarEnabled( bool value ) {calendarEnabled_ = value;}

    /// Set the dialog date.
    /**
     * @param[in] value  dialog date
     */
    virtual void setDate( const QDate& value );

    /// Set the dialog date using day count.
    /**
     * @param[in] value  number of days from today
     */
    virtual void setDays( int value );

    /// Set days direction.
    /**
     * @param[in] value  direction
     */
    virtual void setDaysDirection( Direction value );

    /// Set days range max amount.
    /**
     * @param[in] value  days
     */
    virtual void setDaysMax( int value );

    /// Set days range min amount.
    /**
     * @param[in] value  days
     */
    virtual void setDaysMin( int value );

    /// Set do not print radio button state.
    /**
     * @param[in] value  @c true to set do not print, @c false otherwise
     */
    virtual void setDoNotPrint( bool value );

    /// Set do not print radio button visibility.
    /**
     * @param[in] value  @c true to make do not print visible, @c false otherwise
     */
    virtual void setDoNotPrintVisible( bool value );

    /// Set the hours modifier. This will be shown to the operator if the widget is edited.
    /**
     * @param[in] value  hours
     */
    virtual void setHoursModifier( int value );

    /// Set the highest possible date that may be selected.
    /**
     * @param[in] value  date
     */
    virtual void setRangeMax( const QDate& value );

    /// Set the lowest possible date that may be selected.
    /**
     * @param[in] value  date
     */
    virtual void setRangeMin( const QDate& value );

    /// Set required.
    /**
     * @return  @c true for required, @c false otherwise
     */
    virtual void setRequired( bool value ) {required_ = value;}

    /// Set dialog title.
    /**
     * @param[in] value  dialog title
     */
    virtual void setTitle( const QString& value );

    /// Retrieve title of dialog.
    /**
     * @return  dialog title
     */
    virtual QString title() const;

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Show event.
    /**
     * @param[in,out] event  show event
     */
    virtual void showEvent( QShowEvent *event );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve priority of this dialog.
    /**
     * When showing a high priority dialog all dialogs of lower priority will be automatically
     * closed.
     *
     * @return  priority of this dialog
     */
    virtual int priority() const {return Low;}

    /// Retrieve renderer appearance.
    /**
     * @return  appearance object
     */
    virtual hobart::widgetRendererAppearanceBase *rendererAppearance() const;

    /// Retrieve renderer style.
    /**
     * @return  style
     */
    virtual int rendererStyle() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Event filter.
    /**
     * @param[in,out] obj  object event filter was attached to
     * @param[in,out] event  event that is occuring
     * @return  @c true if event @p event should be filtered, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *event );

private Q_SLOTS:

    /// Slot for date button pressed.
    void onDateClicked( QWidget* widget );

    /// Slot for when days edited.
    void onDaysTextEdited( const QString& value );

    /// Signal for when input is accepted.
    void onDaysTextEditingFinished();

    /// Signal for when do not print is toggled.
    void onDoNotPrintToggled( bool value );

    /// Slot for previous month pressed.
    void onPrevMonthClicked();

    /// Slot for next month pressed.
    void onNextMonthClicked();

private:

    typedef QMap<QString, QPushButton*> pushButtonMap;
    typedef QMap<QString, QLabel*> labelMap;

    QDate now_;
    QDate original_;

    QDate selectedDate_;
    bool calendarEnabled_;

    bool required_;

    Direction daysDirection_;
    QIntValidator *daysValidator_;

    QLabel *month_;
    QPushButton *prevMonth_;
    QPushButton *nextMonth_;

    QLabel *title_;
    QLabel *date_;

    QLineEdit *days_;
    QLabel *hoursModifier_;
    QCheckBox *doNotPrint_;

    labelMap labels_;

    pushButtonMap buttons_;
    QSignalMapper *signalMapper_;

    bool ignoreSignals_;

    static const bool convertUppercase_;

    static const int dateMaxDays_;
    static const int doNotPrintMagic_;

    /// Initialize.
    void initialize();

    /// Update the calendar controls to selected date.
    void updateCalendar();

    /// Create string from day number in current locale.
    static QString longDayName( int value );

    /// Create string from month number in current locale.
    static QString longMonthName( int value );

    /// Create string from day number in current locale.
    static QString shortDayName( int value );

    /// Create string from month number in current locale.
    static QString shortMonthName( int value );

    /// Create string from date in current locale.
    static QString toString( const QDate& date, const QString& format = QString( "MMM d, yy" ) );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // CALENDARDIALOG_H
