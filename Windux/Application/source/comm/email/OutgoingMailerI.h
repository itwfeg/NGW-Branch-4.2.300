#ifndef OUTGOINGMAILERI_H
#define OUTGOINGMAILERI_H



class Email;



class OutgoingMailerI
{
	public:
		// Constructors and destructor
		virtual ~OutgoingMailerI() {}


		// Accessor methods


		// Processing methods
		virtual void send(Email* email) = 0;
};



#endif // OUTGOINGMAILERI_H
