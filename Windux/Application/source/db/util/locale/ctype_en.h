#ifndef DBCTYPE_EN_H
#define DBCTYPE_EN_H



#include "dbctype.h"



#ifndef NO_NAMESPACES
namespace std
{
#endif



class ctype_en : public ctype
{
	public:
		ctype_en() : ctype(0) {}


	protected:
		bool do_is(mask m, char c) const { return _classification_[c]&m; }
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
		void do_test() const {}


	private:
		static int _classification_[256];
};



#ifndef NO_NAMESPACES
}
#endif



#endif // DBCTYPE_EN_H
