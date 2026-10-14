/**
 * @file productionentryprintingdialog.h
 * Dialog showing current status of Production Entry or Print Multiple Labels, and allowing display of current weight on platter.
 *
 * @copyright Copyright (c) 2022 ITWFEG, LLC
 * @author Larry Pevoar
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef PRODUCTIONENTRYPRINTINGDIALOG_H
#define PRODUCTIONENTRYPRINTINGDIALOG_H

#include "../sessiondialog.h"

class QLabel;
class QPushButton;
class QProgressBar;


///////////////////////////////////////////////////////////////////////////////////////////////////

/// Production Entry Printing dialog.
class ProductionEntryPrintingDialog : public SessionDialog
{
    Q_OBJECT

    typedef ProductionEntryPrintingDialog _Myt;
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
    explicit ProductionEntryPrintingDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~ProductionEntryPrintingDialog();

    /// Whether or not clicking the overlay rejects the dialog.
    /**
     * @return  @c true if allowed, @c false otherwise
     */
    virtual bool allowOverlayClickDismiss() const {return true;} //allowDismiss_;}


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
    void onStopPrintingClicked();

private:

    QString totalTare_;

    QLabel *warningLabel_;

    QLabel *currentWeightLabel_;
    QLabel *currentWeight_;

    QLabel *printProgressLabel_;
    QProgressBar *printProgress_;

    QPushButton *stopPrinting_;

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

    void sleepFor( int ms ) const;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // PRODUCTIONENTRYPRINTINGDIALOG_H
