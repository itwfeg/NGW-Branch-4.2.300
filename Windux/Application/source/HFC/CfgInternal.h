/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CFGINTERNAL_H
#define CFGINTERNAL_H

#include "evolution.h"
#include "CfgBase.h"
#include "HfcObject.h"
#include "Power.h"
#include "ScaleModel.h"

#include <math.h>
#include <map>

#include "CCriticalSection.h"

class HobString;
namespace util_env
{
class Properties;
}

class CfgInternal : public HfcObject
{
	public:

		CfgInternal();
		CfgInternal(const char *data, int len, int formatType);
		~CfgInternal();

		static const char *getInternalConfigFileName();
		static std::string getInternalConfigFileNameStr();
		static void setInternalConfigDirectory(const char *directory);

		static const char *getNetworkConfigFileName();
		static std::string getNetworkConfigFileNameStr();
		static void setNetworkConfigDirectory(const char *directory);

		std::string getName(int fieldCode) const;

		// Open the internal config properties file(s).
		void load();
		void checkReloadOfCfgI();

		void show();

		void test();

		virtual const char *serialize(int format) const;

		
		std::string toString(const std::string& name) const;
		int toInt(const std::string& name) const;
		bool toBool(const std::string& name) const;

		std::string toString(int fieldCode) const;
		int toInt(int fieldCode) const;
		bool toBool(int fieldCode) const;

		void checkTracingCfgI();

		static const std::string SCALEMODEL;
		static const std::string SCALENAME;
		static const std::string SCALESERVICENAME;
		static const std::string WEBSERVICENAME;
		static const std::string APPINSTALLEREXE;

		static const std::string SYSTEMGRAPHICSLOCATION;
		static const std::string USERGRAPHICSLOCATION;
		static const std::string IMAGESLOCATION;
		static const std::string SYSTEMUSERROOTDIR;
		static const std::string GRAPHICSEXTENSIONS;
		static const std::string GRAPHICFILESPECIFIERS;
		static const std::string NOGRAPHICSELECTEDNAME;
		static const std::string SYSTEMGRAPHICSTAR;
		static const std::string USERGRAPHICSTAR;
		static const std::string PRODUCTGRAPHICSTAR;
		static const std::string PRODUCTGRAPHICSDIR;
		static const std::string SYSTEMPRODUCTGRAPHICSDIR;
		static const std::string PRODUCTGRAPHICSDEFAULTEXTENSION;
		static const std::string CATEGORYGRAPHICSTAR;
		static const std::string CATEGORYGRAPHICSDIR;
		static const std::string SYSTEMCATEGORYGRAPHICSDIR;
		static const std::string CATEGORYGRAPHICSDEFAULTEXTENSION;
		static const std::string SPECIALSGRAPHICSTAR;
		static const std::string SPECIALSGRAPHICSDIR;
		static const std::string SPECIALSGRAPHICSDEFAULTEXTENSION;
		static const std::string STREAMINGGRAPHICSTAR;
		static const std::string STREAMINGGRAPHICSDIR;
		static const std::string STREAMINGGRAPHICSDEFAULTEXTENSION;
		static const std::string SOFTKEYGRAPHICSTAR;
		static const std::string SOFTKEYGRAPHICSDIR;
		static const std::string SOFTKEYGRAPHICSDEFAULTEXTENSION;
		static const std::string RESERVEDSOFTKEYGRAPHICSDIR;
		static const std::string SELFSERVEGRAPHICSDIR;
		static const std::string SYSTEMSELFSERVEGRAPHICSDIR;
		static const std::string SELFSERVEGRAPHICSDEFAULTEXTENSION;
		static const std::string PLANOGRAMGRAPHICSDIR;
		static const std::string PLANOGRAMGRAPHICSDEFAULTEXTENSION;
		static const std::string VIDEOSDIR;
		static const std::string VIDEOSDEFAULTEXTENSION;

		static const std::string LANGFILENAME;

		static const std::string WEBPAGESROOTDIR;
		static const std::string GENERALWEBPAGEDIRNAME;
		static const std::string LINUXWEBPAGEDIRNAME;
		static const std::string LANGUAGEFILESDIR;
		static const std::string PYTHONSCRIPTSDIR;

		static const std::string KEYMOUNTDIR;

		static const std::string SERVICELOGINID;
		static const std::string SERVICEKEYREMOVETIMEOUT;
		static const std::string SERVICEKEYFILE;
		static const std::string SUPERKEYFILE;
		static const std::string SELFSERVEKEYFILE;

		static const std::string DIRECTORYSEPARATOR;
		static const std::string TEMPORARYDBDIRECTORY;
		static const std::string DBLOCATION;
		static const std::string EMPTYDBLOCATION;
		static const std::string DBDEFAULTSFILE;
		static const std::string DBBACKUPENABLED;
		static const std::string DBBACKUPLOCATION;
		static const std::string DBMIRRORLOCATION;
		static const std::string DBCHECKBEFORESHUTDOWN;
		static const std::string DBCHECKINDICES;
		static const std::string DBCHECKINTEGRITY;
		static const std::string DBCHECKINTEGRITYFREQUENCY;
		static const std::string DBREBUILDINDICESENABLED;
		static const std::string VERSIONINDEPENDENTBACKUPFILE;
		static const std::string AUTOLOADVERSIONINDEPENDENTDATA;
		static const std::string BACKUPVERSIONINDEPENDENTDATANOW;
		static const std::string MAKECOPYOFVERSIONINDEPENDENTDATA;

		static const std::string AUTOBACKUPDATABASELOCATION;
		static const std::string AUTOBACKUPPLUDATAFILE;
		static const std::string AUTOBACKUPFLASHKEYDATAFILE;
		static const std::string AUTOBACKUPLABELDATAFILE;
		static const std::string AUTOBACKUPOPERATORDATAFILE;
		static const std::string AUTOBACKUPSTORENAMENUMBERFILE;
		static const std::string AUTOBACKUPCONFIGFILE;
		static const std::string AUTOBACKUPSYNCHCONFIGFILE;

		static const std::string PRIMARYCONSOLESESSIONPRIORITY;
		static const std::string REMOTECONSOLESESSIONPRIORITY;
		static const std::string MAXREMOTECONSOLESESSIONS;
		static const std::string MAXCONSECUTIVERTCMESSAGES;
		static const std::string EVENTMANAGERENABLED;
		static const std::string SESSIONMANAGERENABLED;
		static const std::string SESSIONMANAGERSTARTUPDELAY;
		static const std::string SESSIONMANAGERPERIODICTIMER;
		static const std::string REALTIMECONTROLLERENABLED;
		static const std::string TIMERMANAGERENABLED;
		static const std::string VERSIONMANAGERENABLED;
		static const std::string USBENABLEDATBOOTUP;
		static const std::string INTERRUPTXFERTIMEOUT;
		static const std::string BULKXFERTIMEOUT;
		static const std::string EXPIREDRECORDSCHECKFREQUENCY;

		static const std::string FAKEWEIGHERMANAGERENABLED;
		static const std::string FAKEWEIGHERFREQUENCY;
		static const std::string FAKEPRINTERMANAGERENABLED;
		static const std::string FAKECLAMANAGERENABLED;
		static const std::string FAKEWRAPPERMANAGERENABLED;
		static const std::string TESTBROWSERENABLED;
		static const std::string TESTBROWSERMSGDELAY;
		static const std::string TESTMESSAGESENABLED;

		static const std::string RFIDENABLED;
		static const std::string RFIDDEBOUNCETIME;
		static const std::string RFIDCOMPORT;

		static const std::string DIAGNOSTICSMANAGERENABLED;
		static const std::string DIAGNOSTICSDBNAME;
		static const std::string DIAGNOSTICSDBBACKUPNAME;
		static const std::string DIAGNOSTICSDBOLDNAME;

		static const std::string BARCODESCANNERENABLED;

		static const std::string PRINTERPROBETIME;
		static const std::string WEIGHERPROBETIME;
		static const std::string CUSTDISPPROBETIME;
		static const std::string LABELAPPLIERPROBETIME;
		static const std::string WRAPPERPROBETIME;

		static const std::string USBBULKDRIVEONLYMODE;
		static const std::string USBBULKDRIVENAME;
		static const std::string COMPACTFLASHDRIVENAME;
		static const std::string PERMANENTRAMDRIVENAME;
		static const std::string TEMPORARYRAMDRIVENAME;
		static const std::string EVOLUTIONDIRECTORY;
		static const std::string BOOTPARAMETERSFILENAME ;
		static const std::string DEFAULTNETWORKCONFIGDIR;
		static const std::string TMPNETWORKCONFIGDIR;
		static const std::string FIRMWAREDIRECTORY;
		static const std::string FIRMWAREGENERICFILENAME;
		static const std::string FIRMWAREFILENAMES;

		static const std::string VERSIONMANAGERTEMPDIR;
		static const std::string VERSIONMANAGERFORCEUPGRADE;
		static const std::string DEFAULTUPGRADEFILENAME;
		static const std::string INSTALLOGFILE;
		static const std::string VERSIONINFOFILENAME;
		static const std::string DEFAULTUPGRADEDIRECTORY;
		static const std::string UPGRADEZIPFILE;
		static const std::string UPGRADEINIFILE;

		static const std::string MAXGRAPHICSPERPRODUCT;
		static const std::string PRODUCTNAMEMAXLENGTH;
		static const std::string UPDATEPRINTERCONFIG;
		static const std::string ONLYPRILANGFORPRODSTATS;
		static const std::string PERCENTAGEPRECISION;
		static const std::string MAXLIFEDAYS;
		static const std::string MAXLIFEHOURS;
		static const std::string MAXLIFEMINUTES;

		static const std::string MAXBONUSPOINTS;
		static const std::string QUARTERPOUNDFOR;
		static const std::string HALFPOUNDFOR;
		static const std::string HUNDREDGRAMSFOR;
		static const std::string CONTINUOUSSTATISTICSDELAY;
		static const std::string LOSTPERIPHERALSCHECKTIME;

		static const std::string KEYBOARDNETWEIGHTPRECISION;

		static const std::string SUBTRACTDESCENT;
		static const std::string MAXLABELNUMBER;
		static const std::string LABELFEEDHEELFIRST;
		static const std::string TESTLABELTYPE;
		static const std::string IGNORELABELFIT;
		static const std::string LABELHORIZONTALOFFSET;
		static const std::string DEFAULTNUTRILABELTEMPLATE;
		static const std::string SHELFLIFEEXPLABELFAMILY;
		static const std::string EXPIRATIONLABELEXPIREDLIMIT;
		static const std::string RUNTOTALSLABELFAMILY;

		static const std::string FBWFACTIVATED;
		static const std::string USINGRESTRICTEDLOGINS;
		static const std::string APPINSTALLERISSERVICE;
		static const std::string STARTUPDELAY;
		static const std::string SIMULATEPRINTING;
		static const std::string MONITORMEMORYINTERVAL;

		static const std::string DUALRANGELIMITAVOIR;
		static const std::string DUALRANGELIMITMETRIC;
		static const std::string REZEROWEIGHTLIMITAVOIR;
		static const std::string REZEROWEIGHTLIMITMETRIC;

		static const std::string CONNECTIONIDENTIFICATIONTIMER;
		static const std::string CONNECTIONIDENTIFICATIONDELAY;
		static const std::string COMMSESSIONREADMESSAGETIMER;
		static const std::string COMMSESSIONREADMESSAGEDELAY;
		static const std::string WAITBEFORECONNECTINGTIMER;
		static const std::string WAITBEFORECONNECTINGDELAY;

		static const std::string PINGCONNECTIVITYFREQUENCY;
		static const std::string PINGCONNECTIVITYATTEMPTS;
		static const std::string OFFLINEPINGCONNECTIVITYFREQUENCY;
		static const std::string OFFLINEPINGCONNECTIVITYATTEMPTS;

		static const std::string SYNCHRONIZATIONENABLED;
		static const std::string SYNCHRONIZEALLSECONDARIESTIMER;
		static const std::string NOTIFYLOCALBATCHESTIMER;
		static const std::string RECORDSPERSYNCITERATION;
		static const std::string DELAYPERSYNCITERATION;
		static const std::string MAXIMUMNUMBERRETRIES;
		static const std::string RETRYCOMMANDTIMER;
		static const std::string HOURSBEFOREDEREGISTERING;
		static const std::string DELAYPURGINGBATCHES;
		static const std::string HOURSBEFOREREMOVINGDELETEDBATCHES;
		static const std::string RECORDSPERAPPLYITERATION;
		static const std::string DELAYPERAPPLYITERATION;
		static const std::string DELAYPERDELETEITERATION;
		static const std::string SYNCHRONIZATIONSTATUSMESSAGESENABLED;

		static const std::string USEPRODUCTIONPLANNINGDIALOG;
		static const std::string HTTPREQUESTMETHOD;
		static const std::string HTTPREQUESTCONTENTTYPE;
		static const std::string HTTPWAITINGDELAY;
		static const std::string HTTPREADRESPONSETIMER;
		static const std::string HTTPREADSOMERESPONSEDELAY;
		static const std::string HTTPREADNORESPONSEDELAY;
		static const std::string HTTPMAXRESPONSELINELENGTH;
		static const std::string HTTPREPORTERRORS;
		static const std::string HTTPQUERYSTORE;
		static const std::string HTTPQUERYDEPT;
		static const std::string HTTPQUERYMODEL;
		static const std::string HTTPQUERYPLU;
		static const std::string HTTPQUERYWEIGHT;
		static const std::string HTTPQUERYQUANTITY;
		static const std::string HTTPQUERYLANGUAGE;
		static const std::string HTTPUSEOFFLINESPOOL;
		static const std::string HTTPOFFLINESPOOLRETRY;
		static const std::string HTTPOFFLINESPOOLMAXREQUESTAGE;

		static const std::string EXPANDEDTEXTWIDTH;
		static const std::string EXPANDEDTEXTHEIGHT;
		static const std::string EXPANDEDTEXTFONTNAME;
		static const std::string EXPANDEDTEXTPOINTSIZE;
		static const std::string SPECIALMESSAGEWIDTH;
		static const std::string SPECIALMESSAGEHEIGHT;
		static const std::string SPECIALMESSAGEFONTNAME;
		static const std::string SPECIALMESSAGEPOINTSIZE;
		static const std::string PRODUCTNOTEWIDTH;
		static const std::string PRODUCTNOTEHEIGHT;
		static const std::string PRODUCTNOTEFONTNAME;
		static const std::string PRODUCTNOTEPOINTSIZE;
		static const std::string MARQUEETEXTWIDTH;
		static const std::string MARQUEETEXTHEIGHT;
		static const std::string MARQUEETEXTFONTNAME;
		static const std::string MARQUEETEXTPOINTSIZE;
		static const std::string PRODUCTTEXT5WIDTH;
		static const std::string PRODUCTTEXT5HEIGHT;
		static const std::string PRODUCTTEXT5FONTNAME;
		static const std::string PRODUCTTEXT5POINTSIZE;
		static const std::string PRODUCTTEXT6WIDTH;
		static const std::string PRODUCTTEXT6HEIGHT;
		static const std::string PRODUCTTEXT6FONTNAME;
		static const std::string PRODUCTTEXT6POINTSIZE;
		static const std::string PRODUCTTEXT7WIDTH;
		static const std::string PRODUCTTEXT7HEIGHT;
		static const std::string PRODUCTTEXT7FONTNAME;
		static const std::string PRODUCTTEXT7POINTSIZE;
		static const std::string PRODUCTTEXT8WIDTH;
		static const std::string PRODUCTTEXT8HEIGHT;
		static const std::string PRODUCTTEXT8FONTNAME;
		static const std::string PRODUCTTEXT8POINTSIZE;
		static const std::string PRODUCTTEXT9WIDTH;
		static const std::string PRODUCTTEXT9HEIGHT;
		static const std::string PRODUCTTEXT9FONTNAME;
		static const std::string PRODUCTTEXT9POINTSIZE;

		static const std::string PRODDESCRIPTIONWIDTH;
		static const std::string PRODDESCRIPTIONHEIGHT;
		static const std::string PRODDESCRIPTIONFONTNAME;
		static const std::string PRODDESCRIPTIONPOINTSIZE;
		static const std::string COOLTRACKINGTEXTWIDTH;
		static const std::string COOLTRACKINGTEXTHEIGHT;
		static const std::string COOLTRACKINGTEXTFONTNAME;
		static const std::string COOLTRACKINGTEXTPOINTSIZE;

		static const std::string MAXLABELLENGTHPCT;
		static const std::string MAXLABELLENGTHPCTEPP;
    
		// The following are used by PrinterStockMetrics.cpp, .h
		static const std::string DIMBLENGTH1_000;
		static const std::string DIMBLENGTH1_500;
		static const std::string DIMBLENGTH1_750;
		static const std::string DIMBLENGTH2_000;
		static const std::string DIMBLENGTH2_375;
		static const std::string DIMBLENGTH2_500;
		static const std::string DIMBLENGTH3_000;
		static const std::string DIMBLENGTH3_500;
		static const std::string DIMBLENGTH4_000;
		static const std::string DIMBLENGTH4_500;
		static const std::string DIMBLENGTH5_000;
		static const std::string DIMBLENGTH5_500;
		static const std::string DIMBLENGTH6_000;
		static const std::string DIMBLENGTH6_500;
		static const std::string DIMBLENGTH7_000;
		static const std::string DIMBLENGTH7_500;
		static const std::string DIMBLENGTH8_000;
		static const std::string DIMBLENGTH8_500;
		static const std::string DIMBLENGTH9_000;
		static const std::string DIMBLENGTH9_500;
		static const std::string DIMBLENGTH10_00;
		static const std::string DIMBLENGTHCONTINUOUS;
  
		static const std::string GCDENABLED;
		static const std::string GCDENABLEDSTANDALONE;
		static const std::string DESKTOPSTORELOGOPATH;
		static const std::string DESKTOPSTORELOGOFILENAME;
		static const std::string DEVICESTORELOGOPATH;
		static const std::string DEVICESTORELOGOFILENAME;
		static const std::string DEVICEAPPLICATIONUPDATESDIR;
		static const std::string DEVICEAPPLICATIONUPDATESSOURCEFILENAME;
		static const std::string DEVICEPLAYLISTUPDATESDIR;
		static const std::string LOCALTEMPDIR;
		static const std::string ADAPTERDESCSEARCHRNDIS;
		static const std::string IPSUBNETMASK;
		static const std::string IPADDRESSHLX;
		static const std::string IPADDRESSGCD;
		static const std::string GCDSERVERPORT;
		static const std::string GCDDISPLAYOBJECTXMLPATHFORMAT;
		static const std::string GCDDISCOVERYMESSAGE;
		static const std::string GCDDISCOVERYPORT;

		static const std::string GOODBEEPENABLED;
		static const std::string GOODBEEPFREQUENCY;
		static const std::string GOODBEEPPERIOD;
		static const std::string GOODBEEPDELAY;
		static const std::string BADBEEPENABLED;
		static const std::string BADBEEPFREQUENCY;
		static const std::string BADBEEPPERIOD;
		static const std::string BADBEEPDELAY;

		static const std::string UPKEYBOARDXCOORDINATE;
		static const std::string UPKEYBOARDYCOORDINATE;
		static const std::string UPKEYBOARDWIDTH;
		static const std::string UPKEYBOARDHEIGHT;
		static const std::string DOWNKEYBOARDXCOORDINATE;
		static const std::string DOWNKEYBOARDYCOORDINATE;
		static const std::string DOWNKEYBOARDWIDTH;
		static const std::string DOWNKEYBOARDHEIGHT;
		static const std::string MIDDLEKEYBOARDXCOORDINATE;
		static const std::string MIDDLEKEYBOARDYCOORDINATE;
		static const std::string MIDDLEKEYBOARDWIDTH;
		static const std::string MIDDLEKEYBOARDHEIGHT;

		static const std::string PRODUCTDISCOUNTLEVEL1LABEL;
		static const std::string PRODUCTDISCOUNTLEVEL2LABEL;
		static const std::string PRODUCTDISCOUNTLEVEL3LABEL;

		static const std::string DEFAULTFONTNAME;
		static const std::string NOFONTSELECTEDNAME;
		static const std::string FONTEXTENSIONS;
		static const std::string SYSTEMFONTSDIR;
		static const std::string USERFONTSDIR;

		static const std::string NOTIFYUIOFUPDATESTATUS;

		static const std::string DELAYBETWEENMANAGERTHREADMESSAGES;
		static const std::string DELAYBETWEENSESSIONMESSAGES;

		static const std::string CFGIPARAMETERNOTFOUNDTHROWSEXCEPTION;

		static const std::string NUMBEROFPRODUCTTEXTS;

		static const std::string ACCUMITEMSLABELFAMILYSTART;
		static const std::string ACCUMITEMSITEMLABELTYPE;
		static const std::string ACCUMITEMSTOTALSONLYLABELFAMILYSTART;
		static const std::string ACCUMITEMSITEMCOUNTRENDERNOTIFY;
		static const std::string ACCUMITEMSSIMULATEPRINTING;

		static const std::string PRINTERSTATUSNONREPEATINGMSGSTOUI;

		static const std::string CONTINUOUSNETWORKSTATUSDELAY;
		static const std::string NETWORKCONNECTIVITYONLINEWAITPERIODSECS;
		static const std::string NETWORKCONNECTIVITYOFFLINEWAITPERIODSECS;
		static const std::string NETWORKCONNECTIVITYOFFLINERETRANSMISSIONPERIODSECS;
		static const std::string NETWORKCONNECTIVITYINTERFACEDETECTIONTIME;

		static const std::string AUTOTESTERNEEDSLABELTAKEN;

		static const std::string PLANOGRAMSENABLED;

		static const std::string MAXBESTSELLERSOFTKEYS;
		static const std::string MAXBESTSELLERSOFTKEYSFROMFEATURE;

		static const std::string SOFTKEYPLUNUMBERBEFORENAME;
		static const std::string SOFTKEYNEWLINEBETWEENPLUNUMBERANDNAME;

		static const std::string MESSAGELOGGINGDIRECTORY;					//	Added for the Safeway feature to use the same path as Windux
		static const std::string MESSAGELOGGINGINFOFILENAME;
		static const std::string MESSAGELOGGINGWARNINGSFILENAME;
		static const std::string MESSAGELOGGINGERRORSFILENAME;
		static const std::string MESSAGELOGGINGSTARTUPSHUTDOWNFILENAME;
		static const std::string MESSAGELOGGINGSCALEEVENTSFILENAME;       // Access
		static const std::string MESSAGELOGGINGDATAMAINTENANCEFILENAME;   // Access
		static const std::string MESSAGELOGGINGSCHEDULEDEVENTSFILENAME;   // Access
		static const std::string MESSAGELOGGINGMAXFILESIZE;				  // Access: Added for the Safeway feature to be configurable (# of bytes).

		static const std::string REMOTESCALEMAINTENANCEENABLED;		      // Access
		static const std::string REMOTESCALEMAINTCLROUTBOXDAYS;			  // Access

		static const std::string OPERACCESSLEVELSFORPRODUCTSENABLED;

		static const std::string RECALLSENABLED;

		static const std::string PRODUCTSUPPORTTEXTLABELNUM;
		static const std::string PRODUCTUINUTRILABELNUM;

		static const std::string UNDIMSCREENONZEROWEIGHT;
		static const std::string UNDIMSCREENONNONZEROWEIGHT;
		static const std::string UNDIMSCREENONTAPPLATTER;

		static const std::string COMMREQUIRESDATATYPETAG;

		static const std::string DOWNLOADDIR;
		static const std::string DOWNLOADFILESPECIFIERS;
		static const std::string DOWNLOADCHECKINGINTERVALTIMER;
		static const std::string DOWNLOADCHECKINGUPDATETIMER;
		static const std::string DOWNLOADPROCESSINGDIR;
		static const std::string DOWNLOADOUTPUTDIR;

		static const std::string MERGEDIR;
		static const std::string MERGEFILESPECIFIERS;

		static const std::string KEEPCOPYOFPROCESSEDDATA;
		static const std::string MAKEPROCESSEDFILENAMEUNIQUE;
		static const std::string PROCESSEDDIR;

		static const std::string PYTHONENABLED;
		static const std::string SMBACKENDPORT;
		static const std::string PYTHONSCRIPTPORT;
		static const std::string PYTHONSFTPUPDATEPORT;
		static const std::string PYTHONDBBACKUPRESPONSEPORT;
		static const std::string PYTHONDEVICEEVENTMESSAGESCRIPTNAME;
		static const std::string PYTHONSENDSMBACKENDMESSAGESCRIPTNAME;
		static const std::string PYTHONGETBOARDTYPESCRIPTNAME;

		static const std::string SCALEMANAGEMENTHANDLESUPGRADE;

		static const std::string COREDUMPENABLED;

		static const std::string DEFAULTPINGINTERVAL;

		static const std::string SWITCHTORUNSCREENONSTABLEWEIGHT;

		static const std::string SELFSERVEALPHABETICSEARCHQUERY;

		static const std::string LICENSEPACKAGESTRINGS;
		static const std::string LICENSEDPACKAGESTRINGS;

		static const std::string USBPRINTEROUTPUTFILENAME;
		static const std::string USBPRINTERXCOORDINATE;
		static const std::string USBPRINTERYCOORDINATE;

		static const std::string NETWORKPRINTERDRIVERSDIRECTORY;
		static const std::string KNOWNUSBPRINTERS;
		static const std::string KNOWNUSBPRINTERDRIVERS;
		static const std::string KNOWNLANPRINTERS;
		static const std::string KNOWNLANPRINTERDRIVERS;

		static const std::string EXTPRINTERSPDFEXTENSIONS;
		static const std::string EXTPRINTERSDOCEXTENSIONS;
		static const std::string DEFAULTGHOSTSCRIPTDEVICE;
		static const std::string GHOSTSCRIPTDEVICES;

		static const std::string MFGTESTSERVER;

		static const std::string STAGINGDIR;

		static const std::string STATICPRODUCTDATATARBALLNAME;

		static const std::string MAXCUTTESTNUMBER;
		static const std::string REQUIREMEATSPECSHEET;
		static const std::string CAT3AUDITTRAILDEFAULTFILENAME;
		static const std::string CAT3WEIGHERCONFIGFILENAME;

		static const std::string SELFSERVEPRINTTOSCREENFILENAME;
		static const std::string SELFSERVEPRINTTOSCREENLABELTYPE;
		static const std::string SELFSERVEPRINTTOSCREENLABELTYPEFS;

		static const std::string CONSERVECONTINUOUSSTOCKBYNOTSIZING;

		static const std::string UNITPRICEPRINTOPTIONSENABLED;
		static const std::string UNITPRICEPRINTEXACTCALCBACKUPMETHOD;

		static const std::string SAVELABEL;
		static const std::string IGNOREVNPNTAGSWHENUPTAG;
		static const std::string FORCENONZEROBYCOUNTFORFIXEDWEIGHTITEMS;

		static const std::string ALLOWPRODUCTIONENTRYMODEONWRAPPER;

		static const std::string SCALESETTINGSFILE;

		static const std::string FPMMANAGERNOTATEAMPLAYER;

		static const std::string TEMPLATELABELBARCODEOFFSET;
		static const std::string TEMPLATELABELSHOWOPERATORDIALOGS;

		static const std::string UTILIZECUSTOMERHOOKFOROPERATEMODEONLY;
		static const std::string HFPDDIPADDRESS;
		static const std::string HFPDDPORT;
		static const std::string CONSECUTIVETIMESCLOSESTTOBELOGGEDIN;
		static const std::string GETDEVICESINLOGINRANGEINTERVAL;
		static const std::string GETDEVICESINLOGINRANGEAGE;
		static const std::string GETDEVICESINRANGEINTERVAL;
		static const std::string GETDEVICESINRANGEAGE;
		static const std::string IGNORELABELTAKENDIECUT;
		
		static const std::string BAMNUMOFSIDESREPRESENTINGUNLIMITEDSIDES;

		static const std::string BACKUPROMLABELS;

		static const std::string PRINTTOSCREENBARCODELABELTYPE;
		static const std::string PRINTTOSCREENBARCODEDIRNAME;
		static const std::string PRINTTOSCREENBARCODEFILENAME;
		static const std::string PRINTTOSCREENBARCODEEXTENSION;
		static const std::string SELFSERVEWEIGHTFIELDWIDTH;
		static const std::string SELFSERVEWEIGHTFIELDHEIGHT;
		static const std::string SELFSERVEWEIGHTFIELDFONTSIZE;

		static const std::string CAMERASYSTEMPREDICTWAITTIME;
		static const std::string CAMERASYSTEMPREDICTLOOPS;
		static const std::string CAMERASYSTEMALLOWEDOPERATE;
		static const std::string CAMERASYSTEMALLOWEDITEMSCREEN;

		static const std::string LIMITRUNINMAX;
		static const std::string SELFSERVETIMEOUT;
		static const std::string BARCODEFORMATTOUSEOFFLINE;
		static const std::string BARCODEREQUESTTIMEOUT;

		static const std::string RFIDVOIDLABELTYPENUMBER;


	private:

		mutable Util::CSmartCriticalSection  cfgICS_;

		util_env::Properties *cfgIProperties_;

		std::map<const std::string, std::string> cfgIDefaults_;

		std::vector<std::string> configNames_;  //[endCfgInternalFieldOffsets];

		unsigned int internalConfigChecksum_;
		unsigned int customInternalConfigChecksum_;

		bool cfgIParameterNotFoundThrowsException_;

		bool traceCfgInternal_;
		bool traceCfgInternalExc_;

		static std::string internalConfigFileName_;
		static std::string customInternalConfigFileName_;


		static std::string networkConfigFileName_;


		void init();		

		const std::string *lookupDefault(const std::string &name) const;





	public:


		double getPercentageDivisor() const {int percentagePrecision = toInt(CfgInternal::PERCENTAGEPRECISION); return (percentagePrecision >= 0) ? Power::power((double)10.0, (double)percentagePrecision) : (double)1000.0;}

		HobString getDesktopStoreLogoPath() const {return toString(DESKTOPSTORELOGOPATH);}
		HobString getDesktopStoreLogoFileName() const {return toString(DESKTOPSTORELOGOFILENAME);}
		HobString getDeviceStoreLogoPath() const {return toString(DEVICESTORELOGOPATH);}
		HobString getDeviceStoreLogoFileName() const {return toString(DEVICESTORELOGOFILENAME);}
		HobString getDeviceApplicationUpdatesDir() const {return toString(DEVICEAPPLICATIONUPDATESDIR);}
		HobString getDeviceApplicationUpdatesSourceFileName() const {return toString(DEVICEAPPLICATIONUPDATESSOURCEFILENAME);}
		HobString getDevicePlaylistUpdatesDir() const {return toString(DEVICEPLAYLISTUPDATESDIR);}
		HobString getLocalTempDir() const {return toString(LOCALTEMPDIR);}
		HobString getAdapterDescSearchRNDIS() const {return toString(ADAPTERDESCSEARCHRNDIS);}
		HobString getIpSubnetMask() const {return toString(IPSUBNETMASK);}
		HobString getIpAddressHLX() const {return toString(IPADDRESSHLX);}
		HobString getIpAddressGCD() const {return toString(IPADDRESSGCD);}
		int getGcdServerPort() const {return toInt(GCDSERVERPORT);}
		HobString getGcdDisplayObjectXmlPathFormat() const {return toString(GCDDISPLAYOBJECTXMLPATHFORMAT);}
		HobString getGcdDiscoveryMessage() const {return toString(GCDDISCOVERYMESSAGE);}
		int getGcdDiscoveryPort() const {return toInt(GCDDISCOVERYPORT);}

		
		ScaleModel getScaleModel() const {return scaleModel;}
		void setScaleModel(ScaleModel model) {scaleModel = model;}

		
		/*** Self-Service Internal parameters ***/
		bool isSelfServiceScale() const {return ((scaleModel == ScaleModel_ESS) ? true : false);}
		bool isPrepackScale() const {return ((scaleModel == ScaleModel_EPP) ? true : false);}
		bool isServiceCounterScale() const {return ((scaleModel == ScaleModel_ESC) ? true : false);}
		


		HobString getWebPagesDir() const;

		bool isRFIDHWPresent() const {return RFIDHWPresent;}
		void setRFIDHWPresent(bool value) {RFIDHWPresent = value;}


	private:

		ScaleModel scaleModel;

		HobString systemGraphicsLocation;	//	The full path of the System graphics directory, terminating with a '/'
		HobString userGraphicsLocation;		//	The full path of the User graphics directory, terminating with a '/'
		HobString imagesLocation;			//	The full path of the Images directory, terminating with a '/'

		bool RFIDHWPresent;

		void loadIndividualCfgItems();
		void initIndividualCfgItems();

		void loadScaleModelCfg();

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //CFGINTERNAL_H
