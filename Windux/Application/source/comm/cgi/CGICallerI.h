#ifndef CGICALLERI_H
#define CGICALLERI_H



namespace comm
{

	
	class CGICallerI
	{
		public:
			// Constructors and destructor
			virtual ~CGICallerI() {}


			// Accessor methods


			// Processing methods
			virtual void performCGICall(const std::string& url, const std::string& payload) = 0;
			virtual void reportResult(bool result) = 0;
	};


}



#endif // CGICALLERI_H
