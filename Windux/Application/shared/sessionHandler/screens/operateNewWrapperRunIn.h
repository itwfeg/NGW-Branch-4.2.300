/**
 * @file operateNewWrapperRunIn.h
 * @brief Sets the wrapper into run in mode
  *
  * @copyright Copyright (c) 2014 Hobart Corporation
  * @author Alex Zavala
  * @addtogroup Operate Mode CSU
  * @{
*/
#ifndef OPERATENEWWRAPPERRUNIN_H
#define OPERATENEWWRAPPERRUNIN_H

#include "screenwidget.h"
#include "../platform/python.h"

class MadpEvolWrapperMotorProfile;
class QLabel;
class QPushButton;
class QTimer;

namespace Ui {
class operateNewWrapperRunIn;
}

/**
* @class operateNewWrapperRunIn
* @brief  Class to put the wrapper in run-in mode
*
*/

class operateNewWrapperRunIn  : public ScreenWidget
{
    Q_OBJECT

    typedef operateNewWrapperRunIn _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit operateNewWrapperRunIn(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~operateNewWrapperRunIn();

    // SOF-4216
    void setRunInMaxCycles( int maxCycles );

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private Q_SLOTS:

    /// slot for resetting the cycle counts
    void onCycleCountsClicked();

    /// Slot for timer timeout.
    void onTimerTimeout();

    // SOF-4217
    void generateRunInReport();

protected:

    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private:

    const static int MAX_TIMINGS = 22;
    static const int TITLE_FONT_POINT_SIZE = 26;
    static const int BUTTON_FONT_POINT_SIZE = 22;
    static const int FONT_POINT_SIZE = 18;

    Ui::operateNewWrapperRunIn *ui;

    python *pythonProcess;

    QMap<int, QLabel*> valueLabels_;                    // all QLabel fields on the screen
    QMap<int, QString> currentTimings_;                 // current incoming values
    QMap<int, QString> savedTimings_;                   // read from saved file
    QMap<int, QMap<int, int> > validateRange_;          // upper and lower range for a field

    QLabel *knifeUp_;
    QLabel *knifeDown_;
    QLabel *gripperNarrowClose_;
    QLabel *gripperNarrowOpen_;
    QLabel *gripperWideLeftClose_;
    QLabel *gripperWideLeftOpen_;
    QLabel *gripperWideRightClose_;
    QLabel *gripperWideRightOpen_;
    QLabel *sideClampLeftClose_;
    QLabel *sideClampLeftOpen_;
    QLabel *sideClampRightClose_;
    QLabel *sideClampRightOpen_;
    QLabel *wandUp_;
    QLabel *wandDown_;
    QLabel *filmSelectorUp_;
    QLabel *filmSelectorDown_;
    QLabel *mainPressure_;
    QLabel *linePressure_;
    QLabel *vacuum_;
    QLabel *applierHand_;

    QLabel *lastSavedLabel_;
    QLabel *lastSaved_;

    QPushButton *lblCycleCountsTitle_;
    QPushButton *cycleCounts_;
    long     runinCycleCounter_;
    long     msgCounter_;

    QTimer *saveCycleCountsTimer_;
    QTimer *fakeCycleTimer_;

    /**
     * @brief set variables, connections, pointers, and everything else up
     * @param none
     */
    void initialize();

    /**
     * @brief highlight highlights a mechanism based on its status
     * @param label the label to be highlighted
     * @param status the status of the mechanism
     */
    void highlight(QLabel* label, int status);

    /**
     * @brief highlightGreen highlights a label green
     * @param label the label to be highlighted
     */
    void highlightGreen(QLabel* label);

    /**
     * @brief highlightYellow highlights a label yellow
     * @param label the label to be highlighted
     */
    void highlightYellow(QLabel* label);

    /**
     * @brief highlightRed highlights a label red
     * @param label the label to be highlighted
     */
    void highlightRed(QLabel* label);

    /**
     * @brief updateValues updates the fields with the motorCapacityUsage and status
     * @param msg the MadpEvolWrapperMotorProfile msg sent from the session
     */
    void updateValues(MadpEvolWrapperMotorProfile msg);

    /**
     * @brief determine if label out of range. If it is, flip it red, otherwise normal
     * @param index maps to the enum the field represents
     * @param curValue is the value of the field to displayed
     * @param checkLabel represents the label/field to update
     */
    void outOfRangeLabel(int index, int curValue, QLabel *checkLabel );

    /// Test if a pair of values are within 10 points of one another.
    /**
    * @brief Test if a pair of values are within 10 points of one another.
    * @param index maps to the enum the field represents
    * @param curVal is the value of the field to displayed
    */
    void inRangeByTenLabel(int index, bool curVal = true);

    /**
     * @brief Save Timings to a file for historic use to compare against current timings.
     * @param none
     */
    void saveCycleCountsToFile();

    /**
     * @brief Read Timings from a file to display on the screen to compare against current timings
     * @param none
     */
    void readCycleCountsFromFile();

    /// Save Cycle Count to a file for historic usage
    /**
     * @param none
     */
    void saveCountsToFile();

    /// Save Cycle Count to a file for historic usage
    /**
     * @param ctr - the cycle count text to display
     */
    void updateCycleCounterText(QString ctr);

    void createFakeData();

    void createFakeCycleCounter();

    typedef enum mechanism_t
    {
        Primary     = 0,
        Secondary   = 1,
        Elevator    = 2,
        FrontFolder = 3,
        RearFolder  = 4,
        Pusher      = 5,
        SideFolder  = 6,
        Gripper     = 7,
        SideClamps  = 8,
        SealerBelt  = 9
    } mechanism;

    // SOF-4217
    int maxRunInCycles_;
    QString serialNumber_;
    QString applicationVer_;
    QStringList appVersionList_;
    QStringList bootVersionList_;

    // SOF-4216
    QString strMaxRunInCycles;
    void checkForLimitCycles();
};
#endif // OPERATENEWWRAPPERRUNIN_H
