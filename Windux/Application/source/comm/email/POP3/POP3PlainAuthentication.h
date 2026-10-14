#ifndef POP3PLAINAUTHENTICATION_H
#define POP3PLAINAUTHENTICATION_H



#include <string>



namespace email
{


	class POP3PlainAuthentication : public POP3AuthenticationMethod
	{
		public:
			// --- Types and subtypes ---


			// --- Constructors and destructor ---
			POP3PlainAuthentication();
			~POP3PlainAuthentication();


			// --- Getters and setters ---


			// --- Other function members ---


		protected:


		private:
			// --- Instance data members ---


			// --- Class data members ---


#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


}



#endif //POP3PLAINAUTHENTICATION_H

