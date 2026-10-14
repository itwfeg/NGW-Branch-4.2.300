/**
 * @file pluoperatescreen.h
 * Base class for operate screen with PLU entry.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef PLUOPERATESCREEN_H
#define PLUOPERATESCREEN_H

#include "screenwidget.h"

namespace hobart {
class labeledNumericLineEdit;
class labeledPluEdit;
}

class OperateMode;

class DomCurrentPLU;

class QIntValidator;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Base class for operate screen with PLU entry.
class PluOperateScreen : public ScreenWidget
{
    Q_OBJECT

    typedef PluOperateScreen _Myt;
    typedef ScreenWidget _Mybase;

public:

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~PluOperateScreen();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set forced tray entry.
    /**
     * @param[in] value  @c true if forced, @c false otherwise
     */
    virtual void setForcedTrayEntry( bool value );

    /// Set tray name.
    /**
     * @param[in] name  name
     */
    virtual void setTrayName( const QString& name );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate the user interface to current language.
    virtual void translate();

protected:

    OperateMode *operateMode_;                      ///< Pointer to operate mode.

    hobart::labeledPluEdit *plu_;		            ///< Product entry widget.		FT-1466
    QIntValidator *pluValidator_;                   ///< Validator for product entry widget.

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit PluOperateScreen( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve current product.
    /**
     * @return  pointer to product record.
     */
    DomCurrentPLU *currentProduct() const;

    /// Set current item number.
    /**
     * @param[in] value  item number
     */
    virtual void setCurrentItemNumber( const QString& value );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private Q_SLOTS:

    /// Slot for PLU data changed.
    void onCurrentProductChanged( bool nof );

    /// Slot to handle plu entry.
    void onPluEditingFinished();

    /// Slot to handle plu return pressed.
    void onPluReturnPressed();

    /// Slot to handle plu text editing.
    void onPluTextEdited( const QString& text );

private:

    bool ignoreSignals_;
    bool hasRemoteLic;

    QString currentItemNumber_;

    static const int pluValidatorBottom_;
    static const int pluValidatorTop_;

    /// Initialize.
    void initialize();

    /// Update plu auto lookup.
    void updatePluAutoLookup();

    /// Convert number of digits into a top range integer.
    static int makeIntValidatorTop( int digits );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // PLUOPERATESCREEN_H
