/**
 * @file superReportingClasses.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "superReportingClasses.h"

#include "../sessionmainwindow.h"

#include "../dialogs/numericinputboxdialog.h"

#include "../modes/supervisormode.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <numericlineedit.h>
#include <palette.h>

#include <ScaleConfiguration.h>
#include <TokenParser.h>
#include <UiUtilities.h>

#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QPushButton>
#include <QSignalMapper>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

const int superReportingClasses::fontPointSize_ = 18;

///////////////////////////////////////////////////////////////////////////////////////////////////
superReportingClassesItemDelegate::superReportingClassesItemDelegate( QObject *parent ) :
    _Mybase( parent )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QWidget *superReportingClassesItemDelegate::createEditor( QWidget *parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    QWidget *w( _Mybase::createEditor( parent, option, index ) );
    QLineEdit *lineEdit;

    if (( w ) && ( (lineEdit = dynamic_cast<QLineEdit*>( w )) ))
    {
        // install validator
        lineEdit->setValidator( new QIntValidator( 0, 999999, lineEdit ) );
    }

    return w;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
superReportingClasses::superReportingClasses( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    current_( NULL ),
    dirty_( false ),
    dirtyRefresh_( false ),
    clicks_( 0 ),
    insert_( false )
{
    // set object name
    setObjectName( "superReportingClasses" );

    // initialize
    initialize();
    translate();

    // request classes
    session_->send( ClassDev, ClassGetTree );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
superReportingClasses::~superReportingClasses()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superReportingClasses::translate()
{
    // title
    title_->setText( tr( "Reporting Classes" ) );

    // buttons
    insertRow_->setText( tr( "Insert\nClass" ) );
    insertChild_->setText( tr( "Insert\nSub-Class" ) );
    delete_->setText( tr( "Delete" ) );

    // table headers
    QStringList tableHeaders;
    tableHeaders.push_back( tr( "Number" ) );
    tableHeaders.push_back( tr( "Description" ) );
    tableHeaders.push_back( tr( "Reporting Class" ) );

    // table
    table_->setHeaderLabels( tableHeaders );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool superReportingClasses::onScreenChanging()
{
    // save!
    saveCurrentItem();

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superReportingClasses::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "superReportingClasses", "onSessionMessage" );

    int device = msg[0].toInt();
    int event = msg[1].toInt();

    // block signals while processing
    table_->blockSignals( true );

    // class information
    if (( ClassDev == device ) && ( ClassDtWrite == event ))
    {
        DomClassTreeNode m( msg );

        QTreeWidgetItem *item = findItem( m.classNumber() );

        if ( !item )
            LOG_WARN( "class value '%s' does not exist", qPrintable( m.classNumber() ) );

        else
        {
            item->setData( 1, 0, session_->uiUtilities->stripFontChanges( m.classDescription() ) );
            item->setData( 2, 0, m.reportingClass() );
        }
    }

    // class tree
    else if (( ClassDev == device ) && ( ClassGetTree == event ))
    {
        DomClassTreeRecords m( msg );

        // clear existing data
        table_->clear();

        current_ = NULL;
        dirty_ = false;
        dirtyRefresh_ = false;
        clicks_ = 0;

        // create root node
        QTreeWidgetItem *rootNode = createItem( "999999" );

        table_->addTopLevelItem( rootNode );

        // tokenize data
        QVector<QString> treeList( tokenizeTreeList( m.treeList() ) );

        LOG_DEBUG( "count %d", treeList.count() );

        if ( treeList.count() != m.numOfItems() )
            LOG_WARN( "mismatch in size of tree list %d %d", treeList.count(), m.numOfItems() );

        int count = qMin( treeList.count(),  m.numOfItems() );

        LOG_DEBUG( "processing %d nodes...", count );

        for ( int i = 0; i < count; i++ )
        {
            LOG_TRACE( "add child node...", NULL );
            parseChildren( treeList[i] );
        }

        // expand all data
        table_->expandAll();

        // select root element
        if ( rootNode )
        {
            rootNode->setSelected( true );
            onItemSelectionChanged(); // FT-547
        }
    }

    // inserted class
    else if (( Error == device ) && ( ErrClassDevice == event ))
    {
        returnedErrorsClassDev m( msg );

        if ( 110002 == m.errorCode() )
            if ( returnedErrorsClassDev::ActionYes == m.errorData2() )
            {
                if ( insert_ )
                {
                    // create
                    session_->send( ClassDev, ClassDtCreateNew,
                        insertClass_ + "," +
                        insertReportingClass_ + "," +
                        "\"\"" );

                    // find reporting class
                    QTreeWidgetItem *parent = findItem( insertReportingClass_ );

                    if ( !parent )
                        session_->send( ClassDev, ClassGetTree );
                    else
                    {
                        int i = 0;

                        // create child item
                        QTreeWidgetItem *item = createItem( insertClass_ );

                        // insert child into tree
                        while ( i < parent->childCount() )
                        {
                            QTreeWidgetItem *child( parent->child( i ) );

                            if ( insertClass_.toInt() < child->data( 0, 0 ).toString().toInt() )
                            {
                                parent->insertChild( i, item );
                                break;
                            }

                            ++i;
                        }

                        // append child
                        if ( i >= parent->childCount() )
                            parent->addChild( item );

                        // select inserted item
                        table_->clearSelection();

                        item->setSelected( true );
                        setCurrentItem( item );
                    }

                    insert_ = false;
                }
            }
    }

    // done blocking
    table_->blockSignals( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superReportingClasses::onButtonClicked( QWidget *button )
{
    LOG_CLASS_INFO( "superReportingClasses", "onButtonClicked" );

    // sanity check
    if ( !button )
        return;

    QList<QTreeWidgetItem*> items = table_->selectedItems();

    // nothing selected (only one thing selectable at a time)
    if ( 1 != items.count() )
        return;

    QTreeWidgetItem *item( items.at( 0 ) );

    // insert row
    // insert child
    if (( insertRow_ == button ) || ( insertChild_ == button ))
    {
        LOG_INFO( "insert button clicked", NULL );

        NumericInputBoxDialog dialog( mainWindow_ );

        dialog.setText( tr( "Please enter the class number:" ) );
        dialog.setValidator( new QIntValidator( 0, 999999, this ) );

        if ( QDialog::Accepted == dialog.exec() )
        {
            QString inputText( dialog.inputText() );
            QString reportingClass;

            if ( current_ )
            {
                if ( insertRow_ == button )
                    reportingClass = current_->parent()->data( 0, 0 ).toString();
                else if ( insertChild_ == button )
                    reportingClass = current_->data( 0, 0 ).toString();
            }

            if (( inputText.length() ) && ( "0" != inputText ) && ( reportingClass.length() ))
            {
                // insert!
                insertClass_ = inputText;
                insertReportingClass_ = reportingClass;
                insert_ = true;

                session_->send( ClassDev, ClassDtRead, inputText );
            }
        }
    }

    // delete
    else if ( delete_ == button )
    {
        QString value( item->data( 0, 0 ).toString() );

        LOG_INFO( "delete button clicked '%s'", NULL );
        session_->send( ClassDev, ClassDtDelete, value );

        // request classes
        session_->send( ClassDev, ClassGetTree );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superReportingClasses::onItemChanged( QTreeWidgetItem *item, int column )
{
    if ( current_ == item )
        if (( 1 == column ) || ( 2 == column ))
        {
            dirty_ = true;
            dirtyRefresh_ = ( 2 == column );
        }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superReportingClasses::onItemClicked( QTreeWidgetItem *item, int column )
{
    setItemEditable( item, column );

    // edit after selected and clicked
    if ( 1 < ++clicks_ )
        table_->editItem( item, column );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superReportingClasses::onItemDoubleClicked( QTreeWidgetItem *item, int column )
{
    setItemEditable( item, column );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superReportingClasses::onItemSelectionChanged()
{
    LOG_CLASS_INFO( "superReportingClasses", "onItemSelectionChanged" );

    QList<QTreeWidgetItem*> items = table_->selectedItems();

    // nothing selected (only one thing selectable at a time)
    if ( 1 != items.count() )
    {
        LOG_DEBUG( "nothing selected", NULL );

        insertRow_->setEnabled( false );
        insertChild_->setEnabled( false );
        delete_->setEnabled( false );

        setCurrentItem( NULL );
    }

    // process selected item
    else
    {
        QTreeWidgetItem *item( items.at( 0 ) );

        QString value( item->data( 0, 0 ).toString() );
        bool canInsertRow = ("999999" != value);
        bool canDelete = (("999999" != value) && ("999995" != value));

        insertRow_->setEnabled( canInsertRow );
        insertChild_->setEnabled( true );
        delete_->setEnabled( canDelete );

        setCurrentItem( item );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superReportingClasses::initialize()
{
    modifyPaletteHighlight setHighlight( hobart::colorPalette::blue );
    modifyPaletteHighlightText setHighlightText( hobart::colorPalette::white );

    modifyFontPointSize setFontPointSize( scaleFontPointSizeForScreen( fontPointSize_ ) );

    setFontPointSize( this );

    // signal mapper
    buttons_ = new QSignalMapper( this );

    connect( buttons_, SIGNAL( mapped(QWidget*) ), SLOT( onButtonClicked(QWidget*) ) );

    // title
    title_ = new QLabel( this );
    title_->setObjectName( "title" );

    // insert row
    insertRow_ = new QPushButton( this );
    insertRow_->setEnabled( false );
    insertRow_->setObjectName( "insertRow" );

    buttons_->setMapping( insertRow_, insertRow_ );
    connect( insertRow_, SIGNAL( clicked() ), buttons_, SLOT( map() ) );

    // insert child
    insertChild_ = new QPushButton( this );
    insertChild_->setEnabled( false );
    insertChild_->setObjectName( "insertChild" );

    buttons_->setMapping( insertChild_, insertChild_ );
    connect( insertChild_, SIGNAL( clicked() ), buttons_, SLOT( map() ) );

    // delete
    delete_ = new QPushButton( this );
    delete_->setEnabled( false );
    delete_->setObjectName( "delete" );

    buttons_->setMapping( delete_, delete_ );
    connect( delete_, SIGNAL( clicked() ), buttons_, SLOT( map() ) );

    // table
    table_ = new QTreeWidget( this );
    table_->setObjectName( "table" );
    table_->setSelectionMode ( QAbstractItemView::SingleSelection );
    table_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    table_->setUniformRowHeights( true );
    setFontPointSize( table_ );
    setHighlight( table_ );
    setHighlightText( table_ );    

    table_->setStyleSheet(
        "QTreeView::branch:has-siblings:!adjoins-item {"
                "border-image: url(\":/images/screens/vline.png\") 0;}"
        "QTreeView::branch:has-siblings:adjoins-item {"
                "border-image: url(\":/images/screens/branch-more.png\") 0;}"
        "QTreeView::branch:!has-children:!has-siblings:adjoins-item {"
                "border-image: url(\":/images/screens/branch-end.png\") 0;}"
        "QTreeView::branch:has-children:!has-siblings:closed,"
        "QTreeView::branch:closed:has-children:has-siblings {"
                "border-image: none;"
                "image: url(\":/images/screens/plus.png\");}"
        "QTreeView::branch:open:has-children:!has-siblings,"
        "QTreeView::branch:open:has-children:has-siblings {"
                "border-image: none;"
                "image: url(\":/images/screens/minus.png\");}" );

    table_->setColumnCount( 3 );
    table_->setColumnWidth( 0, scaleWidthForScreen( 192 ) );
    table_->setColumnWidth( 1, scaleWidthForScreen( 512 ) );
    table_->setColumnWidth( 2, scaleWidthForScreen( 192 ) );

    table_->setItemDelegateForColumn( 2, new superReportingClassesItemDelegate( this ) );

    connect( table_, SIGNAL( itemChanged(QTreeWidgetItem*,int) ), SLOT( onItemChanged(QTreeWidgetItem*,int) ) );
    connect( table_, SIGNAL( itemClicked(QTreeWidgetItem*,int) ), SLOT( onItemClicked(QTreeWidgetItem*,int) ) );
    connect( table_, SIGNAL( itemDoubleClicked(QTreeWidgetItem*,int) ), SLOT( onItemDoubleClicked(QTreeWidgetItem*,int) ) );
    connect( table_, SIGNAL( itemSelectionChanged() ), SLOT( onItemSelectionChanged() ) );

    // setup layout
    QVBoxLayout *screenLayout = new QVBoxLayout( this );
    screenLayout->setMargin( marginSpacing_ );

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addWidget( title_ );
    hlayout->addStretch();
    hlayout->addWidget( insertRow_ );
    hlayout->addWidget( insertChild_ );
    hlayout->addWidget( delete_ );

    screenLayout->addLayout( hlayout );
    screenLayout->addWidget( table_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superReportingClasses::parseChildren( const QString& data )
{
    LOG_CLASS_INFO( "superReportingClasses", "parseChildren" );

    QVector<QString> v( TokenParser::splitStringTokens( data ) );
    QTreeWidgetItem *parent;

    // must have 3 elements
    // 0 = class
    // 1 = child count
    // 2 = child classes
    if ( 3 != v.count() )
        LOG_WARN( "bad vector count %d", v.count() );

    // parent must exist
    else if ( !(parent = findItem( v[0] )) )
        LOG_WARN( "parent value '%s' does not exist", qPrintable( v[0] ) );

    else
    {
        // tokenize data
        QVector<QString> children( TokenParser::splitStringTokens( v[2] ) );
        int childrenSize = v[1].toInt();

        LOG_DEBUG( "count %d", children.count() );

        if ( children.count() != childrenSize )
            LOG_WARN( "mismatch in child count %d %d", children.count(), childrenSize );

        int count = qMin( children.count(), childrenSize );

        LOG_DEBUG( "processing %d nodes...", count );

        for ( int i = 0; i < count; i++ )
        {
            QTreeWidgetItem *node;

            LOG_TRACE( "add child node...", NULL );
            parent->addChild( (node = createItem( children[i] )) );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QTreeWidgetItem *superReportingClasses::createItem( const QString& value )
{
    QTreeWidgetItem *item = new QTreeWidgetItem();

    // set item data
    item->setData( 0, 0, value );
    item->setData( 1, 0, "" );
    item->setData( 2, 0, "" );

    // request class info
    session_->send( ClassDev, ClassDtRead, value );

    return item;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QTreeWidgetItem *superReportingClasses::findItem( const QString& value )
{
    QList<QTreeWidgetItem*> items = table_->findItems( value, Qt::MatchFixedString | Qt::MatchCaseSensitive | Qt::MatchRecursive, 0 );

    // found it!
    if ( 1 == items.size() )
        return items[0];

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superReportingClasses::setItemEditable( QTreeWidgetItem *item, int column )
{
    QString value( item->data( 0, 0 ).toString() );
    bool canEdit = (( "999999" != value ) && ( "999995" != value ));

    // block signals which changing
    table_->blockSignals( true );

    // set editable
    Qt::ItemFlags flags( item->flags() );

    if (( canEdit ) && (( 1 == column ) || ( 2 == column )))
        flags |= Qt::ItemIsEditable;
    else
        flags &= ~Qt::ItemIsEditable;

    item->setFlags( flags );

    // done blocking
    table_->blockSignals( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superReportingClasses::setCurrentItem( QTreeWidgetItem *item )
{
    // nothing to do
    if ( current_ == item )
        return;

    // save current item before switch
    saveCurrentItem();

    // update
    current_ = item;
    dirty_ = false;
    dirtyRefresh_ = false;
    clicks_ = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superReportingClasses::saveCurrentItem()
{
    // save current
    if (( current_ ) && ( dirty_ ))
    {
        // save!
        session_->send( ClassDev, ClassDtSave,
            current_->data( 0, 0 ).toString() + "," +
            current_->data( 2, 0 ).toString() + "," +
            "\"" + current_->data( 1, 0 ).toString() + "\"" );

        // request classes
        if ( dirtyRefresh_ )
            session_->send( ClassDev, ClassGetTree );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QVector<QString> superReportingClasses::tokenizeTreeList( const QString& listOf )
{
    QString temp( listOf.trimmed() );

    // create delimiters
    temp.replace( "\"\",", "\"\":" );

    // split list
    QList<QString> list = temp.split( ":" );

    // populate result
    QVector<QString> result;

    foreach ( const QString& s, list )
    {
        QString formatted;

        // remove leading and trailing quote
        if (( 2 <= s.length() ) && ( '\"' == s[0] ) && ( '\"' == s[s.length()-1] ))
            formatted = s.mid( 1, s.length()-2 );
        else
            formatted = s;

        result.append( formatted );
    }

    return result;
}

