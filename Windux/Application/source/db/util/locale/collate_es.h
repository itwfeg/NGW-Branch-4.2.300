#ifndef DBCOLLATE_ES_H
#define DBCOLLATE_ES_H



#include "dbcollate.h"



#ifndef NO_NAMESPACES
namespace std
{
#endif



class collate_es : public collate
{
	friend class transformIterator;

	public:
		collate_es() : collate(0) {}


	protected:
		int do_compare(const char* b, const char* e, const char* b2, const char* e2) const;
		string_type do_transform(const char* b, const char* e) const;
//		long do_hash(const char* b, const char* e) const;
		void do_test() const;


	private:
		static unsigned char _trans_[256];

		void test_do_compare(const std::string& str1, const std::string& str2) const;
		void test_do_transform(const std::string& str) const;
};



#ifndef NO_NAMESPACES
}
#endif



#endif // DBCOLLATE_ES_H
