/* COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */


#include <fstream>


#include "Graphic.h"
#include "Configuration.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"
#include "HobToken.h"
#include "filesys/directory.h"

#include "Long.h"
#include "Hex.h"
#include "Integer.h"
#include "Bool.h"
#include "util/nibble/nibble.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int Graphic::dataType = 0;
std::string Graphic::dataTypeName("Graphic"); 
#endif


using namespace std;


std::list<ResourceTagsFieldOffsets> Graphic::fieldList;


Graphic::Graphic()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

Graphic::Graphic(const Graphic &g)
    : HfcObject()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	index = g.getIndex();
	type = g.getType();
	graphicFile.setFullPath(g.getGraphicFile().getFullPath());
	errorCode = g.getErrorCode();
	//graphicImage = g.getGraphicImage();
	loaded = false; // = g.isLoaded();
}

Graphic::Graphic(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	switch (formatType)
	{
		case BrowserFormat:
		{
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetName:
						token.unQuoteForCSV();
						setGraphicName(token);
						break;
					default:
						break;
				}
				loopCnt++;
			}
			break;
		}

/*		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		{
			const char * pBegin = data;
			int i = 0, j = 0;

			try
			{
				// Get past the RT header
				for (; ((i < len) && (data[i] != 0x1F)); i++);

				if (i != len)
				{
					if (strncmp(pBegin, "FN", 2) == 0)//ResourceTags::resourceName, 2) == 0)
					{
						HobString fileName(pBegin + 2, i - 2);
						setGraphicName(fileName);

						i++;
						pBegin = &data[i];


						if (strncmp(pBegin, "rs", 2) == 0)//ResourceTags::resourceResourceData, 2) == 0)
						{
							i += 2;
							pBegin = &data[i];

							char tmp[3];
							unsigned int val = 0;
							int sizeCntr = 0;
							char * pDest = (char *)data;

							
							for ( j = i; ((i < len) && (data[i] != 0x1E)); sizeCntr++, j++, i+=2)
							{
								strncpy(tmp, &data[i], 2);
								tmp[2] = 0;
								sscanf(tmp, "%02X", (int*)&val);
								pDest[j] = val;
							}

							if (sizeCntr > 0)
							{
								setPngBinaryImage(pBegin, sizeCntr);
							} 

						}


					}
				}
			}
			catch (...)
			{
				Debug::trace("Graphic", "  *****************************************************************");
				Debug::trace("Graphic", "  ****************   Just got an unknown exception  ***************");
				Debug::trace("Graphic", "  ****************  Graphic(data, len, formatType)  ***************");
				Debug::trace("Graphic", "  *****************************************************************");
			}


			break;
		}
*/

		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		{
			try
			{
				ResourceTags graphicTags;
				HobString evolData(data, len);                       //convert data to HobString
				if (Debug::isTracing("Graphic"))
				{
					Debug::trace("Graphic", "len: " + util_string::inttostring(len));
					Debug::trace("Graphic", "evolData.size(): " + util_string::inttostring(evolData.size()));
				}

				HobTokenizerUS tok(&evolData, UnitSeparator);        //use the UnitSeparator to parse the string
				HobString temp;
				int offset = 0;
				HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp,HobTaggedToken));
				if ( token )
				{
					while ( tok.next(token) != -1 )
					{
						//get the tag and find out what offset goes with the tag
						temp = token->getTag();
						offset = temp.size() ? graphicTags.getOffset(temp) : 0;
						if ( offset )
						{
							setReceived(offset, true);

							switch (offset)
							{
								case fieldOffsetName:
									setGraphicName(token->getData());
									break;

								case fieldOffsetResourceData:
								{
									std::string nibblized = token->getData();
									std::string unnibblized;

									Debug::trace("Graphic", "nibbData.size(): " + util_string::inttostring(nibblized.size()));
									//Debug::trace("Graphic", HobString("nibbData: ") + Hex::toString(nibblized.c_str(), nibblized.size()));

									util_nibble::unnibblize(nibblized, unnibblized, nibblized.size() / 2);

									Debug::trace("Graphic", "unnibblized.size(): " + util_string::inttostring(unnibblized.size()));
									//Debug::trace("Graphic", HobString("unnibblized: ") + Hex::toString(unnibblized.c_str(), unnibblized.size()));

									setPngBinaryImage(unnibblized);
									break;
								}

								default:
									break;
							}
						}
					}

					delete token;
				}
			} 
			catch (...)
			{
				Debug::trace("Graphic", "  *****************************************************************");
				Debug::trace("Graphic", "  ****************  Just got an unknown exception   ***************");
				Debug::trace("Graphic", "  ****************  Graphic(data, len, formatType)  ***************");
				Debug::trace("Graphic", "  *****************************************************************");
			}

				break;	
		}	 
		default:
			// todo - throw exception("invalid format");
			break;
	}
}

Graphic::Graphic(const std::string &graphicName, short graphicType, Filename *filePathInfo)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("Graphic", HobString("constructor(name,type) - name:") + graphicName + HobString("  type:") + HobString(Integer::toString(graphicType)));

	init();
	setGraphicName(graphicName);
	setType(graphicType, filePathInfo);

	if (type == TYPE_USERLABELGRAPHIC || type == TYPE_ROMLABELGRAPHIC)
		load();
	else if (exists())
		setErrorCode(RECORD_OK);
}


Graphic::~Graphic()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	//if (graphicImage)
	//	delete graphicImage;

	// SOF - 4019 Big Y Self - Serve: Store logo rendering seems to fail frequently on label - tied back to SOF-3335 Printing noise for store graphic
	if (graphicImage)
		graphicImageToDelete.push_back(graphicImage);

	while (!graphicImageToDelete.empty())
	{
		delete graphicImageToDelete.front();
		graphicImageToDelete.pop_front();
	}

	Debug::trace("Delete.Graphic", HobString(" Graphic: ") + HobString(Hex::toString((long long) this)) + HobString("  Graphic name:") + getGraphicName());
}


void Graphic::init()
{
	index = 0;
	type = TYPE_UNKNOWN;
	errorCode = RECORD_NOT_FOUND;
	graphicImage = NULL;
	loaded = false;

	initFieldList();
}


// static
void Graphic::initFieldList()
{
	if (fieldList.empty())
	{
		fieldList.push_back(fieldOffsetName);
		fieldList.push_back(fieldOffsetResourceData);

		ResourceTags graphicTags((FieldOffsetsList *)&fieldList);
	}
}


void Graphic::unload()		//	HTA-15
{
	Debug::trace("Graphic", HobString("unload()"));

	errorCode = RECORD_NOT_FOUND;
	loaded = false;
	if (graphicImage)
	{
		//delete graphicImage;
		// SOF - 4019 Big Y Self - Serve: Store logo rendering seems to fail frequently on label - tied back to SOF-3335 Printing noise for store graphic
		graphicImageToDelete.push_back(graphicImage);
		graphicImage = (GraphicImage *)NULL;
	}
}



/* NOTE:  You must call setGraphicName(), and setType() - to complete the file's full path - before calling load().
 */
void Graphic::load()
{
	Debug::trace("Graphic", HobString("load - will attempt to load: ") + graphicFile.getFullPath());

	unload();	//	Unload the current graphic

	if (!graphicFile.getFullPath().empty())
	{
		//	We know now where the file is and its extension. Load it.
		// SOF - 4019 Big Y Self - Serve: Store logo rendering seems to fail frequently on label - tied back to SOF-3335 Printing noise for store graphic
		//GraphicImage *graphicImage = new GraphicImage(graphicFile.getFullPath());
		//if (graphicImage)
		GraphicImage *g = new GraphicImage(graphicFile.getFullPath());
		if (g)
		{
			if (g->isValid())
			{
				setGraphicImage(g);
				setErrorCode(RECORD_OK);
				Debug::trace("Graphic", "load - loaded graphic");
				loaded = true;
			}
			else
			{
				//	We were not able to open this file for some reason.
				setErrorCode(g->getPingToHobStatus());
				delete graphicImage;
				//graphicImage = (GraphicImage *)NULL;
				Debug::trace("Graphic", HobString("load - DID NOT load graphic - errorcode:") + HobString(Integer::toString(errorCode)));
			}
		}
		else
			Debug::trace("Graphic", "load - DID NOT load graphicImage");
	}
}


short Graphic::getIndex() const
{
	return index;
}
void Graphic::setIndex(short index)
{
	this->index = index;
}

short Graphic::getType() const
{
	return type;
}
void Graphic::setType(short newType, Filename *filePathInfo)
{
	Configuration *config = Configuration::getInstance();

	Filename	tempFilename;
	if (type != newType)
	{
		bool determinedType = true;
		Debug::trace("Graphic", HobString("setType - setting to type:") + HobString(Integer::toString(newType)));
		switch (newType)
		{
			case TYPE_LABELGRAPHIC:
				if (!locateLabelGraphic())
				{
					type = TYPE_UNKNOWN;
					determinedType = false;
				}
				else
					// locating the label graphic found the graphic and set the type, so prepare to use the type it found.
					newType = type;
				break;

			case TYPE_USERLABELGRAPHIC:
				if (graphicFile.getFileNameExtension().empty())
					graphicFile.setFileNameExtension(PNG_FILE_EXTENSION);
				graphicFile.setPath(config->toStringCfgI(CfgInternal::USERGRAPHICSLOCATION));
				break;

			case TYPE_ROMLABELGRAPHIC:
				if (graphicFile.getFileNameExtension().empty())
					graphicFile.setFileNameExtension(PNG_FILE_EXTENSION);
				graphicFile.setPath(config->toStringCfgI(CfgInternal::SYSTEMGRAPHICSLOCATION));
				break;

			case TYPE_PRODUCTGRAPHIC:
				if (graphicFile.getFileNameExtension().empty())
					graphicFile.setFileNameExtension(config->toStringCfgI(CfgInternal::PRODUCTGRAPHICSDEFAULTEXTENSION));
				if (filePathInfo)
					graphicFile.setPath(filePathInfo->getPath());
				else
				{
					tempFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
					tempFilename.appendPath(config->toStringCfgI(CfgInternal::PRODUCTGRAPHICSDIR));
					graphicFile.setPath(tempFilename.getPath());
				}
				break;

			case TYPE_CATEGORYGRAPHIC:
				if (graphicFile.getFileNameExtension().empty())
					graphicFile.setFileNameExtension(config->toStringCfgI(CfgInternal::CATEGORYGRAPHICSDEFAULTEXTENSION));
				if (filePathInfo)
					graphicFile.setPath(filePathInfo->getPath());
				else
				{
					tempFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
					tempFilename.appendPath(config->toStringCfgI(CfgInternal::CATEGORYGRAPHICSDIR));
					graphicFile.setPath(tempFilename.getPath());
				}
				break;

			case TYPE_SPECIALSGRAPHIC:
				if (graphicFile.getFileNameExtension().empty())
					graphicFile.setFileNameExtension(config->toStringCfgI(CfgInternal::SPECIALSGRAPHICSDEFAULTEXTENSION));
				if (filePathInfo)
					graphicFile.setPath(filePathInfo->getPath());
				else
				{
					tempFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
					tempFilename.appendPath(config->toStringCfgI(CfgInternal::SPECIALSGRAPHICSDIR));
					graphicFile.setPath(tempFilename.getPath());
				}
				break;

			case TYPE_STREAMINGGRAPHIC:
				if (graphicFile.getFileNameExtension().empty())
					graphicFile.setFileNameExtension(config->toStringCfgI(CfgInternal::STREAMINGGRAPHICSDEFAULTEXTENSION));
				if (filePathInfo)
					graphicFile.setPath(filePathInfo->getPath());
				else
				{
					tempFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
					tempFilename.appendPath(config->toStringCfgI(CfgInternal::STREAMINGGRAPHICSDIR));
					graphicFile.setPath(tempFilename.getPath());
				}
				break;

			case TYPE_SOFTKEYGRAPHIC:
				if (graphicFile.getFileNameExtension().empty())
					graphicFile.setFileNameExtension(config->toStringCfgI(CfgInternal::SOFTKEYGRAPHICSDEFAULTEXTENSION));
				if (filePathInfo)
					graphicFile.setPath(filePathInfo->getPath());
				else
				{
					tempFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
					tempFilename.appendPath(config->toStringCfgI(CfgInternal::SOFTKEYGRAPHICSDIR));
					graphicFile.setPath(tempFilename.getPath());
				}
				break;

			case TYPE_PLANOGRAMGRAPHIC:
				if (graphicFile.getFileNameExtension().empty())
					graphicFile.setFileNameExtension(config->toStringCfgI(CfgInternal::PLANOGRAMGRAPHICSDEFAULTEXTENSION));
				if (filePathInfo)
					graphicFile.setPath(filePathInfo->getPath());
				else
				{
					tempFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
					tempFilename.appendPath(config->toStringCfgI(CfgInternal::PLANOGRAMGRAPHICSDIR));
					graphicFile.setPath(tempFilename.getPath());
				}
				break;

			case TYPE_VIDEOS:
				if (graphicFile.getFileNameExtension().empty())
					graphicFile.setFileNameExtension(config->toStringCfgI(CfgInternal::VIDEOSDEFAULTEXTENSION));
				if (filePathInfo)
					graphicFile.setPath(filePathInfo->getPath());
				else
				{
					tempFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
					tempFilename.appendPath(config->toStringCfgI(CfgInternal::VIDEOSDIR));
					graphicFile.setPath(tempFilename.getPath());
				}
				break;

			default:
				determinedType = false;
				break;
		}

		Debug::trace("Graphic", HobString("setType - graphicFile fullPath: ") + graphicFile.getFullPath().c_str());

		if (determinedType)
		{
			type = newType;
			Debug::trace("Graphic", HobString("setType - determined type - type:") + HobString(Integer::toString(type)));
		}
		else
		{
			if (graphicImage)
			{
				// SOF - 4019 Big Y Self - Serve: Store logo rendering seems to fail frequently on label - tied back to SOF-3335 Printing noise for store graphic
				//delete graphicImage;
				graphicImageToDelete.push_back(graphicImage);
				graphicImage = (GraphicImage *)NULL;
			}
			loaded = false;
			errorCode = RECORD_NOT_FOUND;
		}
	}
}

const File &Graphic::getGraphicFile() const
{
	return graphicFile;
}
const std::string &Graphic::getGraphicNameBase() const
{
	return graphicFile.getFileNameBase();
}

const std::string &Graphic::getGraphicName() const
{
	return graphicFile.getFileNameBase();
}
void Graphic::setGraphicName(const std::string &name)
{
	//	Check to see if the filename contains an extension; if so, use it.
	string::size_type extPos = name.find_last_of(".");
	if (extPos != string::npos)
	{
		graphicFile.setFileName(name);
	}
	else
	{
		graphicFile.setFileNameBase(name);
		graphicFile.setFileNameExtension("");
	}
}


HobErrorCode Graphic::getErrorCode() const
{
	return errorCode;
}
void Graphic::setErrorCode(HobErrorCode errorCode)
{
	this->errorCode = errorCode;
}


GraphicImage *Graphic::getGraphicImage()
{
	return graphicImage;
}
void Graphic::setGraphicImage(GraphicImage *graphicImage)
{
	this->graphicImage = graphicImage;
}
void Graphic::setPngBinaryImage(const std::string &image)
{
	if (graphicImage)
	{
		// SOF - 4019 Big Y Self - Serve: Store logo rendering seems to fail frequently on label - tied back to SOF-3335 Printing noise for store graphic
		//delete graphicImage;
		graphicImageToDelete.push_back(graphicImage);
		graphicImage = (GraphicImage *)NULL;
	}
	graphicImage = new GraphicImage();
	graphicImage->setBinaryImage(image.data(), (int)image.size());
}

void Graphic::setPngBinaryImage(const char *data, int size)
{
	if (graphicImage)
	{
		// SOF - 4019 Big Y Self - Serve: Store logo rendering seems to fail frequently on label - tied back to SOF-3335 Printing noise for store graphic
		//delete graphicImage;
		graphicImageToDelete.push_back(graphicImage);
		graphicImage = (GraphicImage *)NULL;
	}
	graphicImage = new GraphicImage();
	graphicImage->setBinaryImage(data, size);
}


bool Graphic::exists() const
{
	return (graphicFile.getFullPath().empty()) ? false : graphicFile.exists();
}


bool Graphic::isLoaded() const
{
	return loaded;
}


bool Graphic::isCustom() const	// is this a custom graphic?
{
	return !isRom();
}
bool Graphic::isRom() const	// is this a ROM graphic?
{
	return (type == TYPE_UNKNOWN || type == TYPE_ROMLABELGRAPHIC) ? true : false;
}


int Graphic::readImageFromFile(HobString &data) const
{
	if (!exists())
		return -1;

	int ret = 0;

    // Alamy: Use another variable name to prevent confliction with member variable
	ifstream readGraphicFile(graphicFile.getFullPath().c_str(), ios::in | ios::binary);
	if (readGraphicFile.is_open())
	{
		data = "";
		char buff[2048];
		while (!readGraphicFile.eof())
		{
			readGraphicFile.read(buff, 2048);
			int bytesread = readGraphicFile.gcount();
			data.append((char *)buff, bytesread);
		}
	}
	else
		ret = -1;

	readGraphicFile.close();

	return ret;
}


const char *Graphic::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	switch (format)
	{
		case BrowserFormat:
			for (int loopCnt = 1; loopCnt < endResourceTagsFieldOffsets; loopCnt++)
			{
				if (loopCnt > 1)
					serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetName:
						serializedData.append(getGraphicNameBase());
						break;

					case fieldOffsetIsCustom:
						serializedData.append(Bool::toString(isCustom()));
						break;

					case fieldOffsetResourceData:
					{
						string nibb = " ";
						HobString graphicData;
						if (readImageFromFile(graphicData) >= 0)
						{

							serializedData.reserve(serializedData.size() + (graphicData.size() * 2) + 1);
							Hex::nibblize(serializedData, graphicData.data(), (int)graphicData.size(), Hex::APPEND); 

							if (Debug::isTracing("Graphic"))
							{
								Debug::trace("Graphic", "******************************************************************************");
								HobString bl;
								if (graphicImage)		//	PTR 1667: Make sure we have one.
									bl = Integer::toString(graphicImage->getBinaryImageSize());
								else
									bl = "0";
								HobString nl(Integer::toString((int)serializedData.size()));
								Debug::trace("Graphic", HobString("Graphic.serializing - ") + graphicFile.getFullPath() + HobString("!  binary length:") +
																	bl + HobString("  nibblized length:") + nl);
								Debug::trace("Graphic", "******************************************************************************");
							}

/*
 							nibb = Hex::nibblize(graphicData.data(), graphicData.size());
							if (Debug::isTracing("Graphic") && (long)nibb.size() != (long)(2 * graphicData.size()))
							{
								Debug::trace("Graphic", "******************************************************************************");
								HobString bl(Integer::toString(graphicImage->getBinaryImageSize()));
								HobString nl(Integer::toString(nibb.size()));
								Debug::trace("Graphic", HobString("Graphic.serialize - nibblized length not correct!!!  binary length:") +
																	bl + HobString("  nibblized length:") + nl);
								Debug::trace("Graphic", "******************************************************************************");
								Debug::trace("Graphic", HobString("Graphic.serialize - binary data: ") + Hex::toString(graphicImage->getBinaryImage(), graphicImage->getBinaryImageSize()));
								Debug::trace("Graphic", "******************************************************************************");
								Debug::trace("Graphic", HobString("Graphic.serialize - nibb   data: ") + nibb);
								Debug::trace("Graphic", "******************************************************************************");
							}*/
						}
						else
						{
							//serializedData.append(nibb);
						}
						break;
					}

					default:
						break;
				}
			}
			break;

		case SynchronizationTaggedFormat:
		{
			ResourceTags graphicTags;
			HobString tag;
			HobString fieldData = "";
			
			int offset = 0;
			std::list<ResourceTagsFieldOffsets>::iterator fieldIter = fieldList.begin();
			while (fieldIter != fieldList.end())
			{
				offset = *fieldIter;
				tag = "";
				switch (offset)
				{
					case fieldOffsetName:
					{
						tag = graphicTags.getTag(offset);
						if (tag.size())
						{
							fieldData =	graphicFile.getFileName();  // includes the extension
							serializedData.append(tag);
							serializedData.append(fieldData);
							serializedData.append(UnitSeparator);
						}
						break;
					}

					case fieldOffsetResourceData:
					{
						tag = graphicTags.getTag(offset);
						if (tag.size())
						{
							if (exists() || isLoaded())
							{
								HobString graphicData("");
								if (readImageFromFile(graphicData) >= 0)
								{
									serializedData.append(tag);
									serializedData.reserve(serializedData.size() + (graphicData.size() * 2) + 1);
									Hex::nibblize(serializedData, graphicData.data(), (int)graphicData.size(), Hex::APPEND); 
									serializedData.append(UnitSeparator);
								}
							}
							else if (graphicImage && graphicImage->getBinaryImageSize())
							{
								serializedData.append(tag);
								serializedData.reserve(serializedData.size() + (graphicImage->getBinaryImageSize() * 2) + 1);
								Hex::nibblize(serializedData, graphicImage->getBinaryImage(), graphicImage->getBinaryImageSize(), Hex::APPEND); 
								serializedData.append(UnitSeparator);
							}
						}
						break;
					}

					default:
						break;
				}
						
				fieldIter++;
			}
			break;
		}

		case EvolutionTaggedFormat:
		{
			ResourceTags graphicTags;
			HobString tag;
			HobString fieldData = "";
			
			int offset = 0;
			std::list<ResourceTagsFieldOffsets>::iterator fieldIter = fieldList.begin();
			while (fieldIter != fieldList.end())
			{
				offset = *fieldIter;
				tag = "";
				switch (offset)
				{
					case fieldOffsetName:
					{
						tag = graphicTags.getTag(offset);

						if (tag.size())
						{
							fieldData =	graphicFile.getFileName();  // includes the extension
							serializedData.append(tag);
							serializedData.append(fieldData);
							serializedData.append(UnitSeparator);
						}

						break;
					}

					case fieldOffsetResourceData:
					{

						tag = graphicTags.getTag(offset);
						if (tag.size())
						{
							HobString graphicData("");
							if (readImageFromFile(graphicData) >= 0)
							{
								serializedData.append(tag);
								serializedData.reserve(serializedData.size() + (graphicData.size() * 2) + 1);
								Hex::nibblize(serializedData, graphicData.data(), (int)graphicData.size(), Hex::APPEND); 
								serializedData.append(UnitSeparator);
							}

						}
						break;
					}

					default:
						break;
				}
						
				fieldIter++;
			}
			break;
		}

		default:
			serializedData = HobString("Graphic::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}


// The graphicName passed in could optionally contain an extension, but should not be a full path.
Graphic *Graphic::findGraphic(const std::string &graphicName, short type)
{
	Graphic *readGraphic = NULL;
	
	if (!graphicName.empty())
	{
		readGraphic = new Graphic(graphicName, type);
		Debug::trace("New.Graphic", HobString("    Graphic - ") + graphicName + HobString(" - ") + HobString("Graphic: ") + HobString(Hex::toString((long long) readGraphic)) + HobString("  Graphic name:") + graphicName + HobString(" - findGraphic()"));
	}
	return readGraphic;
}


bool Graphic::deleteRecord()
{
	if (isRom())
	{
		Debug::trace("Graphic", HobString("deleteRecord - graphic is not a custom graphic, or file is corrupt: ") + graphicFile.getFullPath());
		return false;
	}

	File f;
	if (type == TYPE_PRODUCTGRAPHIC || type == TYPE_CATEGORYGRAPHIC || type == TYPE_SPECIALSGRAPHIC ||
		type == TYPE_STREAMINGGRAPHIC || type == TYPE_SOFTKEYGRAPHIC || type == TYPE_PLANOGRAMGRAPHIC)
	{
		bool isTracing = Debug::isTracing("Graphic");
		if (graphicFile.getPath().size() == 0)
		{
			f.setPath(determinePath());
			f.setFileName(graphicFile.getFileName());
			if (isTracing)
				Debug::trace("Graphic", HobString("used determinePath() since this graphic didn't have one - full filename is now: ") + f.getFullPath());
		}
		else
		{
			Filename temp;
			// FT-309: Pathing to graphics is wrong, looks like graphicFile.getPath() includes CfgInternal::SYSTEMUSERROOTDIR already...
			////temp.setPath(Configuration::getInstance()->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
			//if (isTracing)
			//	Debug::trace("Graphic", HobString("deleteRecord - temp is 1: ") + temp.getFullPath());
			//temp.appendPath(graphicFile.getPath());
			temp.setPath(graphicFile.getPath());
			if (isTracing)
			{
				Debug::trace("Graphic", HobString("deleteRecord - temp is 2: ") + temp.getFullPath());
				Debug::trace("Graphic", HobString("deleteRecord - f is 1: ") + f.getFullPath());
			}
			f.setPath(temp.getPath());
			if (isTracing)
				Debug::trace("Graphic", HobString("deleteRecord - f is 2: ") + f.getFullPath());
			f.setFileName(graphicFile.getFileName());
			if (isTracing)
				Debug::trace("Graphic", HobString("deleteRecord - f is 3: ") + f.getFullPath());
		}
	}
	else
	{
		f.setPath(graphicFile.getPath());
		f.setFileName(graphicFile.getFileName());
	}

	// Can only delete custom or any type of self-serve graphics
	if (f.exists())
	{
		int ret = f.remove();
		if (ret)
		{
			Debug::trace("Graphic", HobString("deleteRecord - Successfully deleted graphic file ") + f.getFullPath());
			graphicImageToDelete.push_back( graphicImage );
			graphicImage = (GraphicImage *)NULL;
			errorCode = RECORD_NOT_FOUND;
			loaded = false;
			return true;
		}
		else
			Debug::trace("Graphic", HobString("deleteRecord - Error deleting graphic file ") + f.getFullPath());
	}
	else
		Debug::trace("Graphic", HobString("deleteRecord - graphic file does not exist to delete: ") + f.getFullPath());

	return false;
}


bool Graphic::saveRecord()
{
	if (Debug::isTracing("Graphic"))
		Debug::trace("Graphic", HobString("saveRecord - entered ... ") + graphicFile.getFullPath());

	// Can only save custom graphics
	if (isCustom() && graphicFile.getFullPath().size() && graphicImage)
	{
		Directory destDir(graphicFile.getPath());
		if (!destDir.exists())
		{
			destDir.create();
		}
		ofstream gFile(graphicFile.getFullPath().c_str(), ios::out | ios::trunc | ios::binary);
		if (gFile)
		{
			char *dataChar = graphicImage->getBinaryImage();
			int totalSize = graphicImage->getBinaryImageSize();

			if (Debug::isTracing("Graphic"))
				Debug::trace("Graphic", HobString("totalSize: ") + HobString(Integer::toString(totalSize)));

			gFile.write(dataChar, totalSize);
			gFile.close();

			if (!graphicFile.exists())
			{
				if (Debug::isTracing("Graphic"))
					Debug::trace("Graphic", HobString("failed to copy graphic to file =") + graphicFile.getFullPath());
				return false;
			}

		}

		return true;
	}
	return false;
}


void Graphic::operator=(const Graphic &g)
{
	index = g.getIndex();
	type = g.getType();
	graphicFile.setFullPath(g.getGraphicFile().getFullPath());
	errorCode = g.getErrorCode();
	//graphicImage = g.getGraphicImage();
	loaded = false; // = g.isLoaded();
}


bool Graphic::locateLabelGraphic()
{
	Debug::trace("Graphic", "locateLabelGraphic() - entering");

	if (graphicFile.getFileName().empty())
		return false;

	Debug::trace("Graphic", HobString("locateLabelGraphic() - filename:") + graphicFile.getFileName());

	HobString ext;
	HobTokenizerCSV *extensionsTok = (HobTokenizerCSV *)NULL;

	File tempFile;

	// If the graphicFile already contains an extension, then use it.
	// If it doesn't, then we'll use all the extensions defined in Internal Config - GRAPHICSEXTENSIONS.
	bool alreadyHaveExtension = false;
	if (graphicFile.getFileNameExtension().empty())
	{
		tempFile.setFileNameBase(graphicFile.getFileNameBase());
		HobString extensions = Configuration::getInstance()->toStringCfgI(CfgInternal::GRAPHICSEXTENSIONS);
		extensionsTok = new HobTokenizerCSV(&extensions, BrowserFormatSeparator);
		Debug::trace("Graphic", HobString("locateLabelGraphic() - do not already have extension on graphicFile: ") + graphicFile.getFullPath());
		Debug::trace("Graphic", HobString("locateLabelGraphic() - do not already have extension, tempFile is: ") + tempFile.getFullPath());
	}
	else
	{
		tempFile.setFileName(graphicFile.getFileName());
		alreadyHaveExtension = true;
		Debug::trace("Graphic", HobString("locateLabelGraphic() - already have extension on graphicFile: ") + graphicFile.getFullPath());
		Debug::trace("Graphic", HobString("locateLabelGraphic() - already have extension, tempFile is: ") + tempFile.getFullPath());
	}

	//	Look for the file first in the User graphics directory.
	tempFile.setPath(Configuration::getInstance()->toStringCfgI(CfgInternal::USERGRAPHICSLOCATION));
	bool foundGraphic = false;

	if (alreadyHaveExtension)
	{
		if (tempFile.exists())
		{
			type = TYPE_USERLABELGRAPHIC;
			foundGraphic = true;
			Debug::trace("Graphic", HobString("locateLabelGraphic() - already have extension - found user: ") + tempFile.getFullPath());
		}
		else
			Debug::trace("Graphic", HobString("locateLabelGraphic() - already have extension - DID NOT find in user tempFile: ") + tempFile.getFullPath());
	}
	else
	{
		while (extensionsTok->next(&ext) != -1)
		{
			tempFile.setFileNameExtension(ext);
			Debug::trace("Graphic", HobString("locateLabelGraphic() - using extension - looking in user for: ") + tempFile.getFullPath());
			if (tempFile.exists())
			{
				type = TYPE_USERLABELGRAPHIC;
				foundGraphic = true;
				Debug::trace("Graphic", HobString("locateLabelGraphic() - using extension - found user: ") + tempFile.getFullPath());
				break;
			}
		}
	}

	if (!foundGraphic)	//	If no User graphic, try the System graphics directory
	{
		Debug::trace("Graphic", HobString("locateLabelGraphic() - did not find graphic in user directory, will look in rom: ") + tempFile.getFullPath());

		tempFile.setPath(Configuration::getInstance()->toStringCfgI(CfgInternal::SYSTEMGRAPHICSLOCATION));

		if (alreadyHaveExtension)
		{
			if (tempFile.exists())
			{
				type = TYPE_ROMLABELGRAPHIC;
				foundGraphic = true;
				Debug::trace("Graphic", HobString("locateLabelGraphic() - already have extension - found rom: ") + tempFile.getFullPath());
			}
			else
				Debug::trace("Graphic", HobString("locateLabelGraphic() - already have extension - DID NOT find in rom tempFile: ") + tempFile.getFullPath());
		}
		else
		{
			int result = extensionsTok->first(&ext);
			while (result != -1)
			{
				tempFile.setFileNameExtension(ext);
				Debug::trace("Graphic", HobString("locateLabelGraphic() - using extension - looking in rom for: ") + tempFile.getFullPath());
				if (tempFile.exists())
				{
					type = TYPE_ROMLABELGRAPHIC;
					foundGraphic = true;
					Debug::trace("Graphic", HobString("locateLabelGraphic() - using extension - found rom: ") + tempFile.getFullPath());
					break;
				}
				result = extensionsTok->next(&ext);
			}
		}
	}

	if (extensionsTok)
		delete extensionsTok;

	if (foundGraphic)
	{
		graphicFile.setPath(tempFile.getPath());
		graphicFile.setFileNameExtension(tempFile.getFileNameExtension());
		Debug::trace("Graphic", HobString("locateLabelGraphic() - setting full path to: ") + graphicFile.getFullPath());
		return true;
	}

	Debug::trace("Graphic", HobString("locateLabelGraphic() - didn't find file: ") + graphicFile.getFileName());
	return false;
}


const string Graphic::determinePath()
{
	// I would like to make this either under control of the graphic's corresponding GraphicManager, or by AbstractGraphicManager,
	// or by another means, maybe a static method, but this is Fastrack and this was the fastest way to get it done.  I don't want
	// to have to update this method when we add some other graphic type.  This code comes from part of
	// AbstractGraphicManager::countAllGraphics(short graphicType)

	Configuration *config = Configuration::getInstance();
	if (!config)
		return "";

	util_filesystem::Filename tempFilename;

	switch (type)
	{
		case Graphic::TYPE_LABELGRAPHIC:
		case Graphic::TYPE_USERLABELGRAPHIC:
			tempFilename.setPath(config->toStringCfgI(CfgInternal::USERGRAPHICSLOCATION));
			break;
		case Graphic::TYPE_ROMLABELGRAPHIC:
			tempFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMGRAPHICSLOCATION));
			break;
		case Graphic::TYPE_SPECIALSGRAPHIC:
			tempFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
			tempFilename.appendPath(config->toStringCfgI(CfgInternal::SPECIALSGRAPHICSDIR));
			break;
		case Graphic::TYPE_PRODUCTGRAPHIC:
			tempFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
			tempFilename.appendPath(config->toStringCfgI(CfgInternal::PRODUCTGRAPHICSDIR));
			break;
		case Graphic::TYPE_CATEGORYGRAPHIC:
			tempFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
			tempFilename.appendPath(config->toStringCfgI(CfgInternal::CATEGORYGRAPHICSDIR));
			break;
		case Graphic::TYPE_STREAMINGGRAPHIC:
			tempFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
			tempFilename.appendPath(config->toStringCfgI(CfgInternal::STREAMINGGRAPHICSDIR));
			break;
		case Graphic::TYPE_SELFSERVEGRAPHIC:
			tempFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
			tempFilename.appendPath(config->toStringCfgI(CfgInternal::SELFSERVEGRAPHICSDIR));
			break;
		case Graphic::TYPE_SOFTKEYGRAPHIC:
			tempFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
			tempFilename.appendPath(config->toStringCfgI(CfgInternal::SOFTKEYGRAPHICSDIR));
			break;
		case Graphic::TYPE_PLANOGRAMGRAPHIC:
			tempFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
			tempFilename.appendPath(config->toStringCfgI(CfgInternal::PLANOGRAMGRAPHICSDIR));
			break;
		case Graphic::TYPE_VIDEOS:
			tempFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
			tempFilename.appendPath(config->toStringCfgI(CfgInternal::VIDEOSDIR));
			break;
		default:
			break;
	}
	return tempFilename.getFullPath();
}
