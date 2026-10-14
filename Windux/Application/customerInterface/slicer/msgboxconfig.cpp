#include "msgboxconfig.h"
#include "ui_msgboxconfig.h"

#include <QDebug>
#include <QSettings>

#include <QTimer>
#include <QDateTime>
#include "configsettings.h"

msgBoxConfig::msgBoxConfig(QWidget *parent) :
    _Mybase(parent),
    ui_(new Ui::msgBoxConfig),
    current_( 0 ),
    maxUniqueId_ (-1)
{
    ui_->setupUi(this);

    // initialize system messages
    initSystemMessages();

    updateMaxUniqueId();

    // load messages
    messagesOrig_ = messages_ = readMessages();

    // init
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    initialize();

    createLayout();
    updateCurrent();

    QTimer* qtimer = new QTimer(this);
    QTimer::singleShot(0, this, SLOT(slt_UpdateTime()));
    connect(qtimer, SIGNAL(timeout()), this, SLOT(slt_UpdateTime()));
    qtimer->start(1000);
}

msgBoxConfig::~msgBoxConfig()
{
    delete ui_;
}

void msgBoxConfig::initialize()
{

    connect(ui_->leftArrow,SIGNAL(clicked(bool)),SLOT(onLeftArrowClicked()));
    connect(ui_->rightArrow,SIGNAL(clicked(bool)),SLOT(onRightArrowClicked()));
    connect(ui_->deleteButton,SIGNAL(clicked(bool)),SLOT(onDeleteClicked()));
    connect(ui_->newButton,SIGNAL(clicked(bool)),SLOT(onNewClicked()));
    connect(ui_->addTimeButton,SIGNAL(clicked(bool)),SLOT(onAddTimeClicked()));
    connect(ui_->addDateButton,SIGNAL(clicked(bool)),SLOT(onAddDateClicked()));
    connect(ui_->saveButton,SIGNAL(clicked(bool)),SLOT(onSave()));
    connect(ui_->cancelButton,SIGNAL(clicked(bool)),SLOT(onCancel()));

    connect(ui_->type,SIGNAL(currentIndexChanged(int)),SLOT(onTypeChanged(int)));

    ui_->type->addItem( tr("Alert on date(s)") );
    ui_->type->addItem( tr("Alert on week day") );
    //ui_->type->addItem( tr("Repeat every day") );
}

void msgBoxConfig::slt_UpdateTime(){
    QDateTime dtime = QDateTime::currentDateTime();
    this->ui_->timeNow->setText(dtime.toString("MMMM dd, yyyy -- hh:mm:ss"));
}

QString msgBoxConfig::configFileName() {
    return configurationRoot + "/messages";
}

void msgBoxConfig::initSystemMessages()
{
    //only init if CONFIG file has never been created before
    if (QFile(configFileName()).exists())
        return;
    messageList msgList;
    QStringList sharpenBladeMsg, sanitizeSlicerMsg, cleanSanitizeSlicerMsg, lubricateCarriageMsg;
    //init msg list with null values for more clear access
    for(int c1 = 0; c1 < NumberMessageTypes; c1++){
        sharpenBladeMsg.append("");
        sanitizeSlicerMsg.append("");
        cleanSanitizeSlicerMsg.append("");
        lubricateCarriageMsg.append("");
    }
    int days;
    /// "Slicer blade should be sharpened every 3 to 4 days depending on usage.
    ///   See operator manual for instructions."
    days = 0;
    days |= Monday;
    days |= Wednesday;
    days |= Friday;
    sharpenBladeMsg[IsSystemMsg] = "1";
    sharpenBladeMsg[IsDisabled] = "0";
    sharpenBladeMsg[Header] = "Sharpen Blade / Affûter la lame";
    sharpenBladeMsg[Message] = "Slicer blade should be sharpened every 3 to 4 days depending on usage. See operator manual for instructions.\nLa lame de la trancheuse devrait être affûtée à tous les 3 ou 4 jours selon son utilisation. Voir le mode d'emploi pour les instructions.";
    sharpenBladeMsg[Type] = "1"; //0 for monthly, 1 for weekly
    sharpenBladeMsg[DayOfWeek] = QString::number(days);
    sharpenBladeMsg[Time1] = "12:00:00"; //remind every day at noon
    sharpenBladeMsg[UniqueId] = "0";
    msgList.append(sharpenBladeMsg);

    /// "Slicer should be sanitized every 2 hours."
    ///   6AM -> every 2 hours (Skip every 8th, because sanitize & clean message)
    days = 0x7f; //every day
    sanitizeSlicerMsg[IsSystemMsg] = "1";
    sanitizeSlicerMsg[IsDisabled] = "0";
    sanitizeSlicerMsg[Header] = "Sanitize Slicer / Désinfecter la trancheuse";
    sanitizeSlicerMsg[Message] = "Slicer should be sanitized every 2 hours.\nLa trancheuse devrait être désinfectée à toutes les 2 heures.";
    sanitizeSlicerMsg[Type] = "1";
    sanitizeSlicerMsg[DayOfWeek] = QString::number(days);
    sanitizeSlicerMsg[Time1] = "08:00:00;10:00:00;12:00:00;16:00:00;18:00:00;20:00:00;00:00:00;02:00:00"; //missing 4am -- not enough time slots
    sanitizeSlicerMsg[UniqueId] = "1";
    msgList.append(sanitizeSlicerMsg);

    /// "Slicer should be cleaned and sanitized every 8 hours."
    ///   6AM -> every 8 hours
    days = 0x7f; //every day
    cleanSanitizeSlicerMsg[IsSystemMsg] = "1";
    cleanSanitizeSlicerMsg[IsDisabled] = "0";
    cleanSanitizeSlicerMsg[Header] = "Clean & Sanitize Slicer / Nettoyer & désinfecter la trancheuse";
    cleanSanitizeSlicerMsg[Message] = "Slicer should be cleaned and sanitized every 8 hours.\nLa trancheuse devrait être nettoyée et désinfectée à toutes les 8 heures.";
    cleanSanitizeSlicerMsg[Type] = "1";
    cleanSanitizeSlicerMsg[DayOfWeek] = QString::number(days);
    cleanSanitizeSlicerMsg[Time1] = "06:00:00;14:00:00;22:00:00";
    cleanSanitizeSlicerMsg[UniqueId] = "2";
    msgList.append(cleanSanitizeSlicerMsg);

    /// "Slicer carriage should be lubricated once per month."
    ///  --- Set message 1st of the month, at 6AM
    lubricateCarriageMsg[IsSystemMsg] = "1";
    lubricateCarriageMsg[IsDisabled] = "0";
    lubricateCarriageMsg[Header] = "Lubricate Carriage / Lubrifier le chariot";
    lubricateCarriageMsg[Message] = "Slicer carriage should be lubricated once per month.\nLe chariot de la trancheuse devrait être lubrifié une fois par mois.";
    lubricateCarriageMsg[Type] = "0";
    lubricateCarriageMsg[Date1] = "1";
    lubricateCarriageMsg[Time1] = "06:00:00";
    lubricateCarriageMsg[UniqueId] = "3";
    msgList.append(lubricateCarriageMsg);

    writeMessages(msgList);
}

void msgBoxConfig::updateMaxUniqueId(){
    QSettings s(configFileName(), QSettings::IniFormat);
    s.beginGroup("CONFIG");
    int count(s.value("Count",0).toInt());
    s.endGroup();
    for(int index(0); index< count; ++index)
    {
        static QString group("MESSAGE%0");
        s.beginGroup(group.arg(index));
        const int uniqueId(s.value("UniqueId", QString()).toInt());
        if(uniqueId > maxUniqueId_)
            maxUniqueId_ = uniqueId;
        s.endGroup();
    }
}

msgBoxConfig::messageList msgBoxConfig::readMessages()
{
    messageList result;

    QSettings s( configFileName(), QSettings::IniFormat );
    s.beginGroup( "CONFIG" );

    // determine message count
    int count( s.value( "Count", 0 ).toInt() );

    s.endGroup();

    // process each message
    for ( int index( 0 ); index < count; ++index )
    {
        static QString group("MESSAGE%0");

        s.beginGroup( group.arg( index ) );

        const bool isSystemMsg( s.value( "IsSystemMsg", false ).toBool() );
        const bool isDisabled( s.value( "IsDisabled", false ).toBool() );

        const QString header( s.value( "Header", QString() ).toString() );
        const QString text( s.value( "Message", QString() ).toString() );
        const int type( s.value( "Type", 0 ).toInt() );

        const int dayOfWeek( s.value( "DayOfWeek", 0 ).toInt() );

        const QString date1( s.value( "Date1", QString() ).toString() );
        const QString date2( s.value( "Date2", QString() ).toString() );

        const QString time1( s.value( "Time1", QString() ).toString() );
        const QString time2( s.value( "Time2", QString() ).toString() );
        const QString time3( s.value( "Time3", QString() ).toString() );
        const QString time4( s.value( "Time4", QString() ).toString() );
        const QString time5( s.value( "Time5", QString() ).toString() );
        const QString time6( s.value( "Time6", QString() ).toString() );
        const QString time7( s.value( "Time7", QString() ).toString() );
        const QString time8( s.value( "Time8", QString() ).toString() );

        const int interval(s.value("Interval", QString()).toInt());

        const int uniqueId(s.value("UniqueId", QString()).toInt());

        const qint64 lastAlerted(s.value("LastAlerted", QString()).toLongLong());
        const qint64 lastConfirmCompleted(s.value("LastConfirmCompleted", QString()).toLongLong());

        s.endGroup();

        // ---- //

        QStringList current;

        current.append( QString::number( isSystemMsg ) );
        current.append( QString::number( isDisabled ) );

        current.append( header );
        current.append( text );
        current.append( QString::number( type ) );

        current.append( QString::number( dayOfWeek ) );

        current.append( date1 );
        current.append( date2 );

        current.append( time1 );
        current.append( time2 );
        current.append( time3 );
        current.append( time4 );
        current.append( time5 );
        current.append( time6 );
        current.append( time7 );
        current.append( time8 );

        current.append( QString::number(interval) );

        current.append( QString::number(uniqueId) );

        current.append( QString::number(lastAlerted) );
        current.append( QString::number(lastConfirmCompleted) );

        result.append( current );
    }

    return result;
}

void msgBoxConfig::writeMessages(const messageList &values)
{
    int index( 0 );

    QSettings s( configFileName(), QSettings::IniFormat );

    s.beginGroup( "CONFIG" );
    s.setValue( "Count", values.size() );
    s.endGroup();

    foreach ( const QStringList &value, values )
    {
        static QString group("MESSAGE%0");

        s.beginGroup( group.arg( index++ ) );

        if ( 0 < value.size() )
            s.setValue( "IsSystemMsg", (0 == value.at(IsSystemMsg).toInt()) ? false : true );
        if ( 1 < value.size() )
            s.setValue( "IsDisabled", (0 == value.at(IsDisabled).toInt()) ? false : true );

        if ( 2 < value.size() )
            s.setValue( "Header", value.at(Header) );
        if ( 3 < value.size() )
            s.setValue( "Message", value.at(Message) );
        if ( 4 < value.size() )
            s.setValue( "Type", value.at(Type) );

        if ( 5 < value.size() )
            s.setValue( "DayOfWeek", value.at(DayOfWeek).toInt() );

        if ( 6 < value.size() )
            s.setValue( "Date1", value.at(Date1) );
        if ( 7 < value.size() )
            s.setValue( "Date2", value.at(Date2) );

        if ( 8 < value.size() )
            s.setValue( "Time1", value.at(Time1) );
        if ( 9 < value.size() )
            s.setValue( "Time2", value.at(Time2) );
        if ( 10 < value.size() )
            s.setValue( "Time3", value.at(Time3) );
        if ( 11 < value.size() )
            s.setValue( "Time4", value.at(Time4) );
        if ( 12 < value.size() )
            s.setValue( "Time5", value.at(Time5) );
        if ( 13 < value.size() )
            s.setValue( "Time6", value.at(Time6) );
        if ( 14 < value.size() )
            s.setValue( "Time7", value.at(Time7) );
        if ( 15 < value.size() )
            s.setValue( "Time8", value.at(Time8) );
        if ( 16 < value.size() )
            s.setValue( "Interval", value.at(Interval) );
        if ( 17 < value.size() )
            s.setValue( "UniqueId", value.at(UniqueId) );
        if ( 18 < value.size() )
            s.setValue( "LastAlerted", value.at(LastAlerted) );
        if ( 19 < value.size() )
            s.setValue( "LastConfirmCompleted", value.at(LastConfirmCompleted));

        s.endGroup();
    }

}

void msgBoxConfig::showEvent(QShowEvent *e)
{
    const QRect p( parentWidget()->geometry() );
    const QRect c( rect() );

    move( p.center() - c.center() );

    QDialog::showEvent( e );
}

void msgBoxConfig::onLeftArrowClicked()
{
    saveCurrent();

    if ( --current_ < 0 )
        current_ = messages_.size() - 1;

    updateCurrent();
}

void msgBoxConfig::onRightArrowClicked()
{
    saveCurrent();

    if ( messages_.size() <= ++current_ )
        current_ = 0;

    updateCurrent();
}

void msgBoxConfig::onDeleteClicked()
{
    messages_.removeAt( current_ );

    if ( messages_.size() <= current_ )
        current_ = 0;

    updateCurrent();
    //rewrite all of the messages *after deleting everything to get rid of current_*
    QSettings s(configFileName(), QSettings::IniFormat);
    s.clear();
    writeMessages(messages_);
}

void msgBoxConfig::onNewClicked()
{
    QStringList current;

    current.append("0");    // IsSystemMsg
    current.append("0");    // IsDisabled

    current.append("");     // Header
    current.append("");     // Message
    current.append("0");    // Type

    current.append("0");    // DayOfWeek

    current.append("");     // Date 1
    current.append("");     // Date 2

    current.append("");     // Time 1
    current.append("");     // Time 2
    current.append("");     // Time 3
    current.append("");     // Time 4
    current.append("");     // Time 5
    current.append("");     // Time 6
    current.append("");     // Time 7
    current.append("");     // Time 8

    current.append("");     // Interval

                            // UniqueId
    current.append(QString::number(maxUniqueId_ + 1));
    maxUniqueId_++;

    current.append("");     // LastAlerted
    current.append("");     // LastConfirmCompleted

    saveCurrent();

    messages_.append( current );
    current_ = messages_.size() - 1;

    updateCurrent();
}

void msgBoxConfig::onAddTimeClicked()
{
    //TODO Debug and Implement
    //TimeEdit will not get assigned as child of times
    QFrame *f = ui_->times;
    QLabel *l = new QLabel(ui_->times);
    QTimeEdit *t = new QTimeEdit( f );
    l->hide();
    t->setDisplayFormat("hh:mm");
    t->setParent(f);
    ui_->formLayout_7->insertRow((ui_->formLayout_7->rowCount()-1), l, t);
}

void msgBoxConfig::onAddDateClicked()
{
    //TODO Implement
}

void msgBoxConfig::onCancel()
{
    reject();
}

void msgBoxConfig::onSave()
{
    saveCurrent();

    // save messages
    writeMessages( messages_ );
    Q_EMIT sig_MessageUpdated();

    accept();
}

void msgBoxConfig::onTypeChanged(int type) const
{
    ui_->type->blockSignals( true );
    ui_->type->setCurrentIndex( type );
    ui_->type->blockSignals( false );

    // hide all type-specific fields
    ui_->daysLabel->hide();
    ui_->days->hide();

    ui_->datesLabel->hide();
    ui_->datesFrame->hide();

    ui_->timesLabel->hide();
    ui_->times->hide();

    ui_->addTimeLabel->hide();
    ui_->addTimeButton->hide();
    ui_->addDateLabel->hide();
    ui_->addDateButton->hide();

    // show relevant fields
    switch( type )
    {
    case Monthly:
        ui_->datesLabel->show();
        ui_->datesFrame->show();
        //ui_->addDateButton->show();

        ui_->timesLabel->show();
        ui_->times->show();
        //ui_->addTimeButton->show();

        break;
    case Weekly:
        ui_->daysLabel->show();
        ui_->days->show();

        ui_->timesLabel->show();
        ui_->times->show();
        //ui_->addTimeButton->show();

        break;
    case Daily:
        break;

    default:
        break;
    }
}

void msgBoxConfig::createLayout()
{
}

void msgBoxConfig::updateCurrent() const
{
    ui_->leftArrow->setDisabled( messages_.size() <= 1 );
    ui_->rightArrow->setDisabled( messages_.size() <= 1 );
    ui_->deleteButton->setDisabled( messages_.isEmpty() );

    ui_->title->setDisabled( messages_.isEmpty() );
    ui_->message->setDisabled( messages_.isEmpty() );
    ui_->disable->setDisabled( messages_.isEmpty() );

    ui_->type->setDisabled( messages_.isEmpty() );

    ui_->saveButton->setDisabled( messages_.isEmpty() );

    if ( messages_.empty() )
    {
        ui_->daysLabel->hide();
        ui_->days->hide();

        ui_->datesLabel->hide();
        ui_->datesFrame->hide();

        ui_->timesLabel->hide();
        ui_->times->hide();

        return;
    }

    // ---- //

    const QStringList currMessage( messages_[current_] );

    ui_->deleteButton->setDisabled( 0 != currMessage[IsSystemMsg].toInt() );

    // set message info
    ui_->title->setText( currMessage[Header] );
    ui_->message->setPlainText( currMessage[Message] );
    ui_->disable->setChecked( 0 != currMessage[IsDisabled].toInt() );

    // set type info
    int type( currMessage.at(Type).toInt() );
    if (( type < 0 ) || ( MAX_MESSAGE_TYPES <= type ))
        type = 0;

    //Get times and dates
    QStringList dateList = currMessage[Date1].split(";");
    QStringList timeList = currMessage[Time1].split(";");
    //qDebug() << "dateList: " << dateList;
    //qDebug() << "timeList: " << timeList;

    onTypeChanged( type );

    switch( type )
    {
    case Monthly:
        ui_->date1->setValue( dateList.length() > 0 ? dateList[0].toInt() : 0 );
        ui_->date2->setValue( dateList.length() > 1 ? dateList[1].toInt() : 0 );

        //Note: The if statements on each line are a temporary solution until the option to add an "Add Time" or "Add Date" button
        //After that, we will simply iterate through the list depending on how many were added and things will be much more elegant
        ui_->time1->setTime( timeList.length() > 0 ? QTime::fromString(timeList[0]) : QTime() );
        ui_->time2->setTime( timeList.length() > 1 ? QTime::fromString(timeList[1]) : QTime() );
        break;

    case Weekly:
        ui_->sunday->setChecked(    Sunday    & currMessage[DayOfWeek].toInt() );
        ui_->monday->setChecked(    Monday    & currMessage[DayOfWeek].toInt() );
        ui_->tuesday->setChecked(   Tuesday   & currMessage[DayOfWeek].toInt() );
        ui_->wednesday->setChecked( Wednesday & currMessage[DayOfWeek].toInt() );
        ui_->thursday->setChecked(  Thursday  & currMessage[DayOfWeek].toInt() );
        ui_->friday->setChecked(    Friday    & currMessage[DayOfWeek].toInt() );
        ui_->saturday->setChecked(  Saturday  & currMessage[DayOfWeek].toInt() );

        ui_->time1->setTime( timeList.length() > 0 ? QTime::fromString(timeList[0]) : QTime() );
        ui_->time2->setTime( timeList.length() > 1 ? QTime::fromString(timeList[1]) : QTime() );
        ui_->time3->setTime( timeList.length() > 2 ? QTime::fromString(timeList[2]) : QTime() );
        ui_->time4->setTime( timeList.length() > 3 ? QTime::fromString(timeList[3]) : QTime() );

        break;

    case Daily:
        break;

    default:
        break;
    }
}

void msgBoxConfig::saveCurrent()
{
    if ( messages_.empty() )
        return;

    // ---- //

    QStringList currMessage( messages_[current_] );

    // save message info
    currMessage[IsDisabled] = ui_->disable->isChecked() ? "1" : "0";
    currMessage[Header] = ui_->title->text();
    currMessage[Message] = ui_->message->toPlainText();

    // save type info
    currMessage[Type] = QString::number( ui_->type->currentIndex() );

    switch ( ui_->type->currentIndex() )
    {
    case Weekly:
    {
        int day( 0 );
        if ( ui_->sunday->isChecked() )
            day |= Sunday;
        if ( ui_->monday->isChecked() )
            day |= Monday;
        if ( ui_->tuesday->isChecked() )
            day |= Tuesday;
        if ( ui_->wednesday->isChecked() )
            day |= Wednesday;
        if ( ui_->thursday->isChecked() )
            day |= Thursday;
        if ( ui_->friday->isChecked() )
            day |= Friday;
        if ( ui_->saturday->isChecked() )
            day |= Saturday;

        currMessage[DayOfWeek] = QString::number( day );

        //Add times
        currMessage[Time1] = "";
        foreach(QTimeEdit* t, ui_->times->children()){
            if(qobject_cast<QTimeEdit*>(t) == NULL){
                continue;
            }
            currMessage[Time1] += t->time().toString() + ";";
        }
        break;
    }

    case Monthly:
        //Add dates
        currMessage[Date1] = "";
        foreach(QSpinBox* d, ui_->datesFrame->children()){
            if(qobject_cast<QSpinBox*>(d) == NULL){
                continue;
            }
            currMessage[Date1] += d->text() + ";";
        }

        //Add times
        currMessage[Time1] = "";
        qDebug() << "Children: " << ui_->times->children();
        foreach(QTimeEdit* t, ui_->times->children()){
            if(qobject_cast<QTimeEdit*>(t) == NULL){
                continue;
            }
            currMessage[Time1] += t->time().toString() + ";";
        }
        break;

    case Daily:
        break;

    default:
        break;
    }


    // save!!
    messages_[current_] = currMessage;
}
