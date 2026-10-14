/**
  * @file calibui.h
  * @brief Widget used by manufacturing to calibrate the Accelerometer.
  *
  * @copyright Copyright (c) 2018, ITW Hobart Food Equipment Group, U.S.A.
  * @author Job E. Crew
  * @addtogroup calibuiCsu
  * @{
  */

#ifndef CALIBUI_H
#define CALIBUI_H

#ifndef QT_DEBUG
#include "hobart/hobartweigher.h"
#include "hobart/can/candevman.h"
#include "hobart/can/kpc.h"
#endif

#include <math.h>
#include <stdint.h>
#include "generate.h"
#include "warning.h"
#include "failure.h"

#include <QWidget>
#include <QStackedWidget>
#include <QTimer>
#include <QFile>


class QLabel;
class QPushButton;

namespace Ui
{
    class calibui;
}


class calibui : public QWidget
{
    Q_OBJECT

    typedef calibui _Myt;
    typedef QWidget _Mybase;

public:

    /// Weigher states.
    enum weigherState
    {
        zero,                                       ///< Weigher at zero.
        unstable,                                   ///< Weigher unstable.
        stable,                                     ///< Weigher at stable weight.
        underWeight,                                ///< Weigher under zero.
        overWeight,                                 ///< Weigher over weight limit.
        outOfMaint,                                 ///< Weigher out of maintenance zone.
        error                                       ///< Weigher error.
    };

    // ************************************************************************
    // CTOR / DTOR
    // ************************************************************************

    /// Constructor.
    /**
     * @param[in] parent  parent @c ModeWidget widget
     */
    calibui(kpc *device = 0, QWidget *parent = 0);


    /**
     * Destructor.
     */
    ~calibui();

    // ========================================================================
    // Methods
    // ========================================================================

    /**
     * Refresh widget.
     */
    virtual void refresh();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

public Q_SLOTS:


    // ========================================================================
    // Methods
    // ========================================================================

    /// Catch the status containing the weights (counts)
    /**
     * @param[in] weigherState the state of the weigher
     * @param[in] weight the weigher counts from the status (I think, kinda)
     */
    void onUpdateWeigherStateChanged(weigherState state, float weight);

    // NEW
    void onUpdateValueMaxFitParameters(int32_t g, int32_t m, int32_t zerovalue, int32_t spanvalue, int32_t Z0, int32_t D, int32_t x2, int32_t x1, int32_t x0, int32_t y2, int32_t y1, int32_t y0, int32_t xrot0, int32_t xrot1, int32_t xrot2, int32_t yrot0, int32_t yrot1, int32_t yrot2, int32_t zrot0, int32_t zrot1, int32_t zrot2, int32_t xgain, int32_t ygain, int32_t gcoeff, int32_t checksum);

    /// Signal to the UI the X,Y, and Z Gain Factor changed
    /**
     * @param[in] QString mfgDate
     */
    void onUpdateValueMaxDate( uint32_t mfgDate );

    /// Signal to the UI the X,Y, and Z Gain Factor changed
    /**
     * @param[in] QString assemblyNumber
     */
    void onUpdateValueMaxAssemblyNumber( QString assemNum );

    /// Signal to the UI the X,Y, and Z Gain Factor changed
    /**
     * @param[in] QString hardwareVersion
     */
    void onUpdateValueMaxHWVersion( QString hwVersion );

    /// Signal to the UI the X,Y, and Z Gain Factor changed
    /**
     * @param[in] QString serialNumber
     */
    void onUpdateValueMaxSerialNumber( QString serialNum );

    // NEW
    void onUpdateValueMaxWeigherStatus(uint16_t flags, uint32_t rawAtoDCounts, int32_t nonZeroCalibCounts, int32_t zeroCalibCounts, int32_t acclXRawCounts, int32_t acclYRawCounts, int32_t acclZRawCounts, int16_t acclXTiltDegrees, int16_t acclYTiltDegrees);

protected:

    // ************************************************************************
    // Events
    // ************************************************************************

    /**
     * Resize event.
     *
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

    /**
     * Show event.
     *
     * @param[in,out] e  event
     */
    //virtual void showEvent( QShowEvent *e );  dont rem what this was for.  Image related?

private Q_SLOTS:

    /// Slot for button clicked.
    void onClicked();

    /// Slot for clicking Use Pounds
    void onClickedLbs();

    /// slot for clicking Use Kilograms
    void onClickedKgs();

    /// Timeout if the verification step was never verified
    void moveOnWithoutVerification();

private:

    enum step                               // describes where I am in the process
    {
        SetupStep,                          // on starting program
        ScanStep,                           // Click START and scan step displays
        ClampStep,                          // NEW click load cell in place
        SpiderStep,                         // moved down 1
        SpiderStep_2,                       // NEW Spider view
        NEW_ConnectLoadCellStep,            // NEW 5 Connect the Load Cell
        NEW_ConnectAccelerometerStep,       // NEW 6 Connect Accelerometer
        NEW_InstallPlatterStep,             // NEW 7 Install Platter
        NEW_PowerOnStep,                    // NEW 8 Power on the Black Box and confirm status

        //BeginStep,                          // WAS STEP 5 (replace this step with Power On Step - Attach Spider in fixture (was lock loadcell)
        FlatStep,
        FlatStep30,     // 10
        FrontStep,
        FrontStep10,
        FrontStep30,

        RightStep,
        RightStep30,    // 15

        BackStep,
        BackStep10,
        BackStep30,

        LeftStep,
        LeftStep30,     // 20

        WriteStep,
        VerifyStep,
        CompleteStep,
        StartOver       // 24
    };

    enum ValueMaxReq
    {
        VMREQ_ASSEMBLY,       // 0
        VMREQ_HWVERSION,      // 1
        VMREQ_SERIALNUM,      // 2
        VMREQ_DATE,           // 3
        VMREQ_XGAINFACTOR,    // 4
        VMREQ_YGAINFACTOR,    // 5
        VMREQ_ZGAINFACTOR,    // 6
        VMREQ_CALIBRATION,    // 7 - maybe use this one to write?
        VMREQ_CURRENTFACTORS  // 8 - use this one for reading the thingy.
    };

    enum weigher_mode
    {
        SCALE_PREPACK_MODE,
        SCALE_WEIGHT_TRACKING_MODE,
        SCALE_DUMB_MODE,
        SCALE_MANUAL_MODE
    };


    QTimer verifyStepIsVerified_;
    QTimer verifyTestStepIsVerified_;
    QTimer verifyTestStepIsVerifiedWritten_;

    Ui::calibui *ui;

    kpc *device_;

    int step_;                              // What step of the process am I in

    fit_flash_section saved_f;

    QVector<QPixmap> stepPhotos_;           // display photos 1-6
    QStringList photoLabelMsg_;             // label instructions   1-6
    QStringList photoButtonMsg_;            // button instructions 1-6

    QLabel      *showPhoto_;                // pointer to the photo
    QLabel      *explainPhoto_;             // pointer to the instructions
    QPushButton *nextPhoto_;                // pointer to the next step

    QPushButton *loadFileValidate_;

    /// New values - eventually replace them with a victor
    QString accx_level_;
    QString accy_level_;
    QString accz_level_;
    QString accx_front_;
    QString accy_front_;
    QString accz_front_;
    QString accx_left_;
    QString accy_left_;
    QString accz_left_;
    QString accx_back_;
    QString accy_back_;
    QString accz_back_;
    QString accx_right_;
    QString accy_right_;
    QString accz_right_;

    /// Test values
    QString saved_fg_;
    QString saved_fm_;
    QString saved_fzerovalue_;
    QString saved_fspanvalue_;
    QString saved_fZ0_;
    QString saved_fD_;
    QString saved_fx2_;
    QString saved_fx1_;
    QString saved_fx0_;
    QString saved_fy2_;
    QString saved_fy1_;
    QString saved_fy0_;
    QString saved_fxrot0_;
    QString saved_fxrot1_;
    QString saved_fxrot2_;
    QString saved_fyrot0_;
    QString saved_fyrot1_;
    QString saved_fyrot2_;
    QString saved_fzrot0_;
    QString saved_fzrot1_;
    QString saved_fzrot2_;
    QString saved_fxgain_;
    QString saved_fygain_;
    QString saved_fgcoeff_;
    QString saved_fchecksum_;

    QString read_g_;
    QString read_m_;
    QString read_zerovalue_;
    QString read_spanvalue_;
    QString read_Z0_;
    QString read_D_;
    QString read_x2_;
    QString read_x1_;
    QString read_x0_;
    QString read_y2_;
    QString read_y1_;
    QString read_y0_;
    QString read_xrot0_;
    QString read_xrot1_;
    QString read_xrot2_;
    QString read_yrot0_;
    QString read_yrot1_;
    QString read_yrot2_;
    QString read_zrot0_;
    QString read_zrot1_;
    QString read_zrot2_;
    QString read_xgain_;
    QString read_ygain_;
    QString read_gcoeff_;
    QString read_checksum_;

    QString counts_level_0kg_0lb_;
    QString counts_level_5kg_10lb_;
    QString counts_level_10kg_20lb_;
    QString counts_level_15kg_30lb_;

    QString counts_front_0kg_0lb_;
    QString counts_front_5kg_10lb_;
    QString counts_front_10kg_20lb_;
    QString counts_front_15kg_30lb_;

    QString counts_left_0kg_0lb_;
    QString counts_left_5kg_10lb_;
    QString counts_left_10kg_20lb_;
    QString counts_left_15kg_30lb_;

    QString counts_back_0kg_0lb_;
    QString counts_back_5kg_10lb_;
    QString counts_back_10kg_20lb_;
    QString counts_back_15kg_30lb_;

    QString counts_right_0kg_0lb_;
    QString counts_right_5kg_10lb_;
    QString counts_right_10kg_20lb_;
    QString counts_right_15kg_30lb_;

    uint16_t flags_;
    uint32_t rawAtoDCounts_;
    int32_t nonZeroCalibCounts_;
    int32_t zeroCalibCounts_;
    int32_t acclXRawCounts_;
    int32_t acclYRawCounts_;
    int32_t acclZRawCounts_;
    int16_t acclXTiltDegrees_;
    int16_t acclYTiltDegrees_;

    // class variables
    bool passVerification_;                 // true if the written GainFactors equal the calculated averages
    bool timeout_error_;
    bool verifyReadings_;
    bool usePounds_;
    bool freezeReads_;
    bool receivedFitParams_;

    QString units_;
    QStringList unitsSize_;

    // These are the values that are READ FROM the board to be verified
    QString  readSerialNum_;
    QString  readHWVersion_;
    QString  readAssembNum_;
    uint32_t readDateOfMfg_;

    // These are the values that are WRITTEN TO the board
    QString  writtenSerialNum_;
    QString  writtenHWVersion_;
    QString  writtenAssembNum_;
    //QString  writtenDateOfMfg_;
    uint32_t writtenDateOfMfg_; // why did I originally make this a uint?

    bool isHillsboroGravity_;   // using the Troy or Hillsboro gravity constant
    uint32_t grav_const_;       // The gravity at the current location
    uint32_t curTimeSecs_;      // use epoch seconds as they are
    QString formattedDate_;     // QString epochHexDate_;      // not using now

    int serialFrames_;          // used to piece together the 2 frames used for teh serial number
    int assemblyFrames_;        // used to piece together the 2 frames used for the assembly number
    int countmsgs_;             // How many times have we done this

    failure *warningError_;

    QFile outfile_;

    /// Initialize.
    void initialize();

    void updateLoadCellData(int32_t acc_x, int32_t acc_y, int32_t acc_z, uint32_t counts);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// weigher testing

    enum weigherMessage
    {
        wgWakeup = 0x400,
        wgReqWakeup = 0x401,
        wgStatus = 0x402,

        WgValueMaxRequest = 0x420,
        WgValueMaxAssem = 0x421,
        WgValueMaxWriteAssem = 0x423,
        WgValueMaxHwVersion = 0x425,
        WgValueMaxWriteHwVersion = 0x426,
        WgValueMaxSerial = 0x427,
        WgValueMaxWriteSerial = 0x429,
        WgValueMaxFitParameters = 0x430,
        WgValueMaxDate = 0x42b,                                         // 1067
        WgValueMaxWriteDate = 0x42c,

        wgMode = 0x451,
        wgReqStatus = 0x452,                                            // NEW request accelerometer
        wgEnable = 0x454,
        wgRezero = 0x456,
        WgValueMaxWriteFitParameters = 0x461                            // 1121
    };

    #define CAN_MAX_DLEN 8
    typedef QPair<unsigned int, unsigned int> rangeType;
    typedef QList<unsigned int> messageSizeList;
    typedef QPair<rangeType, unsigned int> messageInfo;
    typedef QMap<unsigned int, messageInfo> messageInfoMap;
    typedef QMap<unsigned int, QByteArray> messageMap;

    messageInfoMap messageInfo_;
    messageMap messages_;

    void testThis();
    void addMessage( const rangeType &range, const messageSizeList &sizes );

    /// Initialize text for steps based on units
    void initializeText();

    /// Create layout in case I lose my mind.
    void createLayout();

    /// Accelerometer Values.
    std::vector<double> accValues() const;

    /// Weigher Counts Values.
    std::vector<double> weigherValues() const;

    /// Read from the Weighter the x,y, and z Gain Factors at a specific step.
    /**
     * @param[in] current_step  the current step to read in the calibration procedure
     */
    void readCurrentCalibrationSettings(int current_step);

    /// Set the calibration to a specific step and update instructions
    /**
     * @param[in] current_step  the current step to set the text and picture to in the procedure
     */
    void setNextCalibrationStep(int current_step);

    /// Loads and draw the current pixmap showing the current instruction
    /// Read from the Weighter the x,y, and z Gain Factors at a specific step.
    /**
     * @param[in] current_step  the image to display at the current step
     */
    void setPhotoStep(int current_step);

    // reset all values so we can do the next test
    void initializeTestParameters();

    /// Counts are received all of the time and this method saves the current status of the current counts
    void saveCurrentCounts(int current_step);

    /// Receives all readback values to see if we can read back what we wrote to see if calibrations passes
    QString passOrFailCalibration();

    /// Write all of the calibration data to the weigher
    void writeCalibrationToFlash();

    /// Saves all of the fields scanned with the scanner on the C.O.P.
    void writeScannedFields();

    /// Displays all errors that can occur while writing and verifying the data written to the accelerometer
    void popErrorDialog(QString err_msg, bool popItNoMatterWhat = false);

    /// formats the scanned fields in case they dont follow their own rules
    QString formatScannedField(QString scanned_field, int format_len);

    void sleepFor( int ms ) const;

    //validate that the user rotated the device properly
    bool countsValid(int step);

    void highlightDataLabels(bool acc_x, bool acc_y, bool acc_z, bool counts);
};

#endif // CALIBUI_H
