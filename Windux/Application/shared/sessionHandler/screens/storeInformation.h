/**
  * @file storeInformation.h
  * @brief Allows supervisor to assign store name, store ID and store graphic to the system.
  *
  * @copyright Copyright (c) 2012, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup storeInformationCsu
  * @{
  */

#ifndef STOREINFORMATION_H
#define STOREINFORMATION_H

#include "screenwidget.h"

class QComboBox;
class QIntValidator;
class QLabel;
class QPlainTextEdit;

namespace hobart {
class numericLineEdit;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Allows supervisor to assign store name, store ID and store graphic to the system.
class storeInformation : public ScreenWidget
{
    Q_OBJECT
    
    typedef storeInformation _Myt;
    typedef ScreenWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit storeInformation( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /// Destructor.
    virtual ~storeInformation();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate the user interface to current language.
    virtual void translate();

public Q_SLOTS:

    /// Slot for when a field is changed.
    /**
     * Sets property indicating that the setup needs to be saved when exiting.
     */
    virtual void SetupChanged();

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
     * @return  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private:

    bool bSetupChanged;

    QIntValidator *storeIDValidator_;
    QIntValidator *deptNoValidator_;

    QLabel *lblStoreName;
    QPlainTextEdit *txtStoreName;

    QLabel *lblStoreID;
    hobart::numericLineEdit *txtStoreID;

    QLabel *lblStoreGraphic;
    QComboBox *cbStoreGraphic;

    QLabel *lblDeptNo;
    hobart::numericLineEdit *txtDeptNo;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Used with numericLineEdit widget to validate Store ID.
    int makeValidatorTop( int digits );

    /// Builds the parameter string and saves the setup.
    void SaveDataOnExit();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // STOREINFORMATION_H
