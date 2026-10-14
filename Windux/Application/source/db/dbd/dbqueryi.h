#ifndef DBQUERYI_H
#define DBQUERYI_H


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	template <class DBEntityI> class DBQueryI
	{
		public:

			// Types, subtypes, and friends
			typedef size_t size_type; /* (SJJC port 6/19/06) */


			// Constructors and destructor
			virtual ~DBQueryI() {}


			// Processing methods
			virtual DBEntityI* first() = 0;
			//virtual DBEntityI* next() = 0;
			virtual DBEntityI* next(const DBEntityI* current) = 0;
			//virtual DBEntityI* previous() = 0;
			virtual DBEntityI* previous(const DBEntityI* current) = 0;
			virtual DBEntityI* last() = 0;
			virtual DBEntityI* find(const DBEntityI* current) = 0;

			virtual size_type size() = 0;
			virtual bool empty() = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBQUERYI_H
