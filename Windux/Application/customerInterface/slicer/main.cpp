#include <QApplication>
#include <QTranslator>
#include <QDesktopWidget>
#include <QSettings>
#include <QDebug>
#include <QFileInfo>

#include "simulatedscaleinterface.h"
#include "scale.h"
#include "slicerstatemachine.h"
#include "inputwidgethook.h"
#include "mainwindow.h"
#include "message.h"
#include "slicerstatistics.h"
#include "configsettings.h"

#ifdef USING_RPI
#include "rpi/kniferunin.h"
#include "rpi/onpausebuttonsim.h"
#include "rpi/rotaryencoderknob.h"
#include "rpi/button.h"
#include "rpi/led.h"

#ifdef USING_EK_12KI
#include "and/and_ek12ki_serialinterface.h"
#include "and/hx711.h"
#endif

#endif

#ifdef USING_HOBART
#include "hobart/knifepower.h"
#include "hobart/led.h"
#include "hobart/usbweigher.h"
#endif

#include <iostream>
#include "msgbox.h"

//supported arguments:
//  -sim: simulate knife run using buttons on the UI
//  -noFrame: display the window as frameless
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    const QStringList args( a.arguments() );
    const bool simulate( args.contains("-sim") );
    const bool disp10( args.contains("-10") );
    const bool disp7( args.contains("-7") );

    QCoreApplication::setOrganizationName("Hobart");
    QCoreApplication::setOrganizationDomain("hobartcorp.com");
    QCoreApplication::setApplicationName("Slicer");
    QSettings::setPath(QSettings::NativeFormat, QSettings::UserScope, configurationRoot);

    QFont new_font = QApplication::font();
    new_font.setPointSize( 10 );
    QApplication::setFont( new_font );


#if 0
    SlicerStatistics  mstat(nullptr);

    qDebug() << "WritingSliceTime";
    mstat.writeStatistic(SlicerStatistics::SlicerRunningTime, 55);
    qDebug() << "done";
    qDebug() << "ReadingSliceTime";
    qDebug() << mstat.readStatistic(SlicerStatistics::SlicerRunningTime);

    while(1);
#endif
    ScaleInterface* pScaleInterface( NULL );

    KnifeRunIn *pKnifeRunIn( NULL );
    OnPauseButtonSim *pPauseButton( NULL );

    if ( simulate )
        pKnifeRunIn = new KnifeRunIn();

#ifdef USING_RPI
    std::cout << "using rpi" << std::endl;

    wiringPiSetup();

#ifdef USING_EK_12KI
    if ( !pScaleInterface )
        pScaleInterface = new AND_EK12Ki_SerialInterface();
#endif

    if ( !pKnifeRunIn )
        pKnifeRunIn = new RaspPiKnifeRunIn( 10 );
    if ( !pPauseButton )
        pPauseButton = new RaspPiOnPauseButtonSim( 7 );
#endif

#ifdef USING_HOBART
    std::cout << "using hobart" << std::endl;

    if ( !pScaleInterface )
        pScaleInterface = new HobartUsbWeigher();

    if ( !pKnifeRunIn )
        pKnifeRunIn = new KnifeRunIn();
    if ( !pPauseButton )
        pPauseButton = new OnPauseButtonSim();
#endif

    // validate setup
    if ( !pScaleInterface )
    {
        std::cerr << "no scale interface defined!" << std::endl;
        return -1;
    }
    else if ( !pKnifeRunIn )
    {
        std::cerr << "no knife interface defined!" << std::endl;
        return -1;
    }
    else if ( !pPauseButton )
    {
        std::cerr << "no button interface defined!" << std::endl;
        return -1;
    }

    //if touchscreen found, hide cursor
    //if mouse found, show cursor
    //if neither found, wait for input
    QFileInfo touchscreen_file("dev/input/touchscreenusb");
    QFileInfo service_mouse_file("dev/input/mouse1");
    while(!touchscreen_file.exists() && !service_mouse_file.exists()){
        //do nothing
    }
    if(service_mouse_file.exists()){
        qDebug() << "Found mouse";
        QApplication::setOverrideCursor( QCursor( Qt::ArrowCursor ) );
    }else{
        qDebug() << "Found touch";
        QApplication::setOverrideCursor( QCursor( Qt::BlankCursor ) );
    }

    //instantiate model
    Scale scale(*pScaleInterface);
    SlicerStateMachine slicerStateMachine(*pPauseButton, scale);

    //initialize global keyboard
    inputWidgetHook iwh;

    //initialize
    MainWindow w(simulate);
    w.slt_SetpointChanged(scale.Setpoint());
    w.slt_IncrementModeChanged(slicerStateMachine.IncrementMode());
    scale.slt_IncrementModeChanged(slicerStateMachine.IncrementMode());

    //wire model to view, and vice versa
    QObject::connect( &scale, SIGNAL(sig_WeightChanged(ScaleInterface::State,ScaleInterface::weightInPounds_t)), &w, SLOT(slt_WeightUpdated(ScaleInterface::State,ScaleInterface::weightInPounds_t)));
    QObject::connect( &scale, SIGNAL(sig_Zeroing()), &w, SLOT(slt_Zeroing()));
    QObject::connect( &scale, SIGNAL(sig_AtOrAboveSetpoint()), &w, SLOT(slt_WeightReached()));
    QObject::connect( &scale, SIGNAL(sig_WeigherKeyPressed()), &w, SLOT(slt_WeigherKeyPressed()));
    QObject::connect( &scale, SIGNAL(sig_WeigherConfigChanged(WeigherConfiguration&)), &w, SLOT(slt_WeigherConfigChanged(WeigherConfiguration&)));
    QObject::connect( &scale, SIGNAL(sig_WeigherStatus(char*,int)), &w, SLOT(slt_WeigherStatus(char*,int) ));
    QObject::connect( &scale, SIGNAL(sig_isKnifeRunning(bool)), &slicerStateMachine, SLOT(slt_isKnifeRunning(bool)));
    QObject::connect( &scale, SIGNAL(sig_InfinityMode(Scale::InfinityModeState)), &w, SLOT(slt_RetrieveInfinityMode(Scale::InfinityModeState)));
    QObject::connect( &scale, SIGNAL(sig_TimeoutPopup()), &w, SLOT(slt_TimeoutPopup()));

    QObject::connect( &slicerStateMachine, SIGNAL(sig_SetpointChanged(ScaleInterface::weightInPounds_t)), &w, SLOT(slt_SetpointChanged(ScaleInterface::weightInPounds_t)));
    QObject::connect( &slicerStateMachine, SIGNAL(sig_IncrementModeChanged(SlicerStateMachine::incrementMode_e)), &w, SLOT(slt_IncrementModeChanged(SlicerStateMachine::incrementMode_e)));
    QObject::connect( &slicerStateMachine, SIGNAL(sig_Stopped()), &w, SLOT(slt_Stopped()));
    QObject::connect( &slicerStateMachine, SIGNAL(sig_Running()), &w, SLOT(slt_Running()));
    QObject::connect( &slicerStateMachine, SIGNAL(sig_SetpointReached()), &w, SLOT(slt_SlicingComplete()));
    QObject::connect( &slicerStateMachine, SIGNAL(sig_Paused()), &w, SLOT(slt_Paused()));
    QObject::connect( &slicerStateMachine, SIGNAL(sig_Error(ScaleInterface::State)), &w, SLOT(slt_Error(ScaleInterface::State)));

    QObject::connect( &w, SIGNAL(sig_SimulateKnifeRunInAssert()), pKnifeRunIn, SLOT(slt_SimulateAssert()));
    QObject::connect( &w, SIGNAL(sig_SimulateKnifeRunInDeAssert()), pKnifeRunIn, SLOT(slt_SimulateDeAssert()));
    QObject::connect( &w, SIGNAL(sig_IncrementSetpoint()), &slicerStateMachine, SLOT(slt_IncrementSetpoint()));
    QObject::connect( &w, SIGNAL(sig_DecrementSetpoint()), &slicerStateMachine, SLOT(slt_DecrementSetpoint()));
    QObject::connect( &w, SIGNAL(sig_RezeroWeigher()), &slicerStateMachine, SLOT(slt_Zero()));
    QObject::connect( &w, SIGNAL(sig_CycleIncrementMode()), &slicerStateMachine, SLOT(slt_CycleIncrementMode()));
    QObject::connect( &w, SIGNAL(sig_CheckUpdateSlicerType()), &slicerStateMachine, SLOT(slt_CheckUpdateSlicerType()));
    QObject::connect( &w, SIGNAL(sig_updateSetPoint(double)), &scale, SLOT(slt_updateSetPoint(double)));
    QObject::connect( &w, SIGNAL(sig_updateWeightTolerance(double)), &scale, SLOT(slt_updateWeightTolerance(double)));
    QObject::connect( &w, SIGNAL(sig_ReqConfig()), &scale, SLOT(slt_ReqConfig()));
    QObject::connect( &w, SIGNAL(sig_WriteConfig(WeigherConfiguration&)), &scale, SLOT(slt_WriteConfig(WeigherConfiguration&)));
    QObject::connect( &w, SIGNAL(sig_SetWeigherMode(WeighingMode)), &scale, SLOT(slt_SetWeigherMode(WeighingMode)));
    QObject::connect( &w, SIGNAL(sig_WeigherReset()), &scale, SLOT(slt_WeigherReset()));
    QObject::connect( &w, SIGNAL(sig_IncrementModeChanged(int)), &scale, SLOT(slt_IncrementModeChanged(int)));
    QObject::connect( &w, SIGNAL(sig_Cancel()), &slicerStateMachine, SIGNAL(sig_CancelHalt()));
    QObject::connect( &w, SIGNAL(sig_IncrementStepChanged(ScaleInterface::weightInPounds_t)), &scale, SLOT(slt_IncrementStepChanged(ScaleInterface::weightInPounds_t)));
    QObject::connect( &w, SIGNAL(sig_ModeChanged(int)), &scale, SLOT(slt_ModeChanged(int)));


#ifdef USING_RPI
    //instantiate view
    RotaryEncoderKnob knob(4, 5);
    Button button(1);
    LED led(0, 3, 2);

    (void)QObject::connect(&knob, SIGNAL(sig_Increment()), &slicerStateMachine, SLOT(slt_IncrementSetpoint()));
    (void)QObject::connect(&knob, SIGNAL(sig_Decrement()), &slicerStateMachine, SLOT(slt_DecrementSetpoint()));

    (void)QObject::connect(&button, SIGNAL(sig_ShortClick()), &slicerStateMachine, SLOT(slt_Zero()));
    (void)QObject::connect(&button, SIGNAL(sig_LongClick()), &slicerStateMachine, SLOT(slt_CycleIncrementMode()));
    (void)QObject::connect(&button, SIGNAL(sig_ShortClick()), &w, SLOT(slt_ButtonClick()));

    (void)QObject::connect(&slicerStateMachine, SIGNAL(sig_Stopped()), &led, SLOT(slt_Off()));
    (void)QObject::connect(&slicerStateMachine, SIGNAL(sig_Running()), &led, SLOT(slt_Blue()));
    (void)QObject::connect(&slicerStateMachine, SIGNAL(sig_SetpointReached()), &led, SLOT(slt_Green()));
    (void)QObject::connect(&slicerStateMachine, SIGNAL(sig_Paused()), &led, SLOT(slt_Off()));
    (void)QObject::connect(&slicerStateMachine, SIGNAL(sig_Error()), &led, SLOT(slt_FlashRed()));
#endif

#ifdef USING_HOBART
    KNIFE knife("/usr/local/gpio/slicer_power");
    LED led("/usr/local/gpio/red_led", "/usr/local/gpio/red_blue", "/usr/local/gpio/green_led");

    (void)QObject::connect(&slicerStateMachine, SIGNAL(sig_Stopped()), &knife, SLOT(slt_Off()));
    (void)QObject::connect(&slicerStateMachine, SIGNAL(sig_Running()), &knife, SLOT(slt_On()));
    (void)QObject::connect(&slicerStateMachine, SIGNAL(sig_Paused()), &knife, SLOT(slt_Off()));
    (void)QObject::connect(&slicerStateMachine, SIGNAL(sig_Error(ScaleInterface::State)), &knife, SLOT(slt_Off()));
    (void)QObject::connect(&slicerStateMachine, SIGNAL(sig_CancelHalt()), &knife, SLOT(slt_Off()));

    (void)QObject::connect(&slicerStateMachine, SIGNAL(sig_Stopped()), &led, SLOT(slt_Off()));
    (void)QObject::connect(&slicerStateMachine, SIGNAL(sig_Running()), &led, SLOT(slt_Blue()));
    (void)QObject::connect(&slicerStateMachine, SIGNAL(sig_SetpointReached()), &led, SLOT(slt_Green()));
    (void)QObject::connect(&slicerStateMachine, SIGNAL(sig_Paused()), &led, SLOT(slt_Off()));
    (void)QObject::connect(&slicerStateMachine, SIGNAL(sig_Error(ScaleInterface::State)), &led, SLOT(slt_FlashRed()));
#endif

    w.setWindowFlags(Qt::FramelessWindowHint);

    if ( disp10 )
        w.setFixedSize( QSize( 1024, 640 ) );
    else if ( disp7 )
        w.setFixedSize( QSize( 480, 800 ) );
    else
    {
        QDesktopWidget *desktop( a.desktop() );

        w.setFixedSize( desktop->size() );
        w.setFixedSize( QSize( 480, 800 ) );
    }



    w.show();
    system("echo 1 >> /usr/local/gpio/on_pause_control/value");
    return a.exec();
    system("echo 0 >> /usr/local/gpio/on_pause_control/value");
}
