/**
  * @file selfserveStart.h
  * @brief Widget for Self-Service start screen.
  *
  * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup selfserveStartCsu
  * @{
  */

#ifndef SELFSERVESTART_H
#define SELFSERVESTART_H

#include <QWidget>

namespace Ui {
class selfserveStartG2;
}

class selfServiceHook;
class ScaleConfiguration;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class selfserveStart
 * @brief Widget for Self-Service start screen.
 */
class selfserveStartG2 : public QWidget
{
    Q_OBJECT

    typedef selfserveStartG2 _Myt;
    typedef QWidget _Mybase;

public:

    // ************************************************************************
    // CTOR / DTOR
    // ************************************************************************

    /**
     * Constructor.
     *
     * @param[in] h  pointer to hook
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit selfserveStartG2( selfServiceHook *h, QWidget *parent = NULL );

    /**
     * Destructor.
     */
    virtual ~selfserveStartG2();

    // ************************************************************************
    // Methods
    // ************************************************************************

    /**
     * Refresh widget.
     */
    virtual void refresh();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

    /**
     * Update the connectivity background
     */
    virtual void updateNetworkConnectivity(bool online);

protected:

    // ************************************************************************
    // Events
    // ************************************************************************

    /**
     * Mouse release event.
     *
     * @param[in,out] e  event
     */
    virtual void mouseReleaseEvent( QMouseEvent *e );

    /**
     * Resize event.
     *
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

    /**
     * Show event.
     *
     * @param[in,out] e  event
     */
    virtual void showEvent( QShowEvent *e );

private Q_SLOTS:

    /**
     * Slot to reload images.
     */
    void onReloadImages();

private:

    Ui::selfserveStartG2 *ui;

    selfServiceHook *hook_;

    ScaleConfiguration *scaleConfig_;

    QWidget *screen_;

    /**2
     * Initialize.
     */
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // SELFSERVESTART_H
