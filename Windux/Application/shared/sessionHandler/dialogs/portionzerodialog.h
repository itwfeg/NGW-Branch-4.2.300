/**
 * @file portionzerodialog.h
 * Portioning dialog.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef PORTIONZERODIALOG_H
#define PORTIONZERODIALOG_H

#include "../sessiondialog.h"

class QLabel;
class QPushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Portioning dialog.
class PortionZeroDialog : public SessionDialog
{
    Q_OBJECT

    typedef PortionZeroDialog _Myt;
    typedef SessionDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit PortionZeroDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~PortionZeroDialog();

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Callback for session messages.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessageReceive( const SessionMessage& msg );

private Q_SLOTS:

    /// Slot for new portion clicked.
    void onNewPortionClicked();

private:

    QString totalTare_;

    QLabel *warningLabel_;

    QLabel *portionWeightLabel_;
    QLabel *portionWeight_;

    QLabel *totalTareWeightLabel_;
    QLabel *totalTareWeight_;

    QPushButton *newPortion_;
    QPushButton *done_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Update weight.
    void updateWeight();

    /// Convert weight str to int.
    int weightStrToInt( const QString &weight );

    /// Convert weight int to str.
    QString weightIntToStr( int weight );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // PORTIONZERODIALOG_H
