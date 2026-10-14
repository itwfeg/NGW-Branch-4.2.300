#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QVector>

#include "msgbox.h"
#include "scale.h"
#include "slicerstatemachine.h"
#include "hobart/usb/weigherconfiguration.h"
#include "speedkey.h"
#include <QList>


namespace Ui {
class MainWindow;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Main window.
class MainWindow : public QMainWindow
{
    Q_OBJECT

    typedef MainWindow _Myt;
    typedef QMainWindow _Mybase;

    Ui::MainWindow *ui_;



public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] simulate  @c true to simulate peripherals, @c false otherwise
     */
    explicit MainWindow( bool simulate, QWidget *parent = 0, Qt::WindowFlags flags = 0 );

    /// Destructor.
    virtual ~MainWindow();

    const QList<speedKey*> getSpeedKeys();
    const QList<message*>  getMessages();

    static const int MAX_PRECISION_LBS = 3;
    static const int MAX_PRECISION_OZ = 1;
    static const int MAX_PRECISION_KG = 3;
    static const int MAX_PRECISION_G = 0;

    /// Weigher display modes.
    typedef enum weigherDisplayMode
    {
        displayPounds,                              ///< Show pounds.
        displayOunces,                               ///< Show ounces.
        displayKg,                                   ///< Show kg.
        displayGrams                                ///< Show Grams.
    }WeigherDisplayMode;


public Q_SLOTS:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Setpoint changed.
    /**
     * @param[in] weight  setpoint weight
     */
    void slt_SetpointChanged( ScaleInterface::weightInPounds_t weight );

    /// Weight updated.
    /**
     * @param[in] state  weigher state
     * @param[in] weight  weigher weight
     */
    void slt_WeightUpdated( ScaleInterface::State state, ScaleInterface::weightInPounds_t weight );

    /// Weigher zeroing.
    void slt_Zeroing();

    /// Slicer running.
    void slt_Running();

    /// Slicer complete.
    void slt_SlicingComplete();

    /// Slicer paused.
    void slt_Paused();

    /// Slicer stopped.
    void slt_Stopped();

    /// Slicer error.
    void slt_Error(ScaleInterface::State);

    /// Popup for 30sec timeout (without stopping knife).
    void slt_TimeoutPopup();

    /// Slicer weight reached.
    void slt_WeightReached();

    /// Change increment mode.
    /**
     * @param[in] incrementMode  mode
     */
    void slt_IncrementModeChanged( SlicerStateMachine::incrementMode_e incrementMode );

    /// Handle infinity mode.
    void slt_RetrieveInfinityMode(Scale::InfinityModeState infMode);


    void slt_homepage();
    void slt_menupage();
    void setLabel(double iLabel);

    void slt_WeigherKeyPressed();

    void slt_WeigherConfigChanged(WeigherConfiguration &config);
    void slt_WriteConfig(WeigherConfiguration &config);

    void slt_ReqConfig();
    void slt_RezeroWeigher();
    void slt_WeigherStatus(char*buffer, int buffLen);
    void slt_SetWeigherMode(WeighingMode mode);
    void slt_WeigherReset();

    //Change system language from General Config
    void slt_UpdateLanguage(QString language);

    void onUpIconPressed();
    void onDownIconPressed();
protected:

    weigherDisplayMode mode_;                       ///< Display mode.

    SlicerStateMachine::incrementMode_e incrementMode;

    ScaleInterface::State scaleState_;            ///< Weigher state.
    float weight_;                                  ///< Current weight lb.
    float ounces_;                                  ///< Current weight oz.

    float prevWeight_;

    float setpointWeight_;                          ///< Setpoint lb.
    float setpointOunces_;                          ///< Setpoint oz.

    // ============================================================================================
    // Events
    // ============================================================================================

    /// Resize event
    /**
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

    // ============================================================================================
    // Methods
    // ============================================================================================

    /// Event filter
    /**
     * @param[in,out] obj  object
     * @param[in,out] e  event
     * @return  @c true to filter event, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *e );

    /// Update weight fields.
    virtual void updateWeight();

private slots:
    void slt_HandleSpeedKey();
    /// Scroll text.
    void slt_ScrollText();

    /// Handle flashing.
    void slt_HandleFlash();

    /// Handle changes in input type.
    void slt_CheckInputType();

    void slt_DisplayMessage(QString header, QString msg, QString lastCompleted, int uniqueId);

Q_SIGNALS:

    /// Run knife.
    void sig_SimulateKnifeRunInAssert();

    /// Stop knife.
    void sig_SimulateKnifeRunInDeAssert();

    /// Update weight.
    /**
     * @param[in] weight  weight
     */
    void sig_SimulateWeightUpdate( ScaleInterface::weightInPounds_t weight );

    /// Increment weight setpoint.
    void sig_IncrementSetpoint();

    /// Decrement weight setpoint.
    void sig_DecrementSetpoint();

    /// Cycle increment mode.
    void sig_CycleIncrementMode();

    /// Zero weigher.
    void sig_RezeroWeigher();

    ///weight mode
    void sig_weightmode(bool tt);

    ///update set point
    void sig_updateSetPoint(double value);

    void sig_updateWeightTolerance( double value );

    void sig_WeigherKeyPressed();

    void sig_ReqConfig();

    void sig_WeigherConfigChanged(WeigherConfiguration & config); //Emit signal to [NOT FINAL] mainmenu

    void sig_WriteConfig( WeigherConfiguration & config);

    void sig_WeigherStatus(char*buffer, int buffLen);

    void sig_SetWeigherMode(WeighingMode mode);

    void sig_WeigherReset();

    void sig_IncrementModeChanged(int incMode);

    void sig_CheckUpdateSlicerType();

    void sig_Cancel();

    void sig_IncrementStepChanged(ScaleInterface::weightInPounds_t);

    void sig_ModeChanged(int mode);

private:

    enum flashType
    {
        greenFlashing,
        redFlashing
    };

    enum inputType
    {
        none,
        touch,
        mouse
    };

    QTimer flashingTimer_;
    QTimer textScrollTimer_;
    WeigherConfiguration currentConfig;

    QVector<QString> scrollTexts_;
    int currentTextScrollIndex_;

    int inputType_;

    flashType flashType_;
    int flashTimes_;

    msgbox * mBox;
    ///Initialize
    void initialize();

    void setupSpeedKeys();

    void handleInfinityMode();

    Scale::InfinityModeState setpointInfState_;

    bool isRunning_;

    static const int m_number_chars_weight_displayed = 5;

    int m_digitsPrecision = 2;

    void checkUpdateWeigherModel();
    void updateMode();

    QPixmap invertPixmap(const QPixmap * src);

    int lastPressed;

};

#endif // MAINWINDOW_H
