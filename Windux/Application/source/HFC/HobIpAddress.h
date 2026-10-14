/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HOBIPADDRESS_H
#define HOBIPADDRESS_H



#include "HfcObject.h"


#include <string>



class HobIpAddress : public HfcObject
{
	public:
		HobIpAddress();
		HobIpAddress(const std::string& address);
		explicit HobIpAddress(const char * buffer);
		HobIpAddress(unsigned long address);
		HobIpAddress(struct in_addr address);
		~HobIpAddress();

		void set(const std::string& address);

		short getNib1() const;
		void setNib1(HobString &nib);
		short getNib2() const;
		void setNib2(HobString &nib);
		short getNib3() const;
		void setNib3(HobString &nib);
		short getNib4() const;
		void setNib4(HobString &nib);

		const std::string& getDnsName() const;
		void setDnsName(const std::string& name);

		bool isUseIP() const;
		bool isUseDnsName() const;

		const char * serialize() const;

		void operator=(const HobIpAddress &addr);
		bool operator==(const HobIpAddress &addr) const;
		bool operator!=(const HobIpAddress &addr) const;

		bool isValid() const;
		void getAsString(std::string & result) const;

		static bool isValidIPAddress(const std::string& address);
		static bool validateNib(HobString &nib, int *value = NULL);

		void convertToNib(unsigned long address);


	private:
		short		nib1;
		short		nib2;
		short		nib3;
		short		nib4;
		std::string	dnsName;
		bool		useIP;

   		void init();

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif //HOBIPADDRESS_H
