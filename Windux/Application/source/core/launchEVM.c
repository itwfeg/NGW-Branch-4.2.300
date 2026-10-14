#include "vxWorks.h"
#include "taskLib.h"
#include <iflib.h>

#if 1
/*
	If you want the browser without the address line, Menu bar, etc., 
	then set the #if 1 above to #if 0 and rebuild modEVMLaunch files.
	Make sure you have the file ib5ref_hobart.jar in your browser lib directory.
	You need ib5ref_hobart.jar to run the browser without the extra stuff.
*/
#define BROWSER_WITH_MENU_BAR
#endif

extern EvmStart();
extern EvmSym_libevmawtStd();
extern EvmSym_libevmawt();
extern EvmSym_libjdwpDev();


void launchEvmIceBrowserLocal()
{

char addr[256];
char launchString[5096];

	if (ifAddrGet("fei0", addr)==OK)
	{
		printf ("address = %s\n", addr);

		strcpy (launchString, "-Djava.home=/ram0 -Duser.home=/ram0
		-Djeode.evm.memory.stackSize=131072
		-Djeode.evm.memory.stackOverflowZone=64  
#ifdef BROWSER_WITH_MENU_BAR
		-cp /ram0/ice/lib/ib5core.jar:/ram0/ice/lib/ib5crypto.jar:/ram0/ice/lib/ib5extra.jar:/ram0/ice/lib/ib5http.jar:/ram0/ice/lib/ib5https.jar:/ram0/ice/lib/ib5js.jar:/ram0/ice/lib/ib5ref.jar:/ram0/ice/lib/ib5ri.jar:/ram0/ice/lib/ib5swing.jar:/ram0/ice/lib/ib5util.jar:/ram0/ice/lib/ib5xalan.jar:/ram0/ice/lib/iscl.jar:/ram0/web:/ram0/ice/lib
#else
		-cp /ram0/ice/lib/ib5core.jar:/ram0/ice/lib/ib5crypto.jar:/ram0/ice/lib/ib5extra.jar:/ram0/ice/lib/ib5http.jar:/ram0/ice/lib/ib5https.jar:/ram0/ice/lib/ib5js.jar:/ram0/ice/lib/ib5ref_hobart.jar:/ram0/ice/lib/ib5ri.jar:/ram0/ice/lib/ib5swing.jar:/ram0/ice/lib/ib5util.jar:/ram0/ice/lib/ib5xalan.jar:/ram0/ice/lib/iscl.jar:/ram0/web:/ram0/ice/lib
#endif
		-Dice.browser.verbose=true -Dice.browser.minMemory=0xc00000 
		-Dice.browser.maxMemory=0x1000000 ice.browser.Main -w640 -h480 -nosm -c:ie -m:http 
		-t:awt http://");
		strcat (launchString, addr);
		strcat (launchString, "/home.asp?consoletype=1");

		taskSpawn ("test_evm", 106, 0x1d, 64*1024, EvmStart, launchString, EvmSym_libevmawt,0,0,0,0,0,0,0,0);
	}


}


void launchEvmIceBrowser(char *jworksLoc, char *iceLoc)
{

char addr[256];
char launchString[5096];

	if (ifAddrGet("fei0", addr)==OK)
	{
		printf ("address = %s\n", addr);

		strcpy (launchString, "-Djava.home=");
		strcat (launchString, jworksLoc);
		strcat (launchString, "/jworks -Duser.home=");
		strcat (launchString, jworksLoc);
		strcat (launchString, "/jworks -Djeode.evm.memory.stackSize=131072 -Djeode.evm.memory.stackOverflowZone=64 -Djeode.evm.compiler.threshold=5 -Djeode.evm.compiler.codeBuffTotal=4194304 -Djeode.evm.compiler.dataSize=512000 -cp ");
		strcat (launchString, iceLoc);
		strcat (launchString, "/ice/lib/ib5core.jar:");
		strcat (launchString, iceLoc);
		strcat (launchString, "/ice/lib/ib5crypto.jar:");
		strcat (launchString, iceLoc);
		strcat (launchString, "/ice/lib/ib5extra.jar:");
		strcat (launchString, iceLoc);
		strcat (launchString, "/ice/lib/ib5http.jar:");
		strcat (launchString, iceLoc);
		strcat (launchString, "/ice/lib/ib5https.jar:");
		strcat (launchString, iceLoc);
		strcat (launchString, "/ice/lib/ib5js.jar:");
		strcat (launchString, iceLoc);
#ifdef BROWSER_WITH_MENU_BAR
		strcat (launchString, "/ice/lib/ib5ref.jar:");
#else
		strcat (launchString, "/ice/lib/ib5ref_hobart.jar:");
#endif
		strcat (launchString, iceLoc);
		strcat (launchString, "/ice/lib/ib5ri.jar:");
		strcat (launchString, iceLoc);
		strcat (launchString, "/ice/lib/ib5swing.jar:");
		strcat (launchString, jworksLoc);
		strcat (launchString, "/jworks/lib/fakeswing.jar:");
		strcat (launchString, jworksLoc);
		strcat (launchString, "/jworks/lib/swingall.jar:");
		strcat (launchString, iceLoc);
		strcat (launchString, "/ice/lib/ib5util.jar:");
		strcat (launchString, iceLoc);
		strcat (launchString, "/ice/lib/ib5xalan.jar:");
		strcat (launchString, iceLoc);
		strcat (launchString, "/ice/lib/iscl.jar:");
		strcat (launchString, iceLoc);
		strcat (launchString, "/ice/lib/:");
		strcat (launchString, "/ram0/web/mom.jar:");
		strcat (launchString, "/ram0/web");
		strcat (launchString, " -Dice.browser.verbose=true -Dice.browser.minMemory=0xc00000 -Dice.browser.maxMemory=0x1000000 ice.browser.Main -w640 -h480 -nosm -c:ie -m:http -t:awt http://");
		strcat (launchString, addr);
		strcat (launchString, "/home.asp?consoletype=1");

		printf ("launchString = %s\n", launchString);

		taskSpawn ("test_evm", 106, 0x1d, 64*1024, EvmStart, launchString, EvmSym_libevmawt,0,0,0,0,0,0,0,0);
	}


}

void launchEvmSwingTest()
{

char addr[256];
char launchString[5096];

	if (ifAddrGet("fei0", addr)==OK)
	{
		printf ("address = %s\n", addr);

		strcpy (launchString, "-Djava.home=/ata0a/jworks -Duser.home=/ata0a/jworks
		-Djeode.evm.memory.stackSize=131072
		-Djeode.evm.memory.stackOverflowZone=64  
		-cp /ata0a/test:/ata0a/jworks/swing/swing-pjava-patch.jar:/ata0a/jworks/swing/swingall.jar 
		swingtest");

		taskSpawn ("test_evm", 106, 0x1d, 64*1024, EvmStart, launchString, EvmSym_libevmawt,0,0,0,0,0,0,0,0);
	}


}


void launchTextEditor()
{

char addr[256];
char launchString[5096];

	if (ifAddrGet("fei0", addr)==OK)
	{
		printf ("address = %s\n", addr);

		strcpy (launchString, "-Djava.home=/ata0a/jworks -Duser.home=/ata0a/jworks
		-Djeode.evm.memory.stackSize=131072
		-Djeode.evm.memory.stackOverflowZone=64  
		-cp /ata0a/test:/ata0a/jworks/swing/swing-pjava-patch.jar:/ata0a/jworks/swing/swingall.jar 
		texteditor 3.0 4.0");

		taskSpawn ("test_evm", 106, 0x1d, 64*1024, EvmStart, launchString, EvmSym_libevmawt,0,0,0,0,0,0,0,0);
	}


}


void launchMenuTest(char *jworksLoc, char *iceLoc)
{

char addr[256];
char launchString[5096];

	if (ifAddrGet("fei0", addr)==OK)
	{
		strcpy (launchString, "-Djava.home=");
		strcat (launchString, jworksLoc);
		strcat (launchString, "/jworks -Duser.home=");
		strcat (launchString, jworksLoc);
		strcat (launchString, "/jworks -Djeode.evm.memory.stackSize=131072 -Djeode.evm.memory.stackOverflowZone=64 -Djeode.evm.compiler.threshold=5 -Djeode.evm.compiler.codeBuffTotal=4194304 -Djeode.evm.compiler.dataSize=512000 -cp ");
		strcat (launchString, iceLoc);
		strcat (launchString, "/ice/lib/ib5core.jar:");
		strcat (launchString, iceLoc);
		strcat (launchString, "/ice/lib/ib5crypto.jar:");
		strcat (launchString, iceLoc);
		strcat (launchString, "/ice/lib/ib5extra.jar:");
		strcat (launchString, iceLoc);
		strcat (launchString, "/ice/lib/ib5http.jar:");
		strcat (launchString, iceLoc);
		strcat (launchString, "/ice/lib/ib5https.jar:");
		strcat (launchString, iceLoc);
		strcat (launchString, "/ice/lib/ib5js.jar:");
		strcat (launchString, iceLoc);
#ifdef BROWSER_WITH_MENU_BAR
		strcat (launchString, "/ice/lib/ib5ref.jar:");
#else
		strcat (launchString, "/ice/lib/ib5ref_hobart.jar:");
#endif
		strcat (launchString, iceLoc);
		strcat (launchString, "/ice/lib/ib5ri.jar:");
		strcat (launchString, iceLoc);
		strcat (launchString, "/ice/lib/ib5swing.jar:");
		strcat (launchString, iceLoc);
		strcat (launchString, "/ice/lib/ib5util.jar:");
		strcat (launchString, iceLoc);
		strcat (launchString, "/ice/lib/ib5xalan.jar:");
		strcat (launchString, iceLoc);
		strcat (launchString, "/ice/lib/iscl.jar:");
		strcat (launchString, iceLoc);
		strcat (launchString, "/ice/lib/:");
		strcat (launchString, "/ram0/web");
		strcat (launchString, " -Dice.browser.verbose=true -Dice.browser.minMemory=0xc00000 -Dice.browser.maxMemory=0x1000000 ice.browser.Main -w640 -h480 -nosm -c:ie -m:http -t:awt http://");
		strcat (launchString, addr);
		strcat (launchString, "/home.html");

		printf ("launchString = %s\n", launchString);

		taskSpawn ("test_evm", 106, 0x1d, 64*1024, EvmStart, launchString, EvmSym_libevmawt,0,0,0,0,0,0,0,0);
	}


}


/*
void launchEvmIceBrowserHost()
{

	taskSpawn ("test_evm", 100, 0x1d, 64*1024, EvmStart,
		"-Djava.home=/d/tornado -Duser.home=/d/tornado
		-Djava.ms=0x400000 -Djava.mx=0x500000
		-Djeode.evm.memory.stackSize=131072
		-Djeode.evm.memory.stackOverflowZone=64  
		-cp /d/tornado/ice/lib/ib5core.jar:/d/tornado/ice/lib/ib5crypto.jar:/d/tornado/ice/lib/ib5extra.jar:/d/tornado/ice/lib/ib5http.jar:/d/tornado/ice/lib/ib5https.jar:/d/tornado/ice/lib/ib5js.jar:/d/tornado/ice/lib/ib5ref.jar:/d/tornado/ice/lib/ib5ri.jar:/d/tornado/ice/lib/ib5swing.jar:/d/tornado/ice/lib/ib5util.jar:/d/tornado/ice/lib/iscl.jar:/ram0/web:/d/tornado/jworks/lib/awt.jar:/d/tornado/jworks/lib/ic.jar:/d/tornado/jworks/lib/core.jar:/d/tornado/jworks/lib/i18n.jar:/d/tornado/jworks/lib/awt_g.jar:/d/tornado/jworks/lib/ic_g.jar:/d/tornado/jworks/lib/core_g.jar:/d/tornado/jworks/lib/i18n_g.jar
		-Dice.browser.verbose=true -Dice.browser.minMemory=0x300000 
		-Dice.browser.maxMemory=0x400000 ice.browser.Main -w640 -h480 -c:ie -m:http 
		-t:awt ", EvmSym_libevmawtStd,0,0,0,0,0,0,0,0);
}

void launchEvmIceBrowserTgtsvr()
{

	taskSpawn ("test_evm", 100, 0x1d, 64*1024, EvmStart,
		"-Djava.home=/tgtsvr/jworks -Duser.home=/tgtsvr/jworks
		-Djeode.evm.memory.stackSize=131072
		-Djeode.evm.memory.stackOverflowZone=64  
		-cp /tgtsvr/ice/lib/ib5core.jar:/tgtsvr/ice/lib/ib5crypto.jar:/tgtsvr/ice/lib/ib5extra.jar:/tgtsvr/ice/lib/ib5http.jar:/tgtsvr/ice/lib/ib5https.jar:/tgtsvr/ice/lib/ib5js.jar:/tgtsvr/ice/lib/ib5ref.jar:/tgtsvr/ice/lib/ib5ri.jar:/tgtsvr/ice/lib/ib5swing.jar:/tgtsvr/ice/lib/ib5util.jar:/tgtsvr/ice/lib/iscl.jar:/ram0/web:/tgtsvr/jworks/lib/awt.jar:/tgtsvr/jworks/lib/ic.jar:/tgtsvr/jworks/lib/core.jar:/tgtsvr/jworks/lib/i18n.jar:/tgtsvr/jworks/lib/awt_g.jar:/tgtsvr/jworks/lib/ic_g.jar:/tgtsvr/jworks/lib/core_g.jar:/tgtsvr/jworks/lib/i18n_g.jar
		-Dice.browser.verbose=true -Dice.browser.minMemory=0x600000 
		-Dice.browser.maxMemory=0x800000 ice.browser.Main -w640 -h480 -c:ie -m:http 
		-t:awt http://10.3.135.231", EvmSym_libevmawtStd,0,0,0,0,0,0,0,0);
}

void launchEvmIceBrowserFile()
{

	taskSpawn ("test_evm", 100, 0x1d, 64*1024, EvmStart,
		"-Djava.home=/ram0 -Duser.home=/ram0
		-Djeode.evm.memory.stackSize=131072
		-Djeode.evm.memory.stackOverflowZone=64  
		-cp /ram0/ice/lib/ib5core.jar:/ram0/ice/lib/ib5crypto.jar:/ram0/ice/lib/ib5extra.jar:/ram0/ice/lib/ib5http.jar:/ram0/ice/lib/ib5https.jar:/ram0/ice/lib/ib5js.jar:/ram0/ice/lib/ib5ref.jar:/ram0/ice/lib/ib5ri.jar:/ram0/ice/lib/ib5swing.jar:/ram0/ice/lib/ib5util.jar:/ram0/ice/lib/iscl.jar:/ram0/web:/ram0/lib/awt.jar:/ram0/lib/ic.jar:/ram0/lib/core.jar:/ram0/lib/i18n.jar:/ram0/lib/awt_g.jar:/ram0/lib/ic_g.jar:/ram0/lib/core_g.jar:/ram0/lib/i18n_g.jar
		-Dice.browser.verbose=true -Dice.browser.minMemory=0x1c00000 
		-Dice.browser.maxMemory=0x2000000 ice.browser.Main -w640 -h480 -c:ie -m:http 
		-t:awt FILE://localhost/ram0/web/home.html", EvmSym_libevmawtStd,0,0,0,0,0,0,0,0);
}

void launchEvmTest()
{
	taskSpawn ("test_evm",100, 0x1d, 64*1024, EvmStart,
		"-Djava.home=/ram0 -Duser.home=/ram0
		-Djeode.evm.memory.stackSize=131072
		-Djeode.evm.memory.stackOverflowZone=64  
		-cp /ram0/test
		URLGet http://10.3.135.232/operateRunScreen_origToday2.asp"
		, 0,0,0,0,0,0,0,0,0);
}

void launchEvmTestHost()
{
	taskSpawn ("test_evm",100, 0x1d, 64*1024, EvmStart,
		"-Djava.home=/tgtsvr -Duser.home=/tgtsvr
		-Djeode.evm.memory.stackSize=131072
		-Djeode.evm.memory.stackOverflowZone=64  
		-cp /tgtsvr/test
		hobart.testjvm.testjvm 100000000"
		, EvmSym_libevmawtStd,0,0,0,0,0,0,0,0);
}

void launchEvmCafBenchmarkHost()
{
	taskSpawn ("test_evm", 100, 0x1d, 64*1024, EvmStart,
		"-Djava.home=/tgtsvr -Duser.home=/tgtsvr
		-Djeode.evm.memory.stackSize=131072
		-Djeode.evm.memory.stackOverflowZone=64  
		-cp /tgtsvr/test/embed.zip
		CaffeineMarkEmbeddedApp"
		, EvmSym_libevmawtStd,0,0,0,0,0,0,0,0);
}


void launchEvmDebug()
{

	taskSpawn ("debug_EVM", 100, 0x1d, 64*1024, EvmStart,
		"-Djava.home=/tgtsvr -Duser.home=/tgtsvr
		-Djeode.evm.memory.stackSize=131072
		-Djeode.evm.memory.stackOverflowZone=64 
		-Xdebug -Xnoagent -Xrunjdwp:transport=dt_socket,server=y,suspend=y,address=10.3.135.232:8000 
		-cp /tgtsvr/demos/classes com.windriver.demo.debug.DebugTutorial"
		,EvmSym_libjdwpDev,0,0,0,0,0,0,0,0);
}
*/