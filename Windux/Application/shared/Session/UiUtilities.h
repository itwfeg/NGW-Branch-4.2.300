//! UiUtilities.h
/*! UiUtilities is a replacement class for multiple script files the once use to exist in the web based models of scales.
 *  Currently the list of files converted from Javascript to this file consists of operateScaleScripts. Other files still to
 *  be translated include operateProgrammableFields, operateRunScreenDaysDatesScripts, and operateSaveProductInfo. This class
 *  may eventually include methods from MADP, domOperate, domSuper, and domService.
 * @Author: Joe E. Crew
 * @bugs: TBD
 * @addtogroup thinClientMessagingCsu
*/

#ifndef UIUTILITIES_H
#define UIUTILITIES_H

#include <QMap>
#include <QString>
#include <QVector>
#include <QProcess>
#include <QTimer>
#include <QObject>
#include <QFile>
#include "../../source/HFC/CfgBaseFieldOffsets.h"
class Session;
class ScaleConfiguration;

class MadpInternalConfig;

class MadpScaleConfig;
class MadpCurrencySettings;
class MadpPeripheralList;
class DomCurrentPLU;
class DomDefaultCurrentPLU;

/// Scale flags.
enum scaleFlags
{
    // feature set flags
    sfReducedFeatureSet             = 0x000001,     ///< Cheap version (i.e. HTs)

    // model flags
    sfWeigh                         = 0x010000,     ///< Weigher (i.e. HTi)
    sfWrap                          = 0x020000,     ///< Wrapper (i.e. Access)

    // OS related flags
    sfLinux                         = 0x100000,     ///< Linux OS
    sfWindows                       = 0x200000      ///< Windows OS
};

/// Scale models.
enum scaleModelType
{
    smtUnknown,                                     ///< Unknown
    smtPrepack,                                     ///< Prepack (i.e. AWS, CLA)
    smtServiceCounter                               ///< Counter Scale (i.e. HTi, HTx, HTs)
};

/// Scale features.
enum scaleFeature
{
    SCALE_FPP,                                      ///< Fresh production.
    SCALE_FULLSCREEN_FLASHKEYS,                     ///< Left panel flashkeys.
    SCALE_MEAL_BUILDER,                             ///< Build-A-Meal screen.
    SCALE_NOW_SERVING,                              ///< Now serving.
    SCALE_OFFICE_VIEWER,                            ///< Viewing of MS Office documents.
    SCALE_PDF_VIEWER,                               ///< Viewing of PDF documents.
    SCALE_WEB_BROWSER,                              ///< Web browser (all of them).
    SCALE_TRANS_GCD,                                ///< Transactional GCD.
    SCALE_CAT3_AUDITTRAIL,                          ///< Cat3 Audit Trail/Log
    SCALE_REMOTE_PRINTING,                          ///< Walmart remote printing.
    SCALE_PICK5,                                    ///< Pick 5 items for $19.99
    SCALE_CODE_CHECKER,                             ///< Detection of blown printhead dots
    SCALE_DIGIMARC,                                 ///< Digimarc label printing
    SCALE_HANDSFREEDEVICE,                          ///< Hands Free Login
    SCALE_INTERNATIONAL_ROUNDING,                   ///< international illegal rounding
    SCALE_ENHANCED_CENTERSCREEN,                    ///< Full Sized Center Screen for like Deli Vision
    SCALE_ONLINE_ORDERING                           ///< Online ordering
};

/// User interface utility methods.
class UiUtilities : public QObject
{
    Q_OBJECT
    public:

    UiUtilities( Session *session );

    QString updateDateTime() const;
    QString getFullFormattedDate( int month, const QString& day, const QString& year, const QString& hour, const QString& min, const QString& ampm ) const;

    QString youGottaSetTheSeparator( QString& weight ) const;
    QString	youGottaResetTheSeparator( QString& weight ) const;

    QString stripFontChanges( const QString& origString ) const;

    QString setupCapacityString( const QString& capacity, QString& range, QString& dualRange ) const;
    bool isDualRangeCapable() const;
    void rezeroWeigher();

    void boop();
    void clearOperator();
    void clearPLUData();
    // SOF-5067: For FSS scales, automatically enable the ability to print without a GCD
    void checkAllowPrintingNoGcd();

    QString scaleName() const;
    QString scaleModel() const;
    QString scaleCopyright() const;

    scaleFlags scaleNameEx( QString *name ) const;
    scaleModelType scaleModelEx( QString *model ) const;

    bool isFeatureLicensed( scaleFeature f, bool blocking = false ) const;

    QString ascertainHomepage() const;

    // PJR-145: Wrap-only
    bool isWrapOnly();
    bool isPlaying;
    // SOF-5319
    QString checkSpecialConfig();

private:

    Session *session_;
    ScaleConfiguration *scaleConfig_;
    //SessionMainWindow *mainWindow_;

    MadpScaleConfig& madpScaleConfig;
    MadpInternalConfig& internalConfig;
    MadpCurrencySettings& currencySettings;
    MadpPeripheralList& peripheralList;
    DomCurrentPLU& currentPLU;
    DomDefaultCurrentPLU& defaultCurrentPLU;

    int theScaleModel;
    // SOF-5319
    int noChecks;
    QString specialCharacter;

    QMap<scaleFeature, QString> features;

    QVector<QString> format3EN;
    QVector<QString> format3ES;
    QVector<QString> format3FR;

    QVector<QString> format3ENAbbr;
    QVector<QString> format3ESAbbr;
    QVector<QString> format3FRAbbr;
    void resetPlaying();
};

#endif // UIUTILITIES_H
