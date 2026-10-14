#ifndef COMINGUPONLINEMESSAGE_H
#define COMINGUPONLINEMESSAGE_H



#include "EvolMessage.h"



namespace synchronization
{


	class ComingUpOnLineMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		ComingUpOnLineMessage(bool reg = false);
		ComingUpOnLineMessage(const char* buffer);
		ComingUpOnLineMessage(const ComingUpOnLineMessage& sm);
		virtual ~ComingUpOnLineMessage();


		/* --- Getters and setters --- */
		bool isRegister() const { return register_; }
		void setRegister(bool reg) { register_ = reg; }


		/* --- Function members --- */
		/* Operators*/
		ComingUpOnLineMessage& operator = (const ComingUpOnLineMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Instance Data members --- */
		bool register_;


		/* --- Constructors and destructor --- */


		/* --- Function members --- */


		/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* COMINGUPONLINEMESSAGE_H */
