/**
 * @file superDepartments.h
 * Screen for managing departments.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef SUPERDEPARTMENTS_H
#define SUPERDEPARTMENTS_H

#include "screenwidget.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QTreeWidget;

namespace hobart {
class numericLineEdit;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

class SuperDepartmentsMaint : public ScreenWidget
{
    Q_OBJECT

    typedef SuperDepartmentsMaint _Myt;
    typedef ScreenWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit SuperDepartmentsMaint( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /// Destructor.
    virtual ~SuperDepartmentsMaint();

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
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Handle screen changing.
    /**
     * This method is called before the screen is changed to something new. If for some reason the
     * screen should not be unloaded and destroyed then a value of @c false should be returned.
     *
     * @returns  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

    /// Translate table.
    virtual void translateTable();

private Q_SLOTS:

    /// Slot for item clicked.
    void onClicked();

    /// Slot for editing finished.
    void onEditingFinished();

private:

    /// Department information.
    struct deptInfo
    {
        int number;                                 ///< Department number
        QString namePrim;                           ///< Name (primary language)
        QString nameSec;                            ///< Name (secondary language)
    };

    QLabel *numberLabel_;
    hobart::numericLineEdit *number_;

    QLabel *namePrimaryLabel_;
    QLineEdit *namePrimary_;

    QLabel *nameSecondaryLabel_;
    QLineEdit *nameSecondary_;

    QPushButton *deleteDept_;

    QWidget *buttonLayoutWidget_;

    QTreeWidget *depts_;

    QMap<int, deptInfo> departments_;
    int currentDept_;
    int currentActiveDept_;
    bool sentReadCurrentDept_;

    int handleSignals_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Check to handle signals.
    bool handleSignals() const {return (0 == handleSignals_);}

    /// Set to handle signals.
    void setHandleSignals( bool value ) {
        if ( value )
            --handleSignals_;
        else
            ++handleSignals_;
    }

    /// Reset form.
    void resetForm();

    /// Save current department.
    void saveCurrentDept();

    /// Add department to table.
    void addDept( int number, const QString &namePrim, const QString &nameSec, bool trans = true );

    /// Show department on form.
    void showDept( int number );

    void enableDepts( bool value );

};


///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SUPERDEPARTMENTS_H
