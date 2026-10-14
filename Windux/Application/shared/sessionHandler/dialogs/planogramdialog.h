/**
 * @file planogramdialog.h
 * Dialog for managing planograms.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef PLANOGRAMDIALOG_H
#define PLANOGRAMDIALOG_H

#include "listdialog.h"

class QPushButton;
class QSignalMapper;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dialog for managing planograms.
class PlanogramDialog : public ListDialog
{
    Q_OBJECT

    typedef PlanogramDialog _Myt;
    typedef ListDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit PlanogramDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~PlanogramDialog();

protected:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve renderer appearance.
    /**
     * @return  appearance object
     */
    virtual hobart::widgetRendererAppearanceBase *rendererAppearance() const;

    /// Retrieve renderer style.
    /**
     * @return  style
     */
    virtual int rendererStyle() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Add items to list.
    /**
     * @param[in] files  list of filenames
     * @param[in] dates  list of modfication dates
     */
    virtual void addItemsToList( const QStringList& files, const QStringList& dates );

    /// Request list.
    virtual void requestList();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Callback for session messages.
    /**
     * This base method does nothing. Override to implement your own functionality.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessageReceive( const SessionMessage& msg );

private Q_SLOTS:

    /// Slot for button clicked.
    void onButtonClicked( QWidget *button );

    /// Slot for list selection changed.
    void onListItemSelectionChanged();

private:

    QSignalMapper *signalMapper_;

    QPushButton *delete_;
    QPushButton *view_;

    static const QSize iconSize_;
    static const QSize iconSizeList_;

    /// Initialize.
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // PLANOGRAMDIALOG_H
