#ifndef CGICALLPERFORMEDMESSAGE_H
#define CGICALLPERFORMEDMESSAGE_H



#include <string>

#include "EvolMessage.h"



namespace comm
{


	class CGICallPerformedMessage : public EvolMessage
	{
		public:
			/* --- Constructors and destructor --- */
			CGICallPerformedMessage(bool result);
			CGICallPerformedMessage(const char* buffer);
			CGICallPerformedMessage(const CGICallPerformedMessage& m);
			~CGICallPerformedMessage();


			/* --- Getters and setters --- */
			bool getResult() const { return result_; }
			void setResult(bool result) { result_ = result; }


			/* --- Function members --- */
			/* Operators*/
			CGICallPerformedMessage& operator = (const CGICallPerformedMessage& rhs);

			const char* serialize() const;

	
		private:
			/* --- Instance Data Members --- */
			bool result_;


			/* --- Class Data Members --- */
	#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
	#endif
	};


}



#endif // CGICALLPERFORMEDMESSAGE_H
