#include "passwordentry.h"
#include "ui_passwordentry.h"
#include "configsettings.h"
#include "mainmenu.h"
#include <QSettings>
#include <QTimer>
#include <QDebug>

passwordEntry::passwordEntry(QWidget *parent, passwordType entryType) :
    QDialog(parent),
    ui(new Ui::passwordEntry)
{
    loginType=entryType;
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    initialize();
}

passwordEntry::~passwordEntry()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void passwordEntry::initialize()
{
    QString passwordTypeText="";
    passwordValid = false;

    connect(ui->cancelPushButton,SIGNAL(clicked(bool)),SLOT(reject()));
    connect(ui->loginPushButton,SIGNAL(clicked(bool)),SLOT(onLogin()));

    if (loginType == supervisor)
        passwordTypeText = tr("Supervisor");
    else if (loginType == service)
        passwordTypeText = tr("Service");

    ui->passwordEntrylabel->setText(tr("Enter ") + passwordTypeText + tr(" password"));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void passwordEntry::showEvent(QShowEvent *e)
{
    const QRect p( parentWidget()->geometry() );
    const QRect c( rect() );

    move( p.center() - c.center() );

    QDialog::showEvent( e );
}

bool passwordEntry::passwordConfigured(passwordType entryType)
{
    QSettings settings;

    bool passwordIsSet=false;

    if (entryType == supervisor)
    {
        if (settings.contains("supervisorPassword") && settings.value("supervisorPassword") != "")
            passwordIsSet = true;
    }
    else if (entryType == service)
    {
        if (settings.contains("servicePassword") && settings.value("servicePassword") != "")
            passwordIsSet = true;
    }
    return passwordIsSet;
}

bool passwordEntry::validPassword()
{
    return passwordValid;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void passwordEntry::onLogin()
{
    QString password="";
    QSettings settings;

    if (passwordConfigured(loginType))
    {
        //qDebug()<<"password is configured";

        if (loginType == supervisor)
        {
            password = settings.value("supervisorPassword").toString();
            //qDebug()<<"password is " + password;

            if (password == ui->passwordLineEdit->text())
            {
                //qDebug()<<"password valid";

                ui->alertLabel->setText(tr("Valid Password"));
                passwordValid = true;
                accept();
            }
            else
            {
                //qDebug()<<"password invalid";
                ui->alertLabel->setText(tr("Invalid Password"));
                ui->passwordLineEdit->setText("");
                passwordValid = false;
            }
        }

        else if (loginType == service)
        {
            // No requirement to support
        }
    }
}
