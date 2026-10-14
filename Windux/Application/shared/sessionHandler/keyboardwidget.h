/**
 * @file keyboardwidget.h
 * Virtual keyboard widget.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef KEYBOARDWIDGET_H
#define KEYBOARDWIDGET_H

#include "inputwidget.h"

#include <QMap>

namespace hobart {
    class pushButton;
}

class QSignalMapper;
class QSpacerItem;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Virtual keyboard widget.
class KeyboardWidget : public InputWidget
{
    Q_OBJECT

    Q_ENUMS( KeyLayoutStyle )
    Q_PROPERTY( KeyLayoutStyle keyLayoutStyle READ keyLayoutStyle WRITE setKeyLayoutStyle )

    typedef KeyboardWidget _Myt;
    typedef InputWidget _Mybase;

public:

    /// Number pad key layout styles.
    enum KeyLayoutStyle
    {
        CalculatorKeyLayoutStyle,                   /// 7,8,9 keys on top
        TelephoneKeyLayoutStyle                     /// 1,2,3 keys on top
    };

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    explicit KeyboardWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~KeyboardWidget();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve key layout style.
    /**
     * @return  key layout style
     */
    virtual KeyLayoutStyle keyLayoutStyle() const {return keyLayoutStyle_;}

    /// Set key layout style.
    /**
     * @param[in] value  key layout style
     */
    virtual void setKeyLayoutStyle( KeyLayoutStyle value );

    /// Set the widget locale.
    /**
     * @param[in] locale  locale to set
     */
    virtual void setLocale( const QLocale& locale );

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,ou] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

private Q_SLOTS:

    /// Slot to handle key click.
    void onKeyClicked( QWidget *widget );

private:

    typedef QMap<QString, hobart::pushButton*> pushButtonMap;

    pushButtonMap keys_;
    QSignalMapper *signalMapper_;

    KeyLayoutStyle keyLayoutStyle_;

    QSpacerItem *leftSpacer_;
    QSpacerItem *rightSpacer_;

    bool capsActive_;
    int specialCharsMode_;

    static const int specialCharsModeCount_;

    static const QSize buttonIconSize_;
    static const int buttonFontPointSize_;

    static const int layoutSpacing_;

    /// Initialize widget.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Update keyboard with current key set.
    void updateKeys();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // KEYBOARDWIDGET_H
