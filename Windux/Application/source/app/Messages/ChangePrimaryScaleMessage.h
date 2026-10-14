#ifndef CHANGEPRIMARYSCALEMESSAGE_H
#define CHANGEPRIMARYSCALEMESSAGE_H



#include "EvolMessage.h"



class ChangePrimaryScaleMessage : public EvolMessage
{
	public:
		/* --- Constructors and destructor --- */
		ChangePrimaryScaleMessage(const std::string& primary);
		ChangePrimaryScaleMessage(const char * msgData);
		~ChangePrimaryScaleMessage();


		/* --- Getters and setters --- */
		const std::string& getPrimaryScale() const { return primaryScale_; }
		void setPrimaryScale(const std::string& primary) { primaryScale_ = primary; }


		/* --- Instance function members --- */
		const char * serialize() const;


	private:
		/* --- Instance data members --- */
		std::string primaryScale_;


		/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif /* CHANGEPRIMARYSCALEMESSAGE_H */
