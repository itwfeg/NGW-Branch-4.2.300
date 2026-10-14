#ifndef OFFLINEERRORDIALOG_H
#define OFFLINEERRORDIALOG_H

#include <QDialog>
#include <QString>

class QPushButton;
class QTimer;

/// Offline Error Dialog
class OfflineErrorDialog : public QDialog
{
    Q_OBJECT

    typedef OfflineErrorDialog _Myt;
    typedef QDialog _Mybase;

public:

    // ------------------------------------------------------------------------
    // CTOR
    // ------------------------------------------------------------------------

    /// Constructor.
    /**
     * @param[in,out] parent  parent widget
     * @param[in] f  window flags
     */
    OfflineErrorDialog( QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags() );

    // ------------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------------

    /// Set accept text.
    /**
     * @param[in] value  text
     */
    void setErrorAcceptText( const QString& value ) {acceptText_ = value;}

    /// Set background color.
    /**
     * @param[in] value  color in hexadecimal
     */
    void setErrorBackgroundColor( const QString& value );

    /// Set font family.
    /**
     * @param[in] value  font family name
     */
    void setErrorFontFamily( const QString& value );

    /// Set font point size.
    /**
     * @param[in] value  font point size
     */
    void setErrorFontPointSize( int value );

    /// Set foreground color.
    /**
     * @param[in] value  color in hexadecimal
     */
    void setErrorForegroundColor( const QString& value );

    /// Set dialog height.
    /**
     * @param[in] value  height
     */
    void setErrorHeight( int value ) {form_->setFixedHeight( value );}

    /// Set error message.
    /**
     * @param[in] value  message
     */
    void setErrorMessage( const QString& value ) {msg_ = value;}

    /// Set reject text.
    /**
     * @param[in] value  text
     */
    void setErrorRejectText( const QString& value ) {rejectText_ = value;}

    /// Set dialog width.
    /**
     * @param[in] value  width
     */
    void setErrorWidth( int value ) {form_->setFixedWidth( value );}

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Capture desktop as background.
    void capturePrimaryScreen();

    /// Generate error dialog form.
    void generateForm();

public slots:

    /// Force window to top of stack.
    void forceTopWindow();

protected:

    /// Show event.
    /**
     * @param[in,out] e  event
     */
    void showEvent( QShowEvent *e ) override;

private slots:

    /// Slot for button clicked.
    void onButtonClicked();

private:

    enum {RAISE_WINDOW_TIMER = 2500};

    QWidget *form_;

    QPushButton *accept_;
    QPushButton *reject_;

    QString acceptText_;
    QString rejectText_;
    QString msg_;

    QTimer *t_;

    /// Initialize widget.
    void initialize();

    /// Create widget layout.
    void createLayout();

    // not implemented
    _Myt& operator=( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // OFFLINEERRORDIALOG_H
