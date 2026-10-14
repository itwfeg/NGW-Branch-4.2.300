#ifndef SOFTKEYTOGGLEMANAGER_H
#define SOFTKEYTOGGLEMANAGER_H

#include <QMetaType>
#include <QString>
#include <qobject.h>

/// Hobart namespace.
namespace hobart {


class softkeyToggleManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY( bool toggleFlashkeyON        READ toggleFlashkeyON     WRITE setToggleFlashkeyON     NOTIFY flashkeyOnChanged )
    Q_PROPERTY( int toggleFlashkeyRow        READ toggleFlashkeyRow    WRITE setToggleFlashkeyRow    NOTIFY flashkeyRowChanged )
    Q_PROPERTY( int toggleFlashkeyCol        READ toggleFlashkeyCol    WRITE setToggleFlashkeyCol    NOTIFY flashkeyColChanged )
    Q_PROPERTY( int toggleFlashkeyGroup      READ toggleFlashkeyGroup  WRITE setToggleFlashkeyGroup  NOTIFY flashkeyGroupChanged )
    Q_PROPERTY( int toggleFlashkeyBorder     READ toggleFlashkeyBorder WRITE setToggleFlashkeyBorder NOTIFY flashkeyBorderChanged )
    Q_PROPERTY( QString toggledFlashkeyImage READ toggledFlashkeyImage WRITE setToggledFlashkeyImage NOTIFY flashkeyImageChanged )

    typedef QObject _Mybase;

Q_SIGNALS:

    /**
     * Signal for toggled flashkey on changed.
     *
     * param[in] value  toggled
     */
    void flashkeyOnChanged( int value );

    /**
     * Signal for toggled flashkey row changed.
     *
     * param[in] value  row
     */
    void flashkeyRowChanged( int value );

    /**
     * Signal for toggled flashkey column changed.
     *
     * param[in] value  column
     */
    void flashkeyColChanged( int value );

    /**
     * Signal for toggled flashkey group changed.
     *
     * param[in] value  group
     */
    void flashkeyGroupChanged( int value );

    /**
     * Signal for toggled flashkey border changed.
     *
     * param[in] value  border width
     */
    void flashkeyBorderChanged( int value );

    /**
     * Signal for toggled image
     *
     * param[in] value  fractional price
     */
    void flashkeyImageChanged( QString value );

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     */
    explicit softkeyToggleManager( QObject *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~softkeyToggleManager();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * If softkey can display the toggled image.
     *
     * @returns  @c true to enable, @c false otherwise
     */
    virtual bool toggleFlashkeyON() const;

    /**
     * Retrieve row clicked.
     *
     * @returns  row
     */
    virtual int toggleFlashkeyRow() const;

    /**
     * Retrieve col clicked.
     *
     * @returns  col
     */
    virtual int toggleFlashkeyCol() const;

    /**
     * Retrieve group clicked.
     *
     * @returns  col
     */
    virtual int toggleFlashkeyGroup() const;

    /**
     * Retrieve border clicked.
     *
     * @returns  border width
     */
    virtual int toggleFlashkeyBorder() const;

    /**
      * Retrieve toggled background image.
      *
      * @returns toggled background image name
      */
    virtual QString toggledFlashkeyImage() const;

    ///////////////////////////////////////////////////////////////////////////

    /**
     * Set toggled background image.
     *
     * @param[in] value  image
     */
     virtual void setToggleFlashkeyON( bool value );

    /**
     * Set softkey row clicked.
     *
     * @param[in] value  action feature
     */
     virtual void setToggleFlashkeyRow( int value );

    /**
     * Set softkey column clicked.
     *
     * @param[in] value  action feature
     */
     virtual void setToggleFlashkeyCol( int value );

    /**
     * Set softkey column clicked.
     *
     * @param[in] value  action feature
     */
     virtual void setToggleFlashkeyGroup( int value );

    /**
     * Set softkey border clicked.
     *
     * @param[in] value  action feature
     */
     virtual void setToggleFlashkeyBorder( int value );

    /**
     * Set toggled image on softkey.
     *
     * @param[in] value  image
     */
    virtual void setToggledFlashkeyImage( const QString& value );

private Q_SLOTS:

    void onFlashkeyOnChanged( );
    void onFlashkeyRowChanged( );
    void onFlashkeyColChanged( );
    void onFlashkeyGroupChanged( );
    void onFlashkeyBorderChanged( );
    void onFlashkeyImageChanged( );


private:

    bool toggleFlashkeyON_;                     // toggled in flashkey action, stays true the entire time the key is toggled - any key in any feature
    int toggleFlashkeyRow_;                     // where it is located. Once set, can really just remain set
    int toggleFlashkeyCol_;                     // where it is located. Once set, can really just remain set
    int toggleFlashkeyGroup_;                   // what group is this flashkey a part of
    int toggleFlashkeyBorder_;                  // what the border width is we want to draw around this flashkey
    QString toggledFlashkeyImage_;              // Keep track and save the image name so we can use it anytime we want it

};

}

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */
#endif // SOFTKEYTOGGLEMANAGER_H
