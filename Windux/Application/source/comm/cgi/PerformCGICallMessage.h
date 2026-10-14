#ifndef PERFORMCGICALLMESSAGE_H
#define PERFORMCGICALLMESSAGE_H



#include <string>

#include "EvolMessage.h"



namespace comm
{


	class PerformCGICallMessage : public EvolMessage
	{
		public:
			/* --- Constructors and destructor --- */
			PerformCGICallMessage(const std::string& url, const std::string& payload = std::string());
			PerformCGICallMessage(const char* buffer);
			PerformCGICallMessage(const PerformCGICallMessage& m);
			~PerformCGICallMessage();


			/* --- Getters and setters --- */
			const std::string& getURL() const { return url_; }
			void setURL(const std::string& url) { url_ = url; }

			const std::string& getPayload() const { return payload_; }
			void setPayload(const std::string& payload) { payload_ = payload; }


			/* --- Function members --- */
			/* Operators*/
			PerformCGICallMessage& operator = (const PerformCGICallMessage& rhs);

			const char* serialize() const;

	
		private:
			/* --- Instance Data Members --- */
			std::string url_;
			std::string payload_;


			/* --- Class Data Members --- */
	#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
	#endif
	};


}



#endif // PERFORMCGICALLMESSAGE_H
