#include "keypad.h"
#include "ui_keypad.h"

#include <QDebug>
#include <QSignalMapper>
#include <QString>
#include <QPushButton>
#include <QMainWindow>
#include <QApplication>
#include <QPlainTextEdit>
#include <QTextEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QBitmap>
#include <QSettings>

static const Qt::WindowFlags _InputWidgetWindowFlags( Qt::Tool | Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint );

const int autoRepeatDelay_ = 500;
const int autoRepeatInterval_ = 100;


Keypad::Keypad(QWidget *parent) :
    QFrame(parent, _InputWidgetWindowFlags),
    ui(new Ui::Keypad),
    w_(NULL)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_ShowWithoutActivating );
    this->setFocusPolicy(Qt::NoFocus);
    this->setAutoFillBackground(true);
    QPalette p( palette() );
    p.setColor( backgroundRole(), Qt::darkGray );
    setPalette( p );
    ui->m_pButtonNumberGroup->setExclusive(false);
    ui->m_pButtonLetterGroup->setExclusive(false);
    //ui->m_altButtonGroup->setExclusive(false);
    //move to bottom of parent

    QPixmap backspaceIcon( ":/images/res/backspaceIcon.png" );
    QPixmap capsLockIcon( ":/images/res/capsLockIcon.png" );
    QPixmap clearIcon( ":/images/res/clearIcon.png" );
    QPixmap dismissIcon( ":/images/res/dismissIcon.png" );
    QPixmap returnIcon( ":/images/res/returnIcon.png" );


    //
    // convert pixmaps to proper color
    //

    QList<QPixmap*> pixmaps;
    pixmaps.append( &backspaceIcon );
    pixmaps.append( &capsLockIcon );
    pixmaps.append( &clearIcon );
    pixmaps.append( &dismissIcon );
    pixmaps.append( &returnIcon );

    foreach ( QPixmap *p, pixmaps )
        if ( !p->isNull() )
        {
            QPixmap converted( p->size() );
            converted.fill( Qt::white);//palette().color( foregroundRole() ) );
            converted.setMask( p->createMaskFromColor( Qt::transparent ) );


            p->swap( converted );
        }



    ksh = new KeySignalHandler();

    ///connect the text enter key:
    connect(this, SIGNAL(sig_textEnterOrTab(QString)), ksh, SLOT(slt_HandleKeyPress(QString)));

    ///signal mapper maps string with intended behavior to keyboard signal handler
    ///  that posts/sends keyevent depending on QString passed through signal
    QSignalMapper *mapper = new QSignalMapper(this);
    connect(mapper, SIGNAL(mapped(QString)), ksh, SLOT(slt_HandleKeyPress(QString)));

    m_ShowAltChars(false);
    m_ShowNumbers(false);

    float iconScaleSize = 0.50f;
    QListIterator<QObject *> itr(this->children());
    while(itr.hasNext())
    {
        //initialize the buttons
        QPushButton* b = qobject_cast<QPushButton*>(itr.next());
        if(b != NULL){
            b->setFocusPolicy(Qt::NoFocus);
            b->setAutoRepeat(true);
            b->setAutoRepeatInterval(autoRepeatInterval_);
            b->setAutoRepeatDelay(autoRepeatDelay_);
            QString signalReference = b->text();

            //modify special keys here (with images -- use the object name)
            if(b->objectName().contains("enter", Qt::CaseInsensitive)){
                connect(b, SIGNAL(clicked()), this, SLOT(slt_hideOnEnter()));
                b->setIcon(returnIcon);
                b->setIconSize(QSize(b->size().width() * iconScaleSize, b->size().height() * iconScaleSize));
                continue; //does not connect to anything in ksh
            }
            else if(b->objectName().contains("hide", Qt::CaseInsensitive)){
                connect(b, SIGNAL(clicked()), this, SLOT(dismiss()));
                b->setIcon(dismissIcon);
                b->setIconSize(QSize(b->size().width() * iconScaleSize, b->size().height() * iconScaleSize));
                continue; //does not connect to anything in ksh
            }
            else if(b->objectName().contains("caps", Qt::CaseInsensitive)){
                connect(b, SIGNAL(toggled(bool)), ksh, SLOT(slt_CapsLockStatus(bool)));
                connect(b, SIGNAL(toggled(bool)), this, SLOT(slt_SetCapsLockBtns(bool)));
                b->setIcon(QIcon(capsLockIcon));
                b->setIconSize(QSize(b->size().width() * iconScaleSize, b->size().height() * iconScaleSize));
                b->setCheckable(true);
                continue; //does not connect to anything in ksh
            }
            //sets the toggle between regular and accented letters
            else if(b->objectName().contains("alt", Qt::CaseInsensitive)){
                b->setCheckable(true);
                connect(b, SIGNAL(toggled(bool)), this, SLOT(slt_ToggleAtlChars(bool)));
                continue; //does not connect to anything in ksh
            }
            //sets the toggle between letters and numbers + special chars
            else if(b->objectName().contains("toggle", Qt::CaseInsensitive)){
                b->setCheckable(true);
                connect(b, SIGNAL(toggled(bool)), this, SLOT(slt_SetToggleNumbers(bool)));
                continue; //does not connect to anything in ksh
            }
            else if(b->objectName().contains("backspace", Qt::CaseInsensitive)){
                signalReference = "BACKSPACE";
                b->setIcon(backspaceIcon);
                b->setIconSize(QSize(b->size().width() * iconScaleSize, b->size().height() * iconScaleSize));
            }
            else if(b->objectName().contains("space", Qt::CaseInsensitive)){
                signalReference = " ";
            }

            //connect the buttons to their intended purposes
            mapper->setMapping(b, signalReference);
            connect(b, SIGNAL(clicked()), mapper, SLOT(map()));
        }
    }
}

void Keypad::m_ShowNumbers(bool show){
    QButtonGroup* numbers = ui->m_pButtonNumberGroup;
    foreach(QAbstractButton* qab, numbers->buttons()){
        if(show) qab->show();
        else qab->hide();
    }
}

void Keypad::m_ShowLetters(bool show){
    QButtonGroup* letters = ui->m_pButtonLetterGroup;
    foreach(QAbstractButton* qab, letters->buttons()){
        if(show) qab->show();
        else qab->hide();
    }
}

void Keypad::m_ShowAltChars(bool show){
    QButtonGroup* alts = ui->m_altButtonGroup;
    foreach(QAbstractButton* qab, alts->buttons()){
        if(show) qab->show();
        else qab->hide();
    }
}

//Modifies the letter characters on the keyboard
void Keypad::slt_SetCapsLockBtns(bool capsOn){
    QButtonGroup* letters = ui->m_pButtonLetterGroup;
    foreach(QAbstractButton* qab, letters->buttons()){
        if(capsOn) qab->setText(qab->text().toUpper());
        else qab->setText(qab->text().toLower());
    }

    //Repeat for alt character group
    QButtonGroup* alt = ui->m_altButtonGroup;
    foreach(QAbstractButton* qab, alt->buttons()){
        if(capsOn) qab->setText(qab->text().toUpper());
        else qab->setText(qab->text().toLower());
    }
}

void Keypad::slt_SetToggleNumbers(bool numbersOn){
    m_ShowAltChars(false);
    m_ShowLetters(!numbersOn);
    m_ShowNumbers(numbersOn);
}

void Keypad::slt_ToggleAtlChars(bool altOn){
    m_ShowNumbers(false);
    m_ShowLetters(!altOn);
    m_ShowAltChars(altOn);
}

Keypad::~Keypad()
{
    delete ui;
    delete ksh;
}

void Keypad::invoke(QWidget* widget){
    if((w_ = widget)){
        w_ -> activateWindow();
        QWidget* active = dynamic_cast<QWidget*>(qApp->activeWindow());
        if(active != NULL){
            this->setParent(active);
        }
    }
    show();
    QWidget *screen = this->parentWidget();

    if(screen){
        QPlainTextEdit *plainTextEdit;
        QTextEdit *textEdit;

        // calculate top left corner of input widget
        QPoint topLeft( 0, 0 );

        // for multi line edit use cursor position instead of widget location
        if ( (plainTextEdit = dynamic_cast<QPlainTextEdit*>( widget )) )
            topLeft = plainTextEdit->cursorRect().topLeft();
        else if ( (textEdit = dynamic_cast<QTextEdit*>( widget )) )
            topLeft = textEdit->cursorRect().topLeft();
        else
            topLeft = widget->pos();

        const bool top( topLeft.y() > (screen->height() - height()) );

        //default keyboard to bottom
        int verticalOffset = screen->height() - height();

        // check if alternate location is being requested
        if ( top  )
        verticalOffset = 0;

        // move keyboard!
        move( (screen->width() - width()) / 2, verticalOffset );
    }
    //qDebug("Showing keypad");
}

void Keypad::dismiss(){
    if(w_){
        w_->clearFocus();
    }
    this->setParent(NULL);
    hide();
    //qDebug("Hiding keypad");
}

void Keypad::hideEvent( QHideEvent *e )
{
    QFrame::hideEvent( e );
    w_ = NULL;


}

void Keypad::showEvent( QShowEvent *e )
{
    QFrame::showEvent( e );

}

bool Keypad::event( QEvent *e )
{
    switch ( e->type() )
    {
    case QEvent::WindowActivate:
        // transfer focus back to input widget
        if ( w_ ){
            w_->activateWindow();
        }
        break;

    default:
        break;
    }

    return QFrame::event( e );
}

void Keypad::slt_hideOnEnter(){
    QPlainTextEdit* qpte;
    QTextEdit* qte;
    bool send_enter = false;
    if((qpte = dynamic_cast<QPlainTextEdit*>(w_)))
        send_enter = true;
    else if((qte = dynamic_cast<QTextEdit*>(w_)))
        send_enter = true;
    if(send_enter)
        emit sig_textEnterOrTab("ENTER");
    else
    //if not multiline edit, dismiss on enter button press.
        this->dismiss();
}

