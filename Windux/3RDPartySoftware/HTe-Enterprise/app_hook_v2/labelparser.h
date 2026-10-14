/**
 * @file labelparser.h
 * Simple label parser.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef LABELPARSER_H
#define LABELPARSER_H

#include <QSize>
#include <QString>

class QDataStream;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Simple label parser.
class labelParser
{
    typedef labelParser _Myt;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] bin  label binary blob
     */
    labelParser( const QString& bin );

    /// Deconstructor.
    virtual ~labelParser();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check label validity.
    /**
     * @return  @c true when valid, @c false otherwise
     */
    virtual bool isValid() const {return valid_;}

    /// Retrieve label name.
    /**
     * @return  label name
     */
    virtual QString name() const {return name_;}

    /// Retrieve label version.
    /**
     * @return  label version
     */
    virtual QString version() const {return customerVersion_;}

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Parse input stream.
    /**
     * Code stolen from label.cpp.
     * @param[in,out] is  input stream
     */
    virtual void parse( QDataStream& is );

    /// Parse form input stream.
    /**
     * Code stolen from labelform.cpp.
     * @param[in,out] is  input stream
     */
    virtual void parseForm( QDataStream& is );

private:

    bool valid_;

    int versionMajor_;
    int versionMinor_;

    int id_;
    QString name_;

    bool loaded_;

    QSize size_;
    int rotation_;
    int skew_;

    QString customerVersion_;

    // not implemented
    labelParser() = delete;

    // not implemented
    labelParser( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // LABELPARSER_H
