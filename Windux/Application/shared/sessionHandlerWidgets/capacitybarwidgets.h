/**
 * @file capacitybarwidgets.h
 * Widgets that reside on the capacity bar.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef CAPACITYBARWIDGETS_H
#define CAPACITYBARWIDGETS_H

#include "icons.h"

#include <QLabel>

class MadpNetworkConnectivity;
class MadpNetworkDeviceType;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Label with a background indicator for the capacity bar.
class indicatorLabel : public QLabel
{
    Q_OBJECT

    typedef indicatorLabel _Myt;
    typedef QLabel _Mybase;

public:

    /// Indicator direction.
    enum Direction
    {
        Down,                                   ///< Draw indicator down.
        Up                                      ///< Draw indicator up.
    };

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    explicit indicatorLabel( QWidget *parent = 0 );

    /// Destructor.
    virtual ~indicatorLabel();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve indicator direction.
    /**
     * @return  direction of indicator
     */
    virtual Direction indicatorDirection() const {return indicatorDirection_;}

    /// Retrieve indicator visibility.
    /**
     * @return  visibility of indicator
     */
    virtual bool indicatorVisible() const {return indicatorVisible_;}

    /// Set indicator direction.
    /**
     * @param[in] value  direction of indicator
     */
    virtual void setIndicatorDirection( Direction value );

    /// Set indicator visibility.
    /**
     * @param[in] value  visibility of indicator
     */
    virtual void setIndicatorVisible( bool value );

    /// This property holds the recommended size for the widget.
    /**
     * If the value of this property is an invalid size, no size is recommended.
     *
     * @return  size
     */
    virtual QSize sizeHint() const;

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Paint event.
    /**
     * @param[in,out] event  paint event
     */
    virtual void paintEvent( QPaintEvent *event );

private:

    bool indicatorVisible_;
    Direction indicatorDirection_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Indicator icon for which screen you are currently on.
class screenIndicatorIcon : public icon
{
    Q_OBJECT

    typedef screenIndicatorIcon _Myt;
    typedef icon _Mybase;

Q_SIGNALS:

    /// Signal to swipe the screen left.
    void swipeScreenLeft();

    /// Signal to swipe the screen right.
    void swipeScreenRight();

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] screenCount  number of screens
     * @param[in] parent  parent widget
     */
    explicit screenIndicatorIcon( int screenCount, QWidget *parent = 0 );

    /// Destructor.
    virtual ~screenIndicatorIcon();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve which screen you are currently on.
    /**
     * @return  screen number in range [ 0, screenCount() )
     */
    virtual int screen() const {return screen_;}

    /// Retrieve number of screens.
    /**
     * @return  screen count
     */
    virtual int screenCount() const {return screenCount_;}

    /// Set which screen you are currently on.
    /**
     * @param[in] value  screen number where 0 <= @p value <= screenCount()
     */
    virtual void setScreen( int value );

    /// Set number of screens.
    /**
     * @param[in] value  screen count where 0 < @p value
     */
    virtual void setScreenCount( int value );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Determine best size for widget.
    /**
     * @return  size
     */
    virtual QSize calculateSize() const;

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Mouse release event.
    /**
     * @param[in,out] event  mouse event
     */
    virtual void mouseReleaseEvent( QMouseEvent *event );

    /// Resize event.
    /**
     * @param[in,out] event  resize event
     */
    virtual void resizeEvent( QResizeEvent *event );

private:

    int screen_;
    int screenCount_;

    QVector<QPixmap> images_;

    /// Initialize screens.
    void initialize( int screens );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Recall icon for the capacity bar.
class recallIcon : public alertableIcon
{
    Q_OBJECT

    typedef recallIcon _Myt;
    typedef alertableIcon _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    explicit recallIcon( QWidget *parent = 0 );

    /// Destructor.
    virtual ~recallIcon();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] event  resize event
     */
    virtual void resizeEvent( QResizeEvent *event );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Printer icon for the capacity bar.
class printerIcon : public alertableIcon
{
    Q_OBJECT

    typedef printerIcon _Myt;
    typedef alertableIcon _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    explicit printerIcon( QWidget *parent = 0 );

    /// Destructor.
    virtual ~printerIcon();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] event  resize event
     */
    virtual void resizeEvent( QResizeEvent *event );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Planogram icon for the capacity bar.
class planogramIcon : public alertableIcon
{
    Q_OBJECT

    typedef planogramIcon _Myt;
    typedef alertableIcon _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    explicit planogramIcon( QWidget *parent = 0 );

    /// Destructor.
    virtual ~planogramIcon();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] event  resize event
     */
    virtual void resizeEvent( QResizeEvent *event );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Wireless signal icon for the capacity bar.
class wirelessSignalIcon : public alertableIcon
{
    Q_OBJECT

    Q_ENUMS( ConnectionType )
    Q_PROPERTY( ConnectionType connectionType READ connectionType WRITE setConnectionType )
    Q_PROPERTY( int signalStrength READ signalStrength WRITE setSignalStrength )

    typedef wirelessSignalIcon _Myt;
    typedef alertableIcon _Mybase;

public:

    /// Connection types.
    enum ConnectionType
    {
        WiredConnectionType,                    ///< Using wired connection to access internet.
        WirelessConnectionType                  ///< Using wireless connection to access internet.
    };

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    explicit wirelessSignalIcon( QWidget *parent = 0 );

    /// Destructor.
    virtual ~wirelessSignalIcon();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve connection type.
    /**
     * @return  connection type
     */
    virtual ConnectionType connectionType() const {return connectionType_;}

    /// Set connection type.
    /**
     * @param[in] value  connection type
     */
    virtual void setConnectionType( ConnectionType value );

    /// Set wireless signal strength.
    /**
     * @param[in] value  signal strength where 0 <= @p value <= 5
     */
    virtual void setSignalStrength( int value );

    /// Retrieve wireless signal strength.
    /**
     * @return  signal strength
     */
    virtual int signalStrength() const {return signalStrength_;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Invert the wired icon colors.
    virtual void invertWiredIcon();

    /// Update state from message.
    /**
     * @param[in] msg  pointer to message
     */
    virtual void updateState( const MadpNetworkConnectivity *msg );

    /// Update state from message.
    /**
     * @param[in] msg  pointer to message
     */
    virtual void updateState( const MadpNetworkDeviceType *msg );

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] event  resize event
     */
    virtual void resizeEvent( QResizeEvent *event );

private:

    ConnectionType connectionType_;
    int signalStrength_;

    QPixmap signalStrength0_;
    QPixmap signalStrength1_;
    QPixmap signalStrength2_;
    QPixmap signalStrength3_;
    QPixmap signalStrength4_;
    QPixmap signalStrength5_;
    QPixmap wirelessAlert_;

    QPixmap wired_;
    QPixmap wiredAlert_;

    QPixmap current_;
    QPixmap currentAlert_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Drop down arrow icon for the capacity bar.
class dropDownArrowIcon : public icon
{
    Q_OBJECT

    typedef dropDownArrowIcon _Myt;
    typedef icon _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    explicit dropDownArrowIcon( QWidget *parent = 0 );

    /// Destructor.
    virtual ~dropDownArrowIcon();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] event  resize event
     */
    virtual void resizeEvent( QResizeEvent *event );

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // CAPACITYBARWIDGETS_H
