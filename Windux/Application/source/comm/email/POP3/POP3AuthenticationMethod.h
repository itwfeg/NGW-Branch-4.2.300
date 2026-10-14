#ifndef POP3AUTHENTICATIONMETHOD_H
#define POP3AUTHENTICATIONMETHOD_H



#include <string>


#include "CSmartPtr.h"


namespace email
{


	class POP3AuthenticationMethod
	{
		public:
			// --- Types and subtypes ---
			typedef enum MethodTypes
			{
				InvalidMethod,

				PlainPassword,              // USER and PASS commands
				PasswordDigest,             // APOP command
				PLAIN_SASL,                 // 
				GSSAPI,                     //
				DIGEST_MD5,                 //
				KERBEROS,                   //
				SKEY,                       //
				ANONYMOUS,                  //

				LastMethod
			} MethodType;


			// --- Constructors and destructor ---
			POP3AuthenticationMethod(MethodType type = InvalidMethod);
			virtual ~POP3AuthenticationMethod();


			// --- Getters and setters ---
			MethodType getType() const { return type_; }
			void setType(MethodType type) { type_ = type; }


			// --- Other function members ---
			const std::string& getMethodName() const;
			const std::string& serialize() const;

			// Factory method
			static Util::CSmartPtr<POP3AuthenticationMethod> createMethod(MethodType type = PlainPassword);


		protected:


		private:
			// --- Instance data members ---
			MethodType type_;
			mutable std::string serialization_;


			// --- Class data members ---
			static const std::string _InvalidMethod_;
			static const std::string _PlainPassword_;
			static const std::string _PasswordDigest_;


#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


}



#endif //POP3AUTHENTICATIONMETHOD_H

