/**
 * @file labeltypesscreen.h
 * Label types viewer.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef LABELTYPESSCREEN_H
#define LABELTYPESSCREEN_H

#include "screenwidget.h"

#include <label.h>
#include <mapdataobjectcontainer.h>

class LabelViewerWidget;

class QComboBox;
class QGroupBox;
class QLabel;
class QPushButton;
class QCheckBox;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class LabelTypesScreen
 * @brief Screen for viewing label types.
 */
class LabelTypesScreen : public ScreenWidget
{
    Q_OBJECT

    typedef LabelTypesScreen _Myt;
    typedef ScreenWidget _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit LabelTypesScreen( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~LabelTypesScreen();

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

protected:

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Handle screen changing.
     *
     * This method is called before the screen is changed to something new. If for some reason the
     * screen should not be unloaded and destroyed then a value of @c false should be returned.
     *
     * @returns  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private Q_SLOTS:

    /**
     * Slot for label selection.
     */
    void onLabelIndexChanged( const QString& value );

    /**
     * Slot for previous button.
     */
    void onPreviousButtonClicked();

    /**
     * Slot for next button.
     */
    void onNextButtonClicked();

    /**
     * Slot for delete button.
     */
    void onDeleteButtonClicked();

    /**
     * Slot for printer selection.
     */
    void onPrinterIndexChanged( const QString& value );

    /**
     * Update property indicating that the setup has changed and needs to be saved.
     */
    void onRotationChanged(int index);

    /**
     * Slot for pre-printed label checkbox selection.
     */
    void prePrintStateChange();

private:

    typedef dataObjectContainer<label> labelContainer;

    QLabel *labelType_;
    QComboBox *labelTypeCombo_;
    QPushButton *previousButton_;
    QPushButton *nextButton_;
    QPushButton *deleteButton_;
    QLabel *versionType_;
    QLabel *versionTypeValue_;

    bool noMinorVersion;

    LabelViewerWidget *labelViewer_;

    QGroupBox *selectedLabel_;
    QLabel *selectedLabelSize_;

    QGroupBox *loadedStock_;
    QLabel *printer_;
    QComboBox *printerCombo_;
    QLabel *loadedStockSize_;

    //QLabel *rotationOnLabelStockLabel_;
    //QComboBox *rotationOnLabelStockCombo_;

    //QCheckBox *cbPrimaryLabelPreprint_;
    QCheckBox *cbSecondLabelPreprint_;

    QLabel *cboSpacer_;

    QString labelUnits_;

    QMap<int, QString> labelMetrics_;

    bool prePrintStateChanged;
    bool hasSecondPrinter_;

    /**
     * Initialize.
     */
    void initialize();

    /**
     * Update label type combo dropdown with labels in data store.
     */
    bool updateLabelTypeCombo();

    /**
     * Update OperatorDisplayOptions Config when the Pre-printed label checkboxes are selected.
     */
    void savePreprintConfig();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // LABELTYPESSCREEN_H
