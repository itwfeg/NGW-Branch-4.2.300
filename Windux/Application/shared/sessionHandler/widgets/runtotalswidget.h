/**
 * @file runtotalswidget.h
 * Widget for displaying prepack run totals.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef RUNTOTALSWIDGET_H
#define RUNTOTALSWIDGET_H

#include <QString>
#include <QVector>
#include <QWidget>

class QLabel;

class Session;

typedef QVector<QString> SessionMessage;

class OperateMode;
class ScaleConfiguration;

class DomProductRunTotal;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Widget for displaying prepack run totals.
class runTotalsWidget : public QWidget
{
    Q_OBJECT

    typedef runTotalsWidget _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    explicit runTotalsWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~runTotalsWidget();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate the user interface to current language.
    virtual void translate();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] e  paint event
     */
    virtual void resizeEvent( QResizeEvent *e );

private Q_SLOTS:

    /// Slot to handle PLU data changed.
    void onCurrentProductChanged( bool nof );

    /// Slot for session messages.
    void onSessionMessageReceive( const SessionMessage& msg );

private:

    Session *session_;

    OperateMode *operateMode_;
    ScaleConfiguration *config_;

    DomProductRunTotal *productRunTotals_;
    int productType_;

    QList<QLabel*> labels_;

    QLabel *header_;

    QLabel *packagesLabel_;
    QLabel *packages_;

    QLabel *itemsLabel_;
    QLabel *items_;

    QLabel *totalLabel_;
    QLabel *total_;

    QLabel *weightLabel_;
    QLabel *weight_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Update totals.
    void updateTotals();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // RUNTOTALSWIDGET_H
