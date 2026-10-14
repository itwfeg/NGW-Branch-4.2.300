#ifndef HTTPUNITTESTER_H
#define HTTPUNITTESTER_H



namespace comm
{


	class HTTPUnitTester
	{
		public:
			// Constructors and destructor
			HTTPUnitTester();
			~HTTPUnitTester() {}


			// Accessor methods


			// Processing methods
			bool testHTTPURIParsing();
			bool testHTTPURIQueries();
			bool testHTTPResponse();
			bool testHTTPClient();


		private:
	};


}



#endif // HTTPUNITTESTER_H
