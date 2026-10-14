/**
 * @file filedialog.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "filedialog.h"

#include "../sessionmainwindow.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>

#include <QDir>
#include <QFileSystemModel>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QTreeView>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
FileDialog::FileDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f ),
    listView_( NULL ),
    treeView_( NULL )
{
    // set object name
    setObjectName( "FileDialog" );

    // initialize interface
    initialize();

    // SOF-1207
    QDir dir;
    QStringList paths;

#ifdef Q_OS_WIN
    paths.append( "C:" );
#else
    paths.append( "/" );

    if ( session_->isPrepack() )
    {
        static const QString root( "/root" );

        if ( dir.exists( root ) )
            paths.append( root );
    }
#endif

#ifdef Q_OS_WIN
    static const QString thumb1( "E:" );
    static const QString thumb( "D:" );

    if ( dir.exists( thumb1 ) )
        paths.append( thumb1 );
    else if ( dir.exists( thumb ) )
        paths.append( thumb );
#else
    static const QString thumb3( "/media/thumb3" );
    static const QString thumb2( "/media/thumb2" );
    static const QString thumb1( "/media/thumb1" );
    static const QString thumb( "/media/thumb" );

    if ( dir.exists( thumb3 ) )
        paths.append( thumb3 );
    else if ( dir.exists( thumb2 ) )
        paths.append( thumb2 );
    else if ( dir.exists( thumb1 ) )
        paths.append( thumb1 );
    else if ( dir.exists( thumb ) )
        paths.append( thumb );
#endif

    if ( paths.size() )
    {
        QList<QUrl> urls;

        foreach ( const QString &path, paths )
            urls.append( QUrl( "file://" + path )
                                );
        d_->setSidebarUrls( urls );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
FileDialog::~FileDialog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FileDialog::setOptions( QFileDialog::Options value )
{
#if defined( Q_OS_LINUX )
    d_->setOptions( options_ = (value | QFileDialog::DontUseNativeDialog | QFileDialog::ReadOnly) );
#elif defined( Q_OS_WIN )
    d_->setOptions( options_ = (value | QFileDialog::DontUseNativeDialog | QFileDialog::ReadOnly | QFileDialog::DontResolveSymlinks | QFileDialog::DontUseCustomDirectoryIcons) );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FileDialog::hideEvent( QHideEvent *e )
{
    // hide input widgets
    mainWindow_->hideInputWidgets();

    _Mybase::hideEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FileDialog::showEvent( QShowEvent *e )
{
    _Mybase::showEvent( e );

    // update read only property
    updateReadOnly();

    // SOF-2669
    // disable any low level completers
    const QObjectList children( d_->children() );

    foreach ( QObject *child, children )
    {
        QLineEdit *lineEdit( qobject_cast<QLineEdit*>( child ) );

        if (( lineEdit ) && ( lineEdit->completer() ))
            lineEdit->setCompleter( NULL );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FileDialog::onDirectoryEntered( const QString &dir )
{
    Q_UNUSED( dir );

    // update read only property
    updateReadOnly();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FileDialog::onFileListingClicked( const QModelIndex& index )
{
    Q_UNUSED( index );
/* FT-858: they decided they no longer wanted the "single" click functionality
    QDir path( index.data( QFileSystemModel::FilePathRole ).toString() );

    if ( path.exists() )
        d_->setDirectory( path );
*/
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FileDialog::initialize()
{
    LOG_CLASS_INFO( "FileDialog", "initialize" );

    modifyFontPointSize setFontPointSize12( (mainWindow_->height() * 12) / SessionMainWindow::scalingDividerY_ );

    setFontPointSize12( this );

    //modifyPaletteForeground(setForegroundRole(modifyPaletteForeground::createPalette(hobart::colorPalette::gray)););

    modifyPaletteForeground setForegroundGray( modifyPaletteForeground::createPalette( hobart::colorPalette::gray ) );
        setForegroundGray( this );

    // create widgets
    d_ = new QFileDialog( this );
    d_->setObjectName( "d" );
#if defined( Q_OS_LINUX )
    d_->setOptions( options_ = (QFileDialog::DontUseNativeDialog | QFileDialog::ReadOnly) );
#elif defined( Q_OS_WIN )
    d_->setOptions( options_ = (QFileDialog::DontUseNativeDialog | QFileDialog::ReadOnly | QFileDialog::DontResolveSymlinks | QFileDialog::DontUseCustomDirectoryIcons) );

#endif

    // HW-61: windows is extreamly slow pulling all the file properties on a DOM... lets force into list mode by default
    d_->setViewMode( QFileDialog::List );

    Qt::WindowFlags f( d_->windowFlags() );
    f ^= (f & Qt::WindowType_Mask);
    d_->setWindowFlags( f );

    connect( d_, SIGNAL( accepted() ), SLOT( accept() ) );
    connect( d_, SIGNAL( finished(int) ), SLOT( done(int) ) );
    connect( d_, SIGNAL( rejected() ), SLOT( reject() ) );
    connect( d_, SIGNAL( directoryEntered(QString) ), SLOT( onDirectoryEntered(QString) ) );

    // create layouts
    QVBoxLayout *screenLayout = new QVBoxLayout( this );

    screenLayout->addWidget( d_ );

    // find the list view widget
    QList<QListView*> listViewWidgets( d_->findChildren<QListView*>( "listView" ) );

    if ( 1 != listViewWidgets.count() )
        LOG_WARN( "did not find list view widget!", NULL );
    else
    {
        listView_ = listViewWidgets[0];

        // allow single click of item to expand it
        connect( listView_, SIGNAL( clicked(QModelIndex) ), SLOT( onFileListingClicked(QModelIndex) ) );
    }

    // find the tree view widget
    QList<QTreeView*> treeViewWidgets( d_->findChildren<QTreeView*>( "treeView" ) );

    if ( 1 != treeViewWidgets.count() )
        LOG_WARN( "did not find tree view widget!", NULL );
    else
    {
        treeView_ = treeViewWidgets[0];

        // allow single click of item to expand it
        connect( treeView_, SIGNAL( clicked(QModelIndex) ), SLOT( onFileListingClicked(QModelIndex) ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FileDialog::updateReadOnly()
{
#ifdef Q_OS_WIN
    static const QString thumb( "D:" ); // HW-46
#else
    static const QString thumb( "/media/thumb" );
#endif

    QStringList fullControl;
    bool fullControlAllowed( false );

    QString dir( d_->directory().absolutePath() );

    // list of folders where full control is allowed
    fullControl.append( thumb );

    // check current dir versus folders that allow full control
    foreach ( const QString& i, fullControl )
        if (( i.length() <= dir.length() ) && ( i == dir.left( i.length() ) ))
        {
            fullControlAllowed = true;
            break;
        }

    // restrict access
    if ( !fullControlAllowed )
        d_->setOptions( options_ );

    // allow access
    else
    {
        QFileDialog::Options opt( options_ );
        opt ^= (opt & QFileDialog::ReadOnly);

        d_->setOptions( opt );
    }
}
