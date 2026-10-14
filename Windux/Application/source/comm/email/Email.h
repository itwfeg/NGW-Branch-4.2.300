#ifndef EMAIL_H
#define EMAIL_H

#include <list>

#include "EmailComponent.h"
#include "time/datetime.h"



class Email
{
	public:
		// Types and subtypes
		typedef std::list<std::string> addressList;
		typedef std::list<std::string>::iterator addressListIterator;


		// Constructors and destructor
		Email(const std::string& from, const std::string& to, const std::string& cc,
      	const std::string& bc, const std::string& subject, const std::string& content);
		~Email();


		// Accessor methods
		const util_time::DateTime& getDateTimeSubmitted() const { return dateTime_; }

		const std::string& getFrom() const { return from_; }
		void setFrom(const std::string& from) { from_ = from; }

		addressList* getToList() { return &toList_; }
		void setToList(const addressList& toList) { toList_ = toList; 
			createCommaSepList(toList_, to_); }
		void setTo(const std::string& to) {to_ = to; parseDestList(toList_, to);}
		const std::string& getTo() const { return to_; }

		addressList* getCcList() { return &ccList_; }
		void setCcList(const addressList& ccList) { ccList_ = ccList; 
			createCommaSepList(ccList_, cc_); }
		void setCc(const std::string& cc) {cc_ = cc; parseDestList(ccList_, cc);}
		const std::string& getCc() const { return cc_; }

		addressList* getBccList() { return &bccList_; }
		void setBccList(const addressList& bccList) { bccList_ = bccList; 
			createCommaSepList(bccList_, bcc_); }
		void setBcc(const std::string& bcc) {bcc_ = bcc; parseDestList(bccList_, bcc);}
		const std::string& getBcc() const { return bcc_; }

		const std::string& getSubject() const { return subject_; }
		void setSubject(const std::string& subject) { subject_ = subject; }

		const std::string& getContent() const { return content_; }
		void setData(const std::string& content) { content_ = content; }

		unsigned short getNumRetries() {return numRetries_;}
		void setNumRetries(unsigned short retries) { numRetries_ = retries;}
		void decrementRetries() {numRetries_--;}

		// Process methods

		void buildSMTPEmail(std::string& emailText);


	private:

		void parseDestList(addressList& list, const std::string& commaSepList);
		void createCommaSepList(addressList& list, std::string& commaSepList);
		void removeSpaces(std::string& emailAddr);

		unsigned short numRetries_;
		util_time::DateTime dateTime_;
		std::string from_;
		std::string to_;
		addressList toList_;
		std::string cc_;
		addressList ccList_;
		std::string bcc_;
		addressList bccList_;
		std::string subject_;
		std::string content_;

		static const std::string _AddressSeperator_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif // EMAIL_H
