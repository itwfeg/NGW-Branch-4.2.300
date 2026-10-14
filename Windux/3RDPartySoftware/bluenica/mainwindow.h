/**
 * @file mainwindow.h
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef MAINWINDOW
#define MAINWINDOW

#include <QMainWindow>

class configurationWidget;
class sensorEditorWidget;
class sensorPointEditorWidget;
class sensorPointSelectionWidget;
class sensorSelectionWidget;
class temperatureViewerWidget;
class tempProfileEditorWidget;
class tempProfileSelectionWidget;

class QAbstractButton;
class QDialogButtonBox;
class QPushButton;
class QStackedWidget;
class QTabWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Main window.
class mainWindow : public QMainWindow
{
    Q_OBJECT

    typedef mainWindow _Myt;
    typedef QMainWindow _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    mainWindow( QWidget *parent = 0 );

    /// Destructor.
    virtual ~mainWindow();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate text.
    virtual void translate();

protected:

    /// States.
    enum state
    {
        STATE_UNKNOWN,                              ///< Unknown state.
        STATE_DASHBOARD,                            ///< Dashboard.
        STATE_SENSOR_POINT_CONFIGURATION,           ///< Sensor point configuration.
        STATE_SENSOR_POINT_NEW,                     ///< Create new sensor point.
        STATE_SENSOR_POINT_TEMP_GRAPH,              ///< Sensor point temp graph.
        STATE_SENSOR_TEMP_GRAPH,                    ///< Sensor temp graph.
        STATE_TEMP_PROFILE_CONFIGURATION,           ///< Temperature profile configuration.
        STATE_TEMP_PROFILE_NEW,                     ///< Create new temperature profile.
        STATE_CONFIGURATION                         ///< Configuration.
    };

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve currently selected sensor point id.
    /**
     * @return  sensor point id
     */
    virtual unsigned int selectedSensorPoint() const;

    /// Retrieve currently selected sensor id.
    /**
     * @return  sensor id
     */
    virtual unsigned int selectedSensor() const;

    /// Retrieve currently selected temp profile id.
    /**
     * @return  temp profile id
     */
    virtual unsigned int selectedTempProfile() const;

    /// Set current state.
    /**
     * @param[in] value  state
     */
    virtual void setCurrentState( state value );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Remove all buttons.
    virtual void removeAllButtons();

private Q_SLOTS:

    /// Slot for button clicked.
    void onButtonClicked( QAbstractButton *button );

    /// Slot for current changed.
    void onCurrentChanged( int id );

    /// Slot for item double clicked.
    void onItemDoubleClicked( unsigned int id );

    /// Slot for item selection changed.
    void onItemSelectionChanged();

private:

    state state_;

    QWidget *centralWidget_;

    QStackedWidget *stackedWidget_;
    QTabWidget *dashboard_;
    sensorPointEditorWidget *sensorPointConfiguration_;
    tempProfileEditorWidget *tempProfileConfiguration_;
    configurationWidget *configuration_;
    temperatureViewerWidget *tempGraph_;

    sensorPointSelectionWidget *sensorPointSelection_;
    sensorSelectionWidget *sensorSelection_;
    tempProfileSelectionWidget *tempProfileSelection_;

    QDialogButtonBox *buttons_;
    QPushButton *back_;
    QPushButton *config_;
    QPushButton *delete_;
    QPushButton *edit_;
    QPushButton *exit_;
    QPushButton *new_;
    QPushButton *refresh_;
    QPushButton *save_;
    QPushButton *showPrev12h_;
    QPushButton *showPrev24h_;
    QPushButton *show1Day_;
    QPushButton *show1Week_;
    QPushButton *show2Weeks_;
    QPushButton *show3Days_;
    QPushButton *view_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Retrieve the UTC offset in seconds.
    static int utcOffset();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // MAINWINDOW
