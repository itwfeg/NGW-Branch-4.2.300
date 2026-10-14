/**
 * @file gcdlayoutfileparser.h
 * Parser for reading layout files and modifying the user interface.
 *
 * This parser will parse a layout xml file and apply the settings to the ui.  The "Name" field
 * corresponds to a widget name and the associate properties apply to the widget.
 *
 * As the file is parsed the widget is looked up within the ui and the properties applied to it.
 *
 * Here is an example snippet of a layout file:
 * @code
 *   <?xml version='1.0'?>
 *   <DisplayObjects>
 *     <DisplayObject>
 *     	<Name>metrologyPage</Name>
 *     	<BGBmp>:/layouts/background.png</BGBmp>
 *     	<BGBmpFS>:/layouts/backgroundFS.png</BGBmpFS>
 *     </DisplayObject>
 *     <DisplayObject>
 *       <Name>productDescriptionLabel</Name>
 *       <Left>0</Left>
 *       <Top>1</Top>
 *       <Width>800</Width>
 *       <Height>52</Height>
 *       <FontColorRed>0</FontColorRed>
 *       <FontColorGreen>0</FontColorGreen>
 *       <FontColorBlue>0</FontColorBlue>
 *       <Font>Tahoma</Font>
 *       <FontPointSize>28</FontPointSize>
 *       <HAlign>Center</HAlign>
 *       <VAlign>Center</VAlign>
 *     </DisplayObject>
 *     <DisplayObject>
 *       <Name>storeLogoLabel</Name>
 *       <Left>10</Left>
 *       <Top>168</Top>
 *       <Width>250</Width>
 *       <Height>220</Height>
 *       <BGBmp>:/layouts/MMredblue.png</BGBmp>
 *     </DisplayObject>
 *     <DisplayObject>
 *       <Name>capacityLabel</Name>
 *       <Left>15</Left>
 *       <Top>340</Top>
 *       <Width>270</Width>
 *       <Height>26</Height>
 *       <FontColorRed>0</FontColorRed>
 *       <FontColorGreen>0</FontColorGreen>
 *       <FontColorBlue>0</FontColorBlue>
 *       <Font>Tahoma</Font>
 *       <FontPointSize>8</FontPointSize>
 *       <HAlign>Left</HAlign>
 *       <VAlign>Center</VAlign>
 *     </DisplayObject>
 *   </DisplayObjects>
 * @endcode
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup metrologyVideosCsu
 * @{
 */

#ifndef GCDLAYOUTFILEPARSER_H
#define GCDLAYOUTFILEPARSER_H

#include <QString>
#include <QtXml/QXmlSimpleReader>

#include <QMap>
#include <QList>

class gcdLayout;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Parses layout files and applies them to a ui.
/**
 * Example usage:
 * @code
 *   gcdLayout layout;
 *   gcdLayoutFileParser::parse( "layout.xml", layout );
 * @endcode
 */
class gcdLayoutFileParser : public QXmlDefaultHandler
{
    typedef gcdLayoutFileParser _Myt;
    typedef QXmlDefaultHandler _Mybase;

public:

    /// Destructor.
    virtual ~gcdLayoutFileParser() {}

    /// Parse and apply layout from file.
    /**
     * @param[in] filename  layout xml filename
     * @param[in,out] layout  gcd layout object
     * @return  @c true if layout file was parsed successfully, @c false otherwise
     */
    static bool parse(
        const QString& filename,
        gcdLayout& layout );

public:

    //
    // QXmlErrorHandler
    //

    /// Handle error.
    /**
     * Override of @c QXmlErrorHandler function.
     *
     * @param[in] ex  parser exception
     * @return  @c true if parsing should continue, @c false otherwise
     */
    virtual bool error( const QXmlParseException& ex );

    /// Handle fatal error.
    /**
     * Override of @c QXmlErrorHandler function.
     *
     * @param[in] ex  parser exception
     * @return  @c true if parsing should continue, @c false otherwise
     */
    virtual bool fatalError( const QXmlParseException& ex );

    /// Handle warning.
    /**
     * Override of @c QXmlErrorHandler function.
     *
     * @param[in] ex  parser exception
     * @return  @c true if parsing should continue, @c false otherwise
     */
    virtual bool warning( const QXmlParseException& ex );

    //
    // QXmlContentHandler
    //

    /// Start document.
    /**
     * Override of @c QXmlContentHandler function.
     *
     * @return  @c true if parsing should continue, @c false otherwise
     */
    virtual bool startDocument();

    /// Start element.
    /**
     * Override of @c QXmlContentHandler function.
     *
     * @param[in] namespaceURI  the namespace URI
     * @param[in] localName  the local name (without prefix)
     * @param[in] qName  the qualified name (with prefix)
     * @param[in] atts  the attributes attached to the element
     * @return  @c true if parsing should continue, @c false otherwise
     */
    virtual bool startElement(
        const QString& namespaceURI,
        const QString& localName,
        const QString& qName,
        const QXmlAttributes& atts );

    /// Handle character data.
    /**
     * Override of @c QXmlContentHandler function.
     *
     * @param[in] ch  character data
     * @return  @c true if parsing should continue, @c false otherwise
     */
    virtual bool characters( const QString& ch );

    /// End element.
    /**
     * Override of @c QXmlContentHandler function.
     *
     * @param[in] namespaceURI  the namespace URI
     * @param[in] localName  the local name (without prefix)
     * @param[in] qName  the qualified name (with prefix)
     * @return  @c true if parsing should continue, @c false otherwise
     */
    virtual bool endElement(
        const QString& namespaceURI,
        const QString& localName,
        const QString& qName );

    /// End document.
    /**
     * Override of @c QXmlContentHandler function.
     *
     * @return  @c true if parsing should continue, @c false otherwise
     */
    virtual bool endDocument();

private:

    gcdLayout& layout_;

    typedef QMap<QString, QString> stringMapType;

    unsigned int depth_;

    stringMapType displayObject_;
    stringMapType overlayObject_;
    stringMapType *current_;

    QString elementName_;

    /// Constructor.
    gcdLayoutFileParser( gcdLayout& layout ) : layout_( layout ), depth_( 0 ), current_( NULL ) {}

    /// Handle character data.
    bool objectCharacters( stringMapType& obj, const QString& ch );

    /// Handle display objects element.
    bool displayObjectStartElement(
        const QString& qName,
        const QXmlAttributes& atts );

    /// End display objects element.
    bool displayObjectEndElement(
        const QString& qName );

    /// Handle overlay objects element.
    bool overlayObjectStartElement(
        const QString& qName,
        const QXmlAttributes& atts );

    /// End overlay objects element.
    bool overlayObjectEndElement(
        const QString& qName );

    // not implemented
    gcdLayoutFileParser( const _Myt& );

    // not implemented
    _Myt& operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // GCDLAYOUTFILEPARSER_H
