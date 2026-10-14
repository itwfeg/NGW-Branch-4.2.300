#ifndef WRAPPERSERVICELABELAPPLIERPANEL_H
#define WRAPPERSERVICELABELAPPLIERPANEL_H

#include "modepanel.h"

class QSignalMapper;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Panel for service mode label applier config and TOF sensor calibration
/// ( Added for the 2nd label applier PJR-183 )
///////////////////////////////////////////////////////////////////////////////////////////////////

class WrapperServiceModeLAPanel : public ModePanel
{
    Q_OBJECT

    typedef WrapperServiceModeLAPanel _Myt;
    typedef ModePanel _Mybase;

public:
    enum LabelApplierMode
    {
        PANEL_ID = 300,                             // do not modify, add enums after
        LabelApplierConfig
    };

    explicit WrapperServiceModeLAPanel( int testSuite, SessionMainWindow *mainWindow, SessionDialog *parent = 0 );
    virtual ~WrapperServiceModeLAPanel();

    virtual QSize sizeHint() const;

protected:
    virtual hobart::backgroundWidgetBase *rendererTraits() const;
    virtual QPushButton *createNamedButton( pushButtonList& buttons, const QString& buttonText, const QString& buttonObjectName, int requiredAccessLevel = 0, bool selected = false );

private Q_SLOTS:
    void onBackClicked();

private:
    QSignalMapper *signalMapper_;
    pushButtonList labelApplier_;
    LabelApplierMode mode_;

    static const int buttonsPerRow_;

    void initialize();
    void initializeButtons();

};

#endif // WRAPPERSERVICELABELAPPLIERPANEL_H
