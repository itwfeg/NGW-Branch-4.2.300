/**
 * @file superChangeProductClass.h
 * @brief Widget to view Wrapper System Info.
 *
 * @copyright Copyright (c) 2014-2016 Hobart Corporation
 * @author Todd Davis, Randy Blankley
 */

#ifndef SERVICENEWWRAPPERCONFIG_H
#define SERVICENEWWRAPPERCONFIG_H

#include "screenwidget.h"

class DomEvolWrapperConfig;
//class DomEvolWrapperHeaterConfig;
class DomEvolWrapperSystemInformation;
class MadpPeripheralList;
class MadpInternalConfig;

namespace hobart {
    class numericLineEdit;
}

class QIntValidator;
class QLabel;
class QComboBox;
class QPushButton;
class QToolButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

///  Shows & edits wrapper config
class serviceNewWrapperConfig : public ScreenWidget
{
    Q_OBJECT

    typedef serviceNewWrapperConfig _Myt;
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
    explicit serviceNewWrapperConfig(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /// Destructor.
    virtual ~serviceNewWrapperConfig();

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
     * @param[in,ou] e  event
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

private Q_SLOTS:

    /// Slot for field updated.
    void onFieldUpdated();

    /// Slot for Heater
    void onHeaterClicked();

    /// Slot for toggling applier configs
    void toggleApplierConfigs();

    /// Slot for toggling other wrapper configs
    void toggleWrapperConfigs();

private:
    DomEvolWrapperConfig *evolWrapperConfigData_;
    //DomEvolWrapperHeaterConfig *evolWrapperHeaterConfigData_;
    DomEvolWrapperSystemInformation *evolWrapperSystemInformation_;
    MadpPeripheralList *peripheralList_;
    MadpInternalConfig *internalConfigData_;

    bool ready_;
    bool hasChanged_;

    QIntValidator *sealerBeltSettingValidator_;
    QIntValidator *labelOffsetValidator_;
    QIntValidator *rotationCalibrationOffsetValidator_;
    QIntValidator *wandCalibrationOffsetValidator_;
    QIntValidator *applierAdjustValidator_;

    // ---- //

    QLabel *title_;

    QLabel *sealerBeltSettingLabel_;
    hobart::numericLineEdit *sealerBeltSetting_;

    QLabel *lowerFilmWidthLabel_; // aws
    QComboBox *lowerFilmWidth_;

    QLabel *upperFilmWidthLabel_;
    QComboBox *upperFilmWidth_;

    QLabel *priLabelOffsetLabel_;
    hobart::numericLineEdit *priLabelOffset_;

    QLabel *priRotationCalibrationOffsetLabel_;
    hobart::numericLineEdit *priRotationCalibrationOffset_;

    QLabel *priRotationCalibrationOffset_90DegLabel_;
    hobart::numericLineEdit *priRotationCalibrationOffset_90Deg_;

    QLabel *priRotationCalibrationOffset_180DegLabel_;
    hobart::numericLineEdit *priRotationCalibrationOffset_180Deg_;

    QLabel *priRotationCalibrationOffset_270DegLabel_;
    hobart::numericLineEdit *priRotationCalibrationOffset_270Deg_;

    QLabel *priWandCalibrationOffsetLabel_; // enhanced
    hobart::numericLineEdit *priWandCalibrationOffset_;

    QLabel *packageDirectionLabel_;
    QComboBox *packageDirection_;

    QLabel *secondCLALabel_; // aws only, not enhanced
    QComboBox *secondCLA_;

    QLabel *wrapperOperationLabel_; // aws only, not enhanced
    QComboBox *wrapperOperation_;

    QLabel *hardwareAddOnLabel_;
    QComboBox *hardwareAddOn_;

    QLabel *secLabelOffsetLabel_;
    hobart::numericLineEdit *secLabelOffset_;

    QLabel *secRotationCalibrationOffsetLabel_;
    hobart::numericLineEdit *secRotationCalibrationOffset_;

    QLabel *secRotationCalibrationOffset_90DegLabel_;
    hobart::numericLineEdit *secRotationCalibrationOffset_90Deg_;

    QLabel *secRotationCalibrationOffset_180DegLabel_;
    hobart::numericLineEdit *secRotationCalibrationOffset_180Deg_;

    QLabel *secRotationCalibrationOffset_270DegLabel_;
    hobart::numericLineEdit *secRotationCalibrationOffset_270Deg_;

    QLabel *secWandCalibrationOffsetLabel_; // enhanced
    hobart::numericLineEdit *secWandCalibrationOffset_;

    QLabel *priApplierHandAdjustLabel_;
    hobart::numericLineEdit *priApplierHandAdjust_;

    QLabel *secApplierHandAdjustLabel_;
    hobart::numericLineEdit *secApplierHandAdjust_;

    QLabel *automaticRotationLabel_;
    QComboBox *automaticRotation_;

    QLabel *sealerBeltPkgSensorTypeLabel_;
    QComboBox *sealerBeltPkgSensorType_;

    QLabel *upperFilmPullTensionLabel_; // enhanced
    QComboBox *upperFilmPullTension_;

    QLabel *lowerFilmPullTensionLabel_; // enhanced
    QComboBox *lowerFilmPullTension_;

    QLabel *lblFilmHeater_;              // enhanced
    QComboBox *cboFilmHeater_;

    // buttons to toggle between configs, these will essentially hide/show different configs to kind of mimic a new page being selected
    /*
    QPushButton *toggleLabelApplierConfigButton_;
    QPushButton *toggleGeneralWrapperConfigButton_;
    */

    QToolButton *toggleLabelApplierConfigButton_;
    QToolButton *toggleGeneralWrapperConfigButton_;

    std::vector<QWidget*> wrapperConfigWidgets_;
    std::vector<QWidget*> applierConfigWidgets_;

    bool isTwoPrinterSystem_;

    // ---- //

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Update form fields.
    void updateFormFields();

    /// Save wrapper configuration.
    void saveWrapperConfig();

    /// Make validator top value.
    static int makeValidatorTop( int digits );

    // disable 2PLa configs for non 2P units
    void checkSecondaryLAConfigs();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SERVICENEWWRAPPERCONFIG_H
