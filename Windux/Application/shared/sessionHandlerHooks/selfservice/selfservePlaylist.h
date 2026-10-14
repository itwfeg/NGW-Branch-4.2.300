/**
  * @file selfservePlaylist.h
  * @brief Widget for Self Service Instructions.
  *
  * @copyright Copyright (c) 2019, ITW Hobart Food Equipment Group, U.S.A.
  * @author Job E.
  * @addtogroup selfserveItemLookupCsu
  * @{
  */

#ifndef SELFSERVEPLAYLIST_H
#define SELFSERVEPLAYLIST_H

#include <QWidget>
#include <QTimer>

//#include "../playlistWidget/playlistwidget.h"

class playlistWidget;


namespace Ui {
class selfservePlaylist;
}

class selfServiceHook;
class ScaleConfiguration;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Widget for Self Service Playlist.
class selfservePlaylist : public QWidget
{
    Q_OBJECT

    typedef selfservePlaylist _Myt;
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
    explicit selfservePlaylist( selfServiceHook *h, QWidget *parent = NULL);

    /// Destructor.
    ~selfservePlaylist();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve playlist.
    /**
     * @return  playlist
     */
    virtual QString playlist() const;

    /// Set playlist.
    /**
     * @param[in] value  playlist
     */
    virtual void setPlaylist( const QString& value );


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



private:

    Ui::selfservePlaylist *ui;

    selfServiceHook *hook_;
    ScaleConfiguration *scaleConfig_;

    // from NowServing
    playlistWidget *adverts_;

    /// Initialize.
    void initialize();

    /// Slot to reload store logos.
    void loadLogo();

    /// Create layout.
    //void createLayout();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

#endif // SELFSERVEPLAYLIST_H
