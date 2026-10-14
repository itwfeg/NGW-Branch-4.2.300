/**
 * @file sellbysoftkeybutton.h
 * simple attributes to support toggled softkeys with Add Sell By Day
 *
 * @copyright Copyright (c) 2022 Hobart Corporation
 * @author Joe Crew
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef SELLBYSOFTKEYBUTTON_H
#define SELLBYSOFTKEYBUTTON_H

#include <QObject>
#include <QString>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class sellBySoftkeyButton
 * @brief Add Sell By Day Softkey Action
 */
class sellBySoftkeyButton : public QObject
{
    typedef sellBySoftkeyButton _Myt;

    Q_OBJECT

    Q_PROPERTY( bool toggleExtraDayON READ toggleExtraDayON WRITE setToggleExtraDayON )
    Q_PROPERTY( int toggleExtraDayRow READ toggleExtraDayRow WRITE setToggleExtraDayRow )
    Q_PROPERTY( int toggleExtraDayCol READ toggleExtraDayCol WRITE setToggleExtraDayCol )
    Q_PROPERTY( QString toggledExtraDayImage READ toggledExtraDayImage WRITE setToggledExtraDayImage RESET resetToggledExtraDayImage)

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     */
    explicit sellBySoftkeyButton();

    /**
     * Destructor.
     */
    virtual ~sellBySoftkeyButton();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * If softkey can display the toggled image.
     *
     * @returns  @c true to enable, @c false otherwise
     */
    virtual bool toggleExtraDayON() const  {return toggleExtraDayON_;}

    /**
     * Retrieve row clicked.
     *
     * @returns  row
     */
    virtual int toggleExtraDayRow() const {return toggleExtraDayRow_;}

    /**
     * Retrieve col clicked.
     *
     * @returns  col
     */
    virtual int toggleExtraDayCol() const {return toggleExtraDayCol_;}

    /**
      * Retrieve toggled background image.
      *
      * @returns toggled background image name
      */
    virtual QString toggledExtraDayImage() const {return toggledExtraDayImage_;}

    ///////////////////////////////////////////////////////////////////////////

    /**
     * Set toggled background image.
     *
     * @param[in] value  image
     */
     virtual void setToggleExtraDayON( bool value ) {toggleExtraDayON_ = value;}

    /**
     * Set softkey row clicked.
     *
     * @param[in] value  action feature
     */
     virtual void setToggleExtraDayRow( int value ) {toggleExtraDayRow_ = value;}

    /**
     * Set softkey column clicked.
     *
     * @param[in] value  action feature
     */
     virtual void setToggleExtraDayCol( int value ) {toggleExtraDayCol_ = value;}

    /**
     * Set toggled image on softkey.
     *
     * @param[in] value  image
     */
    virtual void setToggledExtraDayImage( const QString& value );

    /**
     * Reset toggled background image to nothing.
     */
    virtual void resetToggledExtraDayImage();

private:

    bool toggleExtraDayON_;                     // toggled in flashkey action, stays true the entire time the key is toggled - any key in any feature
    int toggleExtraDayRow_;                     // where it is located. Once set, can really just remain set
    int toggleExtraDayCol_;                     // where it is located. Once set, can really just remain set
    QString toggledExtraDayImage_;              // Keep track and save the image name so we can use it anytime we want it
};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // SELLBYSOFTKEYBUTTON_H
