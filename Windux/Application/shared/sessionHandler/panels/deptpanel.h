/**
 * @file deptpanel.h
 * Panel for selecting department in multi-dept scale.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef DEPTPANEL_H
#define DEPTPANEL_H

#include "operatemodepanelex.h"
#include "../widgets/dynamiccontentwidget.h"

class QComboBox;
class QLabel;
class QGroupBox;
class QPushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Panel for selecting department in operate mode.
class DepartmentPanel : public OperateModePanelEx
{
    Q_OBJECT

    typedef DepartmentPanel _Myt;
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
    explicit DepartmentPanel( SessionMainWindow *mainWindow, SessionDialog *parent = 0 );

    /// Destructor.
    virtual ~DepartmentPanel();

    /// Reset flashkeys to default (initial) group.
    /**
     * @param[in] force  @c true to force reset all flashkey banks, @c false otherwise
     */
    virtual void resetFlashkeys( bool force = false );

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

    /// Slot for push button clicked.
    void onClicked();

    /// Slot for combo box index changed.
    void onCurrentIndexChanged( int index );

private:

    /// Department information.
    struct deptInfo
    {
        int number;                                 ///< Department number
        QString namePrim;                           ///< Name (primary language)
        QString nameSec;                            ///< Name (secondary language)
    };

    QLabel *currentDeptLabel_;
    QLabel *currentDept_;

    QGroupBox *selectDeptGroupBox_;
    QComboBox *selectDeptCombo_;
    QPushButton *selectDept_;

    QVector<deptInfo> departments_;
    int currentDeptIndex_;

    hobart::dynamicContentWidget *dcaPrimary_;
    hobart::dynamicContentWidget *dcaSecondary_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Add department to table.
    void addDept( int number, const QString &namePrim, const QString &nameSec );

    /// Set cuttent department number
    void setCurrentDept( int number );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DEPTPANEL_H
