#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include "../sessiondialog.h"

#include <QFont>
#include <QDialog>
#include <QTextEdit>

#define TEXT_EDITOR_MAX_CHARS 2000
#define ESC_CHAR 0x1B

class MadpInternalConfig;

namespace Ui {
class textEditor;
}

class textEditor : public SessionDialog
{
    Q_OBJECT
    
public:
    explicit textEditor(QWidget *parent = 0, QString fieldName="", bool editable=true, QString initialContents="");
    QString getEncodedEditorContents();
    QString getPlainTextEditorContents();
    ~textEditor();
    void translate();

private slots:
    void textFamily(const QString &f);
    void textSize(const QString &p);
    void currentCharFormatChanged(const QTextCharFormat &format);
    void textSelect(bool yes);
    void doneEditing();
    void onTextChanged();

private:
    Ui::textEditor *ui;
    QFont textFont;
    QFont font4Backspacing;
    QTextEdit *textEdit;
    QString lastTextEditHtml;
    void updateFontSizeComboBox();
    void setTextEditRegionFont();
    void setTextEditRegionFontSize();
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void setFieldNameWatermark(QString fieldName);
    void fontChanged(const QFont &f);
    void encodeBuffer();

    QString rstrip(const QString& str, const char *skip = " \r\n\t");
    void decodeBuffer(QString contents);

    bool textSelected;
    QString encodedBuffer;
    QString plainTextBuffer;
    QString escSeq;
    QString doubleEscSeq;
    QString defaultFontFamily;
    QString defaultFontSize;
    QString defaultLabelWidth;
    QString defaultLabelHeight;
    MadpInternalConfig *internalConfigData;

    bool isLinux();

    QString decodeBufferIncomingFontName;
    QString decodeBufferIncomingFontSize;
    bool useFontChanges;
    int indexOfFontName(QString fontName);
    int labelFormSizeIncomingFont;
    int labelFormSizeIncomingFontSize;
    int chosenFontIndex;
};

#endif // TEXTEDITOR_H
