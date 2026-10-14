#ifndef QAEXCEPTION_H
#define QAEXCEPTION_H

#include <exception>
#include <QString>


namespace StarMicronics_QAsura
{


/**
    @ingroup QAsura
    @{
    @~english
        The QAException class is the base class of all exception classes.

    @~japanese
        QAsuraの利用でエラーが発生した場合、このQAExceptionクラスの例外が投げられます。
*/
class QAException : public std::exception
{
public:
    enum ERROR_CODE
    {
        None       = 0, /**< @~english No error.
                             @~japanese エラーなし。 */
        Claimed    = 1, /**< @~english The device is claimed by another control.
                             @~japanese デバイスは、他のアプリケーションにより排他処理されています。 */
        NoService  = 2, /**< @~english The ACP cannot communicate with the service, normally because of a setup or configuration error.
                             @~japanese サービスと通信することができません。 */
        Illegal    = 3, /**< @~english An invalid parameter value was used.
                             @~japanese サポートされていないパラメータが使われました。 */
        NoHardware = 4, /**< @~english The Physical Device is not connected to the ACP or is not powered on.
                             @~japanese */
        Failure    = 5, /**< @~english The ACP cannot perform the requested procedure, even though on-line.
                             @~japanese */
        Timeout    = 6, /**< @~english The Control timed out waiting for a response from the Device.
                             @~japanese */
        Busy       = 7  /**< @~english This request failed because the service is busy.
                             @~japanese */
    };


    QAException(int errorCode);


    QAException(int errorCode, QString errorDescription);


    QAException(int errorCode, QString errorDescription, std::exception origException);


    virtual ~QAException() throw() {};


/**
    @~english
        Gets the error code.

    @return
        Error code.

    @~japanese
        エラーが発生した要因を、QAException::ERROR_CODEで返します。

    @return
        QAException::ERROR_CODEを返します。
*/
    int getErrorCode();


/**
    @~english
        Gets QString containing the error description.

    @return
        Error description with QString

    @~japanese
        エラーが発生した要因の説明を、QStringで示します。

    @return
        QStringにて、エラー原因の説明を返します。
*/
    QString toString();


/**
    @~english
        Gets std::exception.

    @return
        original exception

    @~japanese
        std::exceptionによるエラーの場合、オリジナルの例外を取得できます。

    @return
        std::exceptionを返します。
*/
    std::exception getOrigException();
};


/**
    @}
*/


} // namespace StarMicronics_QAsura


#endif // QAEXCEPTION_H
