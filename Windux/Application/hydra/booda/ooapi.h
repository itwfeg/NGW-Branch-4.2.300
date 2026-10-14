/**
 * @file ooapi.h
 * @brief Online Ordering API.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef OOAPI_H
#define OOAPI_H

#include "serializedjsonapi.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Online Ordering API.
class OnlineOrderingAPI : public SerializedJsonWebInterface
{
    Q_OBJECT

    using _Myt = OnlineOrderingAPI;
    using _Mybase = SerializedJsonWebInterface;

signals:

    /// Signal for category received.
    /**
     * @param[in] obj  data
     */
    void categoryReceived( const QJsonObject& obj );

    /// Signal for category image received.
    /**
     * @param[in] name  image name
     * @param[in] a  image data
     */
    void categoryImageReceived( const QString& name, const QByteArray& a );

    /// Signal for department received.
    /**
     * @param[in] obj  data
     */
    void departmentReceived( const QJsonObject& obj );

    /// Signal for operator received.
    /**
     * @param[in] obj  data
     */
    void operatorReceived( const QJsonObject& obj );

    /// Signal for product received.
    /**
     * @param[in] obj  data
     */
    void productReceived( const QJsonObject& obj );

    /// Signal for product image received.
    /**
     * @param[in] name  image name
     * @param[in] a  image data
     */
    void productImageReceived( const QString& name, const QByteArray& a );

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in,out] parent  parent object
     */
    OnlineOrderingAPI( QObject *parent = nullptr );

    /// Destructor.
    ~OnlineOrderingAPI();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set server address.
    /**
     * @param[in] value  server address (i.e. https://some.server.com:port)
     */
    virtual void setServerAddress( const QString& value ) {address_ = value;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Delete all categories.
    virtual void deleteAllCategories() {deleteAll( DELETE_ALL_CATEGORIES );}

    /// Delete all category images.
    virtual void deleteAllCategoryImages() {deleteAll( DELETE_ALL_CATEGORY_IMAGES );}

    /// Delete all departments.
    virtual void deleteAllDepartments() {deleteAll( DELETE_ALL_DEPARTMENTS );}

    /// Delete all operators.
    virtual void deleteAllOperators() {deleteAll( DELETE_ALL_OPERATORS );}

    /// Delete all products.
    virtual void deleteAllProducts() {deleteAll( DELETE_ALL_PRODUCTS );}

    /// Delete all product images.
    virtual void deleteAllProductImages() {deleteAll( DELETE_ALL_PRODUCT_IMAGES );}

    /// Delete category.
    /**
     * @param[in] dept  department
     * @param[in] num  category number
     */
    virtual void deleteCategory( const QString& dept, const QString& num ) {deleteOne( DELETE_CATEGORY, dept, num );}

    /// Delete category image.
    /**
     * @param[in] filename  file name
     */
    virtual void deleteCategoryImage( const QString& filename ) {deleteOne( DELETE_CATEGORY_IMAGE, filename );}

    /// Delete department.
    /**
     * @param[in] num  department number
     */
    virtual void deleteDepartment( const QString& num ) {deleteOne( DELETE_DEPARTMENT, num );}

    /// Delete operator.
    /**
     * @param[in] id  operator id
     */
    virtual void deleteOperator( const QString& id ) {deleteOne( DELETE_OPERATOR, id );}

    /// Delete product.
    /**
     * @param[in] dept  department
     * @param[in] num  product number
     */
    virtual void deleteProduct( const QString& dept, const QString& num ) {deleteOne( DELETE_PRODUCT, dept, num );}

    /// Delete product image.
    /**
     * @param[in] filename  file name
     */
    virtual void deleteProductImage( const QString& filename ) {deleteOne( DELETE_PRODUCT_IMAGE, filename );}

    /// Retrieve categories.
    virtual void getAllCategories() {readAll( GET_ALL_CATEGORIES );}

    /// Retrieve category image.
    virtual void getAllCategoryImages() {readAll( GET_ALL_CATEGORY_IMAGES );}

    /// Retrieve departments.
    virtual void getAllDepartments() {readAll( GET_ALL_DEPARTMENTS );}

    /// Retrieve operators.
    virtual void getAllOperators() {readAll( GET_ALL_OPERATORS );}

    /// Retrieve products.
    virtual void getAllProducts() {readAll( GET_ALL_PRODUCTS );}

    /// Retrieve product imagess.
    virtual void getAllProductImages() {readAll( GET_ALL_PRODUCT_IMAGES );}

    /// Retrieve category.
    /**
     * @param[in] dept  department
     * @param[in] num  category number
     */
    virtual void getCategory( const QString& dept, const QString& num ) {readOne( GET_CATEGORY, dept, num );}

    /// Retrieve category image.
    /**
     * @param[in] filename  file name
     */
    virtual void getCategoryImage( const QString& filename ) {readOne( GET_CATEGORY_IMAGE, filename );}

    /// Retrieve department.
    /**
     * @param[in] num  department number
     */
    virtual void getDepartment( const QString& num ) {readOne( GET_DEPARTMENT, num );}

    /// Retrieve operator.
    /**
     * @param[in] id  operator id
     */
    virtual void getOperator( const QString& id ) {readOne( GET_OPERATOR, id );}

    /// Retrieve product.
    /**
     * @param[in] dept  department
     * @param[in] num  product number
     */
    virtual void getProduct( const QString& dept, const QString& num ) {readOne( GET_PRODUCT, dept, num );}

    /// Retrieve product image.
    /**
     * @param[in] filename  file name
     */
    virtual void getProductImage( const QString& filename ) {readOne( GET_PRODUCT_IMAGE, filename );}

    /// Insert/Update category.
    /**
     * @param[in] obj  data
     */
    virtual void updateCategory( const QJsonObject& obj ) {writeOne( PUT_CATEGORY, obj );}

    /// Insert/Update category image.
    /**
     * @param[in] obj  data
     */
    virtual void updateCategoryImage( const QJsonObject& obj ) {writeOne( PUT_CATEGORY_IMAGE, obj, true );}

    /// Insert/Update department.
    /**
     * @param[in] obj  data
     */
    virtual void updateDepartment( const QJsonObject& obj ) {writeOne( PUT_DEPARTMENT, obj );}

    /// Insert/Update operator.
    /**
     * @param[in] obj  data
     */
    virtual void updateOperator( const QJsonObject& obj ) {writeOne( PUT_OPERATOR, obj );}

    /// Insert/Update product.
    /**
     * @param[in] obj  data
     */
    virtual void updateProduct( const QJsonObject& obj ) {writeOne( PUT_PRODUCT, obj );}

    /// Insert/Update product image.
    /**
     * @param[in] obj  data
     */
    virtual void updateProductImage( const QJsonObject& obj ) {writeOne( PUT_PRODUCT_IMAGE, obj, true );}

private slots:

    /// Slot to process document.
    void onProcessDocument( const QUuid& uuid, const QByteArray& request, const QString& requestType, int status, const QByteArray& response, const QString& responseType );

    /// Slot to process document.
    void onProcessDocumentJson( const QUuid& uuid, const QByteArray& request, const QString& requestType, int status, const QJsonDocument& response );

private:

    enum {REQUEST_TIMEOUT = 30 * 1000};             // 30s
    enum {REQUEST_RETRIES = 3};

    enum Endpoint
    {
        DELETE_CATEGORY,
        DELETE_ALL_CATEGORIES,                      // delete all categories
        GET_CATEGORY,
        GET_ALL_CATEGORIES,                         // get list of all categories
        PUT_CATEGORY,

        DELETE_CATEGORY_IMAGE,
        DELETE_ALL_CATEGORY_IMAGES,                 // delete all categories
        GET_CATEGORY_IMAGE,
        GET_ALL_CATEGORY_IMAGES,                    // get list of all categories
        PUT_CATEGORY_IMAGE,

        DELETE_DEPARTMENT,
        DELETE_ALL_DEPARTMENTS,                     // delete all departments
        GET_DEPARTMENT,
        GET_ALL_DEPARTMENTS,                        // get list of all departments
        PUT_DEPARTMENT,

        DELETE_OPERATOR,
        DELETE_ALL_OPERATORS,                       // delete all operators
        GET_OPERATOR,
        GET_ALL_OPERATORS,                          // get list of all operators
        PUT_OPERATOR,

        DELETE_PRODUCT,
        DELETE_ALL_PRODUCTS,                        // delete all products
        GET_PRODUCT,
        GET_ALL_PRODUCTS,                           // get list of all products
        PUT_PRODUCT,

        DELETE_PRODUCT_IMAGE,
        DELETE_ALL_PRODUCT_IMAGES,                  // delete all product images
        GET_PRODUCT_IMAGE,
        GET_ALL_PRODUCT_IMAGES,                     // get list of all product images
        PUT_PRODUCT_IMAGE,
    };

    using EndpointMap = QMap<Endpoint, QString>;

    EndpointMap endpointNames_;
    EndpointMap endpoints_;

    using PendingRequestsMap = QMap<QUuid, Endpoint>;

    PendingRequestsMap pendingRequests_;

    using ImageRequestsMap = QMap<QUuid, QString>;

    ImageRequestsMap imageRequests_;

    QString address_;

    /// Load endpoints.
    void loadEndpoints();

    /// Delete all records.
    void deleteAll( Endpoint e );

    /// Delete single record.
    void deleteOne( Endpoint e, const QString& key0, const QString& key1 = QString(), const QString& key2 = QString() );

    /// Read all records.
    void readAll( Endpoint e );

    /// Read single record.
    void readOne( Endpoint e, const QString& key0, const QString& key1 = QString(), const QString& key2 = QString() );

    /// Insert/Update single record.
    void writeOne( Endpoint e, const QJsonObject& obj, bool isRequest = false );

    /// Parse record.
    void parseRecord( Endpoint e, const QJsonDocument& doc );

    /// Parse image list.
    void parseImageList( Endpoint e, const QJsonDocument& doc );

    /// Retrieve endpoint url.
    QUrl endpointUrl( Endpoint e, const QString& key0 = QString(), const QString& key1 = QString(), const QString& key2 = QString() ) const;

    // not implemented
    OnlineOrderingAPI( const _Myt& ) = delete;

    // not implemented
    OnlineOrderingAPI( const _Myt&& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt&& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // OOAPI_H
