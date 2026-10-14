/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include <fstream>
//#include <ugl/ugl.h>
// #include <ugl/driver/font/udftfnt.h>
//#include <ugl/driver/font/HobartFontExtension.h> 

#include "Font.h"
#include "Configuration.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"
#include "HobToken.h"

#include "Long.h"
#include "Hex.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int Font::dataType = 0;
std::string Font::dataTypeName("Font"); 
#endif


using namespace std;


std::list<ResourceTagsFieldOffsets> Font::fieldList;

Font::Font()
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif

    init();
}

#if defined(LINUX_FIX)
    Font::Font(const Font &f)
: HfcObject()
#else
    Font::Font(const Font &f)
#endif
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif

    init();

    fontName = f.getFontName();
    errorCode = f.getErrorCode();
    custom = f.isCustom();
}

Font::Font(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif

    init();

    Debug::trace("Font", HobString("creating Font: ") + HobString(Integer::toString(len)));

    switch (formatType)
    {
        case EvolutionTaggedFormat:
            {
                //convert data to HobString
                HobString evolData(data, len);
                initFieldList();
                ResourceTags fontTags((FieldOffsetsList *)&fieldList);			

                //use the UnitSeparator to parse the string
                HobTokenizerUS tok(&evolData, UnitSeparator);
                HobString temp;

                HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp,HobTaggedToken));
                while (tok.next(token) != -1)
                {
                    temp = token->getData();
                    int offset = fontTags.getOffset(token->getTag().c_str());
                    setReceived(offset, (temp.length() == 0) ? false : true);

                    switch(offset)
                    {
                        case fieldOffsetName:
                            setFontName(token->getData());
                            // If received via comm, it must be a custom font;
                            // only custom fonts are sent via comm;
                            custom = true;
                            break;

                        case fieldOffsetResourceData:
                            {
                                binaryImage = "";
                                HobString unnibbData = token->getData();

                                Debug::trace("Font", HobString("unnibbData.size(): ") + HobString(Integer::toString((int)unnibbData.size())));
                                //						Debug::trace("Font", HobString("unnibbData: ") + Hex::toString(unnibbData.c_str(), unnibbData.size()));

                                for (unsigned int i = 0; i < unnibbData.size(); i+=2)
                                {
                                    HobString oneByte(unnibbData.substr(i,2));
                                    unsigned char val = (unsigned char)Hex::valueOf(oneByte.c_str());
                                    binaryImage.append(1, val);
                                }

                                Debug::trace("Font", HobString("created Font: ") + HobString(Integer::toString((int)binaryImage.size())));
                                errorCode = RECORD_OK;
                                break;
                            }

                        default:
                            break;
                    }

                }	
                delete token;
                break;	
            }		 

        default:
            // todo - throw exception("invalid format");
            break;
    }
}

Font::Font(const HobString &fontName)
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif

    init();
    setFontName(fontName);	//	Don't save the full path unless we decide we need it.   
    load();
}

Font::~Font()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif

    Debug::trace("Delete.Font", HobString(" Font: ") + HobString(Hex::toString((long long) this)) + HobString("  Font name:") + fontName);
}


void Font::init()
{
    fontName = "";
    errorCode = RECORD_NOT_FOUND;
    custom = false;

    binaryImage = "";
}


// static
void Font::initFieldList()
{
    if (fieldList.empty())
    {
        fieldList.push_back(fieldOffsetName);
        fieldList.push_back(fieldOffsetResourceData);

        ResourceTags graphicTags((FieldOffsetsList *)&fieldList);
    }
}

/* NOTE:  You must call setFontName() before calling load().
*/
void Font::load()
{
    HobString readFilename("");
    HobString path("");
    HobString filename("");
    HobString ext;
    ifstream *fp = NULL;

    errorCode = RECORD_NOT_FOUND;

    if (fontName.size() == 0)
        return;

    HobString extensions = Configuration::getInstance()->toStringCfgI(CfgInternal::FONTEXTENSIONS);
    Debug::trace("Font", HobString("extensions: ") + extensions);
    HobTokenizerCSV extensionsTok(&extensions, BrowserFormatSeparator);

    //	Look for the file first in the User Fonts directory.
    path = Configuration::getInstance()->toStringCfgI(CfgInternal::USERFONTSDIR);

    Debug::trace("Font", HobString("path1: ") + path);

    //	Check to see if the filename already contains an extension.
    // If it does, don't need to use the list of extensions stored in configuration
    string::size_type extPos = fontName.find_last_of(".");
    if (extPos != string::npos)
    {
        filename = path + fontName;
        Debug::trace("Font", HobString("filename4: ") + filename);

        fp = new ifstream(filename.c_str());
        if (fp && fp->is_open())
        {
            readFilename = filename;
            custom = true;
        }
        else if (fp)
        {
            delete fp;
            fp = NULL;
        }
    }
    else
    {
        while (extensionsTok.next(&ext) != -1)
        {
            filename = path + fontName + "." + ext;
            Debug::trace("Font", HobString("filename3: ") + filename);

            fp = new ifstream(filename.c_str());
            if (fp && fp->is_open())
            {
                readFilename = filename;
                custom = true;
                break;
            }
            else if (fp)
            {
                delete fp;
                fp = NULL;
            }
        }
    }

    if (!fp)	//	If no User graphic, try the System graphics directory
    {
        path = Configuration::getInstance()->toStringCfgI(CfgInternal::SYSTEMFONTSDIR);

        Debug::trace("Font", HobString("path2: ") + path);

        if (extPos != string::npos)
        {
            filename = path + fontName;
            Debug::trace("Font", HobString("filename2: ") + filename);

            fp = new ifstream(filename.c_str());
            if (fp && fp->is_open())
            {
                readFilename = filename;
                custom = true;
            }
            else if (fp)
            {
                delete fp;
                fp = NULL;
            }
        }
        else
        {
            int result = extensionsTok.first(&ext);
            while (result != -1)
            {
                filename = path + fontName + "." + ext;
                Debug::trace("Font", HobString("filename1: ") + filename);

                fp = new ifstream(filename.c_str());
                if (fp && fp->is_open())
                {
                    readFilename = filename;
                    custom = false;
                    break;
                }
                else if (fp)
                {
                    delete fp;
                    fp = NULL;
                }
                result = extensionsTok.next(&ext);
            }
        }
    }

    if (fp)
    {
        if (fp->is_open())
        {
            fp->close();

            fontFileName = readFilename;

            errorCode = readImageFromFile(binaryImage);
        }
        delete fp;
    }
}

const std::string &Font::getFontNameBase() const
{
    return fontFileName.getFileNameBase();
}
const HobString &Font::getFontName() const
{
    return fontName;
}
void Font::setFontName(const HobString &fontName)
{
    this->fontName = fontName;
}

HobErrorCode Font::getErrorCode() const
{
    return errorCode;
}
void Font::setErrorCode(HobErrorCode errorCode)
{
    this->errorCode = errorCode;
}

bool Font::isCustom() const	// is this a custom graphic?
{
    return custom;
}
bool Font::isRom() const	// is this a ROM graphic?
{
    return !custom;
}
void Font::setCustom(bool flag) 	// is this a custom graphic?
{
    custom = flag;
}

int Font::readImageFromFile(HobString &data) const
{
    int ret = -1;

    if (fontFileName.getFullPath().empty() == false)
    {
        ifstream fontFile(fontFileName.getFullPath().c_str(), ios::in | ios::binary);
        if (fontFile.is_open())
        {
            data = "";
            char buff[2048];
            while (!fontFile.eof())
            {
                fontFile.read(buff, 2048);
                int bytesread = fontFile.gcount();
                data.append((char *)buff, bytesread);
            }

            fontFile.close();
            ret = 0;
        }
    }

    return ret;
}


const char *Font::serialize(int format) const
{
    initFieldList();
    ResourceTags fontTags((FieldOffsetsList *)&fieldList);

    serializedData = "";
    switch (format)
    {
        case EvolutionTaggedFormat:
            {
                HobString tag;
                HobString fieldData = "";

                for (int loopCnt = 1; loopCnt < endResourceTagsFieldOffsets; loopCnt++)
                {
                    tag = "";
                    switch (loopCnt)
                    {
                        case fieldOffsetName:
                            {
                                tag = fontTags.getTag(loopCnt);
                                fieldData =	fontName;
                                //	Check to see if the filename contains an extension; if so, remove it.
                                string::size_type extPos = fieldData.find_last_of(".");
                                if (extPos != string::npos)
                                    fieldData.erase(extPos);
                                break;
                            }

                        case fieldOffsetResourceData:
                            {
                                tag = fontTags.getTag(loopCnt);
                                string nibb = " ";

                                if ((binaryImage.size() > 0) || (readImageFromFile(binaryImage) >= 0))
                                {
                                    fieldData = Hex::nibblize(binaryImage.data(), (int)binaryImage.size());
                                    if (Debug::isTracing("Font")) // && (long)fieldData.size() != (long)(2 * binaryImage.size()))
                                    {
                                        HobString bl(Integer::toString((int)binaryImage.size()));
                                        HobString nl(Integer::toString((int)fieldData.size()));
                                        Debug::trace("Font", HobString("nibblized binary length: ") + bl + HobString("  nibblized length: ") + nl);
                                        //								Debug::trace("Font", HobString("binary data: ") + Hex::toString(binaryImage.c_str(), binaryImage.size()));
                                        //								Debug::trace("Font", HobString("nibb   data: ") + fieldData);
                                    }
                                }
                                break;
                            }

                        default:
                            break;
                    }

                    if (tag.size())
                    {
                        serializedData.append(tag);
                        serializedData.append(fieldData);
                        serializedData.append(UnitSeparator);
                    }
                }
                break;
            }

        default:
            // todo - throw exception("invalid format");
            break;
    }
    return serializedData.data();
}

bool Font::deleteRecord()
{
   return true;
}


bool Font::saveRecord()
{
    bool rv = false;
    return rv;
}

Font *Font::findFont(HobString fontName)
{
    Font *readFont = NULL;

    if(!fontName.empty())
        readFont = new Font(fontName);

    Debug::trace("New.Font", HobString("    Font - ") + fontName + HobString(" - ") + HobString("Font: ") + HobString(Hex::toString((long long) readFont)) + HobString("  Font name:") + fontName + HobString(" - findFont()"));
    return readFont;
}

void Font::operator=(const Font &f)
{
    fontName = f.getFontName();
    errorCode = f.getErrorCode();
    custom = f.isCustom();
    load();
}

void Font::show() const
{
    bool isTracing = Debug::isTracing("Font");

    if (isTracing == false)
    {
        Debug::addTraceCategory("Font");
    }

    Debug::trace("Font", "Font: ");

    Debug::trace("Font", HobString("fontName: ") + fontName);
    Debug::trace("Font", HobString("fontFileName: ") + HobString(fontFileName.getFileName()));
    Debug::trace("Font", HobString("isCustom: ") + Bool::toString(isCustom()));
    Debug::trace("Font", HobString("errorCode: 0x") + Hex::toString(errorCode));

    Debug::trace("Font", HobString("binaryImage (size): ") + HobString(Integer::toString((int)binaryImage.size())));
    Debug::trace("Font", HobString("binaryImage: ") + binaryImage);


    if (isTracing == false)
    {
        Debug::removeTraceCategory("Font");
    }
}
