#include "failure.h"
#include "ui_failure.h"

failure::failure(QWidget *parent, QString warningText) :
    QDialog(parent),
    ui(new Ui::failure)
{
    ui->setupUi(this);

    details_ = warningText;

    initialize();
}

failure::~failure()
{
    delete ui;
}

void failure::initialize()
{
    connect(ui->butDone,    SIGNAL(clicked()),    this,   SLOT(onDoneClicked()));

    ui->txtReasons->setText( details_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void failure::onDoneClicked()
{
    ui->txtReasons->clear();

    setVisible( false );
}
