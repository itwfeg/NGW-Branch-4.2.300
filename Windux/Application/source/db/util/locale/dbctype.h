#ifndef DBCTYPE_H
#define DBCTYPE_H



#include "dblocale.h"



#ifndef NO_NAMESPACES
namespace std
{
#endif



class ctype_base
{
	public:
		enum mask
		{
			cntrl = 1,                // control characters
			print = 1 << 1,           // printing characters
			space = 1 << 2,           // whitespace characters
			punct = 1 << 3,           // punctuation characters
			alpha = 1 << 4,           // alphabetic characters
			digit = 1 << 5,           // decimal digits
			xdigit= 1 << 6,           // hexadecimal digits
			upper = 1 << 7,           // uppercase characters
			lower = 1 << 8,           // lowercase characters
			alnum = alpha | digit,    // alphanumeric characters
			graph = alnum | punct
		};
};


class ctype : public ctype_base, public locale::facet
{
	public:
		typedef char char_type;
		explicit ctype(size_t r = 0) : locale::facet(r) {}

		bool is(mask m, char c) const         // is "c" an "m"?
			{ return do_is(m, c); }

		// place classification for each ch in [b:e) into v:
//		const char* is(const char* b, const char* e, mask* v) const;
//			{ return do_is(b, e, v); }

//	 	const char* scan_is(mask m, const char* b, const char* e) const;        // find an m
//	 		{ return do_scan_is(m, b, e); }
//	 	const char* scan_not(mask m, const char* b, const char* e) const;       // find a non-m
//	 		{ return do_scan_not(m, b, e); }

		char toupper(char c) const
			{ return do_toupper(c); }
		const char* toupper(char* b, const char* e) const                       // convert [b:e)
			{ return do_toupper(b, e); }
		char tolower(char c) const
			{ return do_tolower(c); }
		const char* tolower(char* b, const char* e) const                       // convert [b:e)
			{ return do_tolower(b, e); }

//		char widen(char c) const
//			{ return do_widen(c); }
//		const char* widen(const char* b, const char* e, char* b2) const
//  		{ return do_widen(b, e, b2); }
//		char narrow(char c, char def) const
//			{ return do_narrow(c, def); }
//		const char* narrow(const char* b, const char* e, char def, char* b2) const
//			{ return do_narrow(b, e, def, b2); }

		void test() const { do_test(); }

		static locale::id id;                                             // facet identifier object

	protected:
		~ctype() {}

		virtual bool do_is(mask m, char c) const = 0;
//		virtual const char* do_is(const char* b, const char* e, mask* v) const = 0;
//		virtual const char* do_scan_is(mask m, const char* b, const char* e) const = 0;
// 		virtual const char* do_scan_not(mask m, const char* b, const char* e) const = 0;
		virtual char do_toupper(char c) const = 0;
		virtual const char* do_toupper(char* b, const char* e) const 
				{ for ( ; b != e; ++b ) *b = do_toupper(*b); return e; }
		virtual char do_tolower(char c) const = 0;
		virtual const char* do_tolower(char* b, const char* e) const
				{ for ( ; b != e; ++b ) *b = do_tolower(*b); return e; }
//	  	virtual char do_widen(char c) const = 0;
//		virtual const char* do_widen(const char* b, const char* e, char* b2) const = 0;
//	  	virtual char do_narrow(char c, char def) const = 0;
//		virtual const char* do_narrow(const char* b, const char* e, char def, char* b2) const = 0;
		virtual void do_test() const = 0;
};



#ifndef NO_NAMESPACES
}
#endif



#endif // DBCTYPE_H
