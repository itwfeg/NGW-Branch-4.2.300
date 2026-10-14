#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H



#include <string>
#include <map>
#include <list>


#include "HTTPRequest.h"



namespace comm
{


	class HTTPResponse
	{
		public:
			/* --- Types and subtypes --- */
			typedef std::map<std::string, std::string> headerContainer_type;
			typedef std::list<std::string> headerNameContainer_type;


			/* --- Constructors and destructor --- */
			HTTPResponse(HTTPRequest::RequestMethod method = HTTPRequest::INVALID_METHOD);
			~HTTPResponse();


			/* --- Getters and setters --- */
			HTTPRequest::RequestMethod getHTTPRequestMethod() const { return requestMethod_; }
			void setHTTPRequestMethod(HTTPRequest::RequestMethod method) { requestMethod_ = method; }

			int getMajorVersion() const { return majorVersion_; }

			int getMinorVersion() const { return minorVersion_; }

			int getStatusCode() const { return statusCode_; }

			const std::string& getReason() const { return reason_; }


			/* --- Instance Function Members --- */
			bool isResponseEntity() const;
			bool processStatusLine(const std::string& line);
			bool processHeaderLine(const std::string& line);
			int getContentLength() const;
			void showDebug();
			std::string dumpContents();


		private:
			/* --- Instance Data Members --- */
			HTTPRequest::RequestMethod requestMethod_;
			int majorVersion_, minorVersion_;
			int statusCode_;
			std::string reason_;
			headerContainer_type headers_;
			headerNameContainer_type headerNames_;


			/* --- Instance Function Members --- */
			void init();
			std::string::size_type findLinearWhiteSpace(const std::string& line, std::string::size_type off, int* count) const;
			bool validateFieldName(const std::string& fieldName);
			bool validateTokenChars(const std::string& token);
			bool validateDigitChars(const std::string& version);


			/* --- Class Data Members --- */
			static const std::string _HTTPVersion_;
			static const std::string _validSeparators_;

	#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
	#endif
	};


}



#endif // HTTPRESPONSE_H
