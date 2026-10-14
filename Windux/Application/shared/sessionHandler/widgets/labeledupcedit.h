/**
 * @file labeledupcedit.h
 * UPC widget with editable prefix digit.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef LABELEDUPCEDIT_H
#define LABELEDUPCEDIT_H

#include "labeledlineedit.h"

class QComboBox;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class labeledUpcEdit
 * @brief UPC widget with editable prefix digit.
 */
class labeledUpcEdit : public labeledLineEdit
{
    Q_OBJECT

    Q_PROPERTY( int prefix READ prefix WRITE setPrefix NOTIFY prefixChanged )

    typedef labeledUpcEdit _Myt;
    typedef labeledLineEdit _Mybase;

Q_SIGNALS:

    /**
     * Signal for upc prefix changed.
     *
     * param[in] value  prefix digit
     */
    void prefixChanged( int value );

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     * @param[in] image  background image for the widget
     */
    explicit labeledUpcEdit(
        QWidget *parent = 0,
        const QPixmap& image = QPixmap() );

    /**
     * Destructor.
     */
    virtual ~labeledUpcEdit();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve if upc prefix is visible.
     *
     * @returns  @c true if visible, @c false otherwise
     */
    virtual bool isPrefixVisible() const;

    /**
     * Retrieve upc prefix.
     *
     * @returns  prefix digit
     */
    virtual int prefix() const;

public Q_SLOTS:

    /**
     * Clear edit control.
     */
    virtual void clear();

    /**
     * Set upc prefix.
     *
     * @param[in] value  prefix digit
     */
    virtual void setPrefix( int value );

    /**
     * Set if prefix is visible or not.
     *
     * @param[in] value  @c true to show prefix combo, @c false otherwise
     */
    virtual void setPrefixVisible( bool value );

private:

    QComboBox *prefixCombo_;

    /**
     * Initialize.
     */
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // LABELEDUPCEDIT_H
