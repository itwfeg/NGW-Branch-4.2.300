/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef EvolMain_h
#define EvolMain_h

class EvolMain
{
    public:
        EvolMain();
        ~EvolMain();

	void startAll(int ssncnt, char *internalConfigDirectoryName, char *networkConfigDirectoryName, char *primaryInputFileName = "", char *remoteInputFileName = "");

    private:
        void initializeDatabase();
        void startConfig(char *intConfigDirectoryName, char *netConfigDirectoryName);
        void initShutdownHandler();

#ifdef USE_ACCOUNTANT
        static int _dataType_;
        static std::string _dataTypeName_;
#endif
};

#endif //EvolMain_h




