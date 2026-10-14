/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "TestBrowser.h"
#include "HobSocket.h"
#include "CMReaderWriterEvolUI.h"
#include "CMReaderWriterEvolTag.h"
#include "SessionMessage.h"
#include "Messenger.h"

#include "ProductDevEvent.h"
#include "WeigherEvent.h"
#include "SystemEvent.h"
#include "ConfigurationEvent.h"
#include "Configuration.h"
#include "HobTokenizerCSV.h"
#include "HobOperator.h"
#include "GenericHfcObject.h"
#include "HfcWeigherConfig.h"
#include "HfcLabelApplierConfig.h"
#include "HfcPrinterConfig.h"
#include "HfcWeigherSystemStatus.h"
#include "WeigherInfo.h"
#include "PrinterInfo.h"
#include "HfcPrinterSystemStatus.h"
#include "HfcPrinterSensors.h"
#include "TotalType.h"

#include "Debug.h"
#include "Integer.h"
#include "Long.h"
#include "Double.h"
#include "Hex.h"
#include "Bool.h"

#include "GraphicEvent.h"
#include "GraphicFileNames.h"
#include "Graphic.h"
#include "Class.h"
#include "HfcProductSupportDataMsg.h"
#include "FrequentShopper.h"

TestBrowser::TestBrowser(int conn, int form)
	: HobThread( (FUNCPTR)activeFunction, "tstBrwsr", 100, 0, 30000, 20, 2048, MSG_Q_FIFO),
	  connNum(conn),
	  format(form),
	  filename((char *)NULL),
	  commandFile((ifstream *)NULL),
	  rfilename((char *)NULL),
	  responseFile((ofstream *)NULL),
	  messenger((Messenger *)NULL),
	  scnt(0),
	  rcnt(0),
	  delayAfterSend(0),
	  s((Socket *)NULL)
{
}

TestBrowser::TestBrowser(int conn, int form, char *fname)
	: HobThread( (FUNCPTR)activeFunction, "tstBrwsr", 100, 0, 30000, 20, 2048, MSG_Q_FIFO),
	  connNum(conn),
	  format(form),
	  filename(fname),
	  commandFile((ifstream *)NULL),
	  rfilename((char *)NULL),
	  responseFile((ofstream *)NULL),
	  messenger((Messenger *)NULL),
	  scnt(0),
	  rcnt(0),
	  delayAfterSend(0),
	  s((Socket *)NULL)
{
}

TestBrowser::~TestBrowser()
{
	if (commandFile != (ifstream *)NULL)
	{
		commandFile->close();
		delete commandFile;
	}
	if (responseFile != (ofstream *)NULL)
	{
		responseFile->close();
		delete responseFile;
	}
	if (rfilename != (char *)NULL)
		delete rfilename;
	if (messenger != (Messenger *)NULL)
		delete messenger;
}

int TestBrowser::activeFunction(void *pObject)
{
	return ((TestBrowser *) pObject)->activeFunctionTask();
}

int TestBrowser::activeFunctionTask()
{
	start();
	return 0;
}

int TestBrowser::start()
{
	if (filename != (char *)NULL)
	{
		if (strlen(filename) == 0)
			filename = (char *)NULL;
		else
		{
			// Open the command file containing the user's commands.
			commandFile = new ifstream(filename);
			if (! *commandFile)
				return 2;

			// Create a response file name the same as the input file, with an additional extension.
			rfilename = new char[strlen(filename) + 20];
			strcpy(rfilename, filename);
			strcat(rfilename, ".rsp");

			// Open the response file.  All messages received from the Session will be written to the response file.
			responseFile = new ofstream(rfilename);
			if (! *responseFile)
			{
				commandFile->close();
				delete commandFile;
				delete rfilename;
				return 3;
			}
		}
	}

	// Connect to the Session Manager, creating my Session.
	connectToSessionManager();
	if (!s)
	{
		if (*commandFile)
		{
			commandFile->close();
			delete commandFile;
		}
		if (*responseFile)
		{
			responseFile->close();
			delete responseFile;
		}
		if (rfilename != (char *)NULL)
			delete rfilename;

		return 1;
	}

	if (format == BrowserFormat)
		s->setMyReaderWriter(new CMReaderWriterEvolUI());
	else if (format == SMFormat)
		s->setMyReaderWriter(new CMReaderWriterEvolTag());
	else
		s->setMyReaderWriter(new CMReaderWriterEvolUI());
	messenger = new Messenger(this, s);

	hcn = HobString("#") + HobString(Integer::toString(connNum)) + HobString("# ");

	if (filename == (char *)NULL)
		return startNoFile();
	else
		return startFile();
}


void TestBrowser::setDelayAfterSend(int ticks)
{
	delayAfterSend = ticks;
}

void TestBrowser::connectToSessionManager()
{
	s = (Socket *)NULL;

	for (int i=0; i<5; i++)
	{
		s = new Socket();

		Configuration *config = Configuration::getInstance();
		if (config)
		{
			s->setPort(config->getWellKnownPort());
			HobIpAddress ipaddr = config->getIpAddress();
			HobString addr;
			ipaddr.getAsString(addr);
			s->setAddress(addr.c_str());
		}
		else
		{
			s->setPort(6000);
			s->setAddress("90.0.0.1");
		}

		Debug::trace("Browser", hcn + HobString("Will shortly try to connect to SessionManager ..."));
		taskDelay(connNum * 100);

		Debug::trace("Browser", hcn + HobString("Trying to connect to SessionManager at " + HobString(s->toString())));
		s->sConnect();
		if (s->isValid())
		{
			Debug::trace("Browser", hcn + HobString("Connected to SessionManager, mySocket is: " + HobString(s->toString())));
			break;
		}
		else
		{
			delete s;
			s = (Socket *)NULL;
		}
	}
}


int TestBrowser::startNoFile()
{
	char msg[1000];
	*msg = (char)NULL;
	string alphabet("abcdefghijklmnopqrstuvwxyz");

	{
		HobString smsg("9,1,");
		if (connNum == 1)
			smsg.append("1");
		else
			smsg.append("2");

		int bytesSent = messenger->send(smsg.c_str(), smsg.length(), Messenger::DEST_SESSION);
		if (bytesSent < 0)
		{
			Debug::trace("Browser", hcn + HobString("MESSAGE COULD NOT BE SENT"));
			strcpy(msg, "q");
		}
	}

	while (strcmp(msg, "q"))
	{
		SessionMessage *rmsg;
		if ((rmsg = messenger->receive()) != (SessionMessage *)NULL)
		{
			rcnt++;

			const HobString &buff = rmsg->getOriginalBuff();
			HobString h2 = hcn + HobString("recv: *") + buff + HobString("*");
			Debug::trace("Browser", h2);

//			handleMessage(rmsg);
			delete rmsg;
			continue;
		}
		else
		{
			Debug::trace("Browser.nomsg", hcn + "No messages for me.");
			taskDelay(1);
		}

		int chrs = rand() % 27;
//		int chrs = rand() % 100;

		if (chrs >= 0 && chrs < 27)
		{
			HobString smsg("100,100,");	//	SessionDev, SnTestMessage
			smsg.append(alphabet.substr(0, chrs));

			Debug::trace("Browser.send", hcn + HobString("send: *") + smsg + HobString("*"));

			int bytesSent = messenger->send(smsg.c_str(), smsg.length(), Messenger::DEST_SESSION);
			if (bytesSent < 0)
			{
				Debug::trace("Browser", hcn + HobString("MESSAGE COULD NOT BE SENT"));
				break;
			}
			scnt++;
		}

		//Debug::trace("Browser", "delaying ...");
		taskDelay(1000);
	}

	s->cmClose();
	return 0;
}


int TestBrowser::startFile()
{
	char msg[1000];
	*msg = (char)NULL;
	while (1)
	{
		/***********************************************************************
		 * Check for any messages received from my Session.
		 ***********************************************************************/

		SessionMessage *rmsg;
		if ((rmsg = messenger->receive()) != (SessionMessage *)NULL)
		{
			rcnt++;
/*
			const char *rm = rmsg->toString();
			int rml = rmsg->getSerializedLength();
			HobString h = hcn + HobString("Received message:  msgcnt=") +
								HobString(Integer::toString(rcnt)) +
								HobString("  msg=") + HobString(Hex::toString(rm, rml));
			Debug::trace("Browser", h);
*/
//			HfcObject *ho;
//			if ((ho = rmsg->getHfcObject()) != (HfcObject *)NULL)
//			{
//				const char *hod = ho->toString();
//				int len = ho->getSerializedLength();


				const HobString &buff = rmsg->getOriginalBuff();
				HobString h = hcn + HobString("recv: msgcnt=") + HobString(Integer::toString(rcnt)) +
									HobString("  msg=") + HobString(Hex::toString(buff.data(), buff.size()));
				Debug::trace("Browser", h);
				*responseFile << h << endl;

				HobString h2 = hcn + HobString("recv:      *") + buff + HobString("*");
				Debug::trace("Browser", h2);
				*responseFile << h2 << endl;

//			}

			handleMessage(rmsg);

			delete rmsg;
			continue;
		}
		else
		{
			Debug::trace("Browser.nomsg", hcn + "No messages for me.");
			taskDelay(1);
		}


		/***********************************************************************
		 * If we're supposed to quit, do so, after having checked that we
		 * haven't received any messages.
		 ***********************************************************************/
		if (!strcmp(msg, "q") && connNum != 1)
		{
			Debug::trace("Browser", hcn + "See ya!");
			*responseFile << hcn << "See ya!" << endl;
			break;
		}


		HobString smsg = "";
		memset(msg, 0, sizeof(msg));

		/***********************************************************************
		 * Send the next command from the input file.
		 ***********************************************************************/

//		if (!getline(*commandFile, smsg))
		if (!commandFile->getline(msg, sizeof(msg)))
		{
			strcpy(msg, "q");
			Debug::trace("Browser", hcn + "Finished processing command file ...");
			*responseFile << hcn << "Finished processing command file ..." << endl;
			taskDelay(200);
		}
		else
		{
			smsg = msg;

			/* The getline() method will only check for the newline, '\n'.
			 * It will return the carriage return character, '\r'.
			 * Therefore, if the last character read for this line is \r, ignore it.
			 */
			if (smsg[smsg.length()-1] == '\r')
				smsg.erase(smsg.length()-1);

			SessionMessage message;
			message.setFormatType(BrowserFormat);

			// If this line contains a comment, ignore everything from the '//' to the end of the line.
			string::size_type commentPos = smsg.find("//");
			if (commentPos != string::npos)
				smsg = smsg.substr(0, commentPos);

			// If nothing left on the input line, ignore the whole thing.
			if (smsg.length() > 0)
			{
/*
				string::size_type pos2 = (string::size_type)-1;
				string::size_type pos1 = smsg.find(",");  // Look for the first ',' on this line.

				// If no first ',', default the DeviceType and EventType.
				if (pos1 == string::npos)
				{
					message.setDeviceType(SessionDev);
					message.setEventType(SnTestMessage);
				}
				else
				{
					// Set the DeviceType.
					EvolDeviceType dev = (EvolDeviceType) Integer::valueOf(smsg.substr(0,pos1).c_str());
					message.setDeviceType(dev);

					// Look for the second ',' on this line.
					pos2 = smsg.find(",", pos1+1);
					// If no second ',', default the EventType.
					if (pos2 == string::npos)
						message.setEventType(SnTestMessage);
					else
					{
						// Set the EventType.
						int evt = Integer::valueOf(smsg.substr(pos1+1,pos2-pos1-1).c_str());
						message.setEventType(evt);
					}
				}

				strcpy(msg, smsg.c_str());
				Debug::trace("Browser.send", hcn + HobString("Sending message: ") + HobString(Hex::toString(msg,strlen(msg))));

				strcpy(msg, smsg.substr(pos2+1).c_str());
				HfcObject *o = HfcObject::create(UnknownDevice, 0, msg, strlen(msg), BrowserFormat);
				message.setHfcObject(o);

				if (Debug::isTracing("Browser.send"))
				{
					const char *msgdata = message.toString();
					int msglen = message.getSerializedLength();
					Debug::trace("Browser.send", hcn + HobString("Sending message: ") + HobString(Hex::toString(msgdata,msglen)));
				}

				// Send the message to my Session.
				int bytesSent = messenger->send(&message, Messenger::DEST_SESSION);
*/
				const char *b = smsg.c_str();
				int bl = smsg.length();

				HobString hod(b, bl);
				HobString h = hcn + HobString("send: *") + hod + HobString("*");
				*responseFile << h << endl;

				Debug::trace("Browser.send", h);

				int bytesSent = messenger->send(b, bl, Messenger::DEST_SESSION);
//				delete o;
				if (bytesSent < 0)
				{
					Debug::trace("Browser", hcn + HobString("MESSAGE COULD NOT BE SENT"));
					strcpy(msg, "q");
				}

				scnt++;

				// If this message is to delay the session, then also delay this "Browser".

				if (smsg.substr(0,5) == string("9,101"))
				{
					string dtime = smsg.substr(6);
					int delay = Integer::valueOf(dtime.c_str());
					taskDelay(delay);
				}
/*
				if (message.getDeviceType() == SessionDev && message.getEventType() == SnDelay)
				{
					HobTokenizerCSV tok(&hod, BrowserFormatSeparator, 2);
					HobString token;
					if (tok.next(&token) != -1 && tok.next(&token) != -1 && tok.next(&token) != -1)
					{
						if (token.length() > 0)
						{
							int delayTime = Integer::valueOf((char *)token.c_str());
							Debug::trace("Browser", hcn + HobString("Delaying ") + HobString(Integer::toString(delayTime)));
							taskDelay(Integer::valueOf(msg));
						}
					}
				}
*/

				taskDelay(delayAfterSend);

			}
		}
	}

	s->cmClose();
	commandFile->close();
	delete commandFile;
	*responseFile << endl;
	responseFile->close();
	delete responseFile;

	Debug::trace("Browser", hcn + "Exiting ...");
	return 0;
}

void TestBrowser::handleMessage(SessionMessage *msg)
{
	switch(msg->getDeviceType())
	{
		// cases from EvolDeviceType in event.h
		case Weigher:
		case WeigherSystem:
			handleWeigherMessage(msg);
		break;

		case LabelApplier:
			handleLabelApplierMessage(msg);
		break;

		case Printer:
		case PrinterSystem:
			handlePrinterMessage(msg);
		break;

		case ProductDev:
			handleProductDevMessage(msg);
		break;

		case TotalTypeDev:
			handleTotalTypeDevMessage(msg);
		break;

		case GraphicDev:
			handleGraphicDevMessage(msg);
		break;

		case ConfigurationDev:
			handleConfigurationDevMessage(msg);
		break;

		default:
			Debug::trace("Browser", HobString("I don't know what to do with this message! DeviceType = ") + HobString(Integer::toString(msg->getDeviceType())));
		break;
	}
}

void TestBrowser::handleWeigherMessage(SessionMessage *msg)
{
	switch(msg->getDeviceType())
	{
		case Weigher:
		{
			switch(msg->getEventType())
			{
				case WgInquire:
				{
					HfcWeigherConfig *weigherConfig = (HfcWeigherConfig *) msg->getHfcObject();
					int scaleFactor = weigherConfig->getScaleFactor();
					WgFilterSpeed filterSpeed = weigherConfig->getFilterSpeed();

				//	Switch scaleFactor from 1 to 5	OR from 5 to 1
					if(scaleFactor == 1)
					{
						weigherConfig->setScaleFactor(5);
					}
					else
					if(scaleFactor == 5)
					{
						weigherConfig->setScaleFactor(1);
					}

				//	Switch filterSpeed from Normal to Slow OR from Slow to Normal
					if(filterSpeed == NormalFilterSpeed)
					{
						weigherConfig->setFilterSpeed((WgFilterSpeed) SlowFilterSpeed);
					}
					else
					if(filterSpeed == SlowFilterSpeed)
					{
						weigherConfig->setFilterSpeed((WgFilterSpeed) NormalFilterSpeed);
					}
					scaleFactor = weigherConfig->getScaleFactor();
					filterSpeed = weigherConfig->getFilterSpeed();
		/*
				//	Switch maxScaleWeight from 30.01 to 25.09 OR from 25.09 to 30.01
					HobWeight maxScaleWeight = weigherConfig->getMaxScaleWeight();
					if(maxScaleWeight == (float) 30.01)
					{
						weigherConfig->setMaxScaleWeight(25.09);
					}
					else
					if(maxScaleWeight == (float) 25.09)
					{
						weigherConfig->setMaxScaleWeight(30.01);
					}
					maxScaleWeight = weigherConfig->getMaxScaleWeight();

				//	Switch minWeightToPrint from 0.05 to 0.01 OR from 0.01 to 0.05
					HobWeight minWeightToPrint = weigherConfig->getMinWeightToPrint();
					if(minWeightToPrint == (float) 0.05)
					{
						weigherConfig->setMinWeightToPrint(0.01);
					}
					else
					if(minWeightToPrint == (float) 0.01)
					{
						weigherConfig->setMinWeightToPrint(0.05);
					}
					minWeightToPrint = weigherConfig->getMinWeightToPrint();
		*/
				//	Change the event type to weigher config
					weigherConfig->setEventType(WgConfig);

					const char *msgdata = weigherConfig->serialize(BrowserFormat);
					int msglen = weigherConfig->HfcObject::getSerializedLength();

					HobString hod(msgdata, msglen);
					HobString h = hcn + HobString("send: *") + hod + HobString("*");
					*responseFile << h << endl;

					Debug::trace("Browser.send", h);
				//	And send the message back to the Weigher
					int bytesSent = messenger->send(msgdata, msglen, Messenger::DEST_SESSION);
					if (bytesSent < 0)
					{
					}
				}
				break;

				case WgInfo:
				{
					WeigherInfo *weigherInfo = (WeigherInfo *) msg->getHfcObject();
					DeviceAddress weigherAddress = weigherInfo->getStationAddress();

					if(weigherAddress == 0x3333)
						weigherAddress = 0x7777;
					else
					if(weigherAddress == 0x7777)
						weigherAddress = 0x8888;
					else
						weigherAddress = 0x1111;
					weigherInfo->setStationAddress(weigherAddress);

					HobString msgdata("1,104,");		//	Weigher, WgSaveInfo
					msgdata.append(weigherInfo->serialize(BrowserFormat));
					int msglen = msgdata.length();

					HobString hod(msgdata.c_str(), msglen);
					HobString h = hcn + HobString("send: *") + hod + HobString("*");
					*responseFile << h << endl;

					Debug::trace("Browser.send", h);
				//	And send the message back to the Configuration manager
					int bytesSent = messenger->send(msgdata.c_str(), msglen, Messenger::DEST_SESSION);
					if (bytesSent < 0)
					{
					}
				}
				break;

				default:
				break;
			}
		}
			break;

		case WeigherSystem:
		{
			switch(msg->getEventType())
			{
				case SysStatus:
				{
					Debug::trace("Browser", " Received HfcWeigherSystemStatus message -  WeigherSystemStatusInfo:");
					HfcWeigherSystemStatus *weigherSystemStatus = (HfcWeigherSystemStatus *) msg->getHfcObject();
					const SystemStatusInfo *wgSystemStatusInfo = weigherSystemStatus->getSystemStatusInfo();
					if(weigherSystemStatus)
					{
						HobString data = "    ROM_Checksum= 0x0" + HobString(Hex::toString(wgSystemStatusInfo->getROM_Checksum())) +
										 ", systemChecksum= 0x0" + HobString(Hex::toString(wgSystemStatusInfo->getSystemChecksum())) +
										 "\n           addressError= " + HobString(Bool::toString(wgSystemStatusInfo->isAddressError())) +
										 ", addressErrorAddress= 0x0" + HobString(Hex::toString(wgSystemStatusInfo->getAddressTestAddress())) +
										 "\n           bitError= " + HobString(Bool::toString(wgSystemStatusInfo->isBitError())) +
										 ", bitErrorAddress= 0x0" + HobString(Hex::toString(wgSystemStatusInfo->getBitTestAddress())) +
										 "\n           eepromError= " + HobString(Bool::toString(wgSystemStatusInfo->isEepromError()));
						Debug::trace("Browser", data);
					}
				}
					break;
			}
		}
			break;

		default:
			break;
	}
}

void TestBrowser::handleLabelApplierMessage(SessionMessage *msg)
{
	switch(msg->getEventType())
	{
		case LaInquire:
		{
			HfcLabelApplierConfig *labelApplierConfig = (HfcLabelApplierConfig *) msg->getHfcObject();
			bool secondaryPrinterAttached = labelApplierConfig->isSecondaryStationAttached();

			if(secondaryPrinterAttached)
			{
				 labelApplierConfig->setSecondaryStationAttached(false);
			}
			else
			{
				 labelApplierConfig->setSecondaryStationAttached(true);
			}
			secondaryPrinterAttached = labelApplierConfig->isSecondaryStationAttached();

		//	Change the event type to label applier config
			labelApplierConfig->setEventType(LaConfig);

			const char *msgdata = labelApplierConfig->serialize(BrowserFormat);
			int msglen = labelApplierConfig->HfcObject::getSerializedLength();

			HobString hod(msgdata, msglen);
			HobString h = hcn + HobString("send: *") + hod + HobString("*");
			*responseFile << h << endl;

			Debug::trace("Browser.send", h);
		//	And send the message back to the Weigher
			int bytesSent = messenger->send(msgdata, msglen, Messenger::DEST_SESSION);
			if (bytesSent < 0)
			{
			}
//					Debug::trace("Browser", "- Received a LabelApplierInquire message");
		}
		break;

		default:
		break;
	}
}

void TestBrowser::handlePrinterMessage(SessionMessage *msg)
{
	switch(msg->getDeviceType())
	{
		case Printer:
		{
			switch(msg->getEventType())
			{
				case PrInquire:
				{
					HfcPrinterConfig *printerConfig = (HfcPrinterConfig *) msg->getHfcObject();
					LabelWidth labelWidth = printerConfig->getLabelWidth();
					MediaSensorType mediaSensorType = printerConfig->getMediaSensorType();

					if(labelWidth == WideLabel)
					{
						 printerConfig->setLabelWidth(NarrowLabel);
					}
					else
					{
						 printerConfig->setLabelWidth(WideLabel);
					}
					labelWidth = printerConfig->getLabelWidth();

					if(mediaSensorType == PrShootThrough)
					{
						 printerConfig->setMediaSensorType(PrReflective);
					}
					else
					{
						 printerConfig->setMediaSensorType(PrShootThrough);
					}
					mediaSensorType = printerConfig->getMediaSensorType();


				//	Change the event type to printer config
					printerConfig->setEventType(PrConfig);

					const char *msgdata = printerConfig->serialize(BrowserFormat);
					int msglen = printerConfig->HfcObject::getSerializedLength();

					HobString hod(msgdata, msglen);
					HobString h = hcn + HobString("send: *") + hod + HobString("*");
					*responseFile << h << endl;

					Debug::trace("Browser.send", h);
				//	And send the message back to the Printer
					int bytesSent = messenger->send(msgdata, msglen, Messenger::DEST_SESSION);
					if (bytesSent < 0)
					{
					}
		//			Debug::trace("Browser", "- Received a PrinterInquire message");
				}
				break;

				case PrInfo:
				{
					PrinterInfo *printerInfo = (PrinterInfo *) msg->getHfcObject();
					DeviceAddress printerAddress = printerInfo->getStationAddress();

					if(printerAddress == 0x2222)
						printerAddress = 0x2323;
					else
					if(printerAddress == 0x4444)
						printerAddress = 0x4545;
					else
					if(printerAddress == 0x6666)
						printerAddress = 0x6767;
					printerInfo->setStationAddress(printerAddress);

					HobString msgdata("7,102,");		//	Printer, PrSaveInfo
					msgdata.append(printerInfo->serialize(BrowserFormat));
					int msglen = msgdata.length();

					HobString hod(msgdata.c_str(), msglen);
					HobString h = hcn + HobString("send: *") + hod + HobString("*");
					*responseFile << h << endl;

					Debug::trace("Browser.send", h);
				//	And send the message back to the Configuration manager
					int bytesSent = messenger->send(msgdata.c_str(), msglen, Messenger::DEST_SESSION);
					if (bytesSent < 0)
					{
					}
				}
				break;

				case PrSensors:
				{
					Debug::trace("Browser", " Received HfcPrinterSensors message");
				}
				break;

				default:
				break;
			}
		}
			break;

		case PrinterSystem:
		{
			switch(msg->getEventType())
			{
				case SysStatus:
				{
					Debug::trace("Browser", " Received HfcPrinterSystemStatus message -  PrinterSystemStatusInfo:");
					HfcPrinterSystemStatus *printerSystemStatus = (HfcPrinterSystemStatus *) msg->getHfcObject();
					const SystemStatusInfo *prSystemStatusInfo = printerSystemStatus->getSystemStatusInfo();
					if(printerSystemStatus)
					{
						HobString data = "    ROM_Checksum= 0x0" + HobString(Hex::toString(prSystemStatusInfo->getROM_Checksum())) +
										 ", systemChecksum= 0x0" + HobString(Hex::toString(prSystemStatusInfo->getSystemChecksum())) +
										 "\n           addressError= " + HobString(Bool::toString(prSystemStatusInfo->isAddressError())) +
										 ", addressErrorAddress= 0x0" + HobString(Hex::toString(prSystemStatusInfo->getAddressTestAddress())) +
										 "\n           bitError= " + HobString(Bool::toString(prSystemStatusInfo->isBitError())) +
										 ", bitErrorAddress= 0x0" + HobString(Hex::toString(prSystemStatusInfo->getBitTestAddress())) +
										 "\n           eepromError= " + HobString(Bool::toString(prSystemStatusInfo->isEepromError())) +
										 "\n           Printer type = " + HobString(Integer::toString(printerSystemStatus->getPrinterType()));
						Debug::trace("Browser", data);
					}
				}
					break;
			}
		}

		default:
			break;
	}
}

void TestBrowser::handleProductDevMessage(SessionMessage *msg)
{
	switch(msg->getEventType())
	{
		case ProductWrite:
		{
			Product *tempProduct = (Product *) msg->getHfcObject();
			if(tempProduct)
			{
				//	Modify the classNumber so we can test it on the other end.
				HobRecordNum classNumber = tempProduct->getReportingClassNum();
				if(classNumber == 100)
					classNumber = 200;
				else if(classNumber == 200)
					classNumber = 300;
				else if(classNumber == 300)
					classNumber = 400;
				else if(classNumber == 400)
					classNumber = 500;
				else if(classNumber == 500)
					classNumber = 600;
				else
					classNumber = 100;
				tempProduct->setReportingClassNum(classNumber);

				HobString graphicName;

				Graphic *graphic2 = tempProduct->getGraphic(1);
				if(graphic2 == NULL)
				{
					 graphic2 = new Graphic();
					 tempProduct->setGraphic(1, graphic2);
				}
				graphicName = graphic2->getGraphicName();
				graphic2->setGraphicName("BadFileName");

				Graphic *graphic3 = tempProduct->getGraphic(2);
				if(graphic3 == NULL)
				{
					 graphic3 = new Graphic();
					 tempProduct->setGraphic(2, graphic3);
				}
				graphicName = graphic3->getGraphicName();
				graphic3->setGraphicName("stPatricks");

				HobString msgdata("101,7,");		//	ProductDev, ProductWrite
				msgdata.append(tempProduct->serialize(BrowserFormat));
				int msglen = msgdata.length();

				HobString hod(msgdata.c_str(), msglen);
				HobString h = hcn + HobString("send: *") + hod + HobString("*");
				*responseFile << h << endl;

				Debug::trace("Browser.send", h);
			//	And send the message back to the ProductManager
				int bytesSent = messenger->send(msgdata.c_str(), msglen, Messenger::DEST_SESSION);
				if (bytesSent < 0)
				{
				}
#if 1
			//	Make it save the PLU data so we can test this section.
				msgdata = HobString("101,22");		//	ProductDev, ProductSave
				msglen = msgdata.length();

				hod = HobString(msgdata.c_str(), msglen);
				h = hcn + HobString("send: *") + hod + HobString("*");
				*responseFile << h << endl;

				Debug::trace("Browser.send", h);
				//	And send the message back to the ProductManager
				bytesSent = messenger->send(msgdata.c_str(), msglen, Messenger::DEST_SESSION);
				if (bytesSent < 0)
				{
				}
#endif
#if 0
			//	Read another PLU record.
				msgdata = HobString("101,5,24");		//	ProductDev, ProductRead, #24
				msglen = msgdata.length();

				hod = HobString(msgdata.c_str(), msglen);
				h = hcn + HobString("send: *") + hod + HobString("*");
				*responseFile << h << endl;

				Debug::trace("Browser.send", h);
				//	And send the message back to the ProductManager
				bytesSent = messenger->send(msgdata.c_str(), msglen, Messenger::DEST_SESSION);
				if (bytesSent < 0)
				{
				}
#endif
			}
		}
		break;

		case ProductWriteSupportData:
		{
			HfcProductSupportDataMsg *productSupportMsg = (HfcProductSupportDataMsg *) msg->getHfcObject();
			if(productSupportMsg)
			{
				//	Modify the text by placing a '$' at the beginning of the text.
				HobString text = productSupportMsg->getText();
				if(text.empty())
					text = HobString("$");
				else
					text.insert(0, "$");
				productSupportMsg->setText(text);

				HobString msgdata("101,10,");		//	ProductDev, ProductWriteSupportData
				msgdata.append(productSupportMsg->serialize(BrowserFormat));
				int msglen = msgdata.length();

				HobString hod(msgdata.c_str(), msglen);
				HobString h = hcn + HobString("send: *") + hod + HobString("*");
				*responseFile << h << endl;

				Debug::trace("Browser.send", h);
				//	And send the message back to the ProductManager
				int bytesSent = messenger->send(msgdata.c_str(), msglen, Messenger::DEST_SESSION);
				if (bytesSent < 0)
				{
				}

#if 1
		//	Make it save the PLU data so we can test this section.
				msgdata = HobString("101,22,");		//	ProductDev, ProductSave
				msglen = msgdata.length();

				hod = HobString(msgdata.c_str(), msglen);
				h = hcn + HobString("send: *") + hod + HobString("*");
				*responseFile << h << endl;

				Debug::trace("Browser.send", h);
				//	And send the message back to the ProductManager
				bytesSent = messenger->send(msgdata.c_str(), msglen, Messenger::DEST_SESSION);
				if (bytesSent < 0)
				{
				}
#endif
			}
		}
		break;

		default:
		break;
	}
}

void TestBrowser::handleTotalTypeDevMessage(SessionMessage *msg)
{
	switch(msg->getEventType())
	{
		case DtWrite:
		{
			TotalType *tempTotalType = (TotalType *) msg->getHfcObject();
			if(tempTotalType->isReportsToPrepackTotals())
				tempTotalType->setReportsToPrepackTotals(false);
			else
				tempTotalType->setReportsToPrepackTotals(true);

			HobString msgdata("104,2,");		//	TotalTypeDev, TotalTypeWrite
			msgdata.append(tempTotalType->serialize(BrowserFormat));
			int msglen = msgdata.length();

			HobString hod(msgdata.c_str(), msglen);
			HobString h = hcn + HobString("send: *") + hod + HobString("*");
			*responseFile << h << endl;

			Debug::trace("Browser.send", h);
		//	And send the message back to the TotalTypeManager
			int bytesSent = messenger->send(msgdata.c_str(), msglen, Messenger::DEST_SESSION);
			if (bytesSent < 0)
			{
			}
		}
		break;

		default:
		break;
	}
}

void TestBrowser::handleGraphicDevMessage(SessionMessage *msg)
{
	switch(msg->getEventType())
	{
		case GraphicReadAllNames:
		{
			HobString h;
			GraphicFileNames *graphicFiles = (GraphicFileNames *) msg->getHfcObject();
			long recordCount = graphicFiles->getListCount();

			Debug::trace("Browser", "Received Graphic File Names:");

			h = HobString("    Record count = ") + HobString(Long::toString(recordCount));
			Debug::trace("Browser", h);

			std::list<HobString> tbFileNames = graphicFiles->getMyList();
			list <HobString>::const_iterator iter = tbFileNames.begin();

			int index = Configuration::getInstance()->getScaleId();
			HobString readGraphicName("");
			for(long i = 1; i <= recordCount; i++)
			{
				if(iter != tbFileNames.end())
				{
					if(index == i)
					{
						readGraphicName = *iter;
					}
					h = HobString("    ") + HobString(Long::toString(i)) + HobString(")  ") + *iter;
					Debug::trace("Browser", h);
					++iter;
				}
			}
			//	Now send a graphic back to be read
			HobString msgdata("105,1,1,");		//	GraphicDev, GraphicRead, Graphic #1
			msgdata.append(readGraphicName);
			int msglen = msgdata.length();

			HobString hod(msgdata.c_str(), msglen);
			h = hcn + HobString("send: *") + hod + HobString("*");
			*responseFile << h << endl;

			Debug::trace("Browser.send", h);
		//	And send the message back to the ProductManager
			int bytesSent = messenger->send(msgdata.c_str(), msglen, Messenger::DEST_SESSION);
			if (bytesSent < 0)
			{
			}
		}
		break;

		default:
		break;
	}
}


void TestBrowser::handleConfigurationDevMessage(SessionMessage *msg)
{
	switch(msg->getEventType())
	{
		case CfgFrequentShopperWrite:
		{
			CfgFrequentShopper *frequentShopperSetup = (CfgFrequentShopper *) msg->getHfcObject();
			if(frequentShopperSetup)
			{
//				int discountType = frequentShopperSetup->getDefaultDiscountType();
				int discountType = Configuration::getInstance()->getDefaultDiscountType();
				if(discountType == Cents_off_unit_price)
					discountType = Discount_price_by_weight;
				else
				if(discountType == Discount_price_by_weight)
					discountType = Percent_off_net_value;
				else
				if(discountType == Percent_off_net_value)
					discountType = Special_price;
				else
				if(discountType == Special_price)
					discountType = Percent_added_net_value;
				else
				if(discountType == Percent_added_net_value)
					discountType = Discount_per_package;
				else
					discountType = Cents_off_unit_price;
//				frequentShopperSetup->setDefaultDiscountType(discountType);
				Configuration::getInstance()->setDefaultDiscountType(discountType);

				HobString msgdata("112,41,");		//	ConfigurationDev, CfgFrequentShopperWrite
				msgdata.append(frequentShopperSetup->serialize(BrowserFormat));
				int msglen = msgdata.length();

//				HobString hod(msgdata.c_str(), msglen);
				HobString hod(msgdata.c_str());
				msglen = hod.length();
				HobString h = hcn + HobString("send: *") + hod + HobString("*");
				*responseFile << h << endl;

				Debug::trace("Browser.send", h);
			//	And send the message back to the ConfigurationManager
				int bytesSent = messenger->send(msgdata.c_str(), msglen, Messenger::DEST_SESSION);
				if (bytesSent < 0)
				{
				}
			}
		}
		break;

		default:
		break;
	}
}
