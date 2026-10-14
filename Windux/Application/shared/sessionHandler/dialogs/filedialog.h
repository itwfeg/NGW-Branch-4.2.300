/**
 * @file filedialog.h
 * Dialog wrapper for QFileDialog.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include "../sessiondialog.h"

#include <QFileDialog>

class QListView;
class QTreeView;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dialog wrapper for QFileDialog.
class FileDialog : public SessionDialog
{
    Q_OBJECT

    typedef FileDialog _Myt;
    typedef SessionDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit FileDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~FileDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve accept mode.
    /**
     * @return  accept mode
     */
    virtual QFileDialog::AcceptMode acceptMode() const {return d_->acceptMode();}

    /// Set accept mode.
    /**
     * @param[in] value  accept mode
     */
    virtual void setAcceptMode( QFileDialog::AcceptMode value ) {d_->setAcceptMode( value );}

    /// Retrieve directory.
    /**
     * @return  directory
     */
    virtual QDir directory() const {return d_->directory();}

    /// Set directory.
    /**
     * @param[in] value  directory
     */
    virtual void setDirectory( const QString& value ) {d_->setDirectory( value ); updateReadOnly();}

    /// Retrieve file mode.
    /**
     * @return  file mode
     */
    virtual QFileDialog::FileMode fileMode() const {return d_->fileMode();}

    /// Set file mode.
    /**
     * @param[in] value  file mode
     */
    virtual void setFileMode( QFileDialog::FileMode value ) {d_->setFileMode( value );}

    /// Retrieve filter.
    /**
     * @return  filter
     */
    virtual QDir::Filters filter() const {return d_->filter();}

    /// Set filter.
    /**
     * @param[in] value  filter
     */
    virtual void setFilter( QDir::Filters value ) {d_->setFilter( value );}

    /// Set filter.
    /**
     * @param[in] value  filter
     */
    virtual void setFilter( const QString& value ) {d_->setNameFilters( value.split( ";;" ) );}

    /// Set name filter.
    /**
     * @param[in] value  name filter
     */
    virtual void setNameFilter( const QString &filter ) {d_->setNameFilter( filter );}

    /// Retrieve name filters.
    /**
     * @return  name filters
     */
    virtual QStringList nameFilters() const {return d_->nameFilters();}

    /// Set name filters.
    /**
     * @param[in] value  name filters
     */
    virtual void setNameFilters( const QStringList& value ) {d_->setNameFilters( value );}

    /// Retrieve label text.
    /**
     * @return  text
     */
    virtual QString labelText( QFileDialog::DialogLabel label ) const {return d_->labelText( label );}

    /// Set label text
    /**
     * @param[in] value  text
     */
    virtual void setLabelText( QFileDialog::DialogLabel label, const QString& value ) {d_->setLabelText( label, value );}

    /// Retrieve options.
    /**
     * @return  options
     */
    virtual QFileDialog::Options options() const {return d_->options();}

    /// Set options.
    /**
     * @param[in] value  options
     */
    virtual void setOptions( QFileDialog::Options value );

    /// Retrieve selected files.
    /**
     * @return  files
     */
    virtual QStringList selectedFiles() const {return d_->selectedFiles();}

    /// Set file
    /**
     * @param[in] value  file
     */
    virtual void selectFile( const QString& value ) {d_->selectFile( value );}

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Hide event.
    /**
     * @param[in,out] e  hide event
     */
    virtual void hideEvent( QHideEvent *e );

    /// Show event.
    /**
     * @param[in,out] e  show event
     */
    virtual void showEvent( QShowEvent *e );

private Q_SLOTS:

    /// Slot to handle directory entered.
    void onDirectoryEntered( const QString& dir );

    /// Slot to handle file listing clicked.
    void onFileListingClicked( const QModelIndex& index );

private:

    QFileDialog *d_;
    QFileDialog::Options options_;

    QListView *listView_;
    QTreeView *treeView_;

    /// Initialize.
    void initialize();

    /// Update file dialog read only property.
    void updateReadOnly();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // FILEDIALOG_H
