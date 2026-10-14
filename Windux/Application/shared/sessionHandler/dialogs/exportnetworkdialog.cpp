/**
 * @file exportnetworkdialog.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>
#include <filesystem.h>

#include "exportnetworkdialog.h"

#include "../sessioninfo.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QFileInfo>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
ExportNetworkSettingsDialog::ExportNetworkSettingsDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f )
{
    // set object name
    setObjectName( "ExportNetworkSettingsDialog" );

    // initialize
    initialize();
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ExportNetworkSettingsDialog::~ExportNetworkSettingsDialog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize ExportNetworkSettingsDialog::sizeHint() const
{
    return QSize(
        (mainWindow_->width() * 640) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 350) / SessionMainWindow::scalingDividerY_ );
}

void ExportNetworkSettingsDialog::onNTPRemoveClicked()
{

    if(this->ntpRemove_->isChecked())
    {
        this->ntpInfo_->setChecked(false);
    }
    manageNetworkDeviceExport();
}

void ExportNetworkSettingsDialog::onNTPInfoClicked()
{

    if(this->ntpInfo_->isChecked())
    {
        this->ntpRemove_->setChecked(false);
    }
    manageNetworkDeviceExport();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ExportNetworkSettingsDialog::onButtonClicked()
{
    if ( nowServing_ == sender() )
    {
        exportType_ = CurrentNetworkSettings;
        if(this->networkInfo_->isChecked())
        {
            exportNetwork_ = networkYes;
        }
        else exportNetwork_ = networkNo;

        if(this->hostnameInfo_->isChecked())
        {
            exportHostname_ = hostnameYes;
        }
        else exportHostname_ = hostnameNo;

        if(this->ntpInfo_->isChecked())
        {
            exportNTP_ = ntpYes;
        }
        else exportNTP_ = ntpNo;

        if(this->ntpRemove_->isChecked())
        {
            exportNTPRemove_ = ntpRemoveYes;
        }
        else exportNTPRemove_ = ntpRemoveNo;

        // Add export Network and hostname and ntp to output

        accept();
    }

    else if ( scale_ == sender() )
    {
        exportType_ = LastKnownWorkingNetworkSettings;
        accept();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ExportNetworkSettingsDialog::initialize()
{
    // create message
    const QString messageText( tr(
        "\n"
        "Export this scale's saved configured network\n"
        "settings for use on another HT Series device"
        "\n"));

    const QString choicesMessageText( tr(
        "\n"
        "Place a checkmark in each box beside the displayed settings that\n"
        "you want to export for a Network Device, e.g., Kiosk, Set Top Box:" ));

    const QString spacerMessage( "\n\n");

    // create widgets
    message_ = new QLabel( messageText, this );
    message_->setObjectName( "message" );

    spacer_ = new QLabel(spacerMessage, this);
    spacer_->setObjectName( "spacerMessage");

    choicesMessage_ = new QLabel(choicesMessageText, this);
    choicesMessage_->setObjectName("choicesMessage");

    networkInfo_ = new QCheckBox(tr("Export Network Settings"), this);
    networkInfo_->setObjectName("exportNetwork");
    // PIT-56
    //networkInfo_->setChecked(true);
    //networkInfo_->setEnabled(false);
    hostnameInfo_ = new QCheckBox(tr("Export Hostname Setting"), this);
    hostnameInfo_->setObjectName("exportHostname");
    hostnameInfo_->setChecked(false);

    ntpInfo_ = new QCheckBox(tr("Export NTP Settings"), this);
    ntpInfo_->setObjectName("exportNTP");
    ntpInfo_->setChecked(false);
    connect(ntpInfo_, SIGNAL(clicked(bool)), SLOT(onNTPInfoClicked() ));

    ntpRemove_ = new QCheckBox(tr("Remove NTP Settings"), this);
    ntpRemove_->setObjectName("exportNTPRemove");
    ntpRemove_->setChecked(false);
    connect(ntpRemove_, SIGNAL(clicked(bool)), SLOT(onNTPRemoveClicked()));
    connect(hostnameInfo_, SIGNAL(clicked(bool)), SLOT(manageNetworkDeviceExport() ));
    connect(networkInfo_, SIGNAL(clicked(bool)), SLOT(manageNetworkDeviceExport() ));

    nowServing_ = new QPushButton( tr( "Export for a Network Device" ), this );
    nowServing_->setObjectName( "nowServing" );
    nowServing_->setStyleSheet("padding: 7px;");
    connect( nowServing_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    scale_ = new QPushButton( tr( "Export for a Scale" ), this );
    scale_->setObjectName( "scale" );
    scale_->setStyleSheet("padding: 7px;");
    connect( scale_, SIGNAL(clicked()), SLOT(onButtonClicked()) );
    manageNetworkDeviceExport();
}

void ExportNetworkSettingsDialog::manageNetworkDeviceExport()
{
    if(!networkInfo_->isChecked() && !hostnameInfo_->isChecked() && !ntpInfo_->isChecked() && !ntpRemove_->isChecked())
    {
        nowServing_->setEnabled(false);
    }
    else
    {
        nowServing_->setEnabled(true);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ExportNetworkSettingsDialog::createLayout()
{
    QHBoxLayout *buttonLayout( new QHBoxLayout());
    buttonLayout->addStretch();
    buttonLayout->addStretch();

    QVBoxLayout *indentLayout( new QVBoxLayout());
    indentLayout->addWidget(ntpRemove_);
    indentLayout->setMargin(15);
    QVBoxLayout *choicesLayout( new QVBoxLayout());
    choicesLayout->addWidget(networkInfo_);
    choicesLayout->addWidget(hostnameInfo_);
    choicesLayout->addWidget(ntpInfo_);
    choicesLayout->addLayout(indentLayout);
    //choicesLayout->addWidget(ntpRemove_);

    QHBoxLayout * choicesHLayout(new QHBoxLayout());
    choicesHLayout->addStretch();
    choicesHLayout->addLayout(choicesLayout);
    choicesHLayout->addWidget(nowServing_);
    choicesHLayout->addStretch();

    QVBoxLayout *topLayout( new QVBoxLayout() );
    topLayout->addWidget( choicesMessage_);
    topLayout->addLayout(choicesHLayout);
    topLayout->addLayout(buttonLayout);
    topLayout->addStretch();

    QHBoxLayout *bottomLayoutCenter(new QHBoxLayout());
    bottomLayoutCenter->addStretch();
    bottomLayoutCenter->addWidget( scale_ );
    bottomLayoutCenter->addStretch();

    QHBoxLayout *bottomLayout( new QHBoxLayout() );
    //bottomLayout->addStretch();

    //bottomLayout->addWidget( nowServing_ );
    bottomLayout->addWidget( message_ );
    bottomLayout->addLayout(bottomLayoutCenter);
    bottomLayout->addWidget(spacer_);


    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    // SOF-484 Reverse the order of the selections
    screenLayout->addLayout( bottomLayout );
    screenLayout->addLayout( topLayout );
}
