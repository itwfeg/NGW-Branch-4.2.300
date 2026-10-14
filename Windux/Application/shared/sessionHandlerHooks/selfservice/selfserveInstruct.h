/**
  * @file selfserveInstruct.h
  * @brief Widget for Self Service Instructions.
  *
  * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
  * @author Randy Blankley
  */

#ifndef SELFSERVEINSTRUCT_H
#define SELFSERVEINSTRUCT_H

#include <QTimer>
#include <QWidget>

namespace Ui {
class selfserveInstruct;
}

class selfServiceHook;
class ScaleConfiguration;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Widget for Self Service Instructions.
class selfserveInstruct : public QWidget
{
    Q_OBJECT

    typedef selfserveInstruct _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] h  pointer to hook
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit selfserveInstruct( selfServiceHook *h, QWidget *parent = NULL );

    /// Destructor.
    virtual ~selfserveInstruct();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Refresh widget.
    virtual void refresh();

    /// Translate the user interface to current language.
    virtual void translate();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /**
     * Mouse release event.
     *
     * @param[in,out] e  event
     */
    virtual void mouseReleaseEvent( QMouseEvent *e );

    /// Resize event.
    /**
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

    /// Show event.
    /**
     * @param[in,out] e  event
     */
    virtual void showEvent( QShowEvent *e );

private slots:

    /// Slot to reload images.
    void onReloadImages();

    /// Slot to update image.
    void onUpdateImage( bool force = false );

private:

    Ui::selfserveInstruct *ui;

    selfServiceHook *hook_;

    ScaleConfiguration *scaleConfig_;

    int imageIndex_;
    QList<QPixmap> images_;

    QTimer timer_;

    /// Initialize.
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

#endif // SELFSERVEINSTRUCT_H
