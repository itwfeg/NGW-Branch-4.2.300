/**
 * @file labeledunitpriceedit.h
 * Unit price field with support for "fractional" and "pounds for" pricing.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef LABELEDUNITPRICEEDIT_H
#define LABELEDUNITPRICEEDIT_H

#include "labelednumericlineedit.h"

class QComboBox;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class labeledUnitPriceEdit
 * @brief Unit price field with support for "fractional" and "pounds for" pricing.
 */
class labeledUnitPriceEdit : public labeledNumericLineEdit
{
    Q_OBJECT

    Q_PROPERTY( int fractional READ fractional WRITE setFractional NOTIFY fractionalChanged )
    Q_PROPERTY( bool fractionalEnabled READ isFractionalEnabled WRITE setFractionalEnabled )
    Q_PROPERTY( bool metric READ isMetric WRITE setMetric )
    Q_PROPERTY( int weightFor READ weightFor WRITE setWeightFor NOTIFY weightForChanged )
    Q_PROPERTY( bool weightForEnabled READ isWeightForEnabled WRITE setWeightForEnabled )

    typedef labeledUnitPriceEdit _Myt;
    typedef labeledNumericLineEdit _Mybase;

Q_SIGNALS:

    /**
     * Signal for fractional price changed.
     *
     * param[in] value  fractional price
     */
    void fractionalChanged( int value );

    /**
     * Signal for weight for price changed.
     *
     * param[in] value  weight for price
     */
    void weightForChanged( int value );

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     * @param[in] image  background image for the widget
     */
    explicit labeledUnitPriceEdit(
        QWidget *parent = 0,
        const QPixmap& image = QPixmap() );

    /**
     * Destructor.
     */
    virtual ~labeledUnitPriceEdit();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve the fractional price.
     *
     * @returns  fractional price
     */
    virtual int fractional() const;

    /**
     * Retrieve if metric.
     *
     * @returns  @c true if metric, @c false otherwise
     */
    virtual bool isMetric() const {return metric_;}

    /**
     * Retrieve if fractional pricing is enabled.
     *
     * @returns  @c true if enabled, @c false otherwise
     */
    virtual bool isFractionalEnabled() const {return fractional_;}

    /**
     * Retrieve if weight for pricing is enabled.
     *
     * @returns  @c true if enabled, @c false otherwise
     */
    virtual bool isWeightForEnabled() const {return weightFor_;}

    /**
     * Set if control is read only.
     *
     * @param[in] value  @c true for read only, @c false for otherwise
     */
    virtual void setReadOnly( bool value );

    /**
     * Retrieve the weight for lbs (or kgs).
     *
     * @returns  lbs or kgs for
     */
    virtual int weightFor() const;

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Translate the widget fields.
     */
    virtual void translate();

public Q_SLOTS:

    /**
     * Clear edit control.
     */
    virtual void clear();

    /**
     * Set fractional pricing.
     *
     * @param[in] value  fractional pricing
     */
    virtual void setFractional( int value );

    /**
     * Set fractional pricing enabled.
     *
     * @param[in] value  @c true if enabled, @c false otherwise
     */
    virtual void setFractionalEnabled( bool value );

    /**
     * Set if metric.
     *
     * @param[in] value  @c true if metric, @c false otherwise
     */
    virtual void setMetric( bool value );

    /**
     * Set the weight for lbs (or kgs).
     *
     * @param[in] value  lbs or kgs for
     */
    virtual void setWeightFor( int value );

    /**
     * Set weight for pricing enabled.
     *
     * @param[in] value  @c true if enabled, @c false otherwise
     */
    virtual void setWeightForEnabled( bool value );

private Q_SLOTS:

    /**
     * Slot for fractional for changed.
     */
    void onFractionalIndexChanged( int index );

    /**
     * Slot for weight for changed.
     */
    void onWeightForIndexChanged( int index );

private:

    bool fractional_;
    bool weightFor_;

    bool metric_;

    QComboBox *weightForCombo_;
    QLabel *prefix_;

    QComboBox *fractionalCombo_;

    /**
     * Initialize.
     */
    void initialize();

    /**
     * Setup fractional pricing options.
     */
    void setFractionalComboOptions();

    /**
     * Setup weight for pricing options.
     */
    void setWeightForComboOptions();

    /**
     * Update prefix.
     */
    void updatePrefix();

    /**
     * Update suffix.
     */
    void updateSuffix();

    /**
     * Update font point size.
     */
    void updateFontPointSize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // LABELEDUNITPRICEEDIT_H
