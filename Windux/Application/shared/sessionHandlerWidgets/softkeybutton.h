/**
 * @file softkeybutton.h
 * Softkey button widget.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef SOFTKEYBUTTON_H
#define SOFTKEYBUTTON_H

#include <QMutex>
#include <QPushButton>
#include <QStringList>
#include <QCache>

class DomSoftkey;
class DomSoftkeyStyle;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class softkeyButton
 * @brief Softkey button widget.
 */
class softkeyButton : public QPushButton
{
    Q_OBJECT

    Q_ENUMS( CornerStyle )
    Q_PROPERTY( int action READ action WRITE setAction )
//    Q_PROPERTY( int group READ group WRITE setGroupNum )
    Q_PROPERTY( int actionFeature READ actionFeature WRITE setActionFeature )
    Q_PROPERTY( QString actionParam READ actionParam WRITE setActionParam )
    Q_PROPERTY( QColor backgroundColor READ backgroundColor WRITE setBackgroundColor )
    Q_PROPERTY( QString backgroundImage READ backgroundImage WRITE setBackgroundImage RESET resetBackgroundImage )
    Q_PROPERTY( QString toggledBackgroundImage READ toggledBackgroundImage WRITE setToggledBackgroundImage RESET resetToggledBackgroundImage )
    Q_PROPERTY( bool blinkWhenSelected READ blinkWhenSelected WRITE setBlinkWhenSelected )
    Q_PROPERTY( QColor borderColor READ borderColor WRITE setBorderColor )
    Q_PROPERTY( CornerStyle cornerStyle READ cornerStyle WRITE setCornerStyle )
    Q_PROPERTY( bool draggable READ isDraggable WRITE setDraggable )
    Q_PROPERTY( QString foregroundImage READ foregroundImage WRITE setForegroundImage RESET resetForegroundImage )
    Q_PROPERTY( QPoint gridPos READ gridPos WRITE setGridPos )
    Q_PROPERTY( QSize gridSize READ gridSize WRITE setGridSize )
    Q_PROPERTY( bool selected READ isSelected WRITE setSelected )
    Q_PROPERTY( Qt::Alignment textAlignment READ textAlignment WRITE setTextAlignment )
    Q_PROPERTY( QColor textColor READ textColor WRITE setTextColor )
    Q_PROPERTY( bool toggled READ isToggled WRITE setToggled )

    typedef softkeyButton _Myt;
    typedef QPushButton _Mybase;

public:

    static int fontPointSize_;   // Not const, can be set from value sent to ui in MadpScaleConfig
    static bool selfServiceMode_;

    /**
     * Corner styles.
     */
    enum CornerStyle
    {
        NormalCornerStyle,                              ///< Normal (pointed) corners.
        RoundedCornerStyle                              ///< Rounded corners.
    };

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     */
    explicit softkeyButton(
        QWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~softkeyButton();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve softkey action.
     *
     * @returns  action
     */
    virtual int action() const {return action_;}

    /**
     * Retrieve softkey action group.
     *
     * @returns  action feature
     */
    virtual int group() const {return group_;}

    /**
     * Retrieve softkey action feature.
     *
     * @returns  action feature
     */
    virtual int actionFeature() const {return actionFeature_;}

    /**
     * Retrieve softkey action parameter.
     *
     * @returns  action parameter
     */
    virtual QString actionParam() const {return actionParam_;}

    /**
     * Retrieve background color.
     *
     * @returns  color
     */
    virtual QColor backgroundColor() const {return backgroundColor_;}

    /**
     * Retrieve background image.
     *
     * @returns  image name
     */
    virtual QString backgroundImage() const {return backgroundImage_;}

    /**
     * Retrieve toggled background image.
     *
     * @returns toggled background image name
     */
    virtual QString toggledBackgroundImage() const {return toggledBackgroundImage_;}

    /**
     * If widget should blink when selected.
     *
     * @returns  @c true to blink, @c false otherwise
     */
    virtual bool blinkWhenSelected() const {return canBlink_;}

    /**
     * Retrieve border color.
     *
     * @returns  color
     */
    virtual QColor borderColor() const {return borderColor_;}

    /**
     * Retrieve corner style.
     *
     * @returns  style
     */
    virtual CornerStyle cornerStyle() const {return cornerStyle_;}

    /**
     * Retrieve foreground image.
     *
     * @returns  image name
     */
    virtual QString foregroundImage() const {return foregroundImage_;}

    /**
     * Retrieve grid position.
     *
     * @returns  point
     */
    virtual QPoint gridPos() const {return gridPos_;}

    /**
     * Retrieve grid size.
     *
     * @returns  size
     */
    virtual QSize gridSize() const {return gridSize_;}

    /**
     * Retrieve if draggable
     *
     * @returns  @c true if draggable, @c false otherwise
     */
    virtual bool isDraggable() const {return canDrag_;}

    /**
     * Retrieve if selected.
     *
     * @returns  @c true if selected, @c false otherwise
     */
    virtual bool isSelected() const {return selected_;}

    /**
     * Retrieve if toggled.
     *
     * @returns  @c true if toggled, @c false otherwise
     */
    virtual bool isToggled() const {return toggled_;}

    /**
     * Retrieve mime data type.
     *
     * @returns mime data type
     */
    static QString mimeDataType() {return "hobart/x-softkey";}

     /**
     * set overlay disabled flag.
     *
     * @param[in] value  overlayDisabled bool
     */
    virtual void disableOverlay( bool value ) { overlayDisabled_ = value; setDirtyFlag(); }
    
    /**
     * Set softkey action.
     *
     * @param[in] value  action
     */
    virtual void setAction( int value ) {action_ = value;}

    /**
     * Set softkey action group.
     *
     * @param[in] value  action group
     */
    virtual void setGroupNum( int value ) {group_ = value;}

    /**
     * Set softkey action feature.
     *
     * @param[in] value  action feature
     */
    virtual void setActionFeature( int value ) {actionFeature_ = value;}

    /**
     * Set softkey action parameter.
     *
     * @param[in] value  action parameter
     */
    virtual void setActionParam( const QString& value ) {actionParam_ = value;}

    /**
     * Set background color.
     *
     * @param[in] value  color
     */
    virtual void setBackgroundColor( const QColor& value );

    /**
     * Set background color.
     *
     * @param[in] value  color
     */
    virtual void setBackgroundColor( const QString& value ) {setBackgroundColor( makeColor( value ) );}

    /**
     * Set background image.
     *
     * @param[in] value  image
     */
    virtual void setBackgroundImage( const QString& value );

    /**
     * Set background image.
     *
     * @param[in] value  image
     */
    virtual void setFixedBackgroundImage( const QString& value );

    /**
     * set if we are using the new GEN2 User Interface
     *
     * @param[in] value  gen2Active
     */
    virtual void setGen2ActiveUI( bool );

    /**
     * Set toggled background image.
     *
     * @param[in] value  image
     */
    virtual void setToggledBackgroundImage( const QString& value );

    /**
     * Set if widget should blink when selected.
     *
     * @param[in]  @c true to blink, @c false otherwise
     */
    virtual void setBlinkWhenSelected( bool value ) {canBlink_ = value;}

    /**
     * Set border color.
     *
     * @param[in] value  color
     */
    virtual void setBorderColor( const QColor& value );

    /**
     * Set border color.
     *
     * @param[in] value  color
     */
    virtual void setBorderColor( const QString& value ) {setBorderColor( makeColor( value ) );}

    /**
     * Set corner style.
     *reg */
    virtual void setCornerStyle( CornerStyle value );

    /**
     * Set if draggable.
     *
     * @param[in] value  @c true to allow dragging, @c false otherwise
     */
    virtual void setDraggable( bool value ) {canDrag_ = value;}

    /**
     * Set foreground color.
     *
     * @param[in] value  image
     */
    virtual void setForegroundImage( const QString& value );

    /**
     * Set grid position.
     *
     * @param[in] value  point
     */
    virtual void setGridPos( const QPoint& value ) {gridPos_ = value;}

    /**
     * Set grid size.
     *
     * @param[in] value  size
     */
    virtual void setGridSize( const QSize& value ) {gridSize_ = value;}

    /**
     * Set if selected.
     *
     * @param[in]  @c true to select, @c false otherwise
     */
    virtual void setSelected( bool value );

    /**
     * Set text alignment.
     *
     * @param[in] value  alignment
     */
    virtual void setTextAlignment( Qt::Alignment value );

    /**
     * Set text color.
     *
     * @param[in] value  color
     */
    virtual void setTextColor( const QColor& value );

    /**
     * Set text color.
     *
     * @param[in] value  color
     */
    virtual void setTextColor( const QString& value ) {setTextColor( makeColor( value ) );}

    /**
     * Set text margins.
     *
     * @param[in] value  margins
     */
    virtual void setTextMargins( const QMargins& value ) {textMargins_ = value;}

    /**
     * Set if toggled.
     *
     * @param[in]  @c true to toggle, @c false otherwise
     */
    virtual void setToggled( bool value );

    /**
     * Set python script.
     *
     * @param[in] value script
     */
    virtual void setScript( QString& value );

    /**
     * Set python script interval.
     *
     * @param[in] value script
     */
    virtual void setScriptInterval( int& value ) {scriptInterval_ = value;}

    /**
     * Retrieve python script.
     *
     * @returns  script_
     */
    virtual QString script() {return script_;}

    /**
     * Retrieve text alignment.
     *
     * @returns  alignment
     */

    virtual int scriptInterval() {return scriptInterval_;}

    /**
     * Retrieve text alignment.
     *
     * @returns  alignment
     */

    virtual Qt::Alignment textAlignment() const {return textAlignment_;}

    /**
     * Retrieve text color.
     *
     * @returns  color
     */
    virtual QColor textColor() const;

    /**
     * Retrieve text margins.
     *
     * @returns  margins
     */
    virtual QMargins textMargins() const {return textMargins_;}

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Create softkey from session message.
     *
     * param[in] lang  1 for primary language, 2 for secondary language
     * param[in] m  session message
     * param[in] style  style
     */
    virtual void create( int lang, const DomSoftkey* m, const DomSoftkeyStyle *style );

    /**
     * Deserialize button from byte array.
     *
     * @param[in] a  byte array
     */
    virtual void deserialize( const QByteArray& a );

    /**
     * Serialize button into byte array.
     *
     * @returns  byte array
     */
    virtual QByteArray serialize() const;

public Q_SLOTS:

    /**
     * Reset background image to nothing.
     */
    virtual void resetBackgroundImage();

    /**
     * Reset foreground image to nothing.
     */
    virtual void resetForegroundImage();

    /**
     * Slot for overlay updates.
     */
    virtual void updateOverlay( );

    /**
     * Re-render overlay when new command data has arrived.
     *
     * @param[in] scriptName scriptName
     */
    virtual void newData(const QString& scriptName);


    /**
     * Slot for flash timer timeout.
     */
    void onFlashTimeout();

    /**
     * Reset toggled background image to nothing.
     */
    virtual void resetToggledBackgroundImage();

protected:

    QPixmap pressedImage_;                          ///< The depressed (down) image.
    QPixmap releasedImage_;                         ///< The released (up) image.
    QPixmap overlayPM_;

    ///////////////////////////////////////////////////////////////////////////
    // Events

    /**
     * Mouse move event.
     *
     * @param[in,out] e  mouse event
     */
    virtual void mouseMoveEvent( QMouseEvent *e );

    /**
     * Mouse press event.
     *
     * @param[in,out] e  mouse event
     */
    virtual void mousePressEvent( QMouseEvent *e );

    /**
     * Paint event.
     *
     * @param[in,out] e  paint event
     */
    virtual void paintEvent( QPaintEvent *e );

    /**
     * Resize event.
     *
     * @param[in,out] e  resize event
     */
    virtual void resizeEvent( QResizeEvent *e );

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Retrieve full path and filename of softkey image.
     *
     * @param[in] filename  filename as passing in from session
     * @param[out] thumb  thumbnail image path (if exists)
     * @return  image path
     */
    virtual QString makeSoftkeyImage( const QString& filename, QString *thumb = NULL ) const;

    /**
     * Update button images.
     */
    virtual void updatePixmaps();

    /**
     * Update text on images
     */
	void writeText();

    /**
     * Save image size to file so we can create thumbnails.
     */
    static void updateThumbnailHint( const QString& thumb, const QSize& s );

private Q_SLOTS:

    /**
     * Slot for timer timeout.
     */
    void onBlinkTimerTimeout();

private:

    int action_;
    int group_;
    QString actionParam_;
    int actionFeature_;

    QPoint gridPos_;
    QSize gridSize_;

    bool selected_;
    bool toggled_;
    
    bool overlayDisabled_;

    bool canBlink_;
    bool blinking_;

    QTimer *flashTimer_;
    int flashesRemaining_;
    bool flashFlag_;
    bool flashInfinitely_;

    Qt::Alignment textAlignment_;
    QMargins textMargins_;

    QColor backgroundColor_;
    QString backgroundImage_;

    QString toggledBackgroundImage_;

    QColor borderColor_;
    CornerStyle cornerStyle_;

    QString foregroundImage_;

    bool canDrag_;
    QPoint startPos_;
    bool dragging_;

    QStringList systemFolders_;
    bool dirty_;

    bool isGen2UI_;

    QString script_;
    int scriptInterval_;
    QStringList commands_;
    
    static const bool fontBold_;
    static const QString fontFamily_;

    static QTimer *blinkTimer_;

    static const int IMAGECACHE_MAX_ITEMS = 20;
    QCache<QString, QImage> imageCache;

    std::map<std::pair<int,int>, QPixmap> pixmapCache;

    /**
     * Parses overlay_ (sets commands_)
     *  @param[in] command string
     */
    void cmdParser( QString );

    /**
     * Draws overlay
     * * @param[in] QPainter object
     */
    void applyOverlay( QPainter& );

    /**
     * Set dirty flag (for repaint of widget)
     */
    void setDirtyFlag();

    /**
     * Read variable from stream.
     */
    template <class T>
    static T readFromDataStream( QDataStream& is ) {
        T temp;
        is >> temp;
        return temp;
    }

    /**
     * Make a color from string.
     */
    static QColor makeColor( const QString& c );

    /**
     * Save image size to file so we can create thumbnails.
     */
    static QMutex m_;
    static QMap<QString, QSize> thumbHints_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class softkeyButtonFactory
 * @brief Factory class for softkey button widgets.
 */
class softkeyButtonFactory
{
    typedef softkeyButtonFactory _Myt;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     */
    explicit softkeyButtonFactory() {}

    /**
     * Destructor.
     */
    virtual ~softkeyButtonFactory() {}

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Create new @c softkeyButton object.
     *
     * @param[in] parent  parent widget
     */
    virtual softkeyButton *create( QWidget *parent = 0 ) const;

    /**
     * Destroy @c softkeyButton object.
     *
     * @param[in] obj  pointer to object to destroy
     */
    virtual void destroy( softkeyButton *obj ) const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // SOFTKEYBUTTON_H
