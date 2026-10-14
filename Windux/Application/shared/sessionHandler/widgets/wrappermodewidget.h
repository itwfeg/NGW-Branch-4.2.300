/**
 * @file wrappermodewidget.h
 * Widget for displaying the wrapper mode.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef WRAPPERMODEWIDGET_H
#define WRAPPERMODEWIDGET_H

#include <QWidget>

class QLabel;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

//// Widget for displaying the wrapper mode.
class wrapperModeWidget : public QWidget
{
    Q_OBJECT

    typedef wrapperModeWidget _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /// Signal for when widget is pressed.
    /**
     * @param[in] currentState  current indicator state
     */
    void pressed( int currentState );

public:

    /// Indicator flags.
    enum indicatorFlags
    {
        None            = 0x0000,                   ///< None.
        Apply           = 0x0001,                   ///< Apply.
        Label           = 0x0002,                   ///< Label.
        Wrap            = 0x0004,                   ///< Wrap.
        ApplyHidden     = 0x0100,                   ///< Apply is hidden.
        LabelHidden     = 0x0200,                   ///< Label is hidden.
        WrapHidden      = 0x0400                    ///< Wrap is hidden.
    };

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    explicit wrapperModeWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~wrapperModeWidget();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve indicators.
    /**
     * @return  indicator flags
     */
    virtual indicatorFlags indicators() const {return flags_;}

    /// Check if widget is read only.
    /**
     * @return  @c true if read only, @c false otherwise
     */
    virtual bool isReadOnly() const {return readOnly_;}

    /// Set indicators.
    /**
     * @param[in] value  indicator flags
     */
    virtual void setIndicators( indicatorFlags value );

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

    /// Mouse press event.
    /**
     * @param[in,out] e  mouse event
     */
    virtual void mousePressEvent( QMouseEvent *e );

    /// Mouse release event.
    /**
     * @param[in,out] e  mouse event
     */
    virtual void mouseReleaseEvent( QMouseEvent *e );

    /// Resize event.
    /**
     * @param[in,out] e  resize event
     */
    virtual void resizeEvent( QResizeEvent *e );

private:

    static const int offsetAmount_;

    indicatorFlags flags_;
    bool readOnly_;

    QLabel *icon_;

    QLabel *apply_;
    QLabel *wrap_;
    QLabel *label_;

    // ========================================================================

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Update indicator state.
    static void updateIndicator( QLabel *l, bool isHidden, bool isEnabled );

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // WRAPPERMODEWIDGET_H
