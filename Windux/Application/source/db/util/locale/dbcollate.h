#ifndef DBCOLLATE_H
#define DBCOLLATE_H



#include "dblocale.h"



#ifndef NO_NAMESPACES
namespace std
{
#endif



class collate : public locale::facet
{
	public:
		typedef char char_type;
		typedef basic_string<char> string_type;

		explicit collate(size_t r = 0) : locale::facet(r) {}

		int compare(const char* b, const char* e, const char* b2, const char* e2) const
				{ return do_compare(b, e, b2, e2); }
		string_type transform(const char* b, const char* e) const { return do_transform(b, e); }
// 		long hash(const char* b, const char* e) const { return do_hash(b, e); }
		void test() const { do_test(); }

		static locale::id id;                                             // facet identifier object

	protected:
		~collate() {}

		virtual int do_compare(const char* b, const char* e, const char* b2, const char* e2) const = 0;
		virtual string_type do_transform(const char* b, const char* e) const = 0;
// 		virtual long do_hash(const char* b, const char* e) const = 0;
		virtual void do_test() const = 0;
};



#ifndef NO_NAMESPACES
}
#endif



#endif // DBCOLLATE_H
