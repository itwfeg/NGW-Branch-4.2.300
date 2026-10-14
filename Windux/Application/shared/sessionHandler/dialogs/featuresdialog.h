/**
 * @file featuresdialog.h
 * Features scale application dialog.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef FEATURESDIALOG_H
#define FEATURESDIALOG_H

#include "../sessiondialog.h"

class QPlainTextEdit;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Features scale application dialog.
class FeaturesDialog : public SessionDialog
{
    Q_OBJECT

    typedef FeaturesDialog _Myt;
    typedef SessionDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] features  list of installed features
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit FeaturesDialog( const QStringList &features, QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~FeaturesDialog();

private:

    QPlainTextEdit *text_;

    /// Initialize.
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // FEATURESDIALOG_H
