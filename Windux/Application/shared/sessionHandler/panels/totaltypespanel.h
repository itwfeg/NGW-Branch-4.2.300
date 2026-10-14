/**
 * @file totaltypespanel.h
 * Panel for total types in operate mode.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef TOTALTYPESPANEL_H
#define TOTALTYPESPANEL_H

#include "operatemodepanelex.h"

#include <Session.h>

class QComboBox;
class QGroupBox;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Panel for total types in operate mode.
class TotalTypesPanel : public OperateModePanelEx
{
    Q_OBJECT

    typedef TotalTypesPanel _Myt;
    typedef OperateModePanelEx _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c SessionDialog object
     */
    explicit TotalTypesPanel( SessionMainWindow *mainWindow, SessionDialog *parent = 0 );

    /// Destructor.
    virtual ~TotalTypesPanel();

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private Q_SLOTS:

    /**
     * Slot for combo box index changed.
     */
    void onCurrentIndexChanged( int index );

private:

    QGroupBox *groupBox_;
    QComboBox *comboBox_;

    QVector<QString> ids_;
    QVector<QString> namesPri_;
    QVector<QString> namesSec_;

    bool process_;
    bool done_;

    /// Initialize.
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // TOTALTYPESPANEL_H
