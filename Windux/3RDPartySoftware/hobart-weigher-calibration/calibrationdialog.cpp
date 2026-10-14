
#include "calibrationdialog.h"
#include "generate.h"

#include <QButtonGroup>
#include <QDoubleValidator>
#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QSpacerItem>
#include <QTextStream>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
calibrationDialog::calibrationDialog( QWidget *parent ) :
    _Mybase( parent )
{
    initialize();
    createLayout();

    calib_mode_20pt_->setChecked( true );
    calib_method_kg_->setChecked( true );

    accel_res_->setText( "20" );
    accel_sens_->setText( "2.0" );

#ifdef QT_DEBUG
/*
    // adxl312
    accel_res_->setText( "10" );
    accel_sens_->setText( "1.5" );

    accx_level_->setText( "3237" );
    accy_level_->setText( "-274" );
    accz_level_->setText( "117" );
    accx_front_->setText( "3248" );
    accy_front_->setText( "84" );
    accz_front_->setText( "131" );
    accx_back_->setText( "3227" );
    accy_back_->setText( "-544" );
    accz_back_->setText( "93" );
    accx_left_->setText( "3245" );
    accy_left_->setText( "-261" );
    accz_left_->setText( "-242" );
    accx_right_->setText( "3246" );
    accy_right_->setText( "-284" );
    accz_right_->setText( "421" );
*/
    // adxl355
    accx_level_->setText( "1281" );
    accy_level_->setText( "259911" );
    accz_level_->setText( "997" );
    accx_front_->setText( "-20822" );
    accy_front_->setText( "259244" );
    accz_front_->setText( "907" );
    accx_back_->setText( "23870" );
    accy_back_->setText( "258601" );
    accz_back_->setText( "805" );
    accx_left_->setText( "1291" );
    accy_left_->setText( "259185" );
    accz_left_->setText( "-21204" );
    accx_right_->setText( "1325" );
    accy_right_->setText( "258667" );
    accz_right_->setText( "23374" );
/*
    counts_level_0kg_->setText( "533787" );
    counts_level_5kg_->setText( "1955426" );
    counts_level_10kg_->setText( "3377164" );
    counts_level_15kg_->setText( "4798791" );

    counts_front_0kg_->setText( "531410" );
    counts_front_5kg_->setText( "1947119" );
    counts_front_10kg_->setText( "3362191" );
    counts_front_15kg_->setText( "4776769" );

    counts_back_0kg_->setText( "531532" );
    counts_back_5kg_->setText( "1948338" );
    counts_back_10kg_->setText( "3365870" );
    counts_back_15kg_->setText( "4783919" );

    counts_left_0kg_->setText( "531395" );
    counts_left_5kg_->setText( "1947498" );
    counts_left_10kg_->setText( "3363658" );
    counts_left_15kg_->setText( "4779620" );

    counts_right_0kg_->setText( "531510" );
    counts_right_5kg_->setText( "1947951" );
    counts_right_10kg_->setText( "3364550" );
    counts_right_15kg_->setText( "4780885" );
*/

    counts_level_0kg_->setText( "322756" );
    counts_level_5kg_->setText( "1748323" );
    counts_level_10kg_->setText( "3173843" );
    counts_level_15kg_->setText( "4599314" );

    counts_front_0kg_->setText( "320358" );
    counts_front_5kg_->setText( "1740159" );
    counts_front_10kg_->setText( "3159131" );
    counts_front_15kg_->setText( "4577474" );

    counts_back_0kg_->setText( "320399" );
    counts_back_5kg_->setText( "1741050" );
    counts_back_10kg_->setText( "3162254" );
    counts_back_15kg_->setText( "4583924" );

    counts_left_0kg_->setText( "320335" );
    counts_left_5kg_->setText( "1740435" );
    counts_left_10kg_->setText( "3160780" );
    counts_left_15kg_->setText( "4580778" );

    counts_right_0kg_->setText( "320328" );
    counts_right_5kg_->setText( "1740587" );
    counts_right_10kg_->setText( "3160528" );
    counts_right_15kg_->setText( "4580470" );

/*
    // avery
    counts_level_0kg_->setText( "385535" );
    counts_level_5kg_->setText( "1679040" );
    counts_level_10kg_->setText( "2972605" );
    counts_level_15kg_->setText( "4266092" );

    counts_front_0kg_->setText( "383780" );
    counts_front_5kg_->setText( "1670581" );
    counts_front_10kg_->setText( "2956798" );
    counts_front_15kg_->setText( "4242031" );

    counts_back_0kg_->setText( "384673" );
    counts_back_5kg_->setText( "1674589" );
    counts_back_10kg_->setText( "2965029" );
    counts_back_15kg_->setText( "4255802" );

    counts_left_0kg_->setText( "383492" );
    counts_left_5kg_->setText( "1672084" );
    counts_left_10kg_->setText( "2960629" );
    counts_left_15kg_->setText( "4249217" );

    counts_right_0kg_->setText( "383568" );
    counts_right_5kg_->setText( "1672149" );
    counts_right_10kg_->setText( "2960839" );
    counts_right_15kg_->setText( "4249630" );
*/
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
calibrationDialog::~calibrationDialog()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibrationDialog::onPushButtonClicked()
{
    if ( calib_mode_20pt_ == sender() )
    {
        counts_level_5kg_->setEnabled( true );
        counts_level_10kg_->setEnabled( true );

        counts_front_10kg_->setEnabled( true );

        counts_back_10kg_->setEnabled( true );

        counts_left_5kg_->setEnabled( true );
        counts_left_10kg_->setEnabled( true );

        counts_right_5kg_->setEnabled( true );
        counts_right_10kg_->setEnabled( true );
    }

    else if ( calib_mode_12pt_ == sender() )
    {
        counts_level_5kg_->setEnabled( false );
        counts_level_10kg_->setEnabled( false );

        counts_front_10kg_->setEnabled( false );

        counts_back_10kg_->setEnabled( false );

        counts_left_5kg_->setEnabled( false );
        counts_left_10kg_->setEnabled( false );

        counts_right_5kg_->setEnabled( false );
        counts_right_10kg_->setEnabled( false );
    }

    else if ( calib_method_kg_ == sender() )
    {
        counts_0kg_->setText( tr( "Counts 0kg:" ) );
        counts_5kg_->setText( tr( "Counts 0kg:" ) );
        counts_10kg_->setText( tr( "Counts 0kg:" ) );
        counts_15kg_->setText( tr( "Counts 0kg:" ) );
    }

    else if ( calib_method_lb_ == sender() )
    {
        counts_0kg_->setText( tr( "Counts 0lb:" ) );
        counts_5kg_->setText( tr( "Counts 10lb:" ) );
        counts_10kg_->setText( tr( "Counts 20lb:" ) );
        counts_15kg_->setText( tr( "Counts 30lb:" ) );
    }

    else if ( clear_ == sender() )
    {
        foreach ( QLineEdit *widget, inputs_ )
            widget->clear();

        results_->clear();
    }

    else if ( calculate_ == sender() )
    {
        const std::vector<double> counts( weigherValues() );
        const std::vector<double> acc_values( accValues() );

        const bool use_metric( calib_method_kg_->isChecked() ? true : false );

        results_->clear();

        std::string result;

        fit_flash_section f;
        generate_fit( counts, use_metric, acc_values, f, result );

        results_->setPlainText( QString::fromStdString( result ) );

        // scroll to bottom
        if ( results_->verticalScrollBar() )
            results_->verticalScrollBar()->setValue( results_->verticalScrollBar()->maximum() );
    }

    else if ( quit_ == sender() )
    {
        close();
    }

    else if ( loadFileValidate_ == sender() )
    {
        std::vector<double> counts;
        std::vector<double> testData;

        QMultiMap<int, QStringList> testDataMap;

        // file format:
        // counts @ 0kg for no tilt, 5 degree front tilt, 5 degree back tilt, 5 degree left tilt, 5 degree right tilt
        // counts @ 5kg for no tilt, 5 degree front tilt, 5 degree back tilt, 5 degree left tilt, 5 degree right tilt
        // counts @ 10kg for no tilt, 5 degree front tilt, 5 degree back tilt, 5 degree left tilt, 5 degree right tilt
        // counts @ 15kg for no tilt, 5 degree front tilt, 5 degree back tilt, 5 degree left tilt, 5 degree right tilt
        // applied load, counts at that load, x tilt (degrees), y tilt (degrees)
        const QString filename( QFileDialog::getOpenFileName( this, tr( "Select File" ), QString(), tr( "CSV Files (*.csv *.txt)" ) ) );

        if ( filename.length() )
        {
            QFile f( filename );

            if ( f.open( QIODevice::ReadOnly ) )
            {
                QTextStream is( &f );

                // read calibration data
                while ( !is.atEnd() )
                {
                    const QString line( is.readLine() );

                    if ( line.count( ',' ) < 5 )
                        break;

                    const QStringList elements( line.split( ',' ) );

                    counts.push_back( elements.at( 1 ).trimmed().toDouble() );
                    counts.push_back( elements.at( 2 ).trimmed().toDouble() );
                    counts.push_back( elements.at( 3 ).trimmed().toDouble() );
                    counts.push_back( elements.at( 4 ).trimmed().toDouble() );
                    counts.push_back( elements.at( 5 ).trimmed().toDouble() );

                    if ( 20 == counts.size() )
                        break;
                }

                // read test points
                while ( !is.atEnd() )
                {
                    const QString line( is.readLine() );

                    if ( line.count( ',' ) < 3 )
                        break;

                    const QStringList elements( line.split( ',' ) );

                    testDataMap.insert( elements.at( 0 ).trimmed().toInt(), elements );
                }

                foreach ( const QStringList& elements, testDataMap )
                {
                    testData.push_back( elements.at( 0 ).trimmed().toDouble() );
                    testData.push_back( elements.at( 1 ).trimmed().toDouble() );
                    testData.push_back( elements.at( 2 ).trimmed().toDouble() * 0.0174533 );
                    testData.push_back( elements.at( 3 ).trimmed().toDouble() * 0.0174533 );
                }

                f.close();
            }
        }

        // process data
        if (( 20 == counts.size() ) && ( 0 < testData.size() ))
        {
            const std::vector<double> acc_values( accValues() );

            const bool calib_20pt( calib_mode_20pt_->isChecked() ? true : false );
            const bool use_metric( calib_method_kg_->isChecked() ? true : false );

            if ( !calib_20pt )
            {
                counts[5] = 0.0;
                counts[8] = 0.0;
                counts[9] = 0.0;

                counts[10] = 0.0;
                counts[11] = 0.0;
                counts[12] = 0.0;
                counts[13] = 0.0;
                counts[14] = 0.0;
            }

            fit_flash_section f;

            std::string result0;
            std::string result1;

            results_->clear();

            generate_fit( counts, use_metric, acc_values, f, result0 );
            results_->setPlainText( QString::fromStdString( result0 ) );

            test_cases( testData, use_metric, f, result1 );
            results_->setPlainText( QString::fromStdString( result0 + result1 ) );

            // scroll to bottom
            if ( results_->verticalScrollBar() )
                results_->verticalScrollBar()->setValue( results_->verticalScrollBar()->maximum() );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibrationDialog::initialize()
{
    accel_res_validator_ = new QIntValidator( 8, 32, this );
    accel_sens_validator_ = new QDoubleValidator( 0.5, 12.0, 1, this );

    acc_validator_ = new QIntValidator( -9999999, 9999999, this );
    counts_validator_ = new QIntValidator( 0, 9999999, this );

    // ---- //

    calib_mode_ = new QLabel( tr( "Calibration Method:" ), this );

    calib_mode_20pt_ = new QRadioButton( tr( "20 pt" ), this );
    connect( calib_mode_20pt_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    calib_mode_12pt_ = new QRadioButton( tr( "12 pt" ), this );
    connect( calib_mode_12pt_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    QButtonGroup *modes( new QButtonGroup( this ) );
    modes->addButton( calib_mode_20pt_ );
    modes->addButton( calib_mode_12pt_ );

    calib_method_ = new QLabel( tr( "Calibration Weights:" ), this );

    calib_method_kg_ = new QRadioButton( tr( "Kilograms" ), this );
    connect( calib_method_kg_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    calib_method_lb_ = new QRadioButton( tr( "Pounds" ), this );
    connect( calib_method_lb_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    QButtonGroup *methods( new QButtonGroup( this ) );
    methods->addButton( calib_method_kg_ );
    methods->addButton( calib_method_lb_ );

    accel_res_lbl_ = new QLabel( tr( "Acc. Resolution (bits):" ), this );

    accel_res_= new QLineEdit( this );
    accel_res_->setValidator( accel_res_validator_ );

    accel_sens_lbl_ = new QLabel( tr( "Acc. Sensitivity (+/- g):" ), this );

    accel_sens_ = new QLineEdit( this );
    accel_sens_->setValidator( accel_sens_validator_ );

    // ---- //

    accx_ = new QLabel( tr( "Accel X:" ), this );
    accy_ = new QLabel( tr( "Accel Y:" ), this );
    accz_ = new QLabel( tr( "Accel Z:" ), this );
    counts_0kg_ = new QLabel( tr( "Counts 0kg:" ), this );
    counts_5kg_ = new QLabel( tr( "Counts 5kg:" ), this );
    counts_10kg_ = new QLabel( tr( "Counts 10kg:" ), this );
    counts_15kg_ = new QLabel( tr( "Counts 15kg:" ), this );

    level_ = new QLabel( tr( "LEVEL" ), this );
    level_->setAlignment( Qt::AlignHCenter );

    front_ = new QLabel( tr( "FRONT TILT" ), this );
    front_->setAlignment( Qt::AlignHCenter );

    back_ = new QLabel( tr( "BACK TILT" ), this );
    back_->setAlignment( Qt::AlignHCenter );

    left_ = new QLabel( tr( "LEFT TILT" ), this );
    left_->setAlignment( Qt::AlignHCenter );

    right_ = new QLabel( tr( "RIGHT TILT" ), this );
    right_->setAlignment( Qt::AlignHCenter );


    // level
    accx_level_ = new QLineEdit( this );
    accx_level_->setValidator( acc_validator_ );
    inputs_.append( accx_level_ );

    accy_level_ = new QLineEdit( this );
    accy_level_->setValidator( acc_validator_ );
    inputs_.append( accy_level_ );

    accz_level_ = new QLineEdit( this );
    accz_level_->setValidator( acc_validator_ );
    inputs_.append( accz_level_ );

    counts_level_0kg_ = new QLineEdit( this );
    counts_level_0kg_->setValidator( counts_validator_ );
    inputs_.append( counts_level_0kg_ );

    counts_level_5kg_ = new QLineEdit( this );
    counts_level_5kg_->setValidator( counts_validator_ );
    inputs_.append( counts_level_5kg_ );

    counts_level_10kg_ = new QLineEdit( this );
    counts_level_10kg_->setValidator( counts_validator_ );
    inputs_.append( counts_level_10kg_ );

    counts_level_15kg_ = new QLineEdit( this );
    counts_level_15kg_->setValidator( counts_validator_ );
    inputs_.append( counts_level_15kg_ );

    // front
    accx_front_ = new QLineEdit( this );
    accx_front_->setValidator( acc_validator_ );
    inputs_.append( accx_front_ );

    accy_front_ = new QLineEdit( this );
    accy_front_->setValidator( acc_validator_ );
    inputs_.append( accy_front_ );

    accz_front_ = new QLineEdit( this );
    accz_front_->setValidator( acc_validator_ );
    inputs_.append( accz_front_ );

    counts_front_0kg_ = new QLineEdit( this );
    counts_front_0kg_->setValidator( counts_validator_ );
    inputs_.append( counts_front_0kg_ );

    counts_front_5kg_ = new QLineEdit( this );
    counts_front_5kg_->setValidator( counts_validator_ );
    inputs_.append( counts_front_5kg_ );

    counts_front_10kg_ = new QLineEdit( this );
    counts_front_10kg_->setValidator( counts_validator_ );
    inputs_.append( counts_front_10kg_ );

    counts_front_15kg_ = new QLineEdit( this );
    counts_front_15kg_->setValidator( counts_validator_ );
    inputs_.append( counts_front_15kg_ );


    // back
    accx_back_ = new QLineEdit( this );
    accx_back_->setValidator( acc_validator_ );
    inputs_.append( accx_back_ );

    accy_back_ = new QLineEdit( this );
    accy_back_->setValidator( acc_validator_ );
    inputs_.append( accy_back_ );

    accz_back_ = new QLineEdit( this );
    accz_back_->setValidator( acc_validator_ );
    inputs_.append( accz_back_ );

    counts_back_0kg_ = new QLineEdit( this );
    counts_back_0kg_->setValidator( counts_validator_ );
    inputs_.append( counts_back_0kg_ );

    counts_back_5kg_ = new QLineEdit( this );
    counts_back_5kg_->setValidator( counts_validator_ );
    inputs_.append( counts_back_5kg_ );

    counts_back_10kg_ = new QLineEdit( this );
    counts_back_10kg_->setValidator( counts_validator_ );
    inputs_.append( counts_back_10kg_ );

    counts_back_15kg_ = new QLineEdit( this );
    counts_back_15kg_->setValidator( counts_validator_ );
    inputs_.append( counts_back_15kg_ );


    // left
    accx_left_ = new QLineEdit( this );
    accx_left_->setValidator( acc_validator_ );
    inputs_.append( accx_left_ );

    accy_left_ = new QLineEdit( this );
    accy_left_->setValidator( acc_validator_ );
    inputs_.append( accy_left_ );

    accz_left_ = new QLineEdit( this );
    accz_left_->setValidator( acc_validator_ );
    inputs_.append( accz_left_ );

    counts_left_0kg_ = new QLineEdit( this );
    counts_left_0kg_->setValidator( counts_validator_ );
    inputs_.append( counts_left_0kg_ );

    counts_left_5kg_ = new QLineEdit( this );
    counts_left_5kg_->setValidator( counts_validator_ );
    inputs_.append( counts_left_5kg_ );

    counts_left_10kg_ = new QLineEdit( this );
    counts_left_10kg_->setValidator( counts_validator_ );
    inputs_.append( counts_left_10kg_ );

    counts_left_15kg_ = new QLineEdit( this );
    counts_left_15kg_->setValidator( counts_validator_ );
    inputs_.append( counts_left_15kg_ );


    // right
    accx_right_ = new QLineEdit( this );
    accx_right_->setValidator( acc_validator_ );
    inputs_.append( accx_right_ );

    accy_right_ = new QLineEdit( this );
    accy_right_->setValidator( acc_validator_ );
    inputs_.append( accy_right_ );

    accz_right_ = new QLineEdit( this );
    accz_right_->setValidator( acc_validator_ );
    inputs_.append( accz_right_ );

    counts_right_0kg_ = new QLineEdit( this );
    counts_right_0kg_->setValidator( counts_validator_ );
    inputs_.append( counts_right_0kg_ );

    counts_right_5kg_ = new QLineEdit( this );
    counts_right_5kg_->setValidator( counts_validator_ );
    inputs_.append( counts_right_5kg_ );

    counts_right_10kg_ = new QLineEdit( this );
    counts_right_10kg_->setValidator( counts_validator_ );
    inputs_.append( counts_right_10kg_ );

    counts_right_15kg_ = new QLineEdit( this );
    counts_right_15kg_->setValidator( counts_validator_ );
    inputs_.append( counts_right_15kg_ );

    // results
    results_ = new QPlainTextEdit( this );
    results_->setFont( QFontDatabase::systemFont( QFontDatabase::FixedFont ) );

    // buttons
    clear_ = new QPushButton( tr( "Clear Form" ), this );
    connect( clear_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    calculate_ = new QPushButton( tr( "CALCULATE" ), this );
    connect( calculate_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    quit_ = new QPushButton( tr( "Exit" ), this );
    connect( quit_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

    loadFileValidate_ = new QPushButton( tr( "Load/Validate Data" ), this );
    connect( loadFileValidate_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );

#ifndef QT_DEBUG
    loadFileValidate_->setVisible( false );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibrationDialog::createLayout()
{
    static const int spacerHeight( 16 );

    QHBoxLayout *modes( new QHBoxLayout() );
    modes->addWidget( calib_mode_20pt_ );
    modes->addWidget( calib_mode_12pt_ );
    modes->addStretch();

    QHBoxLayout *methods( new QHBoxLayout() );
    methods->addWidget( calib_method_kg_ );
    methods->addWidget( calib_method_lb_ );
    methods->addStretch();

    QFormLayout *setup( new QFormLayout() );
    setup->addRow( calib_mode_, modes );
    setup->addRow( calib_method_, methods );
    setup->addRow( accel_res_lbl_, accel_res_ );
    setup->addRow( accel_sens_lbl_, accel_sens_ );

    QHBoxLayout *labels( new QHBoxLayout() );
    labels->addWidget( new QLabel( this ) );
    labels->addWidget( level_ );
    labels->addWidget( front_ );
    labels->addWidget( back_ );
    labels->addWidget( left_ );
    labels->addWidget( right_ );

    QHBoxLayout *accx( new QHBoxLayout() );
    accx->addWidget( accx_, 1 );
    accx->addWidget( accx_level_, 1 );
    accx->addWidget( accx_front_, 1 );
    accx->addWidget( accx_back_, 1 );
    accx->addWidget( accx_left_, 1 );
    accx->addWidget( accx_right_, 1 );

    QHBoxLayout *accy( new QHBoxLayout() );
    accy->addWidget( accy_, 1 );
    accy->addWidget( accy_level_, 1 );
    accy->addWidget( accy_front_, 1 );
    accy->addWidget( accy_back_, 1 );
    accy->addWidget( accy_left_, 1 );
    accy->addWidget( accy_right_, 1 );

    QHBoxLayout *accz( new QHBoxLayout() );
    accz->addWidget( accz_, 1 );
    accz->addWidget( accz_level_, 1 );
    accz->addWidget( accz_front_, 1 );
    accz->addWidget( accz_back_, 1 );
    accz->addWidget( accz_left_, 1 );
    accz->addWidget( accz_right_, 1 );

    QHBoxLayout *counts_0kg( new QHBoxLayout() );
    counts_0kg->addWidget( counts_0kg_, 1 );
    counts_0kg->addWidget( counts_level_0kg_, 1 );
    counts_0kg->addWidget( counts_front_0kg_, 1 );
    counts_0kg->addWidget( counts_back_0kg_, 1 );
    counts_0kg->addWidget( counts_left_0kg_, 1 );
    counts_0kg->addWidget( counts_right_0kg_, 1 );

    QHBoxLayout *counts_5kg( new QHBoxLayout() );
    counts_5kg->addWidget( counts_5kg_, 1 );
    counts_5kg->addWidget( counts_level_5kg_, 1 );
    counts_5kg->addWidget( counts_front_5kg_, 1 );
    counts_5kg->addWidget( counts_back_5kg_, 1 );
    counts_5kg->addWidget( counts_left_5kg_, 1 );
    counts_5kg->addWidget( counts_right_5kg_, 1 );

    QHBoxLayout *counts_10kg( new QHBoxLayout() );
    counts_10kg->addWidget( counts_10kg_, 1 );
    counts_10kg->addWidget( counts_level_10kg_, 1 );
    counts_10kg->addWidget( counts_front_10kg_, 1 );
    counts_10kg->addWidget( counts_back_10kg_, 1 );
    counts_10kg->addWidget( counts_left_10kg_, 1 );
    counts_10kg->addWidget( counts_right_10kg_, 1 );

    QHBoxLayout *counts_15kg( new QHBoxLayout() );
    counts_15kg->addWidget( counts_15kg_, 1 );
    counts_15kg->addWidget( counts_level_15kg_, 1 );
    counts_15kg->addWidget( counts_front_15kg_, 1 );
    counts_15kg->addWidget( counts_back_15kg_, 1 );
    counts_15kg->addWidget( counts_left_15kg_, 1 );
    counts_15kg->addWidget( counts_right_15kg_, 1 );

    QVBoxLayout *form( new QVBoxLayout() );
    form->addItem( new QSpacerItem( spacerHeight, spacerHeight ) );
    form->addLayout( labels );
    form->addItem( new QSpacerItem( spacerHeight, spacerHeight ) );
    form->addLayout( accx );
    form->addLayout( accy );
    form->addLayout( accz );
    form->addItem( new QSpacerItem( spacerHeight, spacerHeight ) );
    form->addLayout( counts_0kg );
    form->addLayout( counts_5kg );
    form->addLayout( counts_10kg );
    form->addLayout( counts_15kg );
    form->addItem( new QSpacerItem( spacerHeight, spacerHeight ) );

    QHBoxLayout *buttons( new QHBoxLayout() );
    buttons->addWidget( calculate_ );
    buttons->addWidget( clear_ );
    buttons->addStretch();
    buttons->addWidget( loadFileValidate_ );
    buttons->addStretch();
    buttons->addWidget( quit_ );

    QVBoxLayout *layout( new QVBoxLayout( this ) );
    layout->addLayout( setup );
    layout->addLayout( form );
    layout->addWidget( results_, 1 );
    layout->addLayout( buttons );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<double> calibrationDialog::accValues() const
{
    // adxl312: 2^10 bits / +/- 1.5 g = 341.3 LSB/g
    // adxl355: 2^20 bits / +/- 2.0 g = 262144 LSB/g
    const double resolution( pow( 2.0, accel_res_->text().toDouble() ) / (2.0 * accel_sens_->text().toDouble()) );

    std::vector<double> acc_values;
    acc_values.push_back( accx_level_->text().toDouble() / resolution );
    acc_values.push_back( accx_front_->text().toDouble() / resolution );
    acc_values.push_back( accx_back_->text().toDouble() / resolution );
    acc_values.push_back( accx_left_->text().toDouble() / resolution );
    acc_values.push_back( accx_right_->text().toDouble() / resolution );

    acc_values.push_back( accy_level_->text().toDouble() / resolution );
    acc_values.push_back( accy_front_->text().toDouble() / resolution );
    acc_values.push_back( accy_back_->text().toDouble() / resolution );
    acc_values.push_back( accy_left_->text().toDouble() / resolution );
    acc_values.push_back( accy_right_->text().toDouble() / resolution );

    acc_values.push_back( accz_level_->text().toDouble() / resolution );
    acc_values.push_back( accz_front_->text().toDouble() / resolution );
    acc_values.push_back( accz_back_->text().toDouble() / resolution );
    acc_values.push_back( accz_left_->text().toDouble() / resolution );
    acc_values.push_back( accz_right_->text().toDouble() / resolution );

    return acc_values;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<double> calibrationDialog::weigherValues() const
{
    // calibration method
    const bool calib_20pt( calib_mode_20pt_->isChecked() ? true : false );

    std::vector<double> counts;
    counts.push_back( counts_level_0kg_->text().toDouble() );
    counts.push_back( counts_front_0kg_->text().toDouble() );
    counts.push_back( counts_back_0kg_->text().toDouble() );
    counts.push_back( counts_left_0kg_->text().toDouble() );
    counts.push_back( counts_right_0kg_->text().toDouble() );

    counts.push_back( calib_20pt ? counts_level_5kg_->text().toDouble() : 0.0 );
    counts.push_back( counts_front_5kg_->text().toDouble() );
    counts.push_back( counts_back_5kg_->text().toDouble() );
    counts.push_back( calib_20pt ? counts_left_5kg_->text().toDouble() : 0.0 );
    counts.push_back( calib_20pt ? counts_right_5kg_->text().toDouble() : 0.0 );

    counts.push_back( calib_20pt ? counts_level_10kg_->text().toDouble() : 0.0 );
    counts.push_back( calib_20pt ? counts_front_10kg_->text().toDouble() : 0.0 );
    counts.push_back( calib_20pt ? counts_back_10kg_->text().toDouble() : 0.0 );
    counts.push_back( calib_20pt ? counts_left_10kg_->text().toDouble() : 0.0 );
    counts.push_back( calib_20pt ? counts_right_10kg_->text().toDouble() : 0.0 );

    counts.push_back( counts_level_15kg_->text().toDouble() );
    counts.push_back( counts_front_15kg_->text().toDouble() );
    counts.push_back( counts_back_15kg_->text().toDouble() );
    counts.push_back( counts_left_15kg_->text().toDouble() );
    counts.push_back( counts_right_15kg_->text().toDouble() );

    return counts;
}
