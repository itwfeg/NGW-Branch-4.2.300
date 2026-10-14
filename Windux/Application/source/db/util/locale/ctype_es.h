#ifndef DBCTYPE_ES_H
#define DBCTYPE_ES_H



#include "dbctype.h"



#ifndef NO_NAMESPACES
namespace std
{
#endif



class ctype_es : public ctype
{
	public:
		ctype_es() : ctype(0) {}
		ctype_es(int r) : ctype(r) {}


	protected:
		bool do_is(mask m, char c) const { return _class_[(unsigned char)c]&m; }
//		const char* do_is(const char* b, const char* e, mask* v) const;
//		const char* do_scan_is(mask m, const char* b, const char* e) const;
//		const char* do_scan_not(mask m, const char* b, const char* e) const;
		char do_toupper(char c) const;
		const char* do_toupper(char* b, const char* e) const;
		char do_tolower(char c) const;
		const char* do_tolower(char* b, const char* e) const;
//		char do_widen(char c) const;
//		const char* do_widen(const char* b, const char* e, char* b2) const;
//		char do_narrow(char c, char def) const;
//		const char* do_narrow(const char* b, const char* e, char def, char* b2) const;
		void do_test() const;


	private:
		static int _class_[256];

		void test_do_is(const std::string& str, mask m) const;
		void test_do_toupper(const std::string& str) const;
		void test_do_tolower(const std::string& str) const;
		const char* maskString(mask m) const;
};



#ifndef NO_NAMESPACES
}
#endif



#endif // DBCTYPE_ES_H
