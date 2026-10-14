/**
 * @file selfservicebuttonbar.h
 * Self service button bar.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup selfServeCsu
 * @{
 */

#ifndef SELFSERVICEBUTTONBAR_H
#define SELFSERVICEBUTTONBAR_H

#include <QMap>
#include <QWidget>

namespace hobart {
    class pushButton;
}

class Session;
class QPushButton;
class QSpacerItem;
class QSignalMapper;

class selfServiceHook;

class ScaleConfiguration;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Self service button bar.
class selfServiceButtonBar : public QWidget
{
    Q_OBJECT

    typedef selfServiceButtonBar _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /// Emitted when cancel key pressed.
    void cancel();

    /// Emitted when enter key pressed.
    void enter();

    /// Emitted when okay key pressed.
    void okay();

public:

    /// Possible buttons types.
    enum showButtonTypes
    {
        none                    = 0,                ///< No buttons.
        showItemEntryKeyed      = 0x0001,           ///< Item lookup button.
        showItemEntryImages     = 0x0002,           ///< Item search button.
        showToggleLanguage      = 0x0004,           ///< Toggle language button.
        showRezero              = 0x0008,           ///< Rezero weigher button.
        showStartOver           = 0x0010,           ///< Start over button.
        showEnter               = 0x0020,           ///< Enter button. - NOT USED
        showPrintLabel          = 0x0040,           ///< Print label button.
        showCancel              = 0x0080,           ///< Cancel button.
        showOkay                = 0x0100,           ///< Okay button.
        showKeypad              = 0x0200,           ///< keypad
        showBack                = 0x0400,           ///< Back Button

        // max types
        MAX_TYPES               = 0x0800            ///< Not a type! Maximum number of types.
    };

    // ************************************************************************
    // CTOR / DTOR
    // ************************************************************************

    /**
     * @brief CTOR.
     * @param[in] h  pointer to hook
     * @param[in] parent  parent widget
     */
    selfServiceButtonBar( selfServiceHook *h, QWidget *parent = NULL );

    /// DTOR
    virtual ~selfServiceButtonBar();

    // ************************************************************************
    // Methods
    // ************************************************************************

    /// Refresh widget.
    virtual void refresh();

    /// we only want to every the back button, on one condition.
    virtual void setBackButtonOn();

    /**
     * @brief  Show buttons.
     * @param[in] value  buttons to show
     */
    virtual void showButtons( showButtonTypes value );

    /// Translate to current language.
    virtual void translate();

private Q_SLOTS:

    /// Slot to handle button clicked.
    void onButtonClicked( QWidget *w );

private:

    selfServiceHook *hook_;
    Session *s;
    ScaleConfiguration *scaleConfig_;

    hobart::pushButton *toggleLanguage_;
    hobart::pushButton *rezero_;

    hobart::pushButton *itemEntryKeyed_;
    hobart::pushButton *itemEntryImages_;

    hobart::pushButton *startOver_;
    hobart::pushButton *back_;
    hobart::pushButton *printLabel_;

    hobart::pushButton *cancel_;
    hobart::pushButton *okay_;

    hobart::pushButton *keypad_;

    QSpacerItem *buttonSpacer_;

    typedef QMap<showButtonTypes, QWidget*> buttonTypeMap;
    buttonTypeMap buttonTypes_;

    int buttonTypesMask_;

    QSignalMapper *buttonSignalMapper_;
    QWidget *buttons_;

    // not implemented
    selfServiceButtonBar( const _Myt& rhs );

    // not implemented
    _Myt& operator = ( const _Myt& rhs );

    /// Initialize object.
    void initialize();

    /// check 10 inch or 14.
    int checkScreenSize();

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SELFSERVICEBUTTONBAR_H
