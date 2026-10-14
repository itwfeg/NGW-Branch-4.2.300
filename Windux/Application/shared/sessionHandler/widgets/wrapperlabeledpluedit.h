/**
 * @file wrapperlabeledpluedit.h
 * Plu field with support for wrapper modes.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef WRAPPERLABELEDPLUEDIT_H
#define WRAPPERLABELEDPLUEDIT_H

#include "labelednumericlineedit.h"

#include <QMap>
#include <QPair>

class DomWeighModeTotalType;
class MadpPeripheralList;

/// Hobart namespace.
namespace hobart {

class wrapperModeWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Plu field with support for wrapper modes.
class wrapperLabeledPluEdit : public labeledNumericLineEdit
{
    Q_OBJECT

    typedef wrapperLabeledPluEdit _Myt;
    typedef labeledNumericLineEdit _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    //// Constructor.
    /**
     * @param[in] parent  parent widget
     * @param[in] image  background image for the widget
     */
    explicit wrapperLabeledPluEdit(
        QWidget *parent = 0,
        const QPixmap& image = QPixmap() );

    /// Destructor.
    virtual ~wrapperLabeledPluEdit();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set if mode button is visible.
    /**
     * @param[in] value  @c true for visible, @c false for otherwise
     */
    virtual void setModeButtonVisible( bool value );

    /// Set if mode updates are enabled.
    /**
     * When mode updates are enabled the wrapper operate mode will be automatically set when
     * peripherals are gained or lost.
     *
     * @param[in] value  @c true to enable, @c false otherwise
     */
    virtual void setModeUpdateEnabled( bool value );

    /// Set if control is read only.
    /**
     * @param[in] value  @c true for read only, @c false for otherwise
     */
    virtual void setReadOnly( bool value );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate the widget fields.
    virtual void translate();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] e  resize event
     */
    virtual void resizeEvent( QResizeEvent* e );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Update wrapper mode.
    virtual void updateWrapperMode();

private Q_SLOTS:

    /// Slot to handle mode button pressed.
    void onModeButtonPressed( int currentState );

    /// Slot to handle session messages.
    void onSessionMessage( const SessionMessage& msg );

private:

    typedef QMap<int, int> modeIndicatorMap;
    typedef QMap<int, QPixmap> modeBackgroundMap;

    SessionMainWindow *mainWindow_;
    Session *session_;

    wrapperModeWidget *modeButton_;

    bool modeButtonVisible_;
    bool modeUpdateEnabled_;

    modeIndicatorMap modeIndicators_;

    modeBackgroundMap modeBackgrounds_;
    modeBackgroundMap modeBackgroundsFocused_;

    MadpPeripheralList *peripheralList_;
    MadpPeripheralList *prevPeripheralList_;

    DomWeighModeTotalType *weighModeTotalType_;

    // ========================================================================

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Update mode button size.
    void updateModeButtonSize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // WRAPPERLABELEDPLUEDIT_H
