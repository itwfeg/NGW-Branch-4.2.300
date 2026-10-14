/**
 * @file labeledpluedit.h
 * Plu field with support for total types and wrapper modes.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef LABELEDPLUEDIT_H
#define LABELEDPLUEDIT_H

#include "labelednumericlineedit.h"

#include <QMap>
#include <QPair>

class DomWeighModeTotalType;
class MadpPeripheralList;

/// Hobart namespace.
namespace hobart {

class wrapperModeWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Plu field with support for total types and wrapper modes.
class labeledPluEdit : public labeledNumericLineEdit
{
    Q_OBJECT

    typedef labeledPluEdit _Myt;
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
    explicit labeledPluEdit(
        QWidget *parent = 0,
        const QPixmap& image = QPixmap(),
        bool forWrapper = false );

    /// Destructor.
    virtual ~labeledPluEdit();

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

    /// Retrieve tray type visibility.
    /**
     * @return  @c true if tray type is visible, @c false otherwise
     */
    virtual bool isTrayTypeVisible() const {return trayTypeVisible_;}

    /// Retrieve total type visibility.
    /**
     * @return  @c true if total type is visible, @c false otherwise
     */
    virtual bool isTotalTypeVisible() const {return totalTypeVisible_;}

    /// Set font of label and total type.
    /**
     * @param[in] value  font
     */
    virtual void setLabelFont( const QFont& value );

    /// Set tray type object name.
    /**
     * @param[in] value  text value to set
     */
    virtual void setTrayTypeObjectName( const QString& value );

    /// Set if tray type is forced.
    /**
     * @param[in] value  @c true for forced, @c false for otherwise
     */
    virtual void setTrayTypeForced( bool value );

    /// Set if tray type is visible.
    /**
     * @param[in] value  @c true for visible, @c false for otherwise
     * @param[in] text  text to display
     */
    virtual void setTrayTypeVisible( bool value, const QString& text = QString() );

    /// Set total type object name.
    /**
     * @param[in] value  text value to set
     */
    virtual void setTotalTypeObjectName( const QString& value );

    /// Set if total type is visible.
    /**
     * @param[in] value  @c true for visible, @c false for otherwise
     */
    virtual void setTotalTypeVisible( bool value );

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

    /// Update total type name.
    void updateTotalTypeName();

private Q_SLOTS:

    /// Slot to handle mode button pressed.
    void onModeButtonPressed( int currentState );

    /// Slot to handle session messages.
    void onSessionMessage( const SessionMessage& msg );

private:
    enum CLAModels
    {
        NoModel,
        NoModel_1,
        CLAS,
        CLAS_1RLB,
        CLAS_1LRB
    };

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

    QVBoxLayout *trayTypeLayout_;

    QLabel *trayType_;

    QVBoxLayout *totalTypeLayout_;

    QLabel *totalType_;

    bool trayTypeVisible_;
    bool totalTypeVisible_;
    bool showApplyWrapLabel_;

    QString trayTypeText_;

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

#endif // LABELEDPLUEDIT_H
