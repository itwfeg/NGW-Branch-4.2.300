#-------------------------------------------------
#
# Project created by QtCreator 2012-11-13T16:23:57
#
#-------------------------------------------------

QT       += core gui widgets network webkit
QT       += concurrent
QT       += script
QT       += printsupport

# Qt5 uses multimedia
# Qt4 uses phonon
greaterThan( QT_MAJOR_VERSION, 4 ): QT += webkitwidgets multimedia multimediawidgets
else: QT += phonon

TARGET = sessionHandler
TEMPLATE = lib
CONFIG += staticlib

win32-g++: QMAKE_CXXFLAGS += -Wno-multichar
else:linux-g++: QMAKE_CXXFLAGS += -Wno-multichar
else:linux-g++-64: QMAKE_CXXFLAGS += -Wno-multichar
else:linux-timesys-g++: QMAKE_CXXFLAGS += -Wno-multichar

SOURCES += \
    dialogs/aboutdialog.cpp \
    dialogs/calendardialog.cpp \
    dialogs/camerapredictionsdialog.cpp \
    dialogs/colordialog.cpp \
    dialogs/cooldialog.cpp \
    dialogs/cuttingtestdialog.cpp \
    dialogs/dcadialog.cpp \
    dialogs/expandedtextdialog.cpp \
    dialogs/featuresdialog.cpp \
    dialogs/filedialog.cpp \
    dialogs/filemandialog.cpp \
    dialogs/graphicchooserdialog.cpp \
    dialogs/graphicdialog.cpp \
    dialogs/licensingdialog.cpp \
    dialogs/listdialog.cpp \
    dialogs/manufacturingImageDialog.cpp \
    dialogs/messageboxdialog.cpp \
    dialogs/networkinfodialog.cpp \
    dialogs/nowservingdialog.cpp \
    dialogs/operatemodedialog.cpp \
    dialogs/operatorlogindialog.cpp \
    dialogs/planogramdialog.cpp \
    dialogs/portionzerodialog.cpp \
    dialogs/printercleanerdialog.cpp \
    dialogs/printersettingsdialog.cpp \
    dialogs/productionentryprintingdialog.cpp \
    dialogs/productnotedialog.cpp \
    dialogs/recalldialog.cpp \
    dialogs/recallviewerdialog.cpp \
    dialogs/removePeripheralsDialog.cpp \
    dialogs/scaleauditdialog.cpp \
    dialogs/searchdialog.cpp \
    dialogs/sessionerrordialog.cpp \
    dialogs/texteditor.cpp \
    dialogs/timeEntryDialog.cpp \
    dialogs/webbrowserdialog.cpp \
    dialogs/weightsandmeasuresdialog.cpp \
    dialogs/wrappercalibrationdialog.cpp \
    dialogs/wrapperforcetraysizedialog.cpp \
    dialogs/wrappermodedialog.cpp \
    inputwidget.cpp \
    keyboardwidget.cpp \
    modes/modewidget.cpp \
    modes/operatemode.cpp \
    modes/servicemode.cpp \
    modes/supervisormode.cpp \
    numberpadwidget.cpp \
    overlaywidget.cpp \
    panels/cooltextpanel.cpp \
    panels/expirationhistorypanel.cpp \
    panels/loginpanel.cpp \
    panels/mainmenupanel.cpp \
    panels/modepanel.cpp \
    panels/operatemodepanel.cpp \
    panels/operatemodepanelex.cpp \
    panels/panelwidget.cpp \
    panels/servicemodeloginpanel.cpp \
    panels/servicemodepanel.cpp \
    panels/supervisormodepanel.cpp \
    panels/totaltypespanel.cpp \
    panels/voidtransactionpanel.cpp \
    panels/wrapperservicelabelapplierpanel.cpp \
    panels/wrapperservicemodepanel.cpp \
    panels/wrapperservicemodetestspanel.cpp \
    screens/appsscreen.cpp \
    screens/BackupRestore.cpp \
    screens/buildamealscreen.cpp \
    screens/capacitybarwidget.cpp \
    screens/checkUpgradeAccessCLA.cpp \
    screens/checkUpgradeAccessWrapper.cpp \
    screens/cuttingtestreport.cpp \
    screens/deleteAllFiles.cpp \
    screens/deviceTests.cpp \
    screens/emptyscreen.cpp \
    screens/flashkeyeditor.cpp \
    screens/labelFieldsToPrint.cpp \
    screens/labelGraphics.cpp \
    screens/labelplacement.cpp \
    screens/labelTextNames.cpp \
    screens/labeltypesscreen.cpp \
    screens/manufacturingtest.cpp \
    screens/marqueeconfigscreen.cpp \
    screens/marqueeeditorscreen.cpp \
    screens/operateNewWrapperRunIn.cpp \
    screens/operatorsCanModify.cpp \
    screens/pluinfoscreen.cpp \
    screens/plumainscreen.cpp \
    screens/pluoperatescreen.cpp \
    screens/printerLocations.cpp \
    screens/productdescriptioneditorscreen.cpp \
    screens/productmarqueeeditorscreen.cpp \
    screens/removefirmwarefiles.cpp \
    screens/scaleStatistics.cpp \
    screens/screenwidget.cpp \
    screens/serviceCurrencySetup.cpp \
    screens/serviceCustomerDisplayOption.cpp \
    screens/serviceFrequentShopper.cpp \
    screens/serviceHealthAgent.cpp \
    screens/serviceNewWrapperLAConfig.cpp \
    screens/serviceinstallationlog.cpp \
    screens/serviceLabelApplierConfig.cpp \
    screens/serviceLabelApplierInfo.cpp \
    screens/serviceLabelApplierSensors.cpp \
    screens/serviceLabelApplierTests.cpp \
    screens/serviceNewWrapperAdvancedTimings.cpp \
    screens/serviceNewWrapperBspCamera.cpp \
    screens/serviceNewWrapperCalibration.cpp \
    screens/serviceNewWrapperConfig.cpp \
    screens/serviceNewWrapperEditBox.cpp \
    screens/serviceNewWrapperErrorHistory.cpp \
    screens/serviceNewWrapperInterlocks.cpp \
    screens/serviceNewWrapperOneHeightBoxParam.cpp \
    screens/serviceNewWrapperOtherTimings.cpp \
    screens/serviceNewWrapperPkgHistory.cpp \
    screens/serviceNewWrapperSetTimings.cpp \
    screens/serviceNewWrapperSystemInfo.cpp \
    screens/serviceNewWrapperTestAirCompressor.cpp \
    screens/serviceNewWrapperTestCamera.cpp \
    screens/serviceNewWrapperTestElevator.cpp \
    screens/serviceNewWrapperTestFilmSupply.cpp \
    screens/serviceNewWrapperTestGripper.cpp \
    screens/serviceNewWrapperTestIntake.cpp \
    screens/serviceNewWrapperTestKnife.cpp \
    screens/serviceNewWrapperTestLabelApplier.cpp \
    screens/serviceNewWrapperTestSealerBelt.cpp \
    screens/serviceNewWrapperTestSideClamps.cpp \
    screens/serviceNewWrapperTestUnderfolders.cpp \
    screens/serviceNewWrapperTestUpperPusher.cpp \
    screens/serviceNewWrapperVersionInfo.cpp \
    screens/servicePeripheralList.cpp \
    screens/servicePricingMode.cpp \
    screens/servicePrinterConfig.cpp \
    screens/servicePrinterPowerupTests.cpp \
    screens/servicePrinterSensors.cpp \
    screens/serviceRemoveLabels.cpp \
    screens/serviceSecurityLabels.cpp \
    screens/serviceSupervisorOsAccess.cpp \
    screens/serviceWeigherCalibrate.cpp \
    screens/serviceWeigherConfig.cpp \
    screens/serviceWeigherInfo.cpp \
    screens/serviceWeigherPowerupTestResults.cpp \
    screens/setupUtility.cpp \
    screens/shelfLifeExpiration.cpp \
    screens/storeInformation.cpp \
    screens/superApplierRotationOverride.cpp \
    screens/superCategories.cpp \
    screens/superChangeProductAllFields.cpp \
    screens/superChangeProductClass.cpp \
    screens/superChangeProductPriceFields.cpp \
    screens/superChangeProductShelfLife.cpp \
    screens/superChangeProductTare.cpp \
    screens/superChangeProductUnitPrice.cpp \
    screens/superCommInterface.cpp \
    screens/superCoolCreateCategories.cpp \
    screens/superCoolCreateCountries.cpp \
    screens/superCoolCreatePretext.cpp \
    screens/superCoolCreateProdSteps.cpp \
    screens/superCoolMain.cpp \
    screens/superCustomerDisplay.cpp \
    screens/superEmailSetup.cpp \
    screens/superHourlyTotals.cpp \
    screens/superLabelPlacementView.cpp \
    screens/superNormalScaleOperation.cpp \
    screens/supernutritioneditor.cpp \
    screens/superOperatorChanges.cpp \
    screens/superOperatorIds.cpp \
    screens/superOperatorTotals.cpp \
    screens/superPrintConfig.cpp \
    screens/superPrinterSetup.cpp \
    screens/superPrintSetup.cpp \
    screens/superProductDefaults.cpp \
    screens/superProductTotals.cpp \
    screens/superReportingClasses.cpp \
    screens/superScaleManagement.cpp \
    screens/superScaleSynchronization.cpp \
    screens/superScannedCodes.cpp \
    screens/superSecurityLabels.cpp \
    screens/superSpecials.cpp \
    screens/supertextfileeditor.cpp \
    screens/superTotalTypesSetup.cpp \
    screens/superUpgradeFirmwareViaSFTP.cpp \
    screens/superUpgradeFirmwareViaUSB.cpp \
    screens/supervoidtransactions.cpp \
    screens/systemDiagnostics.cpp \
    screens/testscreen.cpp \
    screens/upgradeAccessCLA.cpp \
    screens/upgradeAccessWrapper.cpp \
    screens/webbrowserscreen.cpp \
    screens/weightbarwidget.cpp \
    screens/wrappersettings.cpp \
    sessiondialog.cpp \
    sessiongesturerecognizer.cpp \
    sessionmainwindow.cpp \
    sessionmainwindowoverlay.cpp \
    sessionmenupanel.cpp \
    touchscreenstyle.cpp \
    widgets/accumulateitems.cpp \
    widgets/buildamealwidget.cpp \
    widgets/digitEntry.cpp \
    widgets/dynamiccontentwidget.cpp \
    widgets/dynamicsoftkeybutton.cpp \
    widgets/eanEditor.cpp \
    widgets/gs1Editor.cpp \
    widgets/labeledcooledit.cpp \
    widgets/labeleddateedit.cpp \
    widgets/labeleddoublelineedit.cpp \
    widgets/labeleddoublelineeditarray.cpp \
    widgets/labeleddoublenumericlineedit.cpp \
    widgets/labeleddoublenumericlineeditarray.cpp \
    widgets/labeledgraphicedit.cpp \
    widgets/labeledgraphiceditarray.cpp \
    widgets/labeledlabelviewer.cpp \
    widgets/labeledlineedit.cpp \
    widgets/labeledlineeditarray.cpp \
    widgets/labelednumericlineedit.cpp \
    widgets/labeledplaintextedit.cpp \
    widgets/labeledpluedit.cpp \
    widgets/labeledunitpriceedit.cpp \
    widgets/labeledupcedit.cpp \
    widgets/labelplacementwidget.cpp \
    widgets/labelpreviewwidget.cpp \
    widgets/pdfviewer.cpp \
    widgets/printPrompt.cpp \
    widgets/proxyautoconfig.cpp \
    widgets/runtotalswidget.cpp \
    widgets/shortListEditor.cpp \
    widgets/stackedwebviewer.cpp \
    widgets/webpage.cpp \
    widgets/webview.cpp \
    widgets/webViewer.cpp \
    widgets/webviewernetwork.cpp \
    widgets/webviewernetworkproxyfactory.cpp \
    widgets/weigherCalibrate.cpp \
    widgets/wrappermodewidget.cpp \
    dialogs/exportnetworkdialog.cpp \
    screens/cuttingTestHistory.cpp \
    screens/cuttingTestDetails.cpp \
    widgets/meatspecsheetwidget.cpp \
    screens/meatspecsheetscreen.cpp \
    screens/cuttingTestInitialInformation.cpp \
    screens/serviceWeigherAuditLog.cpp \
    dialogs/auditlogdialog.cpp \
    screens/updateNow.cpp \
    screens/sendToDept.cpp \
    screens/superSupervisorChanges.cpp \
    panels/sizelabelspanel.cpp \
    panels/wrapperservicemodediagnosticspanel.cpp \
    screens/serviceNewWrapperDiagActuators.cpp \
    screens/upgradeNextGenWrapper.cpp \
    screens/diagwizardscreen.cpp \
    dialogs/diagresolutiondialog.cpp \
    dialogs/wrapperauditdialog.cpp \
    dialogs/firmwareversiondialog.cpp \
    panels/deptpanel.cpp \
    screens/superDepartments.cpp \
    screens/serviceWeigherReset.cpp \
    screens/servicePrinterReset.cpp \
    widgets/datamatrixEditor.cpp \
    widgets/minandmaxprintwidget.cpp \
    screens/servicePrinterHeadDotStatus.cpp \
    screens/upgradeKPCSoftware.cpp

HEADERS += \
    dialogs/aboutdialog.h \
    dialogs/calendardialog.h \
    dialogs/camerapredictionsdialog.h \
    dialogs/colordialog.h \
    dialogs/cooldialog.h \
    dialogs/cuttingtestdialog.h \
    dialogs/dcadialog.h \
    dialogs/expandedtextdialog.h \
    dialogs/featuresdialog.h \
    dialogs/filedialog.h \
    dialogs/filemandialog.h \
    dialogs/graphicchooserdialog.h \
    dialogs/graphicdialog.h \
    dialogs/inputboxdialog.h \
    dialogs/licensingdialog.h \
    dialogs/listdialog.h \
    dialogs/manufacturingImageDialog.h \
    dialogs/messageboxdialog.h \
    dialogs/networkinfodialog.h \
    dialogs/nowservingdialog.h \
    dialogs/numericinputboxdialog.h \
    dialogs/operatemodedialog.h \
    dialogs/operatorlogindialog.h \
    dialogs/planogramdialog.h \
    dialogs/portionzerodialog.h \
    dialogs/printercleanerdialog.h \
    dialogs/printersettingsdialog.h \
    dialogs/productionentryprintingdialog.h \
    dialogs/productnotedialog.h \
    dialogs/recalldialog.h \
    dialogs/recallviewerdialog.h \
    dialogs/removePeripheralsDialog.h \
    dialogs/scaleauditdialog.h \
    dialogs/searchdialog.h \
    dialogs/sessionerrordialog.h \
    dialogs/texteditor.h \
    dialogs/textinputboxdialog.h \
    dialogs/timeEntryDialog.h \
    dialogs/webbrowserdialog.h \
    dialogs/weightsandmeasuresdialog.h \
    dialogs/wrappercalibrationdialog.h \
    dialogs/wrapperforcetraysizedialog.h \
    dialogs/wrappermodedialog.h \
    inputwidget.h \
    keyboardwidget.h \
    modes/modewidget.h \
    modes/operatemode.h \
    modes/servicemode.h \
    modes/supervisormode.h \
    numberpadwidget.h \
    overlaywidget.h \
    panels/cooltextpanel.h \
    panels/expirationhistorypanel.h \
    panels/loginpanel.h \
    panels/mainmenupanel.h \
    panels/modepanel.h \
    panels/operatemodepanel.h \
    panels/operatemodepanelex.h \
    panels/panelwidget.h \
    panels/servicemodeloginpanel.h \
    panels/servicemodepanel.h \
    panels/supervisormodepanel.h \
    panels/totaltypespanel.h \
    panels/voidtransactionpanel.h \
    panels/wrapperservicelabelapplierpanel.h \
    panels/wrapperservicemodepanel.h \
    panels/wrapperservicemodetestspanel.h \
    screens/appsscreen.h \
    screens/BackupRestore.h \
    screens/buildamealscreen.h \
    screens/capacitybarwidget.h \
    screens/checkUpgradeAccessCLA.h \
    screens/checkUpgradeAccessWrapper.h \
    screens/cuttingtestreport.h \
    screens/deleteAllFiles.h \
    screens/deviceTests.h \
    screens/emptyscreen.h \
    screens/flashkeyeditor.h \
    screens/labelFieldsToPrint.h \
    screens/labelGraphics.h \
    screens/labelplacement.h \
    screens/labelTextNames.h \
    screens/labeltypesscreen.h \
    screens/manufacturingtest.h \
    screens/marqueeconfigscreen.h \
    screens/marqueeeditorscreen.h \
    screens/operateNewWrapperRunIn.h \
    screens/operatorsCanModify.h \
    screens/pluinfoscreen.h \
    screens/plumainscreen.h \
    screens/pluoperatescreen.h \
    screens/printerLocations.h \
    screens/productdescriptioneditorscreen.h \
    screens/productmarqueeeditorscreen.h \
    screens/removefirmwarefiles.h \
    screens/scaleStatistics.h \
    screens/screenwidget.h \
    screens/serviceCurrencySetup.h \
    screens/serviceCustomerDisplayOption.h \
    screens/serviceFrequentShopper.h \
    screens/serviceHealthAgent.h \
    screens/serviceNewWrapperLAConfig.h \
    screens/serviceinstallationlog.h \
    screens/serviceLabelApplierConfig.h \
    screens/serviceLabelApplierInfo.h \
    screens/serviceLabelApplierSensors.h \
    screens/serviceLabelApplierTests.h \
    screens/serviceNewWrapperAdvancedTimings.h \
    screens/serviceNewWrapperBspCamera.h \
    screens/serviceNewWrapperCalibration.h \
    screens/serviceNewWrapperConfig.h \
    screens/serviceNewWrapperEditBox.h \
    screens/serviceNewWrapperErrorHistory.h \
    screens/serviceNewWrapperInterlocks.h \
    screens/serviceNewWrapperOneHeightBoxParam.h \
    screens/serviceNewWrapperOtherTimings.h \
    screens/serviceNewWrapperPkgHistory.h \
    screens/serviceNewWrapperSetTimings.h \
    screens/serviceNewWrapperSystemInfo.h \
    screens/serviceNewWrapperTestAirCompressor.h \
    screens/serviceNewWrapperTestCamera.h \
    screens/serviceNewWrapperTestElevator.h \
    screens/serviceNewWrapperTestFilmSupply.h \
    screens/serviceNewWrapperTestGripper.h \
    screens/serviceNewWrapperTestIntake.h \
    screens/serviceNewWrapperTestKnife.h \
    screens/serviceNewWrapperTestLabelApplier.h \
    screens/serviceNewWrapperTestSealerBelt.h \
    screens/serviceNewWrapperTestSideClamps.h \
    screens/serviceNewWrapperTestUnderfolders.h \
    screens/serviceNewWrapperTestUpperPusher.h \
    screens/serviceNewWrapperVersionInfo.h \
    screens/servicePeripheralList.h \
    screens/servicePricingMode.h \
    screens/servicePrinterConfig.h \
    screens/servicePrinterPowerupTests.h \
    screens/servicePrinterSensors.h \
    screens/serviceRemoveLabels.h \
    screens/serviceSecurityLabels.h \
    screens/serviceSupervisorOsAccess.h \
    screens/serviceWeigherCalibrate.h \
    screens/serviceWeigherConfig.h \
    screens/serviceWeigherInfo.h \
    screens/serviceWeigherPowerupTestResults.h \
    screens/setupUtility.h \
    screens/shelfLifeExpiration.h \
    screens/storeInformation.h \
    screens/superApplierRotationOverride.h \
    screens/superCategories.h \
    screens/superChangeProductAllFields.h \
    screens/superChangeProductClass.h \
    screens/superChangeProductPriceFields.h \
    screens/superChangeProductShelfLife.h \
    screens/superChangeProductTare.h \
    screens/superChangeProductUnitPrice.h \
    screens/superCommInterface.h \
    screens/superCoolCreateCategories.h \
    screens/superCoolCreateCountries.h \
    screens/superCoolCreatePretext.h \
    screens/superCoolCreateProdSteps.h \
    screens/superCoolMain.h \
    screens/superCustomerDisplay.h \
    screens/superEmailSetup.h \
    screens/superHourlyTotals.h \
    screens/superLabelPlacementView.h \
    screens/superNormalScaleOperation.h \
    screens/supernutritioneditor.h \
    screens/superOperatorChanges.h \
    screens/superOperatorIds.h \
    screens/superOperatorTotals.h \
    screens/superPrintConfig.h \
    screens/superPrinterSetup.h \
    screens/superPrintSetup.h \
    screens/superProductDefaults.h \
    screens/superProductTotals.h \
    screens/superReportingClasses.h \
    screens/superScaleManagement.h \
    screens/superScaleSynchronization.h \
    screens/superScannedCodes.h \
    screens/superSecurityLabels.h \
    screens/superSpecials.h \
    screens/supertextfileeditor.h \
    screens/superTotalTypesSetup.h \
    screens/superUpgradeFirmwareViaSFTP.h \
    screens/superUpgradeFirmwareViaUSB.h \
    screens/supervoidtransactions.h \
    screens/systemDiagnostics.h \
    screens/testscreen.h \
    screens/upgradeAccessCLA.h \
    screens/upgradeAccessWrapper.h \
    screens/webbrowserscreen.h \
    screens/weightbarwidget.h \
    screens/wrappersettings.h \
    sessiondialog.h \
    sessiongesturerecognizer.h \
    sessioninfo.h \
    sessionmainwindow.h \
    sessionmainwindowimpl.h \
    sessionmenupanel.h \
    touchscreenstyle.h \
    widgets/accumulateitems.h \
    widgets/buildamealwidget.h \
    widgets/digitEntry.h \
    widgets/dynamiccontentwidget.h \
    widgets/dynamicsoftkeybutton.h \
    widgets/eanEditor.h \
    widgets/gs1Editor.h \
    widgets/labeledarraywidget.h \
    widgets/labeledcooledit.h \
    widgets/labeleddateedit.h \
    widgets/labeleddoublelineedit.h \
    widgets/labeleddoublelineeditarray.h \
    widgets/labeleddoublenumericlineedit.h \
    widgets/labeleddoublenumericlineeditarray.h \
    widgets/labeledgraphicedit.h \
    widgets/labeledgraphiceditarray.h \
    widgets/labeledlabelviewer.h \
    widgets/labeledlineedit.h \
    widgets/labeledlineeditarray.h \
    widgets/labelednumericlineedit.h \
    widgets/labeledplaintextedit.h \
    widgets/labeledpluedit.h \
    widgets/labeledunitpriceedit.h \
    widgets/labeledupcedit.h \
    widgets/labeledwidget.h \
    widgets/labelplacementwidget.h \
    widgets/labelpreviewwidget.h \
    widgets/pdfviewer.h \
    widgets/printPrompt.h \
    widgets/proxyautoconfig.h \
    widgets/qwebviewselectionsuppressor.h \
    widgets/runtotalswidget.h \
    widgets/shortListEditor.h \
    widgets/stackedwebviewer.h \
    widgets/webpage.h \
    widgets/webview.h \
    widgets/webViewer.h \
    widgets/webviewernetwork.h \
    widgets/webviewernetworkproxyfactory.h \
    widgets/weigherCalibrate.h \
    widgets/wrappermodewidget.h \
    dialogs/exportnetworkdialog.h \
    screens/cuttingTestHistory.h \
    screens/cuttingTestDetails.h \
    widgets/meatspecsheetwidget.h \
    screens/meatspecsheetscreen.h \
    screens/cuttingTestInitialInformation.h \
    screens/serviceWeigherAuditLog.h \
    dialogs/auditlogdialog.h \
    screens/updateNow.h \
    screens/sendToDept.h \
    screens/superSupervisorChanges.h \
    panels/sizelabelspanel.h \
    panels/wrapperservicemodediagnosticspanel.h \
    screens/serviceNewWrapperDiagActuators.h \
    screens/upgradeNextGenWrapper.h \
    screens/diagwizardscreen.h \
    dialogs/diagresolutiondialog.h \
    dialogs/wrapperauditdialog.h \
    dialogs/firmwareversiondialog.h \
    panels/deptpanel.h \
    screens/superDepartments.h \
    screens/serviceWeigherReset.h \
    screens/servicePrinterReset.h \
    widgets/datamatrixEditor.h \
    widgets/minandmaxprintwidget.h \
    screens/servicePrinterHeadDotStatus.h \
    screens/upgradeKPCSoftware.h


RESOURCES += \
    sessionhandler.qrc
    
FORMS += \
    dialogs/filemandialog.ui \
    dialogs/manufacturingImageDialog.ui \
    dialogs/printercleanerdialog.ui \
    dialogs/removePeripheralsDialog.ui \
    dialogs/texteditor.ui \
    dialogs/timeEntryDialog.ui \
    dialogs/weightsandmeasuresdialog.ui \
    screens/BackupRestore.ui \
    screens/checkUpgradeAccessCLA.ui \
    screens/checkUpgradeAccessWrapper.ui \
    screens/deleteAllFiles.ui \
    screens/labelFieldsToPrint.ui \
    screens/labelGraphics.ui \
    screens/labelTextNames.ui \
    screens/manufacturingtest.ui \
    screens/operateNewWrapperRunIn.ui \
    screens/operatorsCanModify.ui \
    screens/printerLocations.ui \
    screens/removefirmwarefiles.ui \
    screens/scaleStatistics.ui \
    screens/serviceCurrencySetup.ui \
    screens/serviceCustomerDisplayOption.ui \
    screens/serviceFrequentShopper.ui \
    screens/serviceNewWrapperLAConfig.ui \
    screens/serviceinstallationlog.ui \
    screens/serviceLabelApplierConfig.ui \
    screens/serviceLabelApplierInfo.ui \
    screens/serviceLabelApplierSensors.ui \
    screens/serviceLabelApplierTests.ui \
    screens/serviceNewWrapperAdvancedTimings.ui \
    screens/serviceNewWrapperConfig.ui \
    screens/serviceNewWrapperEditBox.ui \
    screens/serviceNewWrapperInterlocks.ui \
    screens/serviceNewWrapperOneHeightBoxParam.ui \
    screens/serviceNewWrapperOtherTimings.ui \
    screens/serviceNewWrapperSetTimings.ui \
    screens/serviceNewWrapperSystemInfo.ui \
    screens/serviceNewWrapperTestAirCompressor.ui \
    screens/serviceNewWrapperTestCamera.ui \
    screens/serviceNewWrapperTestElevator.ui \
    screens/serviceNewWrapperTestFilmSupply.ui \
    screens/serviceNewWrapperTestGripper.ui \
    screens/serviceNewWrapperTestIntake.ui \
    screens/serviceNewWrapperTestLabelApplier.ui \
    screens/serviceNewWrapperTestSealerBelt.ui \
    screens/serviceNewWrapperTestUpperPusher.ui \
    screens/serviceNewWrapperVersionInfo.ui \
    screens/servicePeripheralList.ui \
    screens/servicePricingMode.ui \
    screens/servicePrinterConfig.ui \
    screens/servicePrinterPowerupTests.ui \
    screens/servicePrinterSensors.ui \
    screens/serviceRemoveLabels.ui \
    screens/serviceSecurityLabels.ui \
    screens/serviceSupervisorOsAccess.ui \
    screens/serviceWeigherConfig.ui \
    screens/serviceWeigherInfo.ui \
    screens/serviceWeigherPowerupTestResults.ui \
    screens/setupUtility.ui \
    screens/shelfLifeExpiration.ui \
    screens/storeInformation.ui \
    screens/superApplierRotationOverride.ui \
    screens/superCategories.ui \
    screens/superChangeProductAllFields.ui \
    screens/superChangeProductClass.ui \
    screens/superChangeProductPriceFields.ui \
    screens/superChangeProductShelfLife.ui \
    screens/superChangeProductTare.ui \
    screens/superChangeProductUnitPrice.ui \
    screens/superCommInterface.ui \
    screens/superCoolCreateCategories.ui \
    screens/superCoolCreateCountries.ui \
    screens/superCoolCreatePretext.ui \
    screens/superCoolCreateProdSteps.ui \
    screens/superCoolMain.ui \
    screens/superCustomerDisplay.ui \
    screens/superEmailSetup.ui \
    screens/superHourlyTotals.ui \
    screens/superNormalScaleOperation.ui \
    screens/superOperatorChanges.ui \
    screens/superOperatorIds.ui \
    screens/superOperatorTotals.ui \
    screens/superPrintConfig.ui \
    screens/superPrinterSetup.ui \
    screens/superPrintSetup.ui \
    screens/superProductDefaults.ui \
    screens/superProductTotals.ui \
    screens/superReportingClasses.ui \
    screens/superScaleManagement.ui \
    screens/superScaleSynchronization.ui \
    screens/superScannedCodes.ui \
    screens/superSecurityLabels.ui \
    screens/superSpecials.ui \
    screens/superTotalTypesSetup.ui \
    screens/superUpgradeFirmwareViaSFTP.ui \
    screens/superUpgradeFirmwareViaUSB.ui \
    screens/supervoidtransactions.ui \
    screens/systemDiagnostics.ui \
    screens/upgradeAccessCLA.ui \
    screens/upgradeAccessWrapper.ui \
    widgets/digitEntry.ui \
    widgets/eanEditor.ui \
    widgets/gs1Editor.ui \
    widgets/pdfviewer.ui \
    widgets/printPrompt.ui \
    widgets/shortListEditor.ui \
    widgets/webViewer.ui \
    widgets/weigherCalibrate.ui \
    screens/cuttingTestHistory.ui \
    screens/cuttingTestDetails.ui \
    screens/cuttingTestInitialInformation.ui \
    screens/serviceWeigherAuditLog.ui \
    dialogs/auditlogdialog.ui \
    screens/updateNow.ui \
    screens/sendToDept.ui \
    screens/superSupervisorChanges.ui \
    screens/serviceWeigherReset.ui \
    screens/servicePrinterReset.ui \
    widgets/datamatrixEditor.ui \
    dialogs/firmwareversiondialog.ui \
    widgets/minandmaxprintwidget.ui \
    screens/servicePrinterHeadDotStatus.ui \
    screens/upgradeKPCSoftware.ui


INCLUDEPATH += $$PWD/widgets

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../videoPlayerCore/release/ -lvideoPlayerCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../videoPlayerCore/debug/ -lvideoPlayerCore
else:symbian: LIBS += -lvideoPlayerCore
else:unix: LIBS += -L$$OUT_PWD/../videoPlayerCore/ -lvideoPlayerCore

INCLUDEPATH += $$PWD/../videoPlayerCore
DEPENDPATH += $$PWD/../videoPlayerCore

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../videoPlayerCore/release/videoPlayerCore.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../videoPlayerCore/debug/videoPlayerCore.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../videoPlayerCore/libvideoPlayerCore.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Session/release/ -lSession
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Session/debug/ -lSession
else:symbian: LIBS += -lSession
else:unix: LIBS += -L$$OUT_PWD/../Session/ -lSession

INCLUDEPATH += $$PWD/../Session
DEPENDPATH += $$PWD/../Session

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Session/release/Session.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Session/debug/Session.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../Session/libSession.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../marqueeTextEditorWidget/release/ -lmarqueeTextEditorWidget
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../marqueeTextEditorWidget/debug/ -lmarqueeTextEditorWidget
else:symbian: LIBS += -lmarqueeTextEditorWidget
else:unix: LIBS += -L$$OUT_PWD/../marqueeTextEditorWidget/ -lmarqueeTextEditorWidget

INCLUDEPATH += $$PWD/../marqueeTextEditorWidget
DEPENDPATH += $$PWD/../marqueeTextEditorWidget

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../marqueeTextEditorWidget/release/marqueeTextEditorWidget.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../marqueeTextEditorWidget/debug/marqueeTextEditorWidget.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../marqueeTextEditorWidget/libmarqueeTextEditorWidget.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../marqueeLabelWidget/release/ -lmarqueeLabelWidget
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../marqueeLabelWidget/debug/ -lmarqueeLabelWidget
else:symbian: LIBS += -lmarqueeLabelWidget
else:unix: LIBS += -L$$OUT_PWD/../marqueeLabelWidget/ -lmarqueeLabelWidget

INCLUDEPATH += $$PWD/../marqueeLabelWidget
DEPENDPATH += $$PWD/../marqueeLabelWidget

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../marqueeLabelWidget/release/marqueeLabelWidget.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../marqueeLabelWidget/debug/marqueeLabelWidget.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../marqueeLabelWidget/libmarqueeLabelWidget.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../labelViewerWidget/release/ -llabelViewerWidget
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../labelViewerWidget/debug/ -llabelViewerWidget
else:symbian: LIBS += -llabelViewerWidget
else:unix: LIBS += -L$$OUT_PWD/../labelViewerWidget/ -llabelViewerWidget

INCLUDEPATH += $$PWD/../labelViewerWidget
DEPENDPATH += $$PWD/../labelViewerWidget

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../labelViewerWidget/release/labelViewerWidget.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../labelViewerWidget/debug/labelViewerWidget.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../labelViewerWidget/liblabelViewerWidget.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../labelCore/release/ -llabelCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../labelCore/debug/ -llabelCore
else:symbian: LIBS += -llabelCore
else:unix: LIBS += -L$$OUT_PWD/../labelCore/ -llabelCore

INCLUDEPATH += $$PWD/../labelCore
DEPENDPATH += $$PWD/../labelCore

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../labelCore/release/labelCore.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../labelCore/debug/labelCore.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../labelCore/liblabelCore.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../sessionHandlerHooks/release/ -lsessionHandlerHooks
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../sessionHandlerHooks/debug/ -lsessionHandlerHooks
else:symbian: LIBS += -lsessionHandlerHooks
else:unix: LIBS += -L$$OUT_PWD/../sessionHandlerHooks/ -lsessionHandlerHooks

INCLUDEPATH += $$PWD/../sessionHandlerHooks
DEPENDPATH += $$PWD/../sessionHandlerHooks

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../sessionHandlerHooks/release/sessionHandlerHooks.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../sessionHandlerHooks/debug/sessionHandlerHooks.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../sessionHandlerHooks/libsessionHandlerHooks.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../sessionHandlerWidgets/release/ -lsessionHandlerWidgets
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../sessionHandlerWidgets/debug/ -lsessionHandlerWidgets
else:symbian: LIBS += -lsessionHandlerWidgets
else:unix: LIBS += -L$$OUT_PWD/../sessionHandlerWidgets/ -lsessionHandlerWidgets

INCLUDEPATH += $$PWD/../sessionHandlerWidgets
DEPENDPATH += $$PWD/../sessionHandlerWidgets

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../sessionHandlerWidgets/release/sessionHandlerWidgets.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../sessionHandlerWidgets/debug/sessionHandlerWidgets.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../sessionHandlerWidgets/libsessionHandlerWidgets.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../sessionHandlerUtil/release/ -lsessionHandlerUtil
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../sessionHandlerUtil/debug/ -lsessionHandlerUtil
else:symbian: LIBS += -lsessionHandlerUtil
else:unix: LIBS += -L$$OUT_PWD/../sessionHandlerUtil/ -lsessionHandlerUtil

INCLUDEPATH += $$PWD/../sessionHandlerUtil
DEPENDPATH += $$PWD/../sessionHandlerUtil

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../sessionHandlerUtil/release/sessionHandlerUtil.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../sessionHandlerUtil/debug/sessionHandlerUtil.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../sessionHandlerUtil/libsessionHandlerUtil.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../platform/release/ -lplatform
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../platform/debug/ -lplatform
else:symbian: LIBS += -lplatform
else:unix: LIBS += -L$$OUT_PWD/../platform/ -lplatform

INCLUDEPATH += $$PWD/../platform
DEPENDPATH += $$PWD/../platform

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../platform/release/platform.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../platform/debug/platform.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../platform/libplatform.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../logManager/release/ -llogManager
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../logManager/debug/ -llogManager
else:symbian: LIBS += -llogManager
else:unix: LIBS += -L$$OUT_PWD/../logManager/ -llogManager

INCLUDEPATH += $$PWD/../logManager
DEPENDPATH += $$PWD/../logManager

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../logManager/release/logManager.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../logManager/debug/logManager.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../logManager/liblogManager.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../3RDPartySoftware/XpdfWidgetQt/freetype-2.4.11/release/ -lFreeTypeLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../3RDPartySoftware/XpdfWidgetQt/freetype-2.4.11/debug/ -lFreeTypeLib
else:unix:!symbian {
    CONFIG += link_pkgconfig
    PKGCONFIG += freetype2
    #LIBS += -lfreetype -lm
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../XpdfWidgetQt/src/release/ -lXpdfWidgetQtLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../XpdfWidgetQt/src/debug/ -lXpdfWidgetQtLib
else:symbian: LIBS += -lXpdfWidgetQtLib
else:unix: LIBS += -L$$OUT_PWD/../../XpdfWidgetQt/src/ -lXpdfWidgetQtLib

INCLUDEPATH += $$PWD/../../XpdfWidgetQt/src
INCLUDEPATH += $$PWD/../../XpdfWidgetQt/src/xpdf-qt
DEPENDPATH += $$PWD/../../XpdfWidgetQt/src

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../XpdfWidgetQt/src/release/XpdfWidgetQtLib.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../XpdfWidgetQt/src/debug/XpdfWidgetQtLib.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../../XpdfWidgetQt/src/libXpdfWidgetQtLib.a

OTHER_FILES += \
    python/apList.txt \
    python/beep.py \
    python/calibrateTouchscreen.py \
    python/compile.py \
    python/encryptDecrypt.py \
    python/exportConfigs.py \
    python/fullBrightnessRange.py \
    python/generatedPassphrase.txt \
    python/getHostname.py \
    python/getMacAddress.py \
    python/getModelName.py \
    python/getTimezone.py \
    python/history.txt \
    python/HobartQmSettings-DATETIME.txt \
    python/importConfigs.py \
    python/networkSSIDs.txt \
    python/ping.py \
    python/reboot.py \
    python/samplePythonCommand.txt \
    python/saveConfigs.py \
    python/scanNetwork.py \
    python/setHostname.py \
    python/setNetwork.py \
    python/setTime.py \
    python/setVolume.py \
    python/shellCommand.py \
    python/testEncDec.py \
    python/ntp.py \
    python/file_io.py \
    python/bluetoothScan.py \
    python/setBrightness.py \
    python/setWifiMfgTestNetwork.py \
    python/netInterface.py \
    python/calibrate.py \
    python/sampleBeep.py \
    python/getMac.py \
    python/getZone.py \
    python/outputping.py \
    python/bootSettings.py \
    python/getGateway.py \
    python/getConnectionStatus.py \
    python/awkscript \
    db/scalesettings \
    python/sql.py \
    python/sendDeviceEventMessage.py \
    python/setDimDisplayTimeout.py \
    python/processDeviceEvents.py \
    python/setMfgTestNetwork.py \
    python/processMonitor.py \
    python/startupSequence.py \
    python/turnOffDisplays.py \
    python/cleanWirelessDevice.py \
    python/wirelessManager.py \
    python/netManager.py \
    python/outputPing.py \
    python/setNTP.py \
    python/getConnectionType.py \
    python/setTimezone.py \
    python/getTimezoneOffset.py \
    python/setNTPCron.py \
    python/updateCrontab.py \
    python/updateTimeNTP.py \
    python/updateTimeNTP2.py \
    python/setAudioDevice.py \
    python/updateTimeNTP2.py \
    python/installCert.py \
    python/createCertificateSigningRequest.py \
    python/updateTimeNTP2.py \
    python/setAudioDevice.py \
    python/createCertificateSigningRequest.py \
    python/changeEAPTLSCerts.py \
    python/outputCertInfo.py \
    python/deleteCert.py \
    python/activeCertCheck.py \
    python/deleteActiveCert.py \
    python/clearWPASupplicantConfAndDisconnect.py \
    python/exportEAPTLSCerts.py \
    python/deleteOldTrustedRootCert.py \
    python/advancedNetworking.py \
    python/setWirelessLinkRate.py \
    python/installUpgrade.py \
    python/test.py \
    python/sendSmBackendMessage.py \
    python/update_now.py

DISTFILES += \
    python/ntNetworkUtils.py \
    python/osUtils.py \
    python/isProductLicensed.py \
    python/startupSequenceWin32.py \
    ../util/licensing/license.py \
    python/scaleKeys.py \
    python/SPITest.py \
    python/serialFlash.py \
    python/tags.py \
    python/testFlash.py \
    python/installFilePatterns.py \
    python/getNetworkInfo.py \
    python/ntWirelessUtils.py \
    python/ntWiredUtils.py \
    python/getWiredSpeed.py \
    python/encryptDecryptText.py \
    python/getTime.py \
    python/getFullTime.py \
    ../util/licensing/volumeInfo.py \
    python/FileWithEmptyKeys.py \
    python/getBiosVersion.py \
    python/requestScaleServiceShutdown.py \
    python/createWirelessProfile.py \
    db/lastNetworkRun.txt \
    python/shutdownSequence.py \
    python/pseudoShutdown.py \
    python/launchHobartShutdown.py \
    python/certTest.py \
    python/getNetworkStatus.py \
    python/configInfo.py \
    python/lookAtNetworkAgain.py \
    python/connectNetworkStartup.py \
    python/ntCertificateUtils.py \
    python/clearWlanConfAndDisconnect.py \
    python/getSerialFlashInfo.py \
    python/reinstallClientCerts.py \
    python/createWirelessProfileTryg.py \
    python/trygTrial.py \
    python/beep.pyc \
    python/bootSettings.pyc \
    python/chilkatScp.pyc \
    python/configInfo.pyc \
    python/encryptDecrypt.pyc \
    python/file_io.pyc \
    python/getHostname.pyc \
    python/getMacAddress.pyc \
    python/getModelName.pyc \
    python/getNetworkInfo.pyc \
    python/languageSettings.pyc \
    python/messageBox.pyc \
    python/nircmd.exe \
    python/ntNetworkUtils.pyc \
    python/ntplib-0.3.0.zip \
    python/osUtils.pyc \
    python/pexpect-2.3.tar.gz \
    python/pyudev-v0.17.tgz \
    python/pyusb-1.0.0b1.tar.gz \
    python/pyusb-1.0.0b1.zip \
    python/reboot.pyc \
    python/sampleAudio.mp3 \
    python/scaleSettingsInterface.pyc \
    python/sendDeviceEventMessage.pyc \
    python/serialFlash.pyc \
    python/setBrightness.pyc \
    python/setPassword.pyc \
    python/setVolume.pyc \
    python/sftpProtocol.pyc \
    python/shellCommand.pyc \
    python/sql.pyc \
    python/tags.pyc \
    python/dropCaches.sh \
    python/getGcdStatus.sh \
    python/installNtp.sh \
    python/installNtpLib.sh \
    python/installPackages.sh \
    python/installParamiko.sh \
    python/installPexpect.sh \
    python/installPyudev.sh \
    python/installPyusb.sh \
    python/installSetupTools.sh \
    python/EAPXMLFile2.xml \
    python/PEAP2.xml \
    python/PEAP3.xml \
    python/PEAPCombined.xml \
    python/PEAPsample.xml \
    python/TODD.xml \
    python/Wireless Network Connection-wwST5.xml \
    python/WirelessProfile.xml \
    python/WirelessProfileEAPBase.xml \
    python/config.txt \
    python/cronjobs \
    python/EAPXMLFile \
    python/emptyMyCertStore.cmd \
    python/hyvee111wifi.txt \
    python/ntNetworkUtils.py.bak \
    python/showAll.txt \
    python/WirelessProfile \
    python/applicationTimer.py \
    python/buildFeature.py \
    python/chilkatScp.py \
    python/cpuInfo.py \
    python/createDaylightSavingsNTPXML.py \
    python/createMonthlyNTPXML.py \
    python/createWeeklyNTPXML.py \
    python/deviceDiscovery.py \
    python/eraseLicenseKeys.py \
    python/exportFiles.py \
    python/extract.py \
    python/fileSysStatistics.py \
    python/fullBeepRange.py \
    python/generateHostname.py \
    python/getIpAddress.py \
    python/getScaleType.py \
    python/getSmBackendVersion.py \
    python/getWinOSVersion.py \
    python/halt.py \
    python/hteSmScan.py \
    python/hteVersionInfo.py \
    python/hwRev.py \
    python/hwRevFileSync.py \
    python/importFiles.py \
    python/iniParser.py \
    python/installUtils.py \
    python/languageSettings.py \
    python/logger.py \
    python/main.py \
    python/md5checksum.py \
    python/messageBox.py \
    python/moduleFinder.py \
    python/outputPing.py \
    python/pexpectScp.py \
    python/playAudioFile.py \
    python/portScanner.py \
    python/scaleSettingsInterface.py \
    python/setDHCPWirelessLinkRate.py \
    python/setHardwareClock.py \
    python/setPassword.py \
    python/setScaleSettings.py \
    python/setSerialFlashInfo.py \
    python/setStartupClock.py \
    python/sftpProtocol.py \
    python/sftpUpdate.py \
    python/showNetwork.py \
    python/startScaleApplication.py \
    python/sync.py \
    python/testGwConnection.py \
    python/traceroute.py \
    python/updateBuildNumber.py \
    python/updateFBWF.py \
    python/updateKernelSourcePath.py \
    python/updateSplashImage.py \
    python/updateUBoot.py \
    python/updateVersionINI.py \
    python/uploadFile.py \
    python/usbDeviceManager.py \
    python/versionUpdater.py \
    python/wifiDetection.py \
    python/connectNetworkStartupNew.py \
    python/winEventHandler.py \
    python/displayMessageBox.py
    python/wifiDetection.py

DISTFILES += \
    python/dumpState.sh \
    python/customTraceroute.py \
    python/dumpState.py \
    python/ethernetConnectionDetails.py \
    python/getWifiStationInfo.py \
    python/statusLinkDetails.py \
    python/viewDumpStates.py \
    python/getFullAPInfo.py \
    python/getWirelessAPInfo.py \
    python/netConfig2USB.py \
    python/statusNICConfigurations.py \
    python/switchNIC.py \
    python/wirelessConnectionDetails.py \
    python/wirelessLogging.py \
    python/stopWireless.py
