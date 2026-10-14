/**
 * @file selfserviceweightbar.h
 * Self service weight and capacity bar.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup selfServeCsu
 * @{
 */

#ifndef SELFSERVICEWEIGHTBAR_H
#define SELFSERVICEWEIGHTBAR_H

#include <QBrush>
#include <QWidget>

class QLabel;

class selfServiceHook;

namespace hobart {
    class indicatorLabel;
    class wirelessSignalIcon;
}

class selfServiceHook;
class MadpNetworkConnectivity;
class MadpNetworkDeviceType;
class ScaleConfiguration;
class Session;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Self service weight and capacity bar.
class selfServiceWeightBar : public QWidget
{
    Q_OBJECT

    Q_ENUMS( mode )
    Q_PROPERTY( mode currentMode READ currentMode WRITE setCurrentMode NOTIFY currentModeChanged )

    typedef selfServiceWeightBar _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /// Emitted when mode changes.
    void currentModeChanged();

    /// Emitted when weight pressed.
    void weightPressed();

public:

    /// Weight bar modes.
    enum mode
    {
        weightOnly,                                 ///< Show only weight.
        weightAndPrice,                             ///< Show weight and pricing.
        byCountAndPrice,                            ///< Show by count and pricing.
        capacityOnly,                               ///< Show weigher capacity only.
        loginWeightOnly
    };

    // ************************************************************************
    // CTOR / DTOR
    // ************************************************************************

    /**
     * @brief CTOR.
     * @param[in] h  pointer to hook
     * @param[in] parent  parent widget
     */
    selfServiceWeightBar( selfServiceHook *h, QWidget *parent = NULL );

    /// DTOR
    virtual ~selfServiceWeightBar();

    // ************************************************************************
    // Properties
    // ************************************************************************

    /**
     * @brief  Retrieve current mode.
     * @return  mode
     */
    virtual mode currentMode() {return mode_;}

    // ************************************************************************
    // Methods
    // ************************************************************************

    /**
     * @brief  Network connectivity message.
     * @param[in] m  pointer to message
     */
    virtual void networkConnectivity( const MadpNetworkConnectivity *m );

    /**
     * @brief  Network device type message.
     * @param[in] m  pointer to message
     */
    virtual void networkDeviceType( const MadpNetworkDeviceType *m );

    /// Refresh widget.
    virtual void refresh();

    /// Translate to current language.
    virtual void translate();

public Q_SLOTS:

    /**
     * @brief  Set current mode.
     * @param[in] value  mode
     */
    void setCurrentMode( mode value );

    /**
     * @brief  Set number text.
     * @param[in] value  text
     */
    virtual void clearByCount();

protected:

    // ************************************************************************
    // Events
    // ************************************************************************

    /**
     * @brief  Resize event.
     * @param[in] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

    // ************************************************************************
    // Methods
    // ************************************************************************

    /**
     * @brief  Event filter.
     * @param[in] obj  pointer to object
     * @param[in] e  pointer to event
     * @return  @c true to filter event, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *e );

private:

    mode mode_;

    QBrush weightOnlyBrush_;
    QBrush fieldAndPriceBrush_;
    QBrush *background_;

    selfServiceHook *hook_;

    QWidget *weightArea_;
    QLabel *weightLabel_;
    QLabel *weight_;

    QWidget *byCountArea_;
    QLabel *byCountLabel_;
    QLabel *byCount_;

    QWidget *tareArea_;
    QLabel *tareLabel_;
    QLabel *tare_;

    QWidget *unitPriceArea_;
    QLabel *unitPriceLabel_;
    QLabel *unitPrice_;

    QWidget *totalPriceArea_;
    QLabel *totalPriceLabel_;
    QLabel *totalPrice_;

    QWidget *capacityArea_;
    QLabel *capacityLabel_;
    hobart::indicatorLabel *capacityHighRange_;
    hobart::indicatorLabel *capacityLowRange_;
    hobart::indicatorLabel *zero_;

    hobart::wirelessSignalIcon *wirelessSignalIcon_;

    bool showTare_;

    static const int capacityWidgetHeight_;
    static QSize weightWidgetSize_;
    static int weightBarDataPointSize_;

    ScaleConfiguration *scaleConfig_;

    //QFont metFont_;

    // not implemented
    selfServiceWeightBar( const _Myt& rhs );

    // not implemented
    _Myt& operator = ( const _Myt& rhs );

    /// Initialize object.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Update background palette.
    void updateBackgroundPalette();

    /// Update label font size to fit widget.
    void updateLabelFontPointSize( QLabel *w, int margins = 0 );

    int checkScreenSize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SELFSERVICEWEIGHTBAR_H
