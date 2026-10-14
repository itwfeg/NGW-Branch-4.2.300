/**
 * @file capacitybarwidget.h
 * Capacity bar widget.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef CAPACITYBARWIDGET_H
#define CAPACITYBARWIDGET_H

#include "screenwidget.h"

class QLabel;
class QSignalMapper;

class OperateMode;

namespace hobart {
    class dropDownArrowIcon;
    class indicatorLabel;
    class operateMode;
    class planogramIcon;
    class printerIcon;
    class recallIcon;
    class screenIndicatorIcon;
    class wirelessSignalIcon;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Areas where the capacity bar may be located.
enum CapacityBarArea
{
    BottomCapacityBarArea,                  ///< Capacity bar located on the bottom.
    TopCapacityBarArea                      ///< Capacity bar located on the top.
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Capacity bar display object.
class CapacityBarWidget : public ScreenWidget
{
    Q_OBJECT

    typedef CapacityBarWidget _Myt;
    typedef ScreenWidget _Mybase;

Q_SIGNALS:

    /// Signal emitted when the window is moved.
    /**
     * @param[in] where  where the bar was moved to
     */
    void moved( CapacityBarArea where );

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     * @param[in] area  area to display capacity bar
     */
    explicit CapacityBarWidget(
        SessionMainWindow *mainWindow,
        ModeWidget *parent = 0,
        CapacityBarArea area = TopCapacityBarArea );

    /// Destructor.
    virtual ~CapacityBarWidget();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve capacity bar area.
    /**
     * @return  capacity bar area
     */
    virtual CapacityBarArea area() const {return area_;}

    /// Retrieve margins.
    /**
     * Some screens add widgets to the top and/or bottom. This property returns how much space
     * the widgets are taking up. This property is useful because we may want to offset the
     * numeric keypad or other widgets.
     *
     * @return  margins
     */
    virtual QMargins margins() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate the user interface to current language.
    virtual void translate();

public Q_SLOTS:

    /// Slot to set the capacity string.
    virtual void setCapacityText();

    /// Slot to set the zero indicator.
    virtual void setZeroIndicator( const QString& netWeight, const QString& dualRangeIndicator, bool zeroFlag );

    /// Slot to set the screen indicator.
    virtual void setScreenIndicator();

    /// Slot to set the weigh mode.
    virtual void setWeighMode();

    /// Slot to set the date and time string.
    virtual void setDateTimeText();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Mouse move event.
    /**
     * @param[in,out] e  mouse event
     */
    virtual void mouseMoveEvent( QMouseEvent *e );

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

    // ========================================================================
    // Methods
    // ========================================================================

    /// Event filter.
    /**
     * @param[in] obj  widget
     * @param[in,out] e  event
     * @return  @c true if filtered, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *e );

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private Q_SLOTS:

    /// Slot to handle PLU totals changed.
    void onCurrentProductTotalsChanged();

    /// Slot to handle icon click.
    void onIconClicked( QWidget *w );

    /// Slot for operate mode clicked.
    void onOperateModeClicked();

    /// Slot for operator login.
    void onOperatorLoggedIn();

    /// Slot for operator logout.
    void onOperatorLoggedOut();

    /// Slot to swipe screen left.
    void onSwipeScreenLeft();

    /// Slot to swipe screen right.
    void onSwipeScreenRight();

private:

    OperateMode *operateMode_;

    QLabel *capacity_;
    hobart::indicatorLabel *scaleMax_;
    hobart::indicatorLabel *scaleDualMax_;
    hobart::indicatorLabel *zeroIndicator_;
    hobart::screenIndicatorIcon *screenIndicatorIcon_;

    hobart::recallIcon *recallIcon_;
    hobart::printerIcon *printerIcon_;
    hobart::planogramIcon *planogramIcon_;
    hobart::wirelessSignalIcon *wirelessIcon_;

    QLabel *scaleMode_;
    hobart::dropDownArrowIcon *scaleModeIcon_;
    QLabel *operatorName_;
    QLabel *dateTime_;
    QLabel *deptName_;
    QLabel *spacer_;
    QString formattedOperatorName_;

    CapacityBarArea area_;

    QPoint position_;
    bool moving_;

    QString mostRecentPlanogram_;

    QSignalMapper *signalMapper_;

    static const int pixmapOffset_;         ///< Amount to offset screen below this widget (i.e. margin).
    static const bool convertUppercase_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Set scale mode icon visibility.
    void setScaleModeIconVisible();

    /// Calculate field position.
    QRect calcFieldGeometry( const QWidget *widget ) const;

    /// get production mode, dept, date and operator sizes
    void getSizes();

    // SOF-924
    QString deptNamePrimary;
    QString deptNameSecondary;
    void setDeptName();
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // CAPACITYBARWIDGET_H
