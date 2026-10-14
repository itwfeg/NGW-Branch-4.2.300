#include "speedkeyconfig.h"
#include "ui_speedkeyconfig.h"

#include <QColor>
#include <QDebug>
#include <QSettings>
#include <QTimer>
#include <QHBoxLayout>
#include <QStandardItemModel>
#include <QListView>
#include <QObject>

#include "mainwindow.h"
#include "inputvalidator.h"
#include "configsettings.h"

static WeighMode mode_ = WeighMode::WeighMode_Avoir;
static bool advancedOpen_ = false;

speedKeyConfig::speedKeyConfig(QWidget *parent) :
    _Mybase(parent),
    ui_(new Ui::speedKeyConfig),
    current_( 0 )
{
    ui_->setupUi(this);

    // load keys
    keysOrig_ = keys_ = readSpeedKeys();
    // init
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    initialize();
    createLayout();
    updateCurrent();
}

speedKeyConfig::~speedKeyConfig()
{
    delete ui_;
    delete targetWeightValidatorLBS_;
    delete targetWeightValidatorOZ_;
    delete targetWeightValidatorKG_;
    delete targetWeightValidatorG_;
    delete weightToleranceValidator_;
}

QSize speedKeyConfig::speedKeyMenuSize()
{
    QSettings s( configFileName(), QSettings::IniFormat );
    s.beginGroup( "CONFIG" );

    const int w( s.value( "Width", 4 ).toInt() );
    const int h( s.value( "Height", 2 ).toInt() );

    return QSize( w, h );
}

QString speedKeyConfig::configFileName()
{
    if(mode_ == WeighMode::WeighMode_Avoir){
        return configurationRoot + "/speedkeysAvoir";
    }else{
        return configurationRoot + "/speedkeysMetric";
    }
}

speedKeyConfig::speedKeyList speedKeyConfig::readSpeedKeys()
{
    speedKeyList result;

    QSettings s( configFileName(), QSettings::IniFormat );
    s.beginGroup( "CONFIG" );

    // determine speed key count
    int count( s.value( "Count", 0 ).toInt() );

    s.endGroup();

    // process each speed key
    for ( int index( 0 ); index < count; ++index )
    {
        static QString group("KEY%0");

        s.beginGroup( group.arg( index ) );
        const int x( s.value( "X", 0 ).toInt() );
        const int y( s.value( "Y", 0 ).toInt() );
        const int w( s.value( "Width", 1 ).toInt() );
        const int h( s.value( "Height", 1 ).toInt() );

        const int type( s.value( "Type", 0 ).toInt() );
        const QString val1 = s.value( "Value1", QString() ).toString();
        const QString val2( s.value( "Value2", QString() ).toString() );

        const QString text( s.value( "Text", QString() ).toString() );
        const int textHAlign( s.value( "TextHAlign", 0 ).toInt() );
        const int textVAlign( s.value( "TextVAlign", 0 ).toInt() );
        const QString textColor( s.value( "TextColor", "Black" ).toString() );
        const QString textFamily( s.value( "TextFamily", QString() ).toString() );
        const int textFontPointSize( s.value( "TextFontPointSize", 12 ).toInt() );

        const QString backgroundColor( s.value( "BackgroundColor", "White" ).toString() );
        const QString backgroundImage( s.value( "BackgroundImage", QString() ).toString() );

        const QString foregroundImage( s.value( "ForegroundImage", QString() ).toString() );

        const QString val3( s.value( "Value3", QString() ).toString() );
        const float totalWeight( s.value( "totalWeight", QString() ).toFloat() );

        s.endGroup();

        // ---- //

        QStringList current;

        current.append( QString::number( x ) );
        current.append( QString::number( y ) );
        current.append( QString::number( w ) );
        current.append( QString::number( h ) );

        current.append( QString::number( type ) );
        current.append( val1 );
        current.append( val2 );

        current.append( text );
        current.append( QString::number( textHAlign ) );
        current.append( QString::number( textVAlign ) );
        current.append( textColor );
        current.append( textFamily );
        current.append( QString::number( textFontPointSize ) );

        current.append( backgroundColor );
        current.append( backgroundImage );

        current.append( foregroundImage );

        current.append( val3 );
        current.append( QString::number( totalWeight ) );

        result.append( current );
    }

    return result;
}

void speedKeyConfig::writeSpeedKeys(const speedKeyList &values)
{
    int index( 0 );

    QSettings s( configFileName(), QSettings::IniFormat );

    s.beginGroup( "CONFIG" );
    s.setValue( "Count", values.size() );
    s.endGroup();

    foreach ( const QStringList &value, values )
    {
        static QString group("KEY%0");

        s.beginGroup( group.arg( index++ ) );

        if ( 0 < value.size() )
            s.setValue( "X", value.at(0).toInt() );
        if ( 1 < value.size() )
            s.setValue( "Y", value.at(1).toInt() );
        if ( 2 < value.size() )
            s.setValue( "Width", value.at(2).toInt() );
        if ( 3 < value.size() )
            s.setValue( "Height", value.at(3).toInt() );

        if ( 4 < value.size() )
            s.setValue( "Type", value.at(4).toInt() );
        if ( 5 < value.size() )
            s.setValue( "Value1", value.at(5) );
        if ( 6 < value.size() )
            s.setValue( "Value2", value.at(6) );

        if ( 7 < value.size() )
            s.setValue( "Text", value.at(7) );
        if ( 8 < value.size() )
            s.setValue( "TextHAlign", value.at(8).toInt() );
        if ( 9 < value.size() )
            s.setValue( "TextVAlign", value.at(9).toInt() );
        if ( 10 < value.size() )
            s.setValue( "TextColor", value.at(10) );
        if ( 11 < value.size() )
            s.setValue( "TextFamily", value.at(11) );
        if ( 12 < value.size() )
            s.setValue( "TextFontPointSize", value.at(12).toInt() );

        if ( 13 < value.size() )
            s.setValue( "BackgroundColor", value.at(13) );
        if ( 14 < value.size() )
            s.setValue( "BackgroundImage", value.at(14) );

        if ( 15 < value.size() )
            s.setValue( "ForegroundImage", value.at(15) );

        if ( 16 < value.size() )
            s.setValue( "Value3", value.at(16) );

        if( 17 < value.size() )
            s.setValue( "totalWeight", value.at(17));

        s.endGroup();
    }
}

void speedKeyConfig::setMode(WeighMode mode)
{
    mode_ = mode;
}

void speedKeyConfig::writeSpeedKeys(const QList<speedKey *> values)
{
    int index( 0 );

    QSettings s( configFileName(), QSettings::IniFormat );

    s.beginGroup( "CONFIG" );
    s.setValue( "Count", values.size() );
    s.endGroup();

    foreach ( const speedKey* value, values )
    {
        static QString group("KEY%0");

        s.beginGroup( group.arg( index++ ) );

            s.setValue( "X", value->x() );
            s.setValue( "Y", value->y() );
            s.setValue( "Width", value->w());
            s.setValue( "Height", value->h() );

            s.setValue( "Type", value->type());
            s.setValue( "Value1", value->val1() );
            s.setValue( "Value2", value->val2() );
            s.setValue( "Text", value->text() );
            s.setValue( "TextHAlign", value->textHAlign() );
            s.setValue( "TextVAlign", value->textVAlign() );
            s.setValue( "TextColor", value->textColor().name());
            s.setValue( "TextFamily", value->textFontFamily() );
            s.setValue( "TextFontPointSize", value->textFontPointSize() );
            s.setValue( "BackgroundColor", value->backgroundColor().name());
            s.setValue( "BackgroundImage", value->backgroundImage() );
            s.setValue( "ForegroundImage", value->foregroundImage() );
            s.setValue( "Value3", value->val3() );
            s.setValue( "totalWeight", QString::number(value->totalWeight()) );

        s.endGroup();
    }
}

void speedKeyConfig::showEvent(QShowEvent *e)
{
    const QRect p( parentWidget()->geometry() );
    const QRect c( rect() );

    move( p.center() - c.center() );

    QDialog::showEvent( e );
}

void speedKeyConfig::onLeftArrowClicked()
{
    saveCurrent();

    if ( --current_ < 0 )
        current_ = keys_.size() - 1;

    updateCurrent();
}

void speedKeyConfig::onRightArrowClicked()
{
    saveCurrent();

    if ( keys_.size() <= ++current_ )
        current_ = 0;

    updateCurrent();
}

void speedKeyConfig::onDeleteClicked()
{
    keys_.removeAt( current_ );

    if ( keys_.size() <= current_ )
        current_ = 0;

    updateCurrent();
}

void speedKeyConfig::onNewClicked()
{
    QStringList current;

    current.append("-1");
    current.append("-1");
    current.append("1");
    current.append("1");

    current.append("0");
    current.append("");
    current.append("");

    current.append("");
    current.append("0");
    current.append("0");
    current.append("#000000");
    current.append("");
    current.append("12");

    current.append("#ffffff");
    current.append("");

    current.append("");

    current.append("");
    current.append("0.0");

    saveCurrent();

    keys_.append( current );
    current_ = keys_.size() - 1;

    updateCurrent();
}

void speedKeyConfig::onAdvancedClicked()
{
    if(advancedOpen_){
        this->ui_->formWidget->hide();
        advancedOpen_ = false;
    } else {
        this->ui_->formWidget->show();
        advancedOpen_ = true;
    }
}

void speedKeyConfig::onCancel()
{
    reject();
}

void speedKeyConfig::onSave()
{
    saveCurrent();

    // save keys
    writeSpeedKeys(keys_);

    accept();
}

void speedKeyConfig::onWeightTypeChanged(int type) const
{
    ui_->uom->blockSignals( true );
    ui_->uom->setCurrentIndex( type );
    ui_->uom->blockSignals( false );

    this->ui_->targetWeight->clear();
    this->ui_->weightTolerance->clear();
    //qDebug() <<"Type: " << type;
    switch(type)
    {
    case Type_lbs:
    {
        weightToleranceValidator_->setDecimals(MainWindow::MAX_PRECISION_LBS);
        this->ui_->targetWeight->setValidator(targetWeightValidatorLBS_);
        break;
    }
    case Type_oz:
    {
        weightToleranceValidator_->setDecimals(MainWindow::MAX_PRECISION_OZ);
        this->ui_->targetWeight->setValidator(targetWeightValidatorOZ_);
        break;
    }
    case Type_Kg:
    {
        weightToleranceValidator_->setDecimals(MainWindow::MAX_PRECISION_KG);
        this->ui_->targetWeight->setValidator(targetWeightValidatorKG_);
        break;
    }
    case Type_g:
    {
        weightToleranceValidator_->setDecimals(MainWindow::MAX_PRECISION_G);
        this->ui_->targetWeight->setValidator(targetWeightValidatorG_);
        break;
    }
    default:
        break;
    }

    this->ui_->weightTolerance->setValidator(weightToleranceValidator_);
}

void speedKeyConfig::onTypeChanged(int type) const
{
    ui_->type->blockSignals( true );
    ui_->type->setCurrentIndex( type );
    ui_->type->blockSignals( false );

    // hide all type specific fields
    ui_->uomLabel->hide();
    ui_->uom->hide();

    ui_->targetWeightLabel->hide();
    ui_->targetWeight->hide();

    ui_->weightTolerance->hide();
    ui_->toleranceLabel->hide();

    // show relevant fields
    switch( type )
    {
    case None:
        break;
    case InfinityMode:
        //∞
        if(ui_->text->text().isEmpty() || ui_->text->text().isNull() ){
           ui_->text->setText("∞");
        }
        break;

    case PresetWeight:
    {
        ui_->uomLabel->show();
        ui_->uom->show();

        ui_->targetWeightLabel->show();
        ui_->targetWeight->show();

        ui_->weightTolerance->show();
        ui_->toleranceLabel->show();
    }

    default:
        break;
    }
}

void speedKeyConfig::slt_targetWeightChanged() const
{
    //Tolerance should not be lower than 0.005, even if target weight is set below
    weightToleranceValidator_->setTop( std::max(ui_->targetWeight->text().toDouble(), 0.005) );
}

void speedKeyConfig::initialize()
{
    targetWeightValidatorLBS_ = new InputValidator(0.0,10.0, MainWindow::MAX_PRECISION_LBS, this);
    targetWeightValidatorOZ_ = new InputValidator(0.0,160.0, MainWindow::MAX_PRECISION_OZ, this);
    targetWeightValidatorKG_ = new InputValidator(0.0,6, MainWindow::MAX_PRECISION_KG, this);
    targetWeightValidatorG_ = new InputValidator(0.0,6000.0, MainWindow::MAX_PRECISION_G, this);
    weightToleranceValidator_ = new InputValidator(double(0), double(0),9, this);//will be overwritten

    targetWeightValidatorLBS_->setNotation(QDoubleValidator::StandardNotation);
    targetWeightValidatorOZ_->setNotation(QDoubleValidator::StandardNotation);
    weightToleranceValidator_->setNotation(QDoubleValidator::StandardNotation);

    connect(ui_->leftArrow,SIGNAL(clicked(bool)),SLOT(onLeftArrowClicked()));
    connect(ui_->rightArrow,SIGNAL(clicked(bool)),SLOT(onRightArrowClicked()));
    connect(ui_->deleteButton,SIGNAL(clicked(bool)),SLOT(onDeleteClicked()));
    connect(ui_->newButton,SIGNAL(clicked(bool)),SLOT(onNewClicked()));
    connect(ui_->advancedOptions,SIGNAL(clicked(bool)),SLOT(onAdvancedClicked()));
    connect(ui_->saveButton,SIGNAL(clicked(bool)),SLOT(onSave()));
    connect(ui_->cancelButton,SIGNAL(clicked(bool)),SLOT(onCancel()));
    //connect(ui_->positionSelectButton, SIGNAL(clicked(bool)), SLOT(onPositionSelectScreen()));

    connect(ui_->type,SIGNAL(currentIndexChanged(int)),SLOT(onTypeChanged(int)));
    connect(ui_->uom, SIGNAL(currentIndexChanged(int)),SLOT(onWeightTypeChanged(int)));
    connect(ui_->targetWeight, SIGNAL(textChanged(QString)),SLOT(slt_targetWeightChanged()));

    ui_->textHAlign->addItem( tr( "Center" ) );
    ui_->textHAlign->addItem( tr( "Left" ) );
    ui_->textHAlign->addItem( tr( "Right" ) );

    ui_->textVAlign->addItem( tr( "Center" ) );
    ui_->textVAlign->addItem( tr( "Top" ) );
    ui_->textVAlign->addItem( tr( "Bottom" ) );

    ui_->type->addItem( tr( "None" ) );
    ui_->type->addItem( tr( "Preset Weight" ) );
    ui_->type->addItem(tr( "Infinity Mode" ));

    ui_->uom->addItem( tr( "Lbs" ) );
    ui_->uom->addItem( tr( "Oz" ) );
    ui_->uom->addItem( tr( "Kg" ) );
    ui_->uom->addItem( tr( "g" ) );
    if(mode_ == WeighMode::WeighMode_Avoir){
        //hide Metric
        hideComboItem(ui_->uom,2);
        hideComboItem(ui_->uom,3);
    }else{
        //hide Avoir
        hideComboItem(ui_->uom,0);
        hideComboItem(ui_->uom,1);
    }

    QSize speedKeySize = speedKeyMenuSize();
    int x_max = speedKeySize.width();
    int y_max = speedKeySize.height();

    positionButtons = QList<QPushButton*>();
    positionButtonGroup = new QButtonGroup();
    for(int c1 = 0; c1 < y_max; c1++){
        QHBoxLayout* newY = new QHBoxLayout();
        for(int c2 = 0; c2 < x_max; c2++){
            QPushButton* newButton = new QPushButton();
            positionButtons.append(newButton);
            positionButtonGroup->addButton(newButton);
            newY->addWidget(newButton);
            newButton->setFocusPolicy(Qt::NoFocus);
            newButton->setCheckable(true);
            newButton->setStyleSheet("QPushButton{background-color:white;} QPushButton:checked{background-color:green;} QPushButton:disabled{background-color:#ff0000;}");
        }
        this->ui_->positionLayout->addLayout(newY);
    }
    positionButtonGroup->setExclusive(true);

    connect(this->ui_->clearPositionButtonsButton, SIGNAL(clicked(bool)), this, SLOT(slt_clearButtonsChecked()));

    this->ui_->bgColor->addItem(tr("Black"), QColor("Black").name());
    this->ui_->bgColor->addItem(tr("Red"), QColor("Red").name());
    this->ui_->bgColor->addItem(tr("Green"), QColor("Green").name());
    this->ui_->bgColor->addItem(tr("Blue"), QColor("Blue").name());
    this->ui_->bgColor->addItem(tr("White"), QColor("White").name());
    this->ui_->bgColor->addItem(tr("Orange"), QColor("Orange").name());
    this->ui_->bgColor->addItem(tr("Yellow"), QColor("Yellow").name());
    this->ui_->bgColor->addItem(tr("Gray"), QColor("Gray").name());

    this->ui_->textColor->addItem(tr("Black"), QColor("Black").name());
    this->ui_->textColor->addItem(tr("Red"), QColor("Red").name());
    this->ui_->textColor->addItem(tr("Green"), QColor("Green").name());
    this->ui_->textColor->addItem(tr("Blue"), QColor("Blue").name());
    this->ui_->textColor->addItem(tr("White"), QColor("White").name());
    this->ui_->textColor->addItem(tr("Orange"), QColor("Orange").name());
    this->ui_->textColor->addItem(tr("Yellow"), QColor("Yellow").name());
    this->ui_->textColor->addItem(tr("Gray"), QColor("Gray").name());

    //Update preview live based on fields changed
    connect(this->ui_->text, SIGNAL(textChanged(QString)), this, SLOT(slt_previewText(QString)));
    connect(this->ui_->textColor, SIGNAL(currentIndexChanged(int)), this, SLOT(slt_previewTextColor(int)));
    connect(this->ui_->textFontPointSize, SIGNAL(textChanged(QString)), this, SLOT(slt_previewFont(QString)));
    connect(this->ui_->bgColor, SIGNAL(currentIndexChanged(int)), this, SLOT(slt_previewBgColor(int)));
}

void speedKeyConfig::createLayout()
{
    //hide unused UI elements:
    this->ui_->textHAlign->hide();
    this->ui_->textHAlignLabel->hide();
    this->ui_->textVAlign->hide();
    this->ui_->textVAlignLabel->hide();
    this->ui_->bgImage->hide();
    this->ui_->bgImageLabel->hide();

    //Hide Advanced Options by default
    this->ui_->formWidget->hide();
    advancedOpen_ = false;
}

void speedKeyConfig::updateCurrent() const
{
    ui_->leftArrow->setDisabled( keys_.size() <= 1 );
    ui_->rightArrow->setDisabled( keys_.size() <= 1 );
    ui_->deleteButton->setDisabled( keys_.isEmpty() );

    //uncheck and disable the buttons.
    foreach(QAbstractButton* b, positionButtonGroup->buttons()){
        b->setDisabled(keys_.isEmpty());
    }

    positionButtonGroup->setExclusive(false);
    QAbstractButton* checked = positionButtonGroup->checkedButton();
    if(checked != 0) checked->setChecked(!keys_.isEmpty());
    positionButtonGroup->setExclusive(true);

    ui_->text->setDisabled( keys_.isEmpty() );
    ui_->textHAlign->setDisabled( keys_.isEmpty() );
    ui_->textVAlign->setDisabled( keys_.isEmpty() );
    ui_->textColor->setDisabled( keys_.isEmpty() );
    ui_->textFontPointSize->setDisabled( keys_.isEmpty() );

    ui_->bgColor->setDisabled( keys_.isEmpty() );
    ui_->bgImage->setDisabled( keys_.isEmpty() );

    ui_->type->setDisabled( keys_.isEmpty() );


    if ( keys_.empty() )
    {
        ui_->uomLabel->hide();
        ui_->uom->hide();

        ui_->targetWeightLabel->hide();
        ui_->targetWeight->hide();

        return;
    }

    // ---- //

    enableDisablePositionButtons();

    const QStringList c( keys_[current_] );
    //qDebug() << "c " << c ;

    //Update SpeedKey Preview (pass no arguments to field and value)
    updatePreview(-1, "");

    // set text
    ui_->text->setText( c.at(7) );

    int textHAlign( c.at(8).toInt() );
    if (( textHAlign < 0 ) || ( ui_->textHAlign->maxCount() <= textHAlign ))
        textHAlign = 0;

    ui_->textHAlign->setCurrentIndex( textHAlign );

    int textVAlign( c.at(9).toInt() );
    if (( textVAlign < 0 ) || ( ui_->textVAlign->maxCount() <= textVAlign ))
        textVAlign = 0;

    ui_->textVAlign->setCurrentIndex( textVAlign );

    ui_->textColor->setCurrentIndex(ui_->textColor->findData( c.at(10) ));

    ui_->textFontPointSize->setText( c.at(12) );

    // set background
    ui_->bgColor->setCurrentIndex(ui_->bgColor->findData( c.at(13) ));
    ui_->bgImage->setText( c.at( 14 ) );

    // set type info
    int type( c.at(4).toInt() );
    if (( type < 0 ) || ( MAX_KEY_TYPES <= type ))
        type = 0;

    onTypeChanged( type );

    const QString val1( c.at(5) );
    const QString val2( c.at(6) );
    const QString val3( c.at(16) );

    switch( type )
    {
    case None:
        break;

    case PresetWeight:
    {
        //qDebug() << "mode: " << mode_;
        int uom( val1.toInt() );
        if (( uom < 0 ) || ( ui_->uom->count() <= uom ))
            uom = 0;
        ui_->uom->setCurrentIndex( uom );
        ui_->targetWeight->setText( val2 );
        ui_->weightTolerance->setText(val3 );

        break;
    }

    default:
        break;
    }
}

void speedKeyConfig::saveCurrent()
{
    if ( keys_.empty() )
        return;

    // ---- //

    QStringList c( keys_[current_] );

//    // save location
    QSize menuSize = speedKeyMenuSize();
    int checkedIndex = getCheckedIndex();
    int x_checked = checkedIndex % menuSize.width();
    int y_checked = checkedIndex / menuSize.width();

    c[0] = QString::number(x_checked);
    c[1] = QString::number(y_checked);

    // save text
    c[7] = ui_->text->text();
    c[8] = QString::number( ui_->textHAlign->currentIndex() );
    c[9] = QString::number( ui_->textVAlign->currentIndex() );
    c[10] = ui_->textColor->currentData().toString();
    c[12] = ui_->textFontPointSize->text();

    // save background
    c[13] = ui_->bgColor->currentData().toString();
    c[14] = ui_->bgImage->text();

    // save type info
    c[4] = QString::number( ui_->type->currentIndex() );

    switch ( ui_->type->currentIndex() )
    {
    case None:
        c[5] = "";
        c[6] = "";
        c[15] = "";
        break;

    case PresetWeight:
        c[5] = QString::number( ui_->uom->currentIndex() );
        c[6] = ui_->targetWeight->text();
        //Limit tolerance to 0.005
        if(ui_->weightTolerance->text().toDouble() < 0.005){
            ui_->weightTolerance->setText("0.005");
        }
        c[16] = ui_->weightTolerance->text();
        break;
    case InfinityMode:
        c[5] = "";
        c[6] = "";
        c[16] = "";
        break;

    default:
        break;
    }

    // save!!
    keys_[current_] = c;
}

int speedKeyConfig::getCheckedIndex(){
    QAbstractButton* checkedButton = positionButtonGroup->checkedButton();
    for(int ci = 0; ci < positionButtons.size(); ci++){
        if(positionButtons[ci] == checkedButton)
            return ci;
    }

    //If none returned, default to next free space
    for(int ci = 0; ci < positionButtons.size(); ci++){
        if(positionButtons[ci]->isEnabled()){
            return ci;
        }
    }

    //No more spots available
    return -1;
}

void speedKeyConfig::slt_clearButtonsChecked() const
{
    positionButtonGroup->setExclusive(false);
    QAbstractButton* checked = positionButtonGroup->checkedButton();
    if(checked != 0) checked->setChecked(false);
    positionButtonGroup->setExclusive(true);
}

void speedKeyConfig::slt_previewText(QString value)
{
    updatePreview(7, value);
}

void speedKeyConfig::slt_previewTextColor(int index)
{
    //Get correect data from index
    updatePreview(10, this->ui_->textColor->itemData(index).toString());
}

void speedKeyConfig::slt_previewFont(QString value)
{
    updatePreview(12, value);
}

void speedKeyConfig::slt_previewBgColor(int index)
{
    //Get correect data from index
    updatePreview(13, this->ui_->bgColor->itemData(index).toString());
}

void speedKeyConfig::updatePreview(int field, QString value)
{
    //If preview doesn't exist yet, create a new one
    speedKey *key_preview = ui_->preview->findChild<speedKey*>();
    if(!key_preview){
        qDebug() << "Creating new preview...";
        key_preview = new speedKey(ui_->preview);
    }

    //Make x and y zero, since preivew isn't part of a grid
    QSize s_preview = QSize(1,1);
    QStringList c_preview( keys_[current_] );
    c_preview[0] = QString::number(0);
    c_preview[1] = QString::number(0);

    //Update corresponding field for live update (does not save changes)
    if(field != -1){
        c_preview[7] = key_preview->text();
        c_preview[10] = key_preview->textColor().name();
        c_preview[12] = key_preview->textFontPointSize();
        c_preview[13] = key_preview->backgroundColor().name();
        c_preview[field] = value;
    }

    //qDebug() << "Updating " << c_preview[7];
    key_preview->create(s_preview, c_preview);
    key_preview->show();
}

void speedKeyConfig::enableDisablePositionButtons() const
{
    slt_clearButtonsChecked();
    foreach(QAbstractButton *b, positionButtons){
        b->setDisabled(false);
    }

    //set the checked one
    const QStringList c( keys_[current_] );
    int x = c.at(0).toInt();
    int y = c.at(1).toInt();

    QSize menuSize = speedKeyMenuSize();
    int indexToCheck = (x + (y * menuSize.width()));
    if(indexToCheck < menuSize.height() * menuSize.width() && indexToCheck >= 0)
        positionButtons.at(indexToCheck)->setChecked(true);

    for(int c1 = 0; c1 < keys_.size(); c1++){
        if( c1 != current_){
            const QStringList c(keys_[c1]);
            int x = c.at(0).toInt();
            int y = c.at(1).toInt();

            QSize menuSize = speedKeyMenuSize();
            int indexToCheck = (x + (y * menuSize.width()));
            if(indexToCheck < menuSize.height() * menuSize.width() && indexToCheck >= 0)
                positionButtons.at(indexToCheck)->setDisabled(true);
        }
    }

}

void speedKeyConfig::hideComboItem(QComboBox * combo, int row)
{
    QListView* view = qobject_cast<QListView *>(combo->view());
    Q_ASSERT(view != nullptr);
    view->setRowHidden(row, true);
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(combo->model());
    Q_ASSERT(model != nullptr);
    QStandardItem* item = model->item(row);
    item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
}

void speedKeyConfig::showComboItem(QComboBox *combo, int row)
{
    QListView* view = qobject_cast<QListView *>(combo->view());
    Q_ASSERT(view != nullptr);
    view->setRowHidden(row, false);
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(combo->model());
    Q_ASSERT(model != nullptr);
    QStandardItem* item = model->item(row);
    item->setFlags(item->flags() | Qt::ItemIsEnabled);
}
