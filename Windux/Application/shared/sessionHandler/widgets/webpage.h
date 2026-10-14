/**
 * @file webpage.h
 * Hobart web page widget.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef WEBPAGE
#define WEBPAGE

#include <QWebPage>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Hobart web page widget.
class webPage : public QWebPage
{
    Q_OBJECT

    typedef webPage _Myt;
    typedef QWebPage _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    explicit webPage( QObject* parent = 0 );

    /// Destructor.
    virtual ~webPage();

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Check to accept navigation request.
    /**
     * @param[in,out] frame  request frame
     * @param[in] request  request
     * @param[in] type  type
     * @return  @c true to accept, @c false otherwise
     */
    virtual bool acceptNavigationRequest( QWebFrame *frame, const QNetworkRequest &request, NavigationType type );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // hobart

#endif // WEBPAGE

