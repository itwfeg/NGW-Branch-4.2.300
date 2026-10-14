#ifndef REBOOTMESSAGE_H
#define REBOOTMESSAGE_H



#include "EvolMessage.h"



class RebootMessage : public EvolMessage
{
	public:
		/* --- Constructors and destructor --- */
		RebootMessage(bool powerDownOnly = false);
		RebootMessage(const char * msgData);
		~RebootMessage();


		/* --- Getters and setters --- */
		bool isPowerDownOnly() const { return powerDownOnly_; }


		/* --- Instance function members --- */
		const char * serialize() const;


	private:
		/* --- Instance data members --- */
		bool powerDownOnly_;


		/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif /* REBOOTMESSAGE_H */
