#include <logmanager.h>
#include "texteditor.h"
#include "ui_texteditor.h"
#include <ScaleConfiguration.h>
#include <QFontDatabase>
#include <QXmlStreamReader>

#define TEXT_EDITOR_MAX_CHARS 2000
#define ESC_CHAR 0x1B

textEditor::textEditor(QWidget *parent, QString fieldName, bool editable, QString initialContents) : SessionDialog( TopDialogLocation, parent ),
    ui(new Ui::textEditor)
{
    Q_UNUSED( fieldName );

    setObjectName( "textEditor" );

    internalConfigData = &session_->scaleConfig->internalConfig;

    if(isLinux())
    {
        //TBD SPLarge
        //int QFontDatabase::addApplicationFont("")
    }
    else
    {
        //TBD
    }

    //escSeq = QString(ESC_CHAR);
    ui->setupUi(this);
    // default palette
    setPalette( QApplication::palette() );
    setAutoFillBackground( true );

    textEdit = ui->textEditRegion;

    escSeq = QString(ESC_CHAR);
    doubleEscSeq = escSeq + escSeq;

    ui->fontComboBox->removeItem(ui->fontComboBox->findText("cursor.pcf"));
    ui->fontComboBox->removeItem(ui->fontComboBox->findText("Fixed"));
    ui->fontComboBox->removeItem(ui->fontComboBox->findText("Liberation Mono"));
    ui->fontComboBox->removeItem(ui->fontComboBox->findText("Monospace"));
    ui->fontComboBox->removeItem(ui->fontComboBox->findText("Quantum"));
    ui->fontComboBox->removeItem(ui->fontComboBox->findText("Marquee12"));
    ui->fontComboBox->removeItem(ui->fontComboBox->findText("FreeSerif"));

    int SPisAt = ui->fontComboBox->findText("SP");
    ui->fontComboBox->insertItem(SPisAt+1,"SPLarge");

    // TODO must calculate pixel width and height based on
    // height/width provided via message from backend.
    //textEdit->width = ;
    //textEdit->height = ;

    if (!editable)
    {
        textEdit->setEnabled(false);
    }

    lastTextEditHtml = textEdit->document()->toHtml();

    defaultFontFamily = internalConfigData->defaultFontName;
    defaultFontSize = QString::number(internalConfigData->defaultFontSize);

    defaultLabelWidth = QString::number(internalConfigData->defaultLabelWidth);
    defaultLabelHeight = QString::number(internalConfigData->defaultLabelHeight);

    // get font in text edit region
    textFont = ui->textEditRegion->font();

    // ensure that keyboard stays displayed so force all widgets to not allow focus
    // other than textEditRegion
    if (editable)
    {
        ui->cutButton->setEnabled(false);
        ui->cutButton->setFocusPolicy( Qt::NoFocus );

        ui->copyButton->setFocusPolicy( Qt::NoFocus );
        ui->copyButton->setEnabled(false);

        ui->pasteButton->setFocusPolicy( Qt::NoFocus );
    }
    else
    {
        ui->cutButton->hide();
        ui->copyButton->hide();
        ui->pasteButton->hide();
        ui->fontSizeComboBox->hide();
        ui->fontComboBox->hide();
    }

    ui->doneButton->setFocusPolicy( Qt::NoFocus );

    ui->fontComboBox->setFocusPolicy( Qt::NoFocus );
    ui->fontSizeComboBox->setFocusPolicy( Qt::NoFocus );

    updateFontSizeComboBox();

    connect(ui->fontSizeComboBox,SIGNAL(activated(QString)),this,SLOT(textSize(QString)));

    fontChanged(QApplication::font());

    ui->fontSizeComboBox->setCurrentIndex(ui->fontSizeComboBox->findText(QString::number(QApplication::font()
                                                                   .pointSize())));
    textSize(QString::number(QApplication::font().pointSize()));

    textSelected = false;

    // connect buttons to slots
    connect(ui->cutButton,SIGNAL(clicked()),ui->textEditRegion,SLOT(cut()));
    connect(ui->copyButton,SIGNAL(clicked()),ui->textEditRegion,SLOT(copy()));
    connect(ui->pasteButton,SIGNAL(clicked()),ui->textEditRegion,SLOT(paste()));
    connect(ui->doneButton,SIGNAL(clicked()),this,SLOT(doneEditing()));

    connect(ui->fontComboBox,SIGNAL(activated(QString)),this,SLOT(textFamily(QString)));

    connect(textEdit, SIGNAL(copyAvailable(bool)), this, SLOT(textSelect(bool)));
    connect(textEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));

    connect(textEdit, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
            this, SLOT(currentCharFormatChanged(QTextCharFormat)));
    //connect(textEdit, SIGNAL(cursorPositionChanged()),
    //        this, SLOT(cursorPositionChanged()));

    //connect(textEdit->document(), SIGNAL(modificationChanged(bool)),
    //        this, SLOT(setWindowModified(bool)));

    if (editable)
    {
       // if (initialContents == "")
       // {
      //      setFieldNameWatermark(fieldName);
      //  }
      //  else
      //  {
            decodeBuffer(initialContents);
      //  }
    }
    else
    {
        if (initialContents != "")
        {
            ui->watermark->hide();
            decodeBuffer(initialContents);
        }
        else
            ui->watermark->show();
    }

    textEdit->setFocus();

    // TODO: need to send message to back end
    // session_->send( SessionDev, SnReadStatistics );
    ui->fontComboBox->removeItem(ui->fontComboBox->findText("SPLarge"));

}

textEditor::~textEditor()
{
    delete ui;
}


bool textEditor::isLinux()
{
#if defined( Q_OS_LINUX )
    return true;
#elif defined( Q_OS_WIN )
    return false;
#endif
}

void textEditor::setFieldNameWatermark(QString fieldName)
{
    QBrush brush(QColor(205, 205, 205, 205));
    brush.setStyle(Qt::SolidPattern);

    QPalette palette;

    palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
    palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);

    ui->watermark->setPalette(palette);
    ui->watermark->setText(fieldName);
    ui->watermark->hide();
}


// set text editor region font based on font combo box
void textEditor::setTextEditRegionFont()
{
    ui->textEditRegion->setFont(ui->fontComboBox->currentFont() );
}

void textEditor::translate()
{
    ui->retranslateUi(this);
    update();
}

void textEditor::setTextEditRegionFontSize()
{
    // get the currently selected font size in font size combo box
    /*QString fontSizeStr = ui->fontSizeComboBox->currentText();

    QFont currentFontSpec = ui->fontComboBox->currentFont();
    QFont newFontSpec(currentFontSpec.family(),fontSizeStr.toInt(),-1,false);

    ui->textEditRegion->setFont(newFontSpec);
*/
// get the currently selected font size in font size combo box
    QString fontSizeStr = QString::number(ui->fontSizeComboBox->currentText().toInt() *2);

    QFont currentFontSpec = ui->fontComboBox->currentFont();
    QFont newFontSpec(currentFontSpec.family(),fontSizeStr.toInt(),-1,false);

    ui->textEditRegion->setFont(newFontSpec);
}

void textEditor::updateFontSizeComboBox()
{
    /*QFontDatabase fdb;

    // load into font size combo box the family's point sizes
    ui->fontSizeComboBox->clear();
    foreach (int size, fdb.standardSizes())
    {
        // insert all point size values valid for font family
        ui->fontSizeComboBox->addItem(QString::number(size));
    }*/
QFontDatabase fdb;

    ui->fontSizeComboBox->clear();
    // load into font size combo box the family's point sizes
    // Hardcoded sizes for combobox added 9/12/2013

    // Font sizes 3 and 4 removed per Jim Highley on 9/13/2013
    //ui->fontSizeComboBox->addItem("3");
    //ui->fontSizeComboBox->addItem("4");
    ui->fontSizeComboBox->addItem("5");
    ui->fontSizeComboBox->addItem("6");
    ui->fontSizeComboBox->addItem("7");
    ui->fontSizeComboBox->addItem("8");
    ui->fontSizeComboBox->addItem("9");
    ui->fontSizeComboBox->addItem("10");
    ui->fontSizeComboBox->addItem("11");
    ui->fontSizeComboBox->addItem("12");
    ui->fontSizeComboBox->addItem("13");
    ui->fontSizeComboBox->addItem("14");
    ui->fontSizeComboBox->addItem("15");
    ui->fontSizeComboBox->addItem("16");
    ui->fontSizeComboBox->addItem("18");
    ui->fontSizeComboBox->addItem("20");
    ui->fontSizeComboBox->addItem("22");
    ui->fontSizeComboBox->addItem("24");
    ui->fontSizeComboBox->addItem("26");
    ui->fontSizeComboBox->addItem("28");
    ui->fontSizeComboBox->addItem("34");
    ui->fontSizeComboBox->addItem("36");
    ui->fontSizeComboBox->addItem("48");
    ui->fontSizeComboBox->addItem("60");
    ui->fontSizeComboBox->addItem("72");

    // OLD CODE
    /*foreach (int size, fdb.standardSizes())
    {
        // insert all point size values valid for font family
        ui->fontSizeComboBox->addItem(QString::number(size));
    }*/
}

void textEditor::doneEditing()
{
    LOG_CLASS_INFO( "textEditor", "doneEditing" );

    // process qTextEdit contents if text modified
    if (textEdit->document()->isModified())
    {
        QString textData = textEdit->toHtml();
        LOG_DEBUG( "%s", textData.toLatin1().constData() );
        encodeBuffer();
    }
    accept();
}

void textEditor::encodeBuffer()
{
   //OLDEST
    /*LOG_CLASS_INFO( "textEditor", "encodeBuffer" );

    QXmlStreamReader reader(textEdit->document()->toHtml());

    QStringList fontFamilies;
    QStringList fontSizes;
    QStringList fontTextItems;
    QString currentFont="";
    QString currentFontSize="";
    QString currentText;

    QString lastFont="";
    QString lastFontSize="";

    bool isFirstRead = true;

    while (!reader.atEnd())
    {
        if ( isFirstRead )
            isFirstRead = false;
        else
        {
            reader.readNext();
        }

        if (reader.isStartElement()) {

            LOG_DEBUG( "start tag = %s", reader.name().toString().toLatin1().constData() );

            if ((reader.name() == "span") || (reader.name() == "body"))
            {
                if (reader.attributes().hasAttribute(QString("style")))
                {
                    // read style attribute data
                    QString styleSpecString = reader.attributes().value("style").toString();
                    // parse attributes based on semicolon
                    QStringList styleSpecItems = styleSpecString.split(';',QString::SkipEmptyParts,Qt::CaseInsensitive);
                    foreach(QString styleSpec, styleSpecItems)
                    {
                        // parse style items based
                        QStringList styleTypeItems = styleSpec.split(':',QString::KeepEmptyParts,Qt::CaseInsensitive);

                        if (styleTypeItems[0].trimmed() == "font-family")
                            currentFont = styleTypeItems[1];
                        if (styleTypeItems[0].trimmed() == "font-size")
                            currentFontSize = styleTypeItems[1];
                    }
                }
            }
            else if (reader.name() == "br")
            {
                fontFamilies.append(currentFont.mid(1,currentFont.size()-2));

                // remove pt or px specification from size
                fontSizes.append(currentFontSize.mid(0,currentFontSize.size()-2));
                fontTextItems.append(currentText);
                // br found so map to newline
               // ;
            }
        }

        // if we have reached the end of a tag
        else if (reader.isEndElement())
        {
            LOG_DEBUG( "end tag = %s", reader.name().toString().toLatin1().constData() );
            // if a span tag then end of style
            if (reader.name() == "span")
            {
                // remove single quotes around font family
                fontFamilies.append(currentFont.mid(1,currentFont.size()-2));

                // remove pt or px specification from size
                fontSizes.append(currentFontSize.mid(0,currentFontSize.size()-2));
                fontTextItems.append(currentText);
            }

            // check for paragraph end which needs converted to newline
            else if ((reader.name() == "p") || (reader.name() == "br"))
            {
                // remove single quotes around font family
                fontFamilies.append(currentFont.mid(1,currentFont.size()-2));

                // remove pt or px specification from size
                fontSizes.append(currentFontSize.mid(0,currentFontSize.size()-2));
                fontTextItems.append("\n");
            }

        }

        // text of tag
        else if (reader.isCharacters())
        {
            currentText = reader.text().toString();
         //   LOG_DEBUG( "characters = %s", currentText.toLatin1().constData() );
        }
    }

    encodedBuffer.clear();
    for (int i=0; i < fontFamilies.size(); i++)
    {
        LOG_DEBUG( "family = %s", fontFamilies[i].toLatin1().constData() );
        LOG_DEBUG( "size = %s", fontSizes[i].toLatin1().constData() );
        LOG_DEBUG( "text = %s", fontTextItems[i].toLatin1().constData() );

       // if ((lastFont != fontFamilies[i]) || (lastFontSize != fontSizes[i]))
            if (((lastFont != fontFamilies[i]) || (lastFontSize != fontSizes[i])) || (fontFamilies[i] != defaultFontFamily && fontSizes[i] != defaultFontSize))

        {
            encodedBuffer.append(doubleEscSeq);
            encodedBuffer.append("fn=");
            encodedBuffer.append(fontFamilies[i]);
            encodedBuffer.append(";ps=");
            encodedBuffer.append(fontSizes[i]);
            encodedBuffer.append(escSeq);
        }

        // replace " with \" in encoded buffer
        QString textData = fontTextItems[i];
        textData.replace('"',"\"");
        encodedBuffer.append(textData);

        lastFont = fontFamilies[i];
        lastFontSize = fontSizes[i];
    }

    // right trim all whitespace and carriage control
    encodedBuffer = rstrip(encodedBuffer);
    plainTextBuffer = textEdit->toPlainText();
    LOG_DEBUG( "encoded buffer = :%s:", encodedBuffer.toLatin1().constData() );
*/


    //OLD FROM 11/22/2013 below here
    //until three lines of X's

    //textEdit->setAutoFormatting(QTextEdit::AutoNone);
    //QString myHtml = textEdit->document()->toHtml();
    //qDebug() << "myHtml: " << myHtml;
 /*
   QXmlStreamReader reader(textEdit->document()->toHtml());

    QStringList fontFamilies;
    QStringList fontSizes;
    QStringList fontTextItems;
    QString currentFont="";
    QString currentFontSize="";
    QString currentText;

    QString lastFont="";
    QString lastFontSize="";

    encodedBuffer.clear();
    bool isFirstRead = true;
    int newLineCount = 0;
    bool addDefaultFormat = false;          // new code implemented

    while (!reader.atEnd())
    {
        if ( isFirstRead )
            isFirstRead = false;
        else
        {
            reader.readNext();
        }

        if (reader.isStartElement()) {
            //qDebug() << "start Element: " << reader.name();

            //QString myReaderText = reader.text().toString();
            //QString myReaderName = reader.name().toString();

            if ((reader.name() == "span")) //|| (reader.name() == "body"))
            {
                if (reader.attributes().hasAttribute(QString("style")))
                {
                    // read style attribute data
                    QString styleSpecString = reader.attributes().value("style").toString();
                    // parse attributes based on semicolon
                    QStringList styleSpecItems = styleSpecString.split(';',QString::SkipEmptyParts,Qt::CaseInsensitive);
                    foreach(QString styleSpec, styleSpecItems)
                    {
                        // parse style items based
                        QStringList styleTypeItems = styleSpec.split(':',QString::KeepEmptyParts,Qt::CaseInsensitive);

                        if(styleSpecString.contains("font-family"))
                        {
                            if (styleTypeItems[0].trimmed() == "font-family")
                            currentFont = styleTypeItems[1].replace("'","");
                        }
                        else currentFont = decodeBufferIncomingFontName;

                        if (styleTypeItems[0].trimmed() == "font-size")
                            currentFontSize = styleTypeItems[1].replace("pt","");
                    }

                    qDebug() << "addDefaultFormat=" << addDefaultFormat;
                    qDebug() << "useFontChanges  =" << useFontChanges;
                    qDebug() << "currentFont     =" << currentFont;
                    qDebug() << "currentFontSize =" << currentFontSize;
                    qDebug() << "encodedBuffer   =" << encodedBuffer;
                    if((indexOfFontName(currentFont) != labelFormSizeIncomingFont || ui->fontSizeComboBox->findText(currentFontSize) != labelFormSizeIncomingFontSize) || addDefaultFormat)
                    {
                        if(useFontChanges)        // new code implemented - The if part is new
                        {
                            encodedBuffer.append(doubleEscSeq);
                            encodedBuffer.append("fn=");
                            encodedBuffer.append(currentFont);
                            encodedBuffer.append(";ps=");
                            encodedBuffer.append(QString::number(currentFontSize.toInt() / 2));
                            encodedBuffer.append(escSeq);

                           // addDefaultFormat = true;
                        }
                    }
                    if(currentFont == defaultFontFamily && currentFontSize == defaultFontSize && useFontChanges)  // new code implemented - the if is new
                    {
                        addDefaultFormat = true;
                    }
                    // Check the font specs with debug
                }
            }
            else if (reader.name() == "br")
            {

                // br found so map to newline
                //fontFamilies.append(currentFont.mid(1,currentFont.size()-2));

                // remove pt or px specification from size
                //fontSizes.append(currentFontSize.mid(0,currentFontSize.size()-2));
                //fontTextItems.append(currentText);

                //encodedBuffer.append("\n");
                //fontTextItems.replace(fontTextItems.size()-1, fontTextItems.at(fontTextItems.size()-1) + "\n");
            }
            else if (reader.name() == "p")
            {

               // }
            }
        }

        // if we have reached the end of a tag
        else if (reader.isEndElement())
        {
            if (reader.attributes().hasAttribute(QString("style")))
            {

            }

        }

        // text of tag
        else if (reader.isCharacters())
        {
           // qDebug() << "chars are: " << reader.text().toString();
            if(reader.text().toString() != "\np, li { white-space: pre-wrap; }\n" && reader.text().toString() != "\n")
            {
                encodedBuffer.append(reader.text().toString().replace('"',"\""));
            }
            else if(reader.text().toString() == "\n")
            {
                if(newLineCount > 0)
                {
                    encodedBuffer.append("\n");
                }
                newLineCount++;
            }
           // else if(reader.text().toString() == "\n")
           // {
           //     encodedBuffer.append("\n");
           // }
            //currentText = reader.text().toString();
           // qDebug() << "reader text: " << reader.text().toString();

           // LOG_DEBUG( "characters = %s", currentText.toLatin1().constData() );
        }
    }

   // }

    // right trim all whitespace and carriage control
    encodedBuffer = rstrip(encodedBuffer);

    plainTextBuffer = textEdit->toPlainText();
    */
    //XXXXXXXXXXXXXXXX
    //XXXXXXXXXXXXXXXX
    //XXXXXXXXXXXXXXXX

    // Begin new code below on 11/22/2013 to match superchangeProductAllFields.cpp
    //textEdit->setAutoFormatting(QTextEdit::AutoNone);
    QString myHtml = textEdit->document()->toHtml();
   // qDebug() << "myHtml: " << myHtml;
    QXmlStreamReader reader(textEdit->document()->toHtml());

    QStringList fontFamilies;
    QStringList fontSizes;
    QStringList fontTextItems;
    QString currentFont="";
    QString currentFontSize="";
    QString currentText;

    QString lastFont="";
    QString lastFontSize="";

    encodedBuffer.clear();
    bool isFirstRead = true;
    int newLineCount = 0;
    bool addDefaultFormat = false;          // new code implemented

    while (!reader.atEnd())
    {
        if ( isFirstRead )
            isFirstRead = false;
        else
        {
            reader.readNext();
        }

        if (reader.isStartElement()) {
            //qDebug() << "start Element: " << reader.name();

            //QString myReaderText = reader.text().toString();
            //QString myReaderName = reader.name().toString();

            if ((reader.name() == "span") /*|| (reader.name() == "body")*/)
            {
                if (reader.attributes().hasAttribute(QString("style")))
                {
                    // read style attribute data
                    QString styleSpecString = reader.attributes().value("style").toString();
                    // parse attributes based on semicolon
                    QStringList styleSpecItems = styleSpecString.split( QChar(';'),QString::SkipEmptyParts,Qt::CaseInsensitive);
                    foreach(QString styleSpec, styleSpecItems)
                    {
                        // parse style items based
                        QStringList styleTypeItems = styleSpec.split(QChar(':'),QString::KeepEmptyParts,Qt::CaseInsensitive);
                 //       qDebug() << "styleSpecString is: " << styleSpecString;
                 //       qDebug() << "decodeBufferIncomingFontName is: " << decodeBufferIncomingFontName;
                 //       qDebug() << "decodeBufferIncomingFontSize is: " << decodeBufferIncomingFontSize;
                 //       qDebug() << "currentFont is:" << currentFont;
                 //       qDebug() << "Array of styleTypeItems is:" << styleTypeItems;
                        if(styleSpecString.contains("font-family"))
                        {
                            if (styleTypeItems[0].trimmed() == "font-family")
                            currentFont = styleTypeItems[1].replace("'","");
                        }
                        //else currentFont = decodeBufferIncomingFontName;
                        else currentFont = defaultFontFamily;

                        if(styleSpecString.contains("font-size"))
                        {
                            if (styleTypeItems[0].trimmed() == "font-size")
                            currentFontSize = styleTypeItems[1].replace("pt","");
                        }
                        else currentFontSize = defaultFontFamily;
                        currentFontSize = currentFontSize.replace("px","");
                    }
                //    qDebug() << "addDefaultFormat=" << addDefaultFormat;
                //    qDebug() << "useFontChanges  =" << useFontChanges;
                //    qDebug() << "currentFont     =" << currentFont;
                //    qDebug() << "currentFontSize =" << currentFontSize;
                //    qDebug() << "encodedBuffer   =" << encodedBuffer;
                    if((indexOfFontName(currentFont) != labelFormSizeIncomingFont || ui->fontSizeComboBox->findText(currentFontSize) != labelFormSizeIncomingFontSize) || addDefaultFormat)
                    {
                        if(/*addDefaultFormat &&*/ useFontChanges)        // new code implemented - The if part is new
                        {
                            encodedBuffer.append(doubleEscSeq);
                            encodedBuffer.append("fn=");
                            if(currentFont == "SP") currentFont = "Sp";
                            else if (currentFont == "SPLarge") currentFont = "SpLarge";
                            encodedBuffer.append(currentFont);
                            encodedBuffer.append(";ps=");
                            encodedBuffer.append(QString::number(currentFontSize.toInt() / 2));
                            encodedBuffer.append(escSeq);
                            addDefaultFormat = true;
                        }
                    }
                    if(currentFont == defaultFontFamily && currentFontSize == defaultFontSize && useFontChanges)  // new code implemented - the if is new
                    {
                        addDefaultFormat = true;
                    }
                    // Check the font specs with debug
                }
            }
            else if (reader.name() == "br")
            {
            }
            else if (reader.name() == "p")
            {
            }
        }

        // if we have reached the end of a tag
        else if (reader.isEndElement())
        {
            if (reader.attributes().hasAttribute(QString("style")))
            {
            }
        }

        // text of tag
        else if (reader.isCharacters())
        {
           // qDebug() << "chars are: " << reader.text().toString();
            if(reader.text().toString() != "\np, li { white-space: pre-wrap; }\n" && reader.text().toString() != "\n")
            {
                QString intermediateText = reader.text().toString();
                intermediateText = intermediateText.replace('"', "\"\"");

                encodedBuffer.append(intermediateText);
                //encodedBuffer.append(reader.text().toString().replace('"',"\""));
            }
            else if(reader.text().toString() == "\n")
            {
                if(newLineCount > 0)
                {
                    encodedBuffer.append("\n");
                }
                newLineCount++;
            }
           // else if(reader.text().toString() == "\n")
           // {
           //     encodedBuffer.append("\n");
           // }
            //currentText = reader.text().toString();
           // qDebug() << "reader text: " << reader.text().toString();

            //LOG_DEBUG( "characters = %s", currentText.toLatin1().constData() );
        }
    }

    //encodedBuffer.clear();
    //for (int i=0; i < fontFamilies.size(); i++)
   // {
     /*   LOG_DEBUG( "family = %s", fontFamilies[i].toLatin1().constData() );
        LOG_DEBUG( "size = %s", fontSizes[i].toLatin1().constData() );
        LOG_DEBUG( "text = %s", fontTextItems[i].toLatin1().constData() );
*/
   //     if (((lastFont != fontFamilies[i]) || (lastFontSize != fontSizes[i])) /*|| (fontFamilies[i] != defaultFontFamily && fontSizes[i] != defaultFontSize)*/)
  /*      {
            // See if one of these is doubling the output of the first line in the textRegion
            encodedBuffer.append(doubleEscSeq);
            encodedBuffer.append("fn=");
            encodedBuffer.append(fontFamilies[i]);
            encodedBuffer.append(";ps=");
            QString intervalSize;
            intervalSize = fontSizes[i];
           // int what = intervalSize.toInt() / 2;
            encodedBuffer.append(QString::number(intervalSize.toInt() / 2));
            encodedBuffer.append(escSeq);
        }

        // replace " with \" in encoded buffer
        QString textData = fontTextItems[i];
        textData.replace('"',"\"");
        encodedBuffer.append(textData);

        lastFont = fontFamilies[i];
        lastFontSize = fontSizes[i];
        */
   // }

    // right trim all whitespace and carriage control
    encodedBuffer = rstrip(encodedBuffer);
    plainTextBuffer = textEdit->toPlainText();
    /*
    //qDebug() << "XXXXXXXXXXXXXXXXXXXXXencoded buffer is : " << encodedBuffer;


    //encodedBuffer = encodedBuffer.mid(1,encodedBuffer.size()-1);

    QString debugstring = ui->textEditRegion->toPlainText();
    if(fieldName == "description")
    {
        if(ui->textEditRegion->toPlainText() != "")
        {
            formattedDescription = encodedBuffer;
            this->ui->te1Description->setText(ui->textEditRegion->toPlainText());
        }
        else
        {
            formattedDescription = "";
            this->ui->te1Description->clear();
        }
    }
    else if (fieldName == "expandedText")
    {
        if(ui->textEditRegion->toPlainText() != "")
        {
            formattedExpandedText = encodedBuffer;
            this->ui->te5ExpandedText->setText(ui->textEditRegion->toPlainText());
        }
        else
        {
            formattedExpandedText = "";
            this->ui->te5ExpandedText->clear();
            // Debug - The te5ExpandedText should be empty at this point
        }
    }
    else if (fieldName == "expandedText2")
    {
        if(ui->textEditRegion->toPlainText() != "")
        {
            formattedExpandedText2 = encodedBuffer;
            this->ui->te5ExpandedText2->setText(ui->textEditRegion->toPlainText());
        }
        else
        {
            formattedExpandedText2 = "";
            this->ui->te5ExpandedText2->clear();
        }
    }
    else if (fieldName == "specialMessage")
    {
        if(ui->textEditRegion->toPlainText() != "")
        {
            formattedSpecialMessage = encodedBuffer;
            this->ui->te5SpecialMessage->setText(ui->textEditRegion->toPlainText());
        }
        else
        {
            formattedSpecialMessage = "";
            this->ui->te5SpecialMessage->clear();
        }
    }
    else if (fieldName == "productNote")
    {
        if(ui->textEditRegion->toPlainText() != "")
        {
            formattedProductNote = encodedBuffer;
            this->ui->te5ProductNote->setText(ui->textEditRegion->toPlainText());
        }
        else
        {
            formattedProductNote = "";
            this->ui->te5ProductNote->clear();
        }
    }
    else if (fieldName == "nutrifacts")
    {
        if(ui->textEditRegion->toPlainText() != "")
        {
            formattedNutrifacts = encodedBuffer;
            this->ui->te4Nutrifacts->setText(ui->textEditRegion->toPlainText());
        }
        else
        {
            formattedNutrifacts = "";
            this->ui->te4Nutrifacts->clear();
        }
    }
    else if (fieldName == "recipes")
    {
        if(ui->textEditRegion->toPlainText() != "")
        {
            formattedRecipes = encodedBuffer;
            this->ui->te5Recipes->setText(ui->textEditRegion->toPlainText());
        }
        else
        {
            formattedRecipes = "";
            this->ui->te5Recipes->clear();
        }
    }
    else if (fieldName == "cookingInstructions")
    {
        if(ui->textEditRegion->toPlainText() != "")
        {
            formattedCookingInstructions = encodedBuffer;
            this->ui->te5CookingInstructions->setText(ui->textEditRegion->toPlainText());
        }
        else
        {
            formattedCookingInstructions = "";
            this->ui->te5CookingInstructions->clear();
        }
    }
    else if (fieldName == "suggestiveSelling")
    {
        if(ui->textEditRegion->toPlainText() != "")
        {
            formattedSuggestiveSelling = encodedBuffer;
            this->ui->te5SuggestiveSelling->setText(ui->textEditRegion->toPlainText());
        }
        else
        {
            formattedSuggestiveSelling = "";
            this->ui->te5SuggestiveSelling->clear();
        }
    }
    else if (fieldName == "allergens")
    {
        if(ui->textEditRegion->toPlainText() != "")
        {
            formattedAllergens = encodedBuffer;
            this->ui->te5Allergens->setText(ui->textEditRegion->toPlainText());
        }
        else
        {
            formattedAllergens = "";
            this->ui->te5Allergens->clear();
        }

    }
    if(ui->frameReuseText->isVisible())
    {
        ui->teReuse->clear();
        ui->teReuse->setText(ui->textEditRegion->toPlainText());
    }
    this->textEdit->clear();
    //qDebug() << "encodedBuffer final is : " << encodedBuffer;
    defaultFontFamily = internalConfigData->defaultFontName;
    defaultFontSize = QString::number(internalConfigData->defaultFontSize);
    //qDebug() << "this is clearing text";
    QFont clearingFont;
    clearingFont.setFamily(defaultFontFamily);
    clearingFont.setPointSize(defaultFontSize.toInt() * 2);
    font4Backspacing = clearingFont;
    decodeBufferIncomingFontName = "";
    decodeBufferIncomingFontSize = "";
    */
    // right trim all whitespace and carriage control
    //encodedBuffer = rstrip(encodedBuffer);

    //plainTextBuffer = textEdit->toPlainText();
}

QString textEditor::getEncodedEditorContents()
{
    return encodedBuffer;
}

QString textEditor::getPlainTextEditorContents()
{
    return plainTextBuffer;
}

// TODO: I had to implement string right trim
// No right string strip in Qt?
QString textEditor::rstrip(const QString& str, const char *skip) {
  int n = str.size() - 1;
  for (; n >= 0; --n) {
    if (0 == strchr(skip, str.at(n).toLatin1()))
      return str.left(n + 1);
  }
  return "";
}

void textEditor::decodeBuffer(QString contents)
{
   /***

    QStringList fontFamilies;
    QStringList fontSizes;
    QStringList fontTextItems;

    QString attribute;
    QString displayBuffer;
    QXmlStreamWriter stream(&displayBuffer);

    //if (contents.trimmed() == "")
    //    return;

    contents = contents.trimmed();

    // fn=[font family name];ps=[point size];
    QRegExp rx("^fn=([a-z,A-Z,0-9\\s]+);ps=(\\d\\d?)$");

    // Split contents
    QStringList dataItems = contents.split( escSeq );

    QString currentFontFamily = defaultFontFamily;
    QString currentFontSize = defaultFontSize;

    // [ESC][ESC]fn={font family};ps={point size};[ESC][text]
    foreach(QString dataItem, dataItems)
    {
        // if data item is a font specifier
        if (rx.indexIn(dataItem) > -1 )
        {
            // capture the font specification found
            // cap(0) is the entire font specification matched
            // so start at cap(1)
            currentFontFamily = rx.cap(1);
            currentFontSize = rx.cap(2);
        }
        // else text found
        else
        {
            // only add if text found
            if (dataItem.trimmed() != "")
            {
                fontFamilies.append(currentFontFamily);
                fontSizes.append(currentFontSize);
                fontTextItems.append(dataItem);
            }
        }
    }

    // generate HTML representation of input
    stream.setAutoFormatting(false);
    stream.writeStartDocument();
    stream.writeStartElement("html");
    stream.writeStartElement("head");
    stream.writeStartElement("meta");
    stream.writeAttribute("name","qrichtext");
    stream.writeAttribute("content","1");
    stream.writeEndElement(); //meta
   // stream.writeStartElement("style");
   // stream.writeAttribute("type","text/css");
  //  stream.writeCharacters("p, li {white-space: pre-wrap; }");
    stream.writeEndElement(); //head
   // stream.writeStartElement("head");

    stream.writeStartElement("body");
    attribute = "";
    attribute.append(" font-family:'" + defaultFontFamily + "';");
        attribute.append(" font-size:" + defaultFontSize + "px;");


    // attribute.append(" font-family:'" + fontFamilies[0] + "';");
   // attribute.append(" font-size:16px;");
   // attribute.append(" font-weight:400;");
   // attribute.append(" font-style:normal;");
    stream.writeAttribute("style", attribute);

    // traverse all fontspecs in buffer
    for (int i=0; i < fontFamilies.size(); i++)
    {
        // generate span style tag
        attribute = "";

        attribute.append(" font-family:'" + fontFamilies[i] + "';");
        attribute.append(" font-size:" + fontSizes[i] + "pt;");
        stream.writeStartElement("span");
        stream.writeAttribute("style", attribute);
        // span style tag generated
        // see if new line in text and if it is manifest it in html
        // as paragraph tag
        if (fontTextItems[i].count('\n') > 0)
        {
            //split the textitem up looking for multiple \n's

            QStringList newLinesInFontTextItem;
            newLinesInFontTextItem = fontTextItems[i].split("\n", QString::KeepEmptyParts);
            for(int j = 0; j < newLinesInFontTextItem.size(); j++)
            {
                // write start tag for paragraph
                if(j > 0)
                {
                    stream.writeStartElement("br");
                    stream.writeEndElement();
                }

                stream.writeCharacters(newLinesInFontTextItem[j]);
            }

                // write start tag for paragraph
                //stream.writeStartElement("p");
                // write out attribute for paragraph
                //stream.writeAttribute("style"," margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;");

                //stream.writeCharacters(newLinesInFontTextItem[j]);
               // stream.writeEndElement();


        }
        else
        {
            // write out characters
            stream.writeCharacters(fontTextItems[i]);
        }

        //stream.writeEndElement();
         OLD CODE BELOW

        // see if new line in text and if it is manifest it in html
        // as paragraph tag
        if (fontTextItems[i].count('\n') > 0)
        {
            // write start tag for paragraph
            stream.writeStartElement("p");
            // write out attribute for paragraph
            stream.writeAttribute("style"," margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;");
        }

        // generate span style tag
        attribute = "";

        attribute.append(" font-family:'" + fontFamilies[i] + "';");
        attribute.append(" font-size:" + fontSizes[i] + "pt;");
        stream.writeStartElement("span");
        stream.writeAttribute("style", attribute);
        // span style tag generated

        // if more than one font item in list
        if (fontTextItems.length() > 1)
        {
            // if new line in font text
            if (fontTextItems[i].count('\n') > 0)
            {
                // remove the newline since it will be manifested
                // as html paragraph tag
                QString strippedText = rstrip(fontTextItems[i]);
                // write out characters
                stream.writeCharacters(strippedText);
            }
            // no new line in text
            else
            {
                // write out characters
                stream.writeCharacters(fontTextItems[i]);
            }
        }
        // only one character in stream
        else
        {
            // if new line write out BR tag to HTML
            if (fontTextItems[i] == "\n")
            {
                stream.writeStartElement("br");
                stream.writeEndElement();
            }
            // else just write out characters
            else
            {
                stream.writeCharacters(fontTextItems[i]);
            }
        }

        stream.writeEndElement();
        // span tag done

        if (fontTextItems[i].count('\n') > 0)
        {
            // write end tag for paragraph
            stream.writeEndElement();
        }*/
   /***
    }

    stream.writeEndElement(); // body
    stream.writeEndElement(); // html
    stream.writeEndDocument();

    textEdit->document()->clear();
    textEdit->document()->setHtml(displayBuffer);
    QTextCursor cursor = textEdit->textCursor();
    textEdit->mergeCurrentCharFormat(cursor.charFormat());
    if(textEdit->toPlainText() == "")
    {
        QFont defaultFont = QFont(defaultFontFamily, defaultFontSize.toInt());
        this->fontChanged(defaultFont);
        textEdit->setCurrentFont(defaultFont);
    }
***/

    // OLD code from 11/22/2013 below until 3 lines of X's
    //XXXXXXXXXXX
    //XXXXXXXXXXX
    /*
  QStringList fontFamilies;
    QStringList fontSizes;
    QStringList fontTextItems;

    QString attribute;
    QString displayBuffer;
    QXmlStreamWriter stream(&displayBuffer);

    //qDebug() << " decoding is: " << contents;
    //if (contents.trimmed() == "")
    //    return;
    //contents = contents.trimmed();
    // fn=[font family name];ps=[point size];
    QRegExp rx("^fn=([a-z,A-Z,0-9\\s]+);ps=(\\d\\d?)$");

    // Split contents
    QStringList dataItems = contents.split( escSeq );

    // If the size is 1, then no font change sequences were found in the incoming string,
    // therefore, when we save it off to the application, we better not add any on the way out.
    if(dataItems.size() == 1)
        useFontChanges = false;

    QString currentFontFamily;
    if(decodeBufferIncomingFontName != "")
    {
        currentFontFamily = decodeBufferIncomingFontName;
    }
    else
    {
        currentFontFamily = defaultFontFamily;
    }

   // decodeBufferIncomingFontName = "";


    QString currentFontSize;

    if(decodeBufferIncomingFontSize != "")
    {
        currentFontSize = QString::number(decodeBufferIncomingFontSize.toInt() * 2);
    }
    else
    {
        currentFontSize = QString::number(defaultFontSize.toInt() * 2);
    }

   // decodeBufferIncomingFontSize = "";
    // [ESC][ESC]fn={font family};ps={point size};[ESC][text]
    foreach(QString dataItem, dataItems)
    {
        // if data item is a font specifier
        if (rx.indexIn(dataItem) > -1 )
        {
            // capture the font specification found
            // cap(0) is the entire font specification matched
            // so start at cap(1)
            currentFontFamily = rx.cap(1);
            currentFontSize = QString::number(rx.cap(2).toInt() * 2);
        }
        // else text found
        else
        {
            // only add if text found
            //if (dataItem.trimmed() != "")
            {
                fontFamilies.append(currentFontFamily);
                fontSizes.append(currentFontSize);
                fontTextItems.append(dataItem);
            }
        }
    }

    // generate HTML representation of input
    stream.setAutoFormatting(false);
    stream.writeStartDocument();
    stream.writeStartElement("html");
    stream.writeStartElement("head");
    stream.writeStartElement("meta");
    stream.writeAttribute("name","qrichtext");
    stream.writeAttribute("content","1");
    stream.writeEndElement();//meta
    //stream.writeEndElement("head"); //head
    stream.writeEndElement();//head
    stream.writeStartElement("body");
    attribute = "";
   // if(textEdit->toPlainText() == "")
   // {
        QFont defaultFont;
        if(decodeBufferIncomingFontName != "")
        {
            defaultFont.setFamily(decodeBufferIncomingFontName);
            defaultFontFamily = decodeBufferIncomingFontName;
        }
        else
        {
            defaultFont.setFamily(defaultFontFamily);
        }
        if(decodeBufferIncomingFontSize != "")
        {
            defaultFont.setPointSize(decodeBufferIncomingFontSize.toInt() * 2);
            defaultFontSize = QString::number(decodeBufferIncomingFontSize.toInt() * 2);
        }
        else
        {
            defaultFont.setPointSize(defaultFontSize.toInt() * 2);
        }
        this->fontChanged(defaultFont);
        textEdit->setCurrentFont(defaultFont);
   // }
    attribute.append(" font-family:'" + defaultFontFamily + "';");
    attribute.append(" font-size:" + defaultFontSize + "px;");
    stream.writeAttribute("style", attribute);

   stream.writeStartElement("p");
    attribute = "";
    attribute.append(" margin-top:0px;");
    attribute.append(" margin-bottom:0px;");
    attribute.append(" margin-left:0px;");
    attribute.append(" margin-right:0px;");
    attribute.append(" -qt-block-indent:0;");
    attribute.append(" text-indent:0px;");
    stream.writeAttribute("style", attribute);
    bool endfirstPTagEndingNotWritten = true;

//    <p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">

    // traverse all fontspecs in buffer
    for (int i=0; i < fontFamilies.size(); i++)
    {
        // generate span style tag


        attribute = "";

        attribute.append(" font-family:'" + fontFamilies[i] + "';");
        attribute.append(" font-size:" + QString::number(fontSizes[i].toInt()) + "pt;");
        stream.writeStartElement("span");
        stream.writeAttribute("style", attribute);

        QString holder = "";
        if (fontTextItems[i].count('\n') > 0)
        {
            //stream.writeCharacters(fontTextItems[i].replace("\n", "\n"));
            //split the textitem up looking for multiple \n's


            QStringList newLinesInFontTextItem;
            newLinesInFontTextItem = fontTextItems[i].split("\n", QString::KeepEmptyParts);



            for(int j = 0; j < newLinesInFontTextItem.size(); j++)
            {
                // write start tag for paragraph
                if(j > 0)
                {
                    holder = "\n";
                    //stream.writeStartElement("br");
                    stream.writeStartElement("p");
                    attribute = "";
                    attribute.append(" margin-top:0px;");
                    attribute.append(" margin-bottom:0px;");
                    attribute.append(" margin-left:0px;");
                    attribute.append(" margin-right:0px;");
                    attribute.append(" -qt-block-indent:0;");
                    attribute.append(" text-indent:0px;");
                    stream.writeAttribute("style", attribute);
                      //stream.writeEmptyElement("p");
                  //  stream.writeCharacters("\n");
                    //stream.writeStartElement("br");
                    //stream.writeEndElement();

                    if(newLinesInFontTextItem[j] == "")
                    {
                        stream.writeCharacters("");
                        //stream.writeStartElement("br");
                        //stream.writeEndElement();
                    }
                    else stream.writeCharacters(newLinesInFontTextItem[j]);
                    stream.writeEndElement();

                }
                else
                {
                    stream.writeCharacters(newLinesInFontTextItem[j]);
                }
            }

        }
        else
        {
            // write out characters
            stream.writeCharacters(fontTextItems[i]);
        }

        // new SEP 19
        stream.writeEndElement();
    }

    stream.writeEndElement(); // body
    stream.writeEndElement(); // html
    stream.writeEndDocument();

    textEdit->document()->clear();
    //qDebug() << "DECODE display buffer is: " << displayBuffer;

    displayBuffer = rstrip(displayBuffer);
    textEdit->document()->setHtml(displayBuffer);

    QTextCursor cursor = textEdit->textCursor();
    textEdit->mergeCurrentCharFormat(cursor.charFormat());
    //decodeBufferIncomingFontName = "";
   // decodeBufferIncomingFontSize = "";

    //XXXXXXXXXXXXXX
    //XXXXXXXXXXXXXX
    //XXXXXXXXXXXXXX
    */

    // New code below added on 11/22/2013
     QStringList fontFamilies;
     QStringList fontSizes;
     QStringList fontTextItems;

     QString attribute;
     QString displayBuffer;
     QXmlStreamWriter stream(&displayBuffer);

    // qDebug() << " decoding is: " << contents;
     //if (contents.trimmed() == "")
     //    return;
     //contents = contents.trimmed();
     // fn=[font family name];ps=[point size];
     QRegExp rx("^fn=([a-z,A-Z,0-9\\s]+);ps=(\\d\\d?)$");

     // Split contentsll
     QStringList dataItems = contents.split( escSeq );

     // If the size is 1, then no font change sequences were found in the incoming string,
     // therefore, when we save it off to the application, we better not add any on the way out.
     if(dataItems.size() == 1)
         useFontChanges = false;

     foreach(QString dataItem,dataItems)
     {
         qDebug() << "listItem is: " << dataItem;
     }

     QString currentFontFamily;
     if(decodeBufferIncomingFontName != "")
     {
         currentFontFamily = decodeBufferIncomingFontName;
     }
     else
     {
         currentFontFamily = defaultFontFamily;
     }

    // decodeBufferIncomingFontName = "";


     QString currentFontSize;

     if(decodeBufferIncomingFontSize != "")
     {
         currentFontSize = QString::number(decodeBufferIncomingFontSize.toInt() * 2);
     }
     else
     {
         currentFontSize = QString::number(defaultFontSize.toInt() * 2);
     }

    // decodeBufferIncomingFontSize = "";
     // [ESC][ESC]fn={font family};ps={point size};[ESC][text]
     foreach(QString dataItem, dataItems)
     {
         // if data item is a font specifier
         if (rx.indexIn(dataItem) > -1 )
         {
             // capture the font specification found
             // cap(0) is the entire font specification matched
             // so start at cap(1)
             currentFontFamily = rx.cap(1);
             currentFontSize = QString::number(rx.cap(2).toInt() * 2);
         }
         // else text found
         else
         {
             // only add if text found
             //if (dataItem.trimmed() != "")
             {
                 fontFamilies.append(currentFontFamily);
                 fontSizes.append(currentFontSize);
                 fontTextItems.append(dataItem);
             }
         }
     }

     // generate HTML representation of input
     stream.setAutoFormatting(false);
     stream.writeStartDocument();
     stream.writeStartElement("html");
     stream.writeStartElement("head");
     stream.writeStartElement("meta");
     stream.writeAttribute("name","qrichtext");
     stream.writeAttribute("content","1");
     stream.writeEndElement();//meta
     //stream.writeEndElement("head"); //head
     stream.writeEndElement();//head
     stream.writeStartElement("body");
     attribute = "";
    // if(textEdit->toPlainText() == "")
    // {
         QFont defaultFont;
         if(decodeBufferIncomingFontName != "")
         {

             defaultFont.setFamily(decodeBufferIncomingFontName);
             defaultFontFamily = decodeBufferIncomingFontName;
         }
         else
         {
             defaultFont.setFamily(defaultFontFamily);
         }
         if(decodeBufferIncomingFontSize != "")
         {
             defaultFont.setPointSize(decodeBufferIncomingFontSize.toInt() * 2);
             defaultFontSize = QString::number(decodeBufferIncomingFontSize.toInt() * 2);
         }
         else
         {
             defaultFont.setPointSize(defaultFontSize.toInt() * 2);
         }
      //   qDebug() << "defaultFont family is :" << defaultFont.family();
      //   qDebug() << "defaultFont size is: " << QString::number(defaultFont.pointSize());
         this->fontChanged(defaultFont);

         textEdit->setCurrentFont(defaultFont);
         textEdit->document()->setDefaultFont(defaultFont);
    // }
         //OLD
   //  attribute.append(" font-family:'" + defaultFontFamily + "';");
   //  attribute.append(" font-size:" + defaultFontSize + "px;");
         //NEW
        // attribute.append(" text-decoration-color:red;");
         attribute.append(" font-family:'" + defaultFont.family() + "';");
     attribute.append(" font-size:" + QString::number(defaultFont.pointSize()) + "px;");
     stream.writeAttribute("style", attribute);

   /*  stream.writeStartElement("p");
     attribute = "";
     attribute.append(" margin-top:0px;");
     attribute.append(" margin-bottom:0px;");
     attribute.append(" margin-left:0px;");
     attribute.append(" margin-right:0px;");
     attribute.append(" -qt-block-indent:0;");
     attribute.append(" text-indent:0px;");
     stream.writeAttribute("style", attribute);
     bool endfirstPTagEndingNotWritten = true;
     */
 //    <p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">

     // traverse all fontspecs in buffer
     for (int i=0; i < fontFamilies.size(); i++)
     {
         // generate span style tag


         attribute = "";

         attribute.append(" font-family:'" + fontFamilies[i] + "';");
         attribute.append(" font-size:" + QString::number(fontSizes[i].toInt()) + "pt;");
       //  attribute.append(" text-decoration-color: red;");
         stream.writeStartElement("span");
         stream.writeAttribute("style", attribute);

         QString holder = "";
         if (fontTextItems[i].count('\n') > 0)
         {
             //stream.writeCharacters(fontTextItems[i].replace("\n", "\n"));
             //split the textitem up looking for multiple \n's

             QStringList newLinesInFontTextItem;
             newLinesInFontTextItem = fontTextItems[i].split("\n", QString::KeepEmptyParts);

             for(int j = 0; j < newLinesInFontTextItem.size(); j++)
             {
                 // write start tag for paragraph
                 if(j > 0)
                 {
                     holder = "\n";
                     //stream.writeStartElement("br");
                     stream.writeStartElement("p");
                     attribute = "";
                     attribute.append(" margin-top:0px;");
                     attribute.append(" margin-bottom:0px;");
                     attribute.append(" margin-left:0px;");
                     attribute.append(" margin-right:0px;");
                     attribute.append(" -qt-block-indent:0;");
                     attribute.append(" text-indent:0px;");
                     stream.writeAttribute("style", attribute);
                       //stream.writeEmptyElement("p");
                   //  stream.writeCharacters("\n");
                     //stream.writeStartElement("br");
                     //stream.writeEndElement();

                     if(newLinesInFontTextItem[j] == "")
                     {
                         stream.writeCharacters("");
                         //stream.writeStartElement("br");
                         //stream.writeEndElement();
                     }
                     else
                     {
                         QString interText = newLinesInFontTextItem[j].replace(QString("\"\""),QString("\""));
                         stream.writeCharacters(interText);
                         //stream.writeCharacters(newLinesInFontTextItem[j]);
                     }
                     stream.writeEndElement();

                 }
                 else
                 {
                     QString interText1 = newLinesInFontTextItem[j].replace(QString("\"\""),QString("\""));
                     stream.writeCharacters(interText1);
                     //stream.writeCharacters(newLinesInFontTextItem[j]);
                 }
             }

         }
         else
         {
             // write out characters
             QString interText2 = fontTextItems[i].replace(QString("\"\""),QString("\""));
             stream.writeCharacters(interText2);

            // stream.writeCharacters(fontTextItems[i]);
            // stream.writeCharacters("TODD");
         }

         // new SEP 19
         stream.writeEndElement();
     }

     stream.writeEndElement(); // body
     stream.writeEndElement(); // html
     stream.writeEndDocument();

     textEdit->document()->clear();
    // qDebug() << "DECODE display buffer is: " << displayBuffer;

     displayBuffer = rstrip(displayBuffer);
     textEdit->document()->setHtml(displayBuffer);

     QTextCursor cursor = textEdit->textCursor();
     textEdit->mergeCurrentCharFormat(cursor.charFormat());
     //decodeBufferIncomingFontName = "";
    // decodeBufferIncomingFontSize = "";

    // qDebug() << "decode html is: " << textEdit->toHtml();


}

void textEditor::textFamily(const QString &f)
{
    /*
    qDebug() << "font name is : " << f;
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    //fmt.setFont();
    mergeFormatOnWordOrSelection(fmt);
*/
useFontChanges = true;          // if we make changes, then this must be true. Could of been false on the way in

    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    chosenFontIndex = ui->fontComboBox->currentIndex();
    mergeFormatOnWordOrSelection(fmt);

}

void textEditor::textSelect(bool yes)
{
    textSelected = yes;
    ui->cutButton->setEnabled(yes);
    ui->copyButton->setEnabled(yes);
}

void textEditor::textSize(const QString &p)
{
/*    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
*/

 useFontChanges = true;          // if we make changes, then this must be true. Could of been false on the way in

    qreal pointSize = p.toFloat() * 2.0;
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}

void textEditor::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
 /*   QTextCursor cursor = textEdit->textCursor();
    //if (!cursor.hasSelection())
    //    cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);
*/
  QTextCursor cursor = textEdit->textCursor();
    //if (!cursor.hasSelection())
    //    cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);

}
void textEditor::currentCharFormatChanged(const QTextCharFormat &format)
{
 /*   if(ui->textEditRegion->toPlainText() != "")
    {
        fontChanged(format.font());
        font4Backspacing = format.font();
    }
    else
    {
        ui->textEditRegion->setCurrentFont(font4Backspacing);
        //ui->textEditRegion->currentCharFormat().setFont(font4Backspacing);
    }

//    fontChanged(format.font());

*/
   if(ui->textEditRegion->toPlainText() != "")
    {
    	fontChanged(format.font());
        font4Backspacing = format.font();
    }
    else
    {
        if(decodeBufferIncomingFontName != "" && decodeBufferIncomingFontSize != "")
        {
            // change the font4backspacing
            font4Backspacing.setFamily(decodeBufferIncomingFontName);
            font4Backspacing.setPointSize(decodeBufferIncomingFontSize.toInt()* 2);
        }
        ui->textEditRegion->setCurrentFont(font4Backspacing);
        //ui->textEditRegion->currentCharFormat().setFont(font4Backspacing);
    }
}

void textEditor::fontChanged(const QFont &f)
{
   /* //ui->doneButton->setText(f.);
   // ui->fontComboBox->setCurrentIndex();
    //f.rawName();
    ui->fontComboBox->setCurrentIndex(ui->fontComboBox->findText(f.rawName()));
//    ui->fontComboBox->setCurrentIndex(ui->fontComboBox->findText(QFontInfo(f).family()));
    ui->fontSizeComboBox->setCurrentIndex(ui->fontSizeComboBox->findText(QString::number(f.pointSize())));
*/

  QString tempname = f.rawName();
    QString tempfamily = f.family();
    //ui->fontComboBox->setCurrentIndex(chosenFontIndex );
    //ui->fontComboBox->setCurrentIndex( ui->fontComboBox->findText( QFontInfo( f ).family() ) );

    int apple = indexOfFontName(f.family());
    qDebug() << "family is: " << f.family();
    qDebug() << "index is : " << apple;

    ui->fontComboBox->setCurrentIndex(indexOfFontName(f.family()) );

   // if(ui->fontComboBox->currentIndex() != labelFormSizeIncomingFont || ui->fontSizeComboBox->currentIndex() != labelFormSizeIncomingFontSize)
    //{
    //    addDe
    //}
        // ui->fontComboBox->setCurrentFont(f);

   // qDebug() << "name: " << tempname;
   // qDebug() << "family:" << tempfamily;
   // qDebug() << "index: " << ui->fontComboBox->currentIndex();

   // int fontindex = ui->fontComboBox->currentIndex();
    // FT-771: prevent division twice
    if (( f.pointSize() / 2 ) < 5 )
        ui->fontSizeComboBox->setCurrentIndex( 0 );
    else
        ui->fontSizeComboBox->setCurrentIndex(
            ui->fontSizeComboBox->findText( QString::number( f.pointSize() / 2 ) ) );
}

void textEditor::onTextChanged()
{
/*    // only allow TEXT_EDITOR_MAX_CHARS in text editor excluding carriage return
    int numCarriageReturns = textEdit->document()->toPlainText().count('\n');

    if ((textEdit->document()->toPlainText().length() - numCarriageReturns) > 0)
    {
        ui->watermark->hide();
    }
    else
    {
        ui->watermark->show();
    }

    if ((textEdit->document()->toPlainText().length() - numCarriageReturns) > TEXT_EDITOR_MAX_CHARS)
    {
        textEdit->document()->setHtml(lastTextEditHtml);
    }
    else
    {
        lastTextEditHtml = textEdit->document()->toHtml();
    }
*/
 // only allow TEXT_EDITOR_MAX_CHARS in text editor excluding carriage return
    int numCarriageReturns = textEdit->document()->toPlainText().count('\n');

    /*if ((textEdit->document()->toPlainText().length() - numCarriageReturns) > 0)
    {
        ui->watermark->hide();
    }
    else
    {
        ui->watermark->show();
    }
    */
    if ((textEdit->document()->toPlainText().length() - numCarriageReturns) > TEXT_EDITOR_MAX_CHARS)
    {
        textEdit->document()->setHtml(lastTextEditHtml);
        //qDebug() << "htmlHeight is: " << textEdit->size().height();
    }
    else
    {
        lastTextEditHtml = textEdit->document()->toHtml();
        //qDebug() << "htmlHeight1 is: " << textEdit->size().height();

    }


}

int textEditor::indexOfFontName(QString fontName)
{
    // Hobart wants to separately list SPLarge away from its font family SP so this routine is needed to accomodate

    int index = 0;


    index = ui->fontComboBox->findText(fontName);

    // FT-977 Hobart font defaults back to Abalone when Default Label Type is attached to PLU
    // only do this if Sp is not found
    if( -1 == index && fontName == "Sp" )
    {
        // lets try "SP" instead of "Sp"
        fontName = "SP";                            // back to the original
        index = ui->fontComboBox->findText(fontName);
    }
    /*
    if(fontName == "Abalone")
        index = ui->fontComboBox->findText(fontName);
    else if(fontName == "Centurion")
        index = 1;
    else if(fontName == "Everest")
        index = 2;
    else if(fontName == "FreeMono")
        index = 3;
    else if(fontName == "FreeSans")
        index = 4;
    else if(fontName == "Gravilex")
        index = 5;
    else if(fontName == "Hauteclaire")
        index = 6;
    else if(fontName == "Hobart")
        index = ui->fontComboBox->findText(fontName);
    else if(fontName == "HobartBold")
        index = 8;
    else if(fontName == "HobartPlain")
        index = 9;
    else if(fontName == "Liberation Sans")
        index = 10;
    else if(fontName == "Liberation Serif")
        index = 11;
    else if(fontName == "London")
        index = 12;
    else if(fontName == "Patrician")
        index = 13;
    else if(fontName == "Sans Serif")
        index = 14;
    else if(fontName == "Serif")
        index = 15;
    else if(fontName == "SP")
        index = 16;
    else if(fontName == "SPLarge")
        index = 17;
    else if(fontName == "Tancred")
        index = 18;
    else if(fontName == "Ultima")
        index = 19;
    */
    return index;
}

