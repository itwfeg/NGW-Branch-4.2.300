/**
 * @file keyboardinputwidget.h
 * Virtual keyboard widget.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef KEYBOARDINPUTWIDGET_H
#define KEYBOARDINPUTWIDGET_H

#include "inputwidget.h"

#include <QMap>

class QPushButton;
class QSignalMapper;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Virtual keyboard widget.
class KeyboardInputWidget : public InputWidget
{
    Q_OBJECT

    Q_PROPERTY( LayoutStyle layoutStyle READ layoutStyle WRITE setLayoutStyle )
    Q_ENUMS( LayoutStyle )

    typedef KeyboardInputWidget _Myt;
    typedef InputWidget _Mybase;

public:

    /// Number pad key layout styles.
    enum LayoutStyle
    {
        CalculatorLayoutStyle,                      /// 7,8,9 keys on top
        TelephoneLayoutStyle                        /// 1,2,3 keys on top
    };

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    explicit KeyboardInputWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~KeyboardInputWidget();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve layout style.
    /**
     * @return  layout style
     */
    virtual LayoutStyle layoutStyle() const {return layoutStyle_;}

    /// Set layout style.
    /**
     * @param[in] value  layout style
     */
    virtual void setLayoutStyle( LayoutStyle value );

    /// Set the widget locale.
    /**
     * @param[in] locale  locale to set
     */
    virtual void setLocale( const QLocale &locale );

public Q_SLOTS:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Activate input widget.
    /**
     * @param[in] widget  focus widget
     * @param[in] altLocation  use alternate location, if possible (i.e. top/bottom of screen)
     */
    virtual void invoke( QWidget *widget, bool altLocation = false );

private Q_SLOTS:

    /// Slot to handle key click.
    void onKeyClicked( QWidget *widget );

private:

    typedef QMap<QString, QPushButton*> pushButtonMap;

    QSignalMapper *signalMapper_;

    LayoutStyle layoutStyle_;

    pushButtonMap keys_;

    bool capsActive_;
    int specialCharsMode_;

    static const int specialCharsModeCount_;

    static const QSize fixedSize_;
    static const int fontPointSize_;

    static const QSize buttonIconSize_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Groom.
    void groom();

    /// Update keyboard with current key set.
    void updateKeys();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // KEYBOARDINPUTWIDGET_H
