/**
 * @file errormessagewidget.h
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef ERRORMESSAGEWIDGET_H
#define ERRORMESSAGEWIDGET_H

#include <QString>
#include <QWidget>

class QLabel;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Widget to display error messages.
class errorMessageWidget : public QWidget
{
    Q_OBJECT

    Q_PROPERTY( QString errorText READ errorText WRITE setErrorText )
    Q_PROPERTY( bool errorTextVisible READ isErrorTextVisible WRITE setErrorTextVisible )
    Q_PROPERTY( QString text READ text WRITE setText )

    typedef errorMessageWidget _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    errorMessageWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~errorMessageWidget();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve error text.
    /**
     * @return  error text
     */
    virtual QString errorText();

    /// Retrieve error text visbile.
    /**
     * @return  @c true if visible, @c false otherwise
     */
    virtual bool isErrorTextVisible() const;

    /// Retrieve text.
    /**
     * @return  text
     */
    virtual QString text() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate widgets.
    virtual void translate();

public Q_SLOTS:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set error text.
    /**
     * @param[in] value  error text
     */
    virtual void setErrorText( const QString& value );

    /// Set error text visible.
    /**
     * @param[in] value  @c true for visible, @c false otherwise
     */
    virtual void setErrorTextVisible( bool value );

    /// Set text.
    /**
     * @param[in] value  text
     */
    virtual void setText( const QString& value );

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

private:

    QWidget *icon_;

    QLabel *text_;
    QLabel *errorText_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Adjust font point sizes after layout or text changs.
    void adjustFontPointSize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ERRORMESSAGEWIDGET_H
