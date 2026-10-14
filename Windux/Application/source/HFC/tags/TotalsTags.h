#ifndef TOTALSTAGS_H
#define TOTALSTAGS_H



#include "Tags.h"



namespace hfc
{


	class TotalsTags : public Tags
	{
		public:
			/* --- Types and subtypes --- */


			/* --- Constructors and destructor --- */
			TotalsTags() {}
			~TotalsTags() {}


			/* --- Class data members --- */
			static const std::string _totalWeight_;
			static const std::string _totalPrice_;
			static const std::string _totalPackages_;
			static const std::string _totalRuns_;
			static const std::string _totalItems_;
			static const std::string _totalFlags_;
			static const std::string _transactionTypeNumber_;
	};


}



#endif	//TOTALSTAGS_H 
