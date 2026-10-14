#ifndef DBCOLLATE_EN_H
#define DBCOLLATE_EN_H



#include "dbcollate.h"



#ifndef NO_NAMESPACES
namespace std
{
#endif



	class collate_en : public collate
	{
		public:
			collate_en() : collate(0) {}


		protected:
			int do_compare(const char* b, const char* e, const char* b2, const char* e2) const;
			string_type do_transform(const char* b, const char* e) const;
//			long do_hash(const char* b, const char* e) const;
			void do_test() const {}


		private:
	};



#ifndef NO_NAMESPACES
}
#endif



#endif // DBCOLLATE_EN_H
