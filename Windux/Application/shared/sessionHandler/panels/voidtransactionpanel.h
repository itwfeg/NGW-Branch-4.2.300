/**
 * @file voidtransactionpanel.h
 * Panel for void transaction in operate mode.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef VOIDTRANSACTIONPANEL_H
#define VOIDTRANSACTIONPANEL_H

#include "operatemodepanelex.h"

class QHBoxLayout;
class QPushButton;
class QTreeWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Panel for void transaction in operate mode.
class VoidTransactionPanel : public OperateModePanelEx
{
    Q_OBJECT

    typedef VoidTransactionPanel _Myt;
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
    explicit VoidTransactionPanel( SessionMainWindow *mainWindow, SessionDialog *parent = 0 );

    /// Destructor.
    virtual ~VoidTransactionPanel();

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

    /// Slot for export button.
    void onExportButtonClicked();

    /// Slot for select all button.
    void onSelectAllButtonClicked();

    /// Slot for item selection changed.
    void onTableItemSelectionChanged();

    /// Slot for void button.
    void onVoidButtonClicked();

private:

    QTreeWidget *table_;

    QPushButton *export_;
    QPushButton *selectAll_;
    QPushButton *void_;

    // transaction types
    QVector<QString> ids_;
    QVector<QString> namesPri_;
    QVector<QString> namesSec_;

    static const int requestGroupSize_;

    /// Initialize.
    void initialize();

    /// Lookup transaction type name.
    QString lookupTransTypeName( const QString& id ) const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // VOIDTRANSACTIONPANEL_H
