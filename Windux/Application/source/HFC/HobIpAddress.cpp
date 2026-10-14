/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HobIpAddress.h"
#include "HobTokenizer.h"
#include "HobString.h"
#include "Configuration.h"

#include "Integer.h"
#include "Bool.h"



#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HobIpAddress::_dataType_ = 0;
std::string HobIpAddress::_dataTypeName_("HobIpAddress");
#endif



const std::string ipAddressSet = "0123456789";


HobIpAddress::HobIpAddress()
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

    init();
}


HobIpAddress::HobIpAddress(const std::string& address)
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

    set(address);
}


HobIpAddress::HobIpAddress(const char * buffer)
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

    memcpy(&nib1, buffer, sizeof(nib1));
    buffer += sizeof(nib1);

    memcpy(&nib2, buffer, sizeof(nib2));
    buffer += sizeof(nib2);

    memcpy(&nib3, buffer, sizeof(nib3));
    buffer += sizeof(nib3);

    memcpy(&nib4, buffer, sizeof(nib4));
    buffer += sizeof(nib4);

    int dnsSize;
    memcpy(&dnsSize, buffer, sizeof(dnsSize));
    buffer += sizeof(dnsSize);

    if ( dnsSize )
    {
        dnsName.assign(buffer, dnsSize);
        buffer += dnsSize;
    }
    else
        dnsName = "";

    memcpy(&useIP, buffer, sizeof(useIP));
    buffer += sizeof(useIP);
}

/*	Assumes address, when converted to Hex, represents the IP address
 *	reading normally from left to right (i.e. 10.3.72.151 = 0x0A034897 = 167987351)
 */
HobIpAddress::HobIpAddress(unsigned long address)
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

    init();

	convertToNib(address);
}

/*	The struct address, when converted to Hex, represents the IP address
 *	reading from right to left (i.e. 10.3.72.151 = 0x9748030A = 2538078986)
 *	We need to byte swap before processing it.
 */
HobIpAddress::HobIpAddress(struct in_addr address)
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif
	
	init();

    unsigned long ul_address = ntohl(address.s_addr);

	convertToNib(ul_address);
}


HobIpAddress::~HobIpAddress()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(_dataType_);
#endif
}


void HobIpAddress::init()
{
    nib1 = 0;
    nib2 = 0;
    nib3 = 0;
    nib4 = 0;
    dnsName = "";
    useIP = true;
}


void HobIpAddress::set(const std::string& address)
{
    init();

    if ( address.empty() )
        return;

    if ( isValidIPAddress(address) )
    {
        HobTokenizer tok(&address, ".");
        HobString token;
        if (tok.next(&token) != -1)
        {
            setNib1(token);
            if (tok.next(&token) != -1)
            {
                setNib2(token);
                if (tok.next(&token) != -1)
                {
                    setNib3(token);
                    if (tok.next(&token) != -1)
                        setNib4(token);
                }
            }
        }
    }
    else
        setDnsName(address);


    if ( Debug::isTracing("HobIpAddress") )
    {
        HobString myDebug;

        myDebug.append("set(").append(address).append("):   ");

        if (useIP)
        {
            myDebug.append("IP Address = ");
            myDebug.append(Integer::toString(nib1)).append(".");
            myDebug.append(Integer::toString(nib2)).append(".");
            myDebug.append(Integer::toString(nib3)).append(".");
            myDebug.append(Integer::toString(nib4));
        }
        else
            myDebug.append("DNS Name = (").append(dnsName).append(")");

        Debug::trace("HobIpAddress", myDebug);
    }
}


short HobIpAddress::getNib1() const
{
    return nib1;
}
void HobIpAddress::setNib1(HobString &nib)
{
    int value;
    bool isValid = validateNib(nib, &value);
    if (isValid)
        nib1 = (short) value;
}

short HobIpAddress::getNib2() const
{
    return nib2;
}
void HobIpAddress::setNib2(HobString &nib)
{
    int value;
    bool isValid = validateNib(nib, &value);
    if (isValid)
        nib2 = (short) value;
}

short HobIpAddress::getNib3() const
{
    return nib3;
}
void HobIpAddress::setNib3(HobString &nib)
{
    int value;
    bool isValid = validateNib(nib, &value);
    if (isValid)
        nib3 = (short) value;
}

short HobIpAddress::getNib4() const
{
    return nib4;
}
void HobIpAddress::setNib4(HobString &nib)
{
    int value;
    bool isValid = validateNib(nib, &value);
    if (isValid)
        nib4 = (short) value;
}

const std::string& HobIpAddress::getDnsName() const
{
    return dnsName;
}


void HobIpAddress::setDnsName(const std::string& name)
{
    if ( name.empty() )
        return;

    dnsName = name;
    useIP = false;
}


bool HobIpAddress::isUseIP() const
{
    return useIP;
}


bool HobIpAddress::isUseDnsName() const
{
    return (!useIP);
}


const char *HobIpAddress::serialize() const
{
    serializedData = "";

    serializedData.append((const char*)&nib1, sizeof(nib1));
    serializedData.append((const char*)&nib2, sizeof(nib2));
    serializedData.append((const char*)&nib3, sizeof(nib3));
    serializedData.append((const char*)&nib4, sizeof(nib4));

    int dnsSize = (int)dnsName.size();

    serializedData.append((const char*)&dnsSize, sizeof(dnsSize));
    serializedData.append(dnsName, 0, dnsSize);

    serializedData.append((const char*)&useIP, sizeof(useIP));

    return serializedData.data();
}


void HobIpAddress::operator=(const HobIpAddress &addr)
{
    nib1 = addr.getNib1();
    nib2 = addr.getNib2();
    nib3 = addr.getNib3();
    nib4 = addr.getNib4();
    dnsName = addr.getDnsName();
    useIP = addr.isUseIP();
}


bool HobIpAddress::operator==(const HobIpAddress& addr) const
{
    return (getNib1() == addr.getNib1() &&
            getNib2() == addr.getNib2() &&
            getNib3() == addr.getNib3() &&
            getNib4() == addr.getNib4() &&
            isUseIP() == addr.isUseIP() &&
            getDnsName() == addr.getDnsName()) ? true : false;
}


bool HobIpAddress::operator!=(const HobIpAddress& addr) const
{
    return !(*this == addr);
}


bool HobIpAddress::isValid() const
{
    if (nib1 > 0 || nib2 > 0 || nib3 > 0 || nib4 > 0)
        return true;
    else
        return false;
}


void HobIpAddress::getAsString(std::string& result) const
{
    if (isUseDnsName())
        result = dnsName;
    else
        result = HobString(Integer::toString(nib1)) + "." + HobString(Integer::toString(nib2)) + "." + HobString(Integer::toString(nib3)) + "." + HobString(Integer::toString(nib4));

    if (Debug::isTracing("HobIpAddress"))
    {
        HobString myDebug("getAsString():  result = ");
        myDebug.append(result);
        Debug::trace("HobIpAddress", myDebug);
    }
}


bool HobIpAddress::isValidIPAddress(const std::string& address)
{
    HobTokenizer tok(&address, ".");
    bool result = true;
    int count = 0;
    HobString token;

    while ((tok.next(&token) != -1) && (result == true) && (count < 5))
    {
        result = validateNib(token);
        ++count;
    }
    if (result && (count != 4))
        result = false;

    if (Debug::isTracing("HobIpAddress.isValidIp"))
    {
        HobString myDebug("isValidIPAddress(");
        myDebug.append(address);
        myDebug.append(") : ");
        myDebug.append(Bool::toString(result));
        Debug::trace("HobIpAddress", myDebug);
    }

    return result;
}


bool HobIpAddress::validateNib(HobString &nib, int *value)
{
    HobString myDebug;
    bool result = true;
    bool traceValidateNib = Debug::isTracing("HobIpAddress.validateNib");
    int length = (int)nib.length();
    int i;

    if (traceValidateNib)
    {
        myDebug = "validateNib(";
        myDebug.append(nib).append("):  length = ").append(Integer::toString(length));
        Debug::trace("HobIpAddress.validateNib", myDebug);
    }
    if (length == 0 || length > 3)
        result = false;
    else
    {
        for(i = 0; i < length; i++)
        {
            if (Debug::isTracing("HobIpAddress.nib"))
            {
                myDebug = "nib.at(";
                myDebug.append(Integer::toString(i)).append(") = ");
                std::string myTemp(1, nib.at(i));
                myDebug.append(myTemp);
                Debug::trace("HobIpAddress.nib", myDebug);

                myDebug = "ipAddressSet.find(nib.at(";
                myDebug.append(Integer::toString(i)).append(") = ").append(Integer::toString((int)ipAddressSet.find(nib.at(i))));
                Debug::trace("HobIpAddress.nib", myDebug);
            }
            if (ipAddressSet.find(nib.at(i)) == std::string::npos)
            {
                result = false;
                Debug::trace("HobIpAddress.validateNib", "Non-digit in the octet!");
                break;
            }
        }
        if (traceValidateNib)
        {
            myDebug = "result so far = ";
            myDebug.append(Bool::toString(result));
            Debug::trace("HobIpAddress.validateNib", myDebug);
        }
        if (result)
        {
            int tempNib = Integer::valueOf(nib);
            if (traceValidateNib)
            {
                myDebug = "tempNib = ";
                myDebug.append(Integer::toString(tempNib));
                Debug::trace("HobIpAddress.validateNib", myDebug);
            }
            if (tempNib < 0 || tempNib > 255)
            {
                Debug::trace("HobIpAddress.validateNib", "Not a valid octet!");
                result = false;
            }
            else
            {
                Debug::trace("HobIpAddress.validateNib", "A valid octet!");
                if (value != NULL)
                    *value = tempNib;
            }
        }
    }
    if (traceValidateNib)
    {
        myDebug = "EXIT validateNib(";
        if (value != NULL)
            myDebug.append(nib).append(") :  value = ").append(Integer::toString(*value)).append("  result = ").append(Bool::toString(result));
        else
            myDebug.append(nib).append(") :  ").append("  result = ").append(Bool::toString(result));
        Debug::trace("HobIpAddress.validateNib", myDebug);
    }
    return result;
}

void HobIpAddress::convertToNib(unsigned long address)
{
	static short nibble = 256;

	nib4 = (short) (address % nibble);
	address = address / nibble;

	nib3 = (short) (address % nibble);
	address = address / nibble;

	nib2 = (short) (address % nibble);
	address = address / nibble;

	nib1 = (short) address;
}
