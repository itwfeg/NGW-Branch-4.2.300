/**
  * @file selfserveInstruct.h
  * @brief Widget for Self Service Instructions.
  *
  * @copyright Copyright (c) 2023, ITW Hobart Food Equipment Group, U.S.A.
  * @author Joe E Crew
  * @liftedfrom Randy Blankley
  */

#ifndef SELFSERVEINSTRUCTG2_H
#define SELFSERVEINSTRUCTG2_H

#include <QTimer>
#include <QWidget>

namespace Ui {
class selfserveInstructG2;
}

class QLabel;

class selfServiceHook;
class ScaleConfiguration;
class DomWeigherConfigRead;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Widget for Self Service Instructions.
class selfserveInstructG2 : public QWidget
{
    Q_OBJECT

    typedef selfserveInstructG2 _Myt;
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
    explicit selfserveInstructG2( selfServiceHook *h, QWidget *parent = NULL );

    /// Destructor.
    virtual ~selfserveInstructG2();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Refresh widget.
    virtual void refresh();

    /// Refresh weight.
    virtual void refreshWeight();

    ///  Dim the screen opacity to pop dialog boxes.
    virtual void dimScreenOpacity();

    ///  Reset the screen opacity to pop dialog boxes.
    virtual void resetScreenOpacity();

    /// Translate the user interface to current language.
    virtual void translate();

    /// Update the connectivity background
    virtual void updateNetworkConnectivity( bool online );

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

    /**
     * Mouse press event.
     *
     * @param[in,out] e  mouse event to pop weights and measures screen
     */
    virtual void mousePressEvent( QMouseEvent *e );

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

    Ui::selfserveInstructG2 *ui;

    selfServiceHook *hook_;

    ScaleConfiguration *scaleConfig_;
    DomWeigherConfigRead *weigherConfig_;

    QString curSeparator_;

    int imageIndex_;
    QList<QPixmap> images_;

    QLabel *weight_;
    QLabel *capacityLabel_;
    QWidget *capacityArea_;
    QWidget *screen_;

    QLabel *capacityHighRange_;
    QLabel *capacityLowRange_;
    QLabel *zero_;

    QTimer timer_;

    /// get the pixels
    int getScreenHeight();

    /// Initialize.
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

#endif // SELFSERVEINSTRUCT_H
