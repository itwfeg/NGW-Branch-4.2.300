/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */



#include "HfcObject.h"
#include "HfcError.h"
#include "GenericHfcObject.h"
#include "HobOperator.h"
#include "OperatorNote.h"
#include "Product.h"
#include "OnlineOrderProduct.h"
#include "Class.h"
#include "HfcProductCompleteMsg.h"
#include "HfcProductPackageCompleteMsg.h"
#include "HfcProductStartPrintForTransactionMsg.h"
#include "Nutrifacts.h"
#include "ProductText.h"
#include "ProductDefault.h"
#include "HobDateTime.h"
#include "WriteSingleField.h"
#include "LabelType.h"
#include "GraphicFileNames.h"
#include "Graphic.h"
#include "Font.h"
#include "HfcProductSupportDataMsg.h"
#include "HfcProductLabelFormSizeMsg.h"
#include "HfcPrinterGapCalStatus.h"
#include "WeighModeTotalType.h"
#include "DirectoryList.h"
#include "HfcResponse.h"
#include "HobEmailAlert.h"
#include "HeaderRecord.h"

#include "ProductDevEvent.h"
#include "SessionEvent.h"
#include "ClassEvent.h"
#include "ProductTextEvent.h"
#include "SpecialMessageEvent.h"
#include "NutrifactsEvent.h"
#include "MarqueeTextEvent.h"
#include "OperatorEvent.h"

#include "TransactionDevEvent.h"
#include "TransactionTypeEvent.h"

#include "Messages/SynchronizationDevEvent.h"

#include "GraphicEvent.h"
#include "ConfigurationEvent.h"
#include "TouchScreenEvent.h"
#include "TimerEvent.h"
#include "CategoryEvent.h"
#include "NetworkEvent.h"
#include "StreamingGraphicEvent.h"

#include "CfgScale.h"
#include "CfgFrequentShopper.h"
#include "CfgOperatorAccess.h"
#include "CfgPrintedFields.h"
#include "CfgNetwork.h"
#include "CfgStoreInfo.h"
#include "CfgAll.h"
#include "CfgPrepack.h"
#include "CfgMarquee.h"
#include "CfgCurrency.h"
#include "CfgAccumulateItems.h"
#include "CfgSupportedLabelLengths.h"

#include "WeigherEvent.h"

#include "HfcWeigherStatus.h"
#include "HfcWeigherInquire.h"
#include "HfcWeigherSystemStatus.h"
#include "HfcUsbWeigherSystemStatus.h"
#include "HfcWeigherSystemInquire.h"
#include "WeigherInfo.h"
#include "WeigherCalibrationStep.h"
#include "WeigherCalibrationEnd.h"
#include "HfcWeigherSystemCalibrationSwitch.h"
#include "HfcWeigherMode.h"
#include "HfcWeigherCat3Statistics.h"
#include "HfcWeigherCat3AuditRecords.h"
#include "HfcWeigherCat3RecordStatus.h"
#include "HfcWeigherDefaultsComplete.h"
#include "HfcWeigherSystemValueMax.h"
#include "HfcWeigherValueMaxFitParams.h"

#include "HfcPeripheralList.h"
#include "Peripheral.h"
#include "PeripheralEvent.h"

#include "PrinterInfo.h"
#include "HfcPrinterRequestPrintheadPower.h"
#include "HfcPrintheadPower.h"
#include "HfcPrinterSystemRequestStatus.h"
#include "HfcPrinterSystemStatus.h"
#include "HfcPrinterRequestInquire.h"
#include "HfcPrinterInquire.h"
#include "HfcPrinterSystemInquire.h"
#include "HfcPrinterSensorsTracking.h"
#include "HfcPrinterSensors.h"
#include "HfcPrinterStatus.h"
#include "HfcPrinterRequestCalibrate.h"
#include "HfcPrinterPrintTestLabel.h"
#include "HfcPrinterAdvanceLabelStock.h"
#include "HfcPrinterPrinterTypeTracking.h"
#include "HfcPrinterPrinterType.h"
#include "HfcPrinterSetPrinters.h"
#include "HfcPrinterNewAddress.h"
#include "HfcPrinterRequestLabelSizing.h"
#include "HfcPrinterUseContinuousStock.h"
#include "HfcPrinterDefaultsComplete.h"
#include "HfcPrinterTestCassette.h"
#include "HfcPrinterGapCalStatus.h"
#include "HfcPrinterTest.h"

#include "HfcWrapperInquire.h"
#include "HfcWrapperSystemRequestStatus.h"
#include "HfcWrapperSystemStatus.h"
#include "HfcWrapperSensorsTracking.h"
#include "HfcWrapperSensors.h"
#include "HfcWrapperTest.h"
#include "HfcWrapperControl.h"
#include "HfcEvolWrapperControl.h"
#include "HfcWrapperLoad.h"
#include "HfcWrapperCycle.h"
#include "HfcWrapperStatusTracking.h"
#include "HfcWrapperStatus.h"
#include "HfcEvolWrapperStatus.h"
#include "HfcWrapperSettings.h"
#include "HfcEvolWrapperSettings.h"
#include "HfcWrapperTestTracking.h"
#include "HfcWrapperPackCodeInfo.h"
#include "HfcWrapperCycleMode.h"
#include "HfcWrapperErrorHistoryTracking.h"
#include "HfcWrapperErrorHistory.h"
#include "HfcEvolWrapperInquire.h"
#include "HfcWrapperPackageHistory.h"
#include "HfcEvolWrapperConfig.h"
#include "HfcEvolWrapperTest.h"
#include "HfcEvolWrapperTestComplete.h"
#include "HfcEvolWrapperSensors.h"
#include "HfcEvolWrapperPackageDesc.h"
#include "HfcEvolLabelApplierPositionInfo.h"
#include "HfcEvolWrapperPackageHistory.h"
#include "HfcEvolWrapperPackageTimings.h"
#include "HfcEvolWrapperTrayType.h"
#include "HfcEvolWrapperChangeTrayTypePrecedence.h"
#include "HfcEvolWrapperWrapBox.h"
#include "HfcEvolWrapperHeightBox.h"
#include "HfcEvolWrapperRenameHeightBox.h"
#include "HfcEvolWrapperPackageTimingsPointer.h"
#include "HfcEvolWrapperCamerasTracking.h"
#include "HfcEvolWrapperCameraScan.h"
#include "HfcEvolWrapperCameraHeightScan.h"
#include "HfcEvolWrapperSystemStatus.h"
#include "HfcEvolWrapperHeightBoxParam.h"
#include "HfcCUEvolWrapperStatus.h"
#include "HfcUpgradeList.h"
#include "HfcUpgradePeripheral.h"
#include "HfcEvolWrapperEventCapture.h"
#include "HfcEvolWrapperErrorHistory.h"
#include "HfcEvolWrapperPkgDetectorStatus.h"
#include "HfcEvolWrapperCycleCount.h"
#include "HfcEvolWrapperCameraThreshold.h"
#include "HfcEvolWrapperConfigLabelWidthUpdate.h"
#include "HfcEvolWrapperSystemInfo.h"
#include "HfcEvolWrapperSystemInfoWrite.h"
#include "HfcEvolWrapperMotorProfile.h"
#include "HfcEvolWrapperCameraCalibration.h"
#include "HfcEvolWrapperReqUIModel.h"
#include "HfcEvolWrapperUIModel.h"
#include "HfcNGWDimensionOverrides.h"
#include "HfcWrPPPCalibration.h"
#include "HfcWrPPPDistance.h"
#include "HfcWrPPPConfig.h"
#include "HfcWrPPPRequest.h"
#include "HfcWrPPPVersion.h"
#include "HfcWrPPPStatus.h"
#include "HfcNGWWrapperPause.h"
#include "HfcNGWWrapperResume.h"

#include "MarqueeEvent.h"
#include "HfcMarqueeSystemStatus.h"
#include "HfcMarqueeTest.h"
#include "NumericEvent.h"
#include "HfcNumericSystemStatus.h"
#include "HfcNumericTest.h"

#include "HfcGCDSystemStatus.h"
#include "HfcGCDDevStatus.h"

#include "HfcOperationModeStatus.h"

#include "HfcLabelApplierInquire.h"
#include "HfcILARequestSpeed.h"
#include "HfcILASpeed.h"
#include "HfcLabelApplierTest.h"
#include "HfcLabelApplierSetup.h"
#include "HfcLabelApplierStatus.h"
#include "HfcLabelApplierSensorsTracking.h"
#include "HfcLabelApplierSensors.h"
#include "HfcLabelApplierSystemRequestStatus.h"
#include "HfcLabelApplierSystemStatus.h"
#include "HfcLabelApplierMode.h"
#include "HfcEvolLabelApplierPositionInfo.h"
#include "HfcCLASysStatus.h"	//Alex Zavala 8/22
#include "HfcCLASysInfo.h"		//Alex Zavala 9/6
#include "HfcCLASysInfoWrite.h"	//Alex Zavala 9/6
#include "HfcCLATestComplete.h"	//Alex Zavala 9/6
#include "HfcCLARemoveUpgradeFiles.h"//Alex Zavala 10/3
#include "HfcPrinterCutterStatus.h"		// Joe Crew 08-01-2019
#include "HfcPrinterHeadDotStatus.h"    // Joe Crew 08-02-2019
#include "HfcPrinterHeadDotWear.h"		// Joe Crew 08-02-2019
#include "HfcPrinterHeadType.h"

#include "TransactionType.h"

#include "BackupRestoreEvent.h"
#include "BackupRestoreSelections.h"
#include "BackupRestoreInitiateReadAllMsg.h"
#include "HfcProductUpdateValuesMsg.h"
#include "HfcProductDescLookup.h"

#include "ReadTransactionGroupMessage.h"
#include "VoidTransactionsMessage.h"
#include "HourlyTotalEvent.h"
#include "ReadFilteredHourlyTotalsMessage.h"
#include "OperatorTotalEvent.h"
#include "ReadFilteredOperatorTotalsMessage.h"
#include "ProductTotalEvent.h"
#include "ReadFilteredProductTotalsMessage.h"
#include "OperatorChangeEvent.h"
#include "ReadOperatorChangeSummaryMessage.h"
#include "ReadOperatorChangeGroupMessage.h"
#include "WirelessProfilesSelections.h"
#include "CutTestTotalEvent.h"

#include "WindowsConfigurationEvents.h"

#include "TouchScreenCalibrationStep.h"
#include "GenericLabelText.h"

#include "HfcTimeout.h"
#include "HfcVmStatusMsg.h"
#include "HfcUpgradeTransferStatus.h"

#include "HfcEvolWrapperRemoveUpgradeFiles.h"

#include "ReadSoftKeyParams.h"
#include "Category.h"
#include "Special.h"
#include "PresentationMethod.h"
#include "StreamingGraphic.h"
#include "ProductStatistic.h"

#include "HfcPrinterRequestStockMetrics.h"
#include "HfcPrinterStockMetrics.h"

#include "HfcAccessKeyActivation.h"
#include "HfcAccessKeyDeactivation.h"

#include "HfcRequestPing.h"

#include "HfcStartReport.h"

#include "HfcEVMMemoryStatistics.h"

#include "NowServingQueueManagementMessage.h"

#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "Hex.h"

#include "COOLDevEvent.h"
#include "COOLCountry.h"
#include "AllCoolCountries.h"
#include "COOLProductionStep.h"
#include "AllCoolProductionSteps.h"
#include "COOLPretext.h"
#include "AllCoolPretexts.h"
#include "COOLCategory.h"
#include "AllCoolCategories.h"
#include "COOLText.h"
#include "AllCoolTexts.h"
#include "COOLShortlist.h"
#include "COOLList.h"
#include "COOLListText.h"

#include "COOLCategoryCountry.h"
#include "COOLCategoryProdStep.h"
#include "COOLCategoryPretext.h"

#include "SoftkeyDevEvent.h"
#include "Softkey.h"
#include "SoftkeyFeature.h"
#include "SoftkeyGroup.h"
#include "SoftkeyStyle.h"

#include "ExpirationLabel.h"

#include "ImportSelections.h"
#include "ImportEvent.h"

#include "ReadBatchesRecord.h"
#include "Scale.h"
#include "Batch.h"
#include "SynchronizationChange.h"

#include "RFIDEvent.h"
#include "HfcRFIDTIRISReadOnlyId.h"
#include "HfcRFIDStatus.h"

#include "CuttingTest.h"
#include "CutTestDevEvent.h"

#include "GCDEvent.h"
#include "HfcGCDDevStatus.h"
#include "HfcGCDDevFileInfo.h"
#include "HfcGCDDevRequestFileUpload.h"
#include "HfcGCDDevError.h"

#include "HfcHidDeviceEvent.h"
#include "HfcWriteFpmManData.h"

#include "NetworkConnectivityEvents.h"
#include "HfcNetworkStatus.h"

#include "Recall.h"
#include "RecallEvent.h"

#include "Device.h"
#include "DeviceEvent.h"

#include "ProductTextCfg.h"
#include "ProductTextCfgEvent.h"

#include "HfcScannerProductRead.h"
#include "BarcodeScannerEvent.h"

#include "Device.h"

#include "HfcProductLookupMsg.h"

#include "HfcNGWStartUpgrade.h"
#include "HfcNGWUpgradeStatus.h"
#include "HfcNGWUpgradeInfo.h"
#include "HfcNGWUpgradeProgress.h"
#include "HfcNGWUpgradeRequired.h"
#include "HfcDiagnosticsMessage.h"
#include "HfcRequestPrinterLabelCount.h"

#include "HfcKPCBootUpgraderStatus.h"
#include "HfcKPCUpgradeInfo.h"
#include "HfcKPCUpgradeProgress.h"
#include "HfcKPCBootLoaderStatus.h"
#include "HfcKPCLoaderInfo.h"
#include "HfcKPCLoaderProgress.h"

#include "DeptEvent.h"
#include "Dept.h"

#include "RemotePrintRequest.h"
#include "RemotePrintTemplateRequest.h"

#include "BoardSiteEvent.h"
#include "HfcBoardSiteTestCompleteUI.h"
#include "HfcBoardSitePercentComplete.h"
#include "HfcBoardSiteTestStatus.h"

#include "PrinterDonePostImagingSelfServeBarcodeLabelImage.h"
#include "ListCameraSystemPredictions.h"

#include "HfcLabelPrintingCounts.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcObject::_dataType_ = 0;
std::string HfcObject::_dataTypeName_("HfcObject"); 
#endif


/**
 * This class uses the Factory pattern to create HfcObject objects.
 * by using the static creator method in the base class, described by Eckel in Thinking in C++, Version 2
 * This method is the creator method for creating HfcObject objects.
 *
 * devType is an EvolDeviceType found in Event.h
 * eventType is the device's event types found in Event.h
 */

HfcObject *HfcObject::create(EvolDeviceType devType, UINT8 evType, const char *data, int len, int formatType)
{
	HfcObject *newObject = (HfcObject *)NULL;

	switch (devType)
	{
		case ProductDev:
		{
			switch (evType)
			{
				case ProductComplete:
				case ProductIncomplete:
				{
					newObject = new HfcProductCompleteMsg(data, len, formatType);
					if (Debug::isTracing("New.Product"))
						Debug::trace("New.Product", "    Product: " + ptrtostring(newObject) + " - HfcObject::create()");
				}
					break;

				case ProductPackageComplete:
				{
					newObject = new HfcProductPackageCompleteMsg(data, len, formatType);

					if ( Debug::isTracing("New.Product") )
						Debug::trace("New.Product", "    Product: " + util_string::ptrtostring(newObject) + " - HfcObject::create()");

					break;
				}
				case ProductChange:
				case ProductUpdate:
				case ProductPrint:
				case ProductRead:
				case ProductReadAll:
				case ProductWrite:
				case ProductDelete:
				case ProductBatchApplyDelete:
				case ProductCreate:
				case ProductRestore:
				{
					newObject = new Product(data, len, formatType);
					if (Debug::isTracing("New.Product"))
						Debug::trace("New.Product", "    Product: " + ptrtostring(newObject) + "  Product number:" "???" " - HfcObject::create()");
					break;
				}

				case ProductReadOnlineOrderingProduct:
				{
					newObject = ( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgOnlineOrderingEnabled) ? new OnlineOrderProduct(data, len, formatType) : new Product(data, len, formatType) );
					if (Debug::isTracing("New.Product"))
						Debug::trace("New.Product", "    OnlineOrderProduct: " + ptrtostring(newObject) + "  Product number:" "???" " - HfcObject::create()");
					break;
				}

				case ProductReadDefault:
				case ProductWriteDefault:
				{
					newObject = new ProductDefault(data, len, formatType);
					if (Debug::isTracing("New.ProductDefault"))
						Debug::trace("New.ProductDefault", "    ProductDefault: " + ptrtostring(newObject) + "  Product number:" "???" " - HfcObject::create()");
				}
					break;

				case ProductWriteSingleField:
				{
					newObject = new WriteSingleField(data, len, formatType);
					if (Debug::isTracing("New.ProductWriteSingleField"))
						Debug::trace("New.ProductWriteSingleField", "    WriteSingleField: " + ptrtostring(newObject) + "  Product number:" "???" " - HfcObject::create()");
				}
					break;

				case ProductShowLinksToSupportData:
				case ProductReadSupportData:
				case ProductWriteSupportData:
				case ProductChangeSupportData:
				case ProductReadSupportRecord:
				{
					newObject = new HfcProductSupportDataMsg(data, len, formatType);
					if (Debug::isTracing("New.HfcProductSupportDataMsg"))
						Debug::trace("New.HfcProductSupportDataMsg", "    HfcProductSupportDataMsg: " + ptrtostring(newObject) + "  Product number:" "???" " - HfcObject::create()");
				}
					break;

				case ProductWriteCurrentTotalValues:
				{
					newObject = new HfcProductUpdateValuesMsg(data, len, formatType);
					if (Debug::isTracing("New.ProductWriteCurrentTotalValues"))
						Debug::trace("New.ProductWriteCurrentTotalValues", "    : " + ptrtostring(newObject) + " - HfcObject::create()");
				}
					break;

				case ProductReadLabelFormSize:
				{
					newObject = new HfcProductLabelFormSizeMsg(data, len, formatType);

					if ( Debug::isTracing("New.HfcProductLabelFormSizeMsg") )
						Debug::trace("New.HfcProductLabelFormSizeMsg", "    : " + ptrtostring(newObject) + " - HfcObject::create()");

					break;
				}

				case ProductFindByDescription:
				{
					newObject = new HfcProductDescLookup(data, len, formatType);
					if (Debug::isTracing("New.HfcProductDescLookup"))
						Debug::trace("New.HfcProductDescLookup", "    : " + ptrtostring(newObject) + " - HfcObject::create()");
				}
					break;

#ifndef NOTSELFSERVICE
				case ProductReadSoftKeyList:
				{
					newObject = new ReadSoftKeyParams(data, len, formatType);
					if (Debug::isTracing("New.ReadSoftKeyParams"))
						Debug::trace("New.ReadSoftKeyParams", "    : " + ptrtostring(newObject) + " - HfcObject::create()");
				}
					break;
#endif

				case ProductRestoreExpirationLabel:
				{
					newObject = new ExpirationLabel(data, len, formatType);
					if (Debug::isTracing("New.ExpirationLabel"))
						Debug::trace("New.ExpirationLabel", "    : " + ptrtostring(newObject) + " - HfcObject::create()");
				}
					break;

				case ProductStartPrintForTransaction:
				{
					newObject = new HfcProductStartPrintForTransactionMsg(data, len, formatType);
					if (Debug::isTracing("New.Product"))
						Debug::trace("New.Product", "    HfcProductStartPrintForTransactionMsg: " + ptrtostring(newObject) + " - HfcObject::create()");
				}
					break;

				case ProductRemoveAccumulatedItems:
				{
					newObject = new hfc::VoidTransactionsMessage(data, len, formatType);
					break;
				}
				
				case ProductReadToo:
				{
					newObject = new HfcProductLookupMsg(data, len, formatType);

					if ( Debug::isTracing("New.Product") )
						Debug::trace("New.Product", "    Product: " + util_string::ptrtostring(newObject) + " - HfcObject::create()");

					break;
				}

				case ProductPrintProductSupportText:
				{
					newObject = new ProductTextCfg(data, len, formatType);
					if (Debug::isTracing("New.Product"))
						Debug::trace("New.Product", "    ProductTextCfg: " + ptrtostring(newObject) + " - HfcObject::create()");
					break;
				}

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: Product - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;
		}

		case OperationMode:
		{
			switch (evType)
			{
				case OpModeStatus:
					//newObject = (formatType == InternalFormat) ? new HfcOperationModeStatus(data) : new HfcOperationModeStatus(data, len, formatType);
					newObject = new HfcOperationModeStatus(data);
					break;

				case OpModeLabelsPrintedCounts:
					newObject = new HfcLabelPrintingCounts(data, len, formatType);
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: OperationMode - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;
		}

		case System:
		{
			switch (evType)
			{
				case SysPeripheralList:
					Debug::trace("Session.SystemMessage", "HfcObject received SysPeripheralList Message!");
					newObject = (formatType == InternalFormat) ? new HfcPeripheralList(data) : NULL;
					break;
				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: Weigher - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;
		}

		case Weigher:
		{
			switch (evType)
			{
				case WgStatus:
				case WgStatusAndPrint:
					newObject = (formatType == InternalFormat) ? new HfcWeigherStatus(data) : new HfcWeigherStatus(data, len, formatType);
					break;

				case WgInquire:
					newObject = (formatType == InternalFormat) ? new HfcWeigherInquire(data) : new HfcWeigherInquire(data, len, formatType);
					break;

				case WgConfig:
					newObject = (formatType == InternalFormat) ? new HfcWeigherConfig(data) : new HfcWeigherConfig(data, len, formatType);
					break;

				case WgRestoreWeigherSettings:
					Debug::trace("HfcWeigherConfig", HobString("HfcObject() - evType = WgRestoreWeigherSettings"));
					newObject = new HfcWeigherConfig(data, len, formatType);
					break;

				case WgInfo:
				case WgSaveInfo:
					newObject = (formatType == InternalFormat) ? new WeigherInfo(data) : new WeigherInfo(data, len, formatType);
					break;

				case WgCalibrationStep:
					if (Debug::isTracing("WeigherCalibration"))
						Debug::trace("WeigherCalibration", "HfcObject creating a calibration step");
					newObject = new WeigherCalibrationStep(data, len, formatType);
					break;

				case WgCalibrationEnd:
					if (Debug::isTracing("WeigherCalibration"))
						Debug::trace("WeigherCalibration", "HfcObject creating a calibration end");
					newObject = new WeigherCalibrationEnd(data, len, formatType);
					break;

				case WgMode:
					newObject = new HfcWeigherMode(data, len, formatType);
					break;

				case WgCat3AuditStatistics:
					if (Debug::isTracing("cat3"))
						Debug::trace("cat3", "HfcObject creating a HfcWeigherCat3Statistics object for WgCat3AuditStatistics");
					newObject = (formatType == InternalFormat) ? new HfcWeigherCat3Statistics(data) : new HfcWeigherCat3Statistics(data, len, formatType);
					break;

				case WgCat3Records:
					if (Debug::isTracing("cat3"))
						Debug::trace("cat3", "HfcObject creating a HfcWeigherCat3AuditRecords object for WeigherCat3AuditRecords");
					newObject = (formatType == InternalFormat) ? new HfcWeigherCat3AuditRecords(data) : new HfcWeigherCat3AuditRecords(data, len, formatType);
					break;

				case WgCat3RecordStatus:
					if (Debug::isTracing("cat3"))
						Debug::trace("cat3", "HfcObject creating a HfcWeigherCat3RecordStatus object for WeigherCat3RecordStatus");
					newObject = (formatType == InternalFormat) ? new HfcWeigherCat3RecordStatus(data) : new HfcWeigherCat3RecordStatus(data, len, formatType);
					break;
				case WgResetFactoryDefaultsComplete:
					newObject = (formatType == InternalFormat) ? new HfcWeigherDefaultsComplete(data) : new HfcWeigherDefaultsComplete(data, len, formatType);
					break;
				case WgReqValueMax:
					Debug::trace("WeigherValueMaxFitParams", "HfcObject creating a HfcWeigherSystemValueMax object for WgReqValueMax");
					newObject = (formatType == InternalFormat) ? new HfcWeigherSystemValueMax(data) : new HfcWeigherSystemValueMax(data, len, formatType);
					break;
				case WgValueMaxFitParams:
					Debug::trace("WeigherValueMaxFitParams", "HfcObject creating a HfcWeigherValueMaxFitParams object for WgValueMaxFitParams");
					newObject = (formatType == InternalFormat) ? new HfcWeigherValueMaxFitParams(data) : new HfcWeigherValueMaxFitParams(data, len, formatType);
					break;
				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: Weigher - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;
		}

		case WeigherSystem:
		{
			switch (evType)
			{
				case SysStatus:
					newObject = (formatType == InternalFormat) ? new HfcWeigherSystemStatus(data) : new HfcWeigherSystemStatus(data, len, formatType);
					break;

				case SysUsbWgStatus:
					newObject = (formatType == InternalFormat) ? new HfcUsbWeigherSystemStatus(data) : NULL;
					break;

				case SysInquire:
					newObject = (formatType == InternalFormat) ? new HfcWeigherSystemInquire(data) : new HfcWeigherSystemInquire(data, len, formatType);
					break;

				case SysCalibSwitch:
					newObject = (formatType == InternalFormat) ? new HfcWeigherSystemCalibrationSwitch(data) : new HfcWeigherSystemCalibrationSwitch(data, len, formatType);
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: WeigherSystem - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;
		}

		case LabelApplier:
		{
			switch (evType)
			{
				case LaConfig:
					newObject = (formatType == InternalFormat) ? NULL : new HfcLabelApplierConfig(data, len, formatType);
					break;

				case LaInquire:
					newObject = (formatType == InternalFormat) ? new HfcLabelApplierInquire(data) : NULL;
					break;
				//Alex Zavala 8/22 Added the new HfcCLASysStatus msg for CLA EPSAC2
				case LaSysStatus:
					newObject = (formatType == InternalFormat) ? new HfcCLASysStatus(data) : NULL;
					break;
				case LaSetup:
					newObject = new HfcLabelApplierSetup(data, len, formatType);
					break;
				//Added cases for the new messages Alex Zavala 9/6
				case LaSysInfo:
					newObject = (formatType == InternalFormat) ? new HfcCLASysInfo(data) : NULL;
					break;
				case LaSysInfoWrite:
					newObject = (formatType == InternalFormat) ? NULL : new HfcCLASysInfoWrite(data, len, formatType);
					break;
				case LaTestComplete:
					newObject = (formatType == InternalFormat) ? new HfcCLATestComplete(data) : NULL;
					break;
				case LaRemoveUpgradeFiles:
					Debug::trace("HfcCLARemoveUpgradeFiles", HobString(" creating HfcCLARemoveUpgradeFiles for message data"));
					newObject = (formatType == InternalFormat) ? new HfcCLARemoveUpgradeFiles(data) : new HfcCLARemoveUpgradeFiles(data, len, formatType);
					break;
				/*	End of new messages	*/
				case LaILAReqSpeed:
					newObject = new HfcILARequestSpeed(data, len, formatType);
					break;

				case LaILASpeed:
					newObject = (formatType == InternalFormat) ? new HfcILASpeed(data) : new HfcILASpeed(data, len, formatType);
					break;

				case LaTest:
					newObject = (formatType == InternalFormat) ? NULL : new HfcLabelApplierTest(data, len, formatType);
					break;

				case LaStatus:
					newObject = (formatType == InternalFormat) ? new HfcLabelApplierStatus(data) : NULL;
					break;

				case LaSensorsTracking:
					newObject = (formatType == InternalFormat) ? NULL : new HfcLabelApplierSensorsTracking(data, len, formatType);
					break;

				case LaSensors:
					newObject = (formatType == InternalFormat) ? new HfcLabelApplierSensors(data) : NULL;
					break;

				case LaMode:
					newObject = (formatType == InternalFormat) ? NULL : new HfcLabelApplierMode(data, len, formatType);
					break;

				case LaSndLabelPos:
					newObject = new HfcEvolLabelApplierPositionInfo(data, len, formatType);
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: LabelApplier - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;
		}

		case LabelApplierSystem:
			switch (evType)
			{
				case SysReqStatus:
					newObject = new HfcLabelApplierSystemRequestStatus(data);
					break;

				case SysStatus:
					newObject = new HfcLabelApplierSystemStatus(data);
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: LabelApplierSystem - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;

		case Printer:
		{
			switch (evType)
			{
				case PrStatus:
					newObject = (formatType == InternalFormat) ? new HfcPrinterStatus(data) : new HfcPrinterStatus(data, len, formatType);
					break;

				case PrReqInquire:
					newObject = (formatType == InternalFormat) ? new HfcPrinterRequestInquire(data) : new HfcPrinterRequestInquire(data, len, formatType);		//	ALS
					break;

				case PrInquire:
					newObject = (formatType == InternalFormat) ? new HfcPrinterInquire(data) : new HfcPrinterInquire(data, len, formatType);
					break;

				case PrConfig:
					newObject = new HfcPrinterConfig(data, len, formatType);
					break;

				case PrRestorePrinterSettings:
					Debug::trace("HfcPrinterConfig", HobString("HfcObject() - evType = PrRestorePrinterSettings"));
					newObject = new HfcPrinterConfig(data, len, formatType);
					break;

				case PrInfo:
				case PrSaveInfo:
					newObject = (formatType == InternalFormat) ? new PrinterInfo(data) : new PrinterInfo(data, len, formatType);
					break;

				case PrSensorsTracking:
					newObject = (formatType == InternalFormat) ? new HfcPrinterSensorsTracking(data) : new HfcPrinterSensorsTracking(data, len, formatType);
					break;

				case PrSensors:
					newObject = (formatType == InternalFormat) ? new HfcPrinterSensors(data) : new HfcPrinterSensors(data, len, formatType);
					break;

				case PrPrintheadPower:
					newObject = (formatType == InternalFormat) ? new HfcPrintheadPower(data) : new HfcPrintheadPower(data, len, formatType);
					break;

				case PrReqPrintheadPower:
					newObject = (formatType == InternalFormat) ? new HfcPrinterRequestPrintheadPower(data) : new HfcPrinterRequestPrintheadPower(data, len, formatType);
					break;

				case PrReqCalibrate:
					newObject = (formatType == InternalFormat) ? new HfcPrinterRequestCalibrate(data) : new HfcPrinterRequestCalibrate(data, len, formatType);
					break;

				case PrTestLabel:
					newObject = (formatType == InternalFormat) ? new HfcPrinterPrintTestLabel(data) : new HfcPrinterPrintTestLabel(data, len, formatType);
					break;

				case PrReqStockMetrics:
					newObject = (formatType == InternalFormat) ? new HfcPrinterRequestStockMetrics(data) : new HfcPrinterRequestStockMetrics(data, len, formatType);
					break;

				case PrStockMetrics:
					newObject = new HfcPrinterStockMetrics(data);
					break;

				case PrReqAdvLabelStock:
					newObject = (formatType == InternalFormat) ? new HfcPrinterAdvanceLabelStock(data) : new HfcPrinterAdvanceLabelStock(data, len, formatType);
					break;

				case PrCutterStatus:
					newObject = (formatType == InternalFormat) ? new HfcPrinterCutterStatus(data) : new HfcPrinterCutterStatus(data, len, formatType);
					break;

				case PrHeadType:
					newObject = (formatType == InternalFormat) ? new HfcPrinterHeadType(data) : new HfcPrinterHeadType(data, len, formatType);
					break;

				case PrHeadDotStatus:
					newObject = (formatType == InternalFormat) ? new HfcPrinterHeadDotStatus(data) : new HfcPrinterHeadDotStatus(data, len, formatType);
					break;

				case PrHeadDotWear:
					newObject = (formatType == InternalFormat) ? new HfcPrinterHeadDotWear(data) : new HfcPrinterHeadDotWear(data, len, formatType);
					break;

				case PrPrinterType:
					newObject = (formatType == InternalFormat) ? new HfcPrinterPrinterType(data) : new HfcPrinterPrinterType(data, len, formatType);
					break;

				case PrPrinterTypeTracking:
					newObject = (formatType == InternalFormat) ? new HfcPrinterPrinterTypeTracking(data) : new HfcPrinterPrinterTypeTracking(data, len, formatType);
					break;

				case PrSetPrinters:
					newObject = new HfcPrinterSetPrinters(data, len, formatType);
					break;

				case PrNewAddress:
					newObject = (formatType == InternalFormat) ? new HfcPrinterNewAddress(data) : new HfcPrinterNewAddress(data, len, formatType);
					break;

				case PrSaveExternalPrinter:
					newObject = new Device(data, len, ExternalPrinterSaveFormat);
					break;

				case PrSaveProductTextCfg:
					newObject = new ProductTextCfg(data, len, formatType);
					break;

				case PrReqLabelSizing:
					newObject = (formatType == InternalFormat) ? new HfcPrinterRequestLabelSizing(data) : new HfcPrinterRequestLabelSizing(data, len, formatType);
					break;

				case PrUseContinuousStock:
					newObject = (formatType == InternalFormat) ? new HfcPrinterUseContinuousStock(data) : new HfcPrinterUseContinuousStock(data, len, formatType);
					break;
				case PrResetFactoryDefaultsComplete:
					newObject = (formatType == InternalFormat) ? new HfcPrinterDefaultsComplete(data) : new HfcPrinterDefaultsComplete(data, len, formatType);
					break;
				case PrTestCassette:
					newObject = (formatType == InternalFormat) ? new HfcPrinterTestCassette(data) : new HfcPrinterTestCassette(data, len, formatType);
					break;
				case PrDonePostImagingSelfServeBarcodeLabelImage:
				{
					if (formatType == InternalFormat)
					{
						PrinterDonePostImagingSelfServeBarcodeLabelImage *tempObject = new PrinterDonePostImagingSelfServeBarcodeLabelImage(data);
						newObject = new GenericHfcObject( inttostring(tempObject->getFilenameCounter()) );
					}
					else
						newObject = new GenericHfcObject(data, len, formatType);
					break;
				}
				case PrGapCalStatus:
				{					
					newObject = new HfcPrinterGapCalStatus(data); 
					break;
				}
				case PrTest:
				{ 
					//newObject = (formatType == InternalFormat) ? NULL : new HfcPrinterTest(data, len, formatType);
					newObject = (formatType == InternalFormat) ? new HfcPrinterTest(data) : new HfcPrinterTest(data, len, formatType);
					break; 
				}
				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: Printer - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;
		}

		case PrinterSystem:
			switch (evType)
			{
				case SysInquire:
					newObject = (formatType == InternalFormat) ? new HfcPrinterSystemInquire(data) : new HfcPrinterSystemInquire(data, len, formatType);
					break;

				case SysReqStatus:
					newObject = (formatType == InternalFormat) ? new HfcPrinterSystemRequestStatus(data) : new HfcPrinterSystemRequestStatus(data, len, formatType);
					break;

				case SysStatus:
					newObject = (formatType == InternalFormat) ? new HfcPrinterSystemStatus(data) : new HfcPrinterSystemStatus(data, len, formatType);
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: PrinterSystem - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;

		case Wrapper:
		{
			switch (evType)
			{
				case WrConfig:
					newObject = new HfcWrapperConfig(data, len, formatType);
					break;

				case WrEvolConfig:
					newObject = new HfcEvolWrapperConfig(data, len, formatType);
					break;

				case WrInquire:
					newObject = new HfcWrapperInquire(data);
					break;

				case WrSensorsTracking:
					newObject = new HfcWrapperSensorsTracking(data, len, formatType);
					break;

				case WrSensors:
					newObject = (formatType == InternalFormat) ? new HfcWrapperSensors(data) : new HfcWrapperSensors(data, len, formatType);
					break;

				case WrEvolSensors:
					newObject = new HfcEvolWrapperSensors(data);
					break;

				case WrTest:
					newObject = (formatType == InternalFormat) ? new HfcWrapperTest(data) : new HfcWrapperTest(data, len, formatType);
					break;

				case WrEvolTest:
					newObject = (formatType == InternalFormat) ? new HfcEvolWrapperTest(data) : new HfcEvolWrapperTest(data, len, formatType);
					break;

				case WrEvolTestComplete:
					newObject = new HfcEvolWrapperTestComplete(data);
					break;

				case WrControl:
					newObject = (formatType == InternalFormat) ? new HfcWrapperControl(data) : new HfcWrapperControl(data, len, formatType);
					break;

				case WrEvolControl:
					newObject = new HfcEvolWrapperControl(data, len, formatType);
					break;

				case WrLoad:
				case WrEvolLoad:
					newObject = (formatType == InternalFormat) ? new HfcWrapperLoad(data) : new HfcWrapperLoad(data, len, formatType);
					break;

				case WrCycle:
					newObject = new HfcWrapperCycle(data);
					break;

				case WrTestTracking:
					newObject = new HfcWrapperTestTracking(data, len, formatType);
					break;

				case WrStatusTracking:
					newObject = new HfcWrapperStatusTracking(data, len, formatType);
					break;

				case WrStatus:
					newObject = new HfcWrapperStatus(data);
					break;

				case WrEvolStatus:
					newObject = new HfcEvolWrapperStatus(data);
					break;

				case WrSettings:
					newObject = (formatType == InternalFormat) ? new HfcWrapperSettings(data) : new HfcWrapperSettings(data, len, formatType);
					break;
				case WrReadPackCode:
					newObject = new HfcWrapperPackCodeInfo(data, len, formatType);
					break;

				case WrWritePackCode:
					newObject = new HfcWrapperPackCodeInfo(data, len, formatType);
					break;

				case WrSavePackCode:
					newObject = new HfcWrapperPackCodeInfo(data, len, formatType);
					break;

				case WrCycleMode:
					newObject = new HfcWrapperCycleMode(data, len, formatType);
					break;

				case WrErrorHistoryTracking:
					newObject = new HfcWrapperErrorHistoryTracking(data, len, formatType);
					break;

				case WrErrorHistory:
					newObject = new HfcWrapperErrorHistory(data);
					break;

				case WrEvolInquire:
					newObject = new HfcEvolWrapperInquire(data);
					Debug::trace("HfcEvolWrapperInquire", HobString("HfcObject()::create(),  HfcEvolWrapperInquire::show(): -------------- START"));
					dynamic_cast<HfcEvolWrapperInquire *> (newObject)->show();
					Debug::trace("HfcEvolWrapperInquire", HobString("HfcObject()::create(),  HfcEvolWrapperInquire::show(): -------------- END"));
					break;

				case WrPackageHistory:
					newObject = new HfcWrapperPackageHistory(data);
					break;

				case WrEvolPackageDesc:
					newObject = new HfcEvolWrapperPackageDesc(data);
					break;

				case WrEvolPackageHistory:
					newObject = new HfcEvolWrapperPackageHistory(data);
					break;

				case WrEvolSettings:
					newObject = new HfcEvolWrapperSettings(data);
					if (Debug::isTracing("HfcEvolWrapperSettings"))
					{
						HobString msgDebug = "formatType = ";
						msgDebug.append(Integer::toString(formatType));
						Debug::trace("HfcEvolWrapperSettings", msgDebug);

						msgDebug = "HfcObject()::create(),  HfcEvolWrapperSettings::show(): -------------- ";
						HobString msgDebug2 = msgDebug;
						msgDebug2.append("START");
						Debug::trace("HfcEvolWrapperSettings", msgDebug2);
						dynamic_cast<HfcEvolWrapperSettings *> (newObject)->show();
						HobString msgDebug3 = msgDebug;
						msgDebug3.append("END");
						Debug::trace("HfcEvolWrapperSettings", msgDebug3);
					}
					break;

				case WrEvolPackageTimings:
				{
					newObject = new HfcEvolWrapperPackageTimingsPointer(data);

					break;
				}
				case WrEvolRestorePackageTimings:
				{
					HfcEvolWrapperPackageTimings *hewpt = new HfcEvolWrapperPackageTimings(data, len, formatType);
					newObject = new HfcEvolWrapperPackageTimingsPointer(WrEvolRestorePackageTimings, hewpt);

					break;
				}
				case WrEvolAutoConfigTrayType:
				case WrEvolCreateTrayType:
				case WrEvolWriteTrayType:
				{
					newObject = new HfcEvolWrapperTrayType(data, len, formatType);

					break;
				}
				case WrEvolChangeTrayTypePrecedence:
				{
					newObject = new HfcEvolWrapperChangeTrayTypePrecedence(data, len, formatType);

					break;
				}
				case WrEvolWriteWrapBox:
				{
					newObject = new HfcEvolWrapperWrapBox(data, len, formatType);

					break;
				}
				case WrEvolReadHeightBox:
				case WrEvolWriteHeightBox:
				case WrEvolDeleteHeightBox:
				case WrEvolVerifyInHeightBox:
				{
					newObject = new HfcEvolWrapperHeightBox(data, len, formatType);

					break;
				}
				case WrEvolCreateHeightBox:
				{
					newObject = new HfcEvolWrapperHeightBox(data, len, CreateFormat);

					break;
				}
				case WrEvolRenameHeightBox:
				{
					newObject = new HfcEvolWrapperRenameHeightBox(data, len, formatType);

					break;
				}

				case WrEvolCamerasTracking:
					newObject = new HfcEvolWrapperCamerasTracking(data, len, formatType);
					break;

				case WrCameraScan:
					Debug::trace("HfcEvolWrapperCameraScan", HobString(" creating HfcEvolWrapperCameraScan for message data"));
					newObject = new HfcEvolWrapperCameraScan(data);
					Debug::trace("HfcEvolWrapperCameraScan", HobString(" created HfcEvolWrapperCameraScan for message data"));
					break;

				case WrEvolCameraHeightScanTracking:
				{
					//	Reuse this message, changing the Event.
					HfcEvolWrapperCamerasTracking *hewcht = new HfcEvolWrapperCamerasTracking(data, len, formatType);
					if (hewcht)
						hewcht->setEventType(WrEvolCameraHeightScanTracking);
					newObject = hewcht;
					break;
				}
				case WrCameraHeightScan:
					Debug::trace("HfcEvolWrapperCameraHeightScan", HobString(" creating HfcEvolWrapperCameraHeightScan for message data"));
					newObject = new HfcEvolWrapperCameraHeightScan(data);
					Debug::trace("HfcEvolWrapperCameraHeightScan", HobString(" created HfcEvolWrapperCameraHeightScan for message data"));
					break;

				case WrEvolReadAllForAttribute:
				case WrEvolSaveAttributeChanges:
					Debug::trace("HfcEvolWrapperHeightBoxParam", HobString(" creating HfcEvolWrapperHeightBoxParam for message data"));
					newObject = new HfcEvolWrapperHeightBoxParam(data, len, formatType);
					break;

				case WrEvolRemoveUpgradeFiles:
					Debug::trace("HfcEvolWrapperRemoveUpgradeFiles", HobString(" creating HfcEvolWrapperRemoveUpgradeFiles for message data"));
					newObject = (formatType == InternalFormat) ? new HfcEvolWrapperRemoveUpgradeFiles(data) : new HfcEvolWrapperRemoveUpgradeFiles(data, len, formatType);
					break;
				case WrEvolBackupPackageTimings:
					 newObject = new HfcEvolWrapperPackageTimingsPointer(data);
					break;
				case WrEvolEventCapture:
					newObject = new HfcEvolWrapperEventCapture(data);
					break;
				case WrEvolErrorHistory:
					newObject = new HfcEvolWrapperErrorHistory(data);
					break;
				case WrEvolPkgDetectorStatus:
					newObject = new HfcEvolWrapperPkgDetectorStatus(data);
					break;
				case WrEvolCycleCount:
					newObject = new HfcEvolWrapperCycleCount(data);
					break;
				case WrEvolCameraThreshold:
					newObject = new HfcEvolWrapperCameraThreshold(data);  
					break;
				case WrEvolWriteCameraThreshold:
				{	
					HfcEvolWrapperCameraThreshold *pHfcEvolWrapperCameraThreshold = new HfcEvolWrapperCameraThreshold(data, len, formatType);
					
					if(pHfcEvolWrapperCameraThreshold != NULL)
					{
						pHfcEvolWrapperCameraThreshold->setEventType(WrEvolWriteCameraThreshold);
					}
					newObject = pHfcEvolWrapperCameraThreshold;  
					break;
				}
				case WrCameraCalibration:				
					newObject = new	HfcEvolWrapperCameraCalibration(data);
					break;				
				case WrEvolUpdateConfigLabelWidth:
					newObject = new HfcEvolWrapperConfigLabelWidthUpdate(data);
					break;
				case WrEvolSystemInfo:
					newObject = new HfcEvolWrapperSystemInfo(data);
					break;
				case WrEvolSystemInfoWrite:
					newObject = new HfcEvolWrapperSystemInfoWrite(data, len, formatType);
					break;
				case WrEvolSetMinTrayDims:
				case WrEvolSetMaxTrayDims:
				case WrEvolSetMinMaxTrayDims:
					newObject = new HfcEvolWrapperWrapBox(data, len, formatType);
					break;
				case WrEvolMotorProfile:
					newObject = new HfcEvolWrapperMotorProfile(data);
					break;
				case WrReqUIWrapperModel:
					newObject = new HfcEvolWrapperReqUIModel(data);
					break;
				case WrUIWrapperModel:
					newObject = new HfcEvolWrapperUIModel(data);
					break;
                case WrNGWDimensionOverrides:
					newObject = (formatType == InternalFormat) ? new HfcNGWDimensionOverrides(data) : new HfcNGWDimensionOverrides(data, len, formatType);
					break;
				case WrPPPDist:
					Debug::trace("HfcWrPPPDist","Creating new HfcWrPPPDist");
					newObject = new HfcWrPPPDist(data);
					break;
				case WrPPPCalibration:
					Debug::trace("WrPPPCalibration","Creating new HfcWrPPPCal");
					newObject = new HfcWrPPPCal(data);
					break;
				case WrPPPConfig:
					Debug::trace("WrPPPConfig","Creating new WrPPPConfig");
					newObject = new HfcWrPPPConfig(data);
					break;
				case WrPPPRequest:
					Debug::trace("WrPPPRequest","Creating new WrPPPRequest");
					newObject = (formatType == InternalFormat) ? new HfcWrPPPReq(data) : new HfcWrPPPReq(data, len, formatType);
					break;
				case WrPPPVersion:
					Debug::trace("HfcWrPPPVersion","Creating new WrPPPVersion");
					newObject = new HfcWrPPPVersion(data);
					break;	
				case WrPPPStatus:
					Debug::trace("HfcWrPPPVersion","Creating new WrPPPVersion");
					newObject = new HfcWrPPPStatus(data);
					break;
				case WrWrapperPause:
					// SOF-5459 Printer Plate Adjust Message - Do not need this for a single sliding 2nd printer so can it all
					Debug::trace("WrapperManager.PPPMsg", "HfcObject::create() case WrWrapperPause: do not create this anymore");
					//Debug::trace("HfcNGWWrapperPause","Creating new WrWrapperPause");
					//newObject = new HfcNGWWrapperPause(data);	
					break;
				case WrPPPResume: 			
					Debug::trace("HfcNGWWrapperResume","Creating new WrWrapperResume");
					newObject = (formatType == InternalFormat) ? new HfcNGWWrapperResume(data) : new HfcNGWWrapperResume(data, len, formatType);
					//newObject = new HfcNGWWrapperResume(data);	
					break;
				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: Wrapper - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;
		}

		case WrapperSystem:
			switch (evType)
			{
				case SysReqStatus:
					newObject = new HfcWrapperSystemRequestStatus(data);
					break;

				case SysStatus:
					newObject = new HfcWrapperSystemStatus(data);
					break;

				case SysUsbWrStatus:
					newObject = new HfcEvolWrapperSystemStatus(data);
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: WrapperSystem - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;
		case NGWrapper:
			switch (evType)
			{
				Debug::trace("NGWrapper","HFC Object Event: " + Integer::toString(evType));
				case _StartUpgrade:
					newObject = new HfcNGWStartUpgrade(data);
					break;
				case _UpgradeStatus:
					newObject = new HfcNGWUpgradeStatus(data);
					break;
				case _UpgradeProgress:
					newObject = new HfcNGWUpgradeProgress(data);
					break;
				case _UpgradeFinished:
					break;
				case _UpgradeRequired:
					newObject = new HfcNGWUpgradeRequired(data);
					break;
				case _UpgradeInfo:
					newObject = new HfcNGWUpgradeInfo(data);
					break;
			}
			break;

		case KPCBootloader:
			switch (evType)
			{
				Debug::trace("KPCBootloader", "HFC Object Event: " + Integer::toString(evType));
				case KpcBootUpgradeStatus:
					newObject = new HfcKPCBootLoaderStatus(data);
					break;
				case KpcBootUpgradeProgress:
					newObject = new HfcKPCLoaderProgress(data);
					break;
				case KpcBootUpgradeInfo:
					newObject = new HfcKPCLoaderInfo(data);
					break;
			}
			break;

		case KPCBootUpdater:
			switch (evType)
			{
				Debug::trace("KPCBootUpdater", "HFC Object Event: " + Integer::toString(evType));
				case KpcBootUpgraderUpgradeStatus:
					newObject = new HfcKPCBootUpgraderStatus(data);
					break;
				case KpcBootUpgraderUpgradeProgress:
					newObject = new HfcKPCUpgradeProgress(data);
					break;
				case KpcBootUpgraderUpgradeInfo:
					newObject = new HfcKPCUpgradeInfo(data);
					break;
				}
				break;

		case RFIDReader:
			switch (evType)
			{
				case RFIDCurrentID:
					newObject = new HfcRFIDTIRISReadOnlyId(data, len, formatType);
					break;

				case RFIDCurrentStatus:
					newObject = new HfcRFIDStatus(data, len, formatType);
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: RFIDReader - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;

		case SessionDev:
			switch (evType)
			{
				case  SnWriteDateTime:
					newObject = new HobDateTime(data, len, formatType);
					break;

				case SnWriteWeighModeTransactionType:
					newObject = new WeighModeTotalType(data, len, formatType);
					break;

				case SnReadDirectoryList:
					newObject = new DirectoryList(data, len, formatType);
					break;

				case SnCommStart:
					newObject = new hfc::HeaderRecord(data, len, formatType);
					break;

				case SnKeyActivation:
					newObject = (formatType == InternalFormat) ? new HfcAccessKeyActivation(data) : new HfcAccessKeyActivation(data, len, formatType);
					break;

				case SnKeyDeactivation:
					newObject = new HfcAccessKeyDeactivation(data);
					break;

				case SnWriteStatistics:
					newObject = (formatType == InternalFormat) ? new HfcEVMMemoryStatistics(data) : new HfcEVMMemoryStatistics(data, len, formatType);
					break;

				case SnExecuteTask:
					newObject = new GenericHfcObject(data, len, formatType);
					break;
/*
				case SnBackendRemotePrint:
					newObject = new RemotePrintRequest(data, len, formatType);
					break;

				case SnBackendRemotePrintTemplateLabel:
					newObject = new RemotePrintTemplateRequest(data, len, formatType);
					break;
*/
				// FT-1458
				case SnNowServingQueueManagement:
					newObject = new NowServingQueueManagementMessage( data, len, formatType );
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: SessionDev - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;

		case DeptDev:
			switch (evType)
			{
			case DtRead:
			case DtWrite:
			case DtDelete:
			case DtBatchApplyDelete:
			case DtSave:
			case DtRestore:
			case DtCreateNew:
			case SetCurrentDept:
				newObject = new Dept(data, len, formatType);
				if (Debug::isTracing("New.Dept"))
					Debug::trace("New.Dept", "    Dept: " + ptrtostring(newObject) + "  Dept number:" "???" " - HfcObject::create()");
				break;

			default:
				if (Debug::isTracing("New.GenericHfcObject"))
					Debug::trace("New.GenericHfcObject", HobString(" WARNING: Class - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
				newObject = new GenericHfcObject(data, len, formatType);
				break;
			}
			break;

		case ClassDev:
			switch (evType)
			{
				case DtRead:
				case DtWrite:
				case DtDelete:
				case DtBatchApplyDelete:
				case DtSave:
				case DtRestore:
				case DtCreateNew:
					newObject = new Class(data, len, formatType);
					if (Debug::isTracing("New.Class"))
						Debug::trace("New.Class", "    Class: " + ptrtostring(newObject) + "  Class number:" "???" " - HfcObject::create()");
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: Class - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;

		case NutrifactsDev:
			switch (evType)
			{
				case DtRead:
				case DtWrite:
				case DtDelete:
				case DtBatchApplyDelete:
				case DtRestore:
				case NutrifactsNew:
					newObject = new Nutrifacts(data, len, formatType);
					if (Debug::isTracing("New.Nutrifacts"))
						Debug::trace("New.Nutrifacts", "    Nutrifacts: " + ptrtostring(newObject) + " - HfcObject::create()");
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: Nutrifacts - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;

		case ExpandedTextDev:
		case SpecialMessageDev:
		case ProductNotesDev:
		case MarqueeTextDev:
		case ProductText5Dev:
		case ProductText6Dev:
		case ProductText7Dev:
		case ProductText8Dev:
		case ProductText9Dev:
		{
			ProdSupportTextType type = psttUnknown;

			     if (devType == ExpandedTextDev)   type = psttExpandedText;
			else if (devType == SpecialMessageDev) type = psttSpecialMessage;
			else if (devType == ProductNotesDev)   type = psttProductNote;
			else if (devType == MarqueeTextDev)    type = psttMarqueeText;
			else if (devType == ProductText5Dev)   type = psttProductText5;
			else if (devType == ProductText6Dev)   type = psttProductText6;
			else if (devType == ProductText7Dev)   type = psttProductText7;
			else if (devType == ProductText8Dev)   type = psttProductText8;
			else if (devType == ProductText9Dev)   type = psttProductText9;

			switch (evType)
			{
				case DtRead:
				case DtWrite:
				case DtDelete:
				case DtBatchApplyDelete:
				case DtRestore:
				case ProductTextNew:
					newObject = new ProductText(data, len, formatType);
					dynamic_cast<ProductText *>(newObject)->setTextType(type);
					if (Debug::isTracing("New.ProductText"))
						Debug::trace("New.ProductText", "    ProductText:" + ptrtostring(newObject) + "  type:" + inttostring(type) + "  evType:" + inttostring(evType) + " - HfcObject::create()");
					break;

				default:
					if (Debug::isTracing("New.ProductText"))
						Debug::trace("New.ProductText", " WARNING:  ProductText going into GenericHfcObject() - type:" + inttostring(type) + "  evType:" + inttostring(evType));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;
		}

		case ProductDefaultsDev:
			switch (evType)
			{
				case DtRead:
				case DtReadAll:
				case DtWrite:
				case DtSave:
				case DtRestore:
					newObject = new ProductDefault(data, len, formatType);
					if (Debug::isTracing("New.ProductDefault"))
						Debug::trace("New.ProductDefault", "    ProductDefault: " + ptrtostring(newObject) + " - HfcObject::create()");
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: ProductDefault - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;

		case OperatorDev:
			switch (evType)
			{
				case DtRead:
				case DtReadAll:
				case DtWrite:
				case DtDelete:
				case DtBatchApplyDelete:
				case DtSave:
				case DtRestore:
				case DtCreateNew:
				case OpLogin:
				case OpLogout:
					newObject = new HobOperator(data, len, formatType);
					break;

				case OpWriteNote:
				case OpSaveNote:
				case OpRestoreNote:
					newObject = new OperatorNote(data, len, formatType);
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: Class - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;

		case GraphicDev:
		{
			switch (evType)
			{
				case DtRead:
				case DtWrite:
				case DtDelete:
				case DtBatchApplyDelete:
				case DtRestore:
				case DtFirst:
				case DtLast:
				case DtNext:
				case DtPrevious:
				{
					Graphic *g = new Graphic(data, len, formatType);
					if (g)
					{
						g->setType(Graphic::TYPE_USERLABELGRAPHIC);
						newObject = g;
						if (Debug::isTracing("New.Graphic"))
							Debug::trace("New.Graphic", "    HfcObject.create() - " "Graphic File Name: " + ptrtostring(newObject) + " - HfcObject::create()");
					}
					break;
				}

				case GraphicReadAllNames:
					newObject = new GenericHfcObject(data, len, formatType);
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString("    HfcObject::create() -  GraphicReadAllNames"));
					break;

				case ReadProductGraphic:
				case DeleteProductGraphic:
				case BatchApplyDeleteProductGraphic:
				case RestoreProductGraphic:
				{
					Graphic *g = new Graphic(data, len, formatType);
					if (g)
					{
						g->setType(Graphic::TYPE_PRODUCTGRAPHIC);
						newObject = g;
						if (Debug::isTracing("New.Graphic"))
							Debug::trace("New.Graphic", "    HfcObject.create() - " "TYPE_PRODUCTGRAPHIC Graphic File Name: " +
									ptrtostring(newObject) + " - HfcObject::create()");
					}
					break;
				}

				case ReadCategoryGraphic:
				case DeleteCategoryGraphic:
				case BatchApplyDeleteCategoryGraphic:
				case RestoreCategoryGraphic:
				{
					Graphic *g = new Graphic(data, len, formatType);
					if (g)
					{
						g->setType(Graphic::TYPE_CATEGORYGRAPHIC);
						newObject = g;
						if (Debug::isTracing("New.Graphic"))
							Debug::trace("New.Graphic", "    HfcObject.create() - " "TYPE_CATEGORYGRAPHIC Graphic File Name: " +
									ptrtostring(newObject) + " - HfcObject::create()");
					}
					break;
				}

				case ReadSpecialsGraphic:
				case DeleteSpecialsGraphic:
				case BatchApplyDeleteSpecialsGraphic:
				case RestoreSpecialsGraphic:
				{
					Graphic *g = new Graphic(data, len, formatType);
					if (g)
					{
						g->setType(Graphic::TYPE_SPECIALSGRAPHIC);
						newObject = g;
						if (Debug::isTracing("New.Graphic"))
							Debug::trace("New.Graphic", "    HfcObject.create() - " "TYPE_SPECIALSGRAPHIC Graphic File Name: " +
									ptrtostring(newObject) + " - HfcObject::create()");
					}
					break;
				}

				case ReadStreamingGraphic:
				case DeleteStreamingGraphic:
				case BatchApplyDeleteStreamingGraphic:
				case RestoreStreamingGraphic:
				{
					Graphic *g = new Graphic(data, len, formatType);
					if (g)
					{
						g->setType(Graphic::TYPE_STREAMINGGRAPHIC);
						newObject = g;
						if (Debug::isTracing("New.Graphic"))
							Debug::trace("New.Graphic", "    HfcObject.create() - " "TYPE_STREAMINGGRAPHIC Graphic File Name: " +
									ptrtostring(newObject) + " - HfcObject::create()");
					}
					break;
				}

				case ReadSoftkeyGraphic:
				case DeleteSoftkeyGraphic:
				case BatchApplyDeleteSoftkeyGraphic:
				case RestoreSoftkeyGraphic:
				{
					Graphic *g = new Graphic(data, len, formatType);
					if (g)
					{
						g->setType(Graphic::TYPE_SOFTKEYGRAPHIC);
						newObject = g;
						if (Debug::isTracing("New.Graphic"))
							Debug::trace("New.Graphic", "    HfcObject.create() - " "TYPE_SOFTKEYGRAPHIC Graphic File Name: " +
									ptrtostring(newObject) + " - HfcObject::create()");
					}
					break;
				}

				case ReadPlanogramGraphic:
				case DeletePlanogramGraphic:
				case BatchApplyDeletePlanogramGraphic:
				case RestorePlanogramGraphic:
				{
					Graphic *g = new Graphic(data, len, formatType);
					if (g)
					{
						g->setType(Graphic::TYPE_PLANOGRAMGRAPHIC);
						newObject = g;
						if (Debug::isTracing("New.Graphic"))
							Debug::trace("New.Graphic", "    HfcObject.create() - " "TYPE_PLANOGRAMGRAPHIC Graphic File Name: " +
									ptrtostring(newObject) + " - HfcObject::create()");
					}
					break;
				}


				case ReadAllSelfServeGraphicNames:
					newObject = new GenericHfcObject(data, len, formatType);
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString("    HfcObject::create() -  ReadAllSelfServeGraphicNames"));
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: GraphicDev - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;
		}

		case FontDev:
			switch (evType)
			{
				case DtRead:
				case DtWrite:
				case DtDelete:
				case DtRestore:
/*
				case DtFirst:
				case DtLast:
				case DtNext:
				case DtPrevious:
*/
					newObject = new Font(data, len, formatType);
					if (Debug::isTracing("New.Font"))
						Debug::trace("New.Font", "    HfcObject.create() - " "Font File Name: " + ptrtostring(newObject) + " - HfcObject::create()");
					break;

/*
				case FontReadAllNames:
					newObject = new GraphicFileNames(data, len, formatType);
					Debug::trace("New.GraphicFileNames", HobString("    Graphic File Names: ") + HobString(Hex::toString((long) newObject)) + HobString(" - HfcObject::create()"));
					break;
*/

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: FontDev - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;

		case LabelTypeDev:
			switch (evType)
			{
				case DtRead:
				case DtReadAll:
				case DtWrite:
				case DtDelete:
				case DtBatchApplyDelete:
				case DtSave:
				case DtRestore:
				case DtFirst:
				case DtLast:
				case DtNext:
				case DtPrevious:
					newObject = new LabelType(data, len, formatType);
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: LabelTypeDev - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;

		case ConfigurationDev:
		{
			switch (evType)
			{
				case CfgGCDWrite:
				case CfgGCDSave:
					newObject = new CfgGCD(data, len, formatType);
					if (Debug::isTracing("New.Configuration"))
						Debug::trace("New.Configuration", "    HfcObject.create() - CfgGCD " + util_string::ptrtostring(newObject) + " - HfcObject::create()");
					break;
				case CfgScaleWrite:
				case CfgScaleSave:
					newObject = new CfgScale(data, len, formatType);
					if (Debug::isTracing("New.Configuration"))
						Debug::trace("New.Configuration", "    HfcObject.create() - CfgScale " + util_string::ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case CfgExpirationLabelNoteWrite:
				case CfgExpirationLabelNoteSave:
					newObject = new CfgScale(data, len, ExpirationLabelMessageFormat);
					if (Debug::isTracing("New.Configuration"))
						Debug::trace("New.Configuration", "    HfcObject.create() - CfgScale - ExpirationLabelMessageFormat " + util_string::ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case DtReadSingleField:
				case DtWriteSingleField:
				case DtSaveSingleField:
					newObject = new WriteSingleField(data, len, formatType);
					if (Debug::isTracing("New.Configuration"))
						Debug::trace("New.Configuration", "    WriteSingleField: " + ptrtostring(newObject) + "  Product number:" "???" " - HfcObject::create()");
					break;

				case CfgFrequentShopperWrite:
				case CfgFrequentShopperSave:
					newObject = new CfgFrequentShopper(data, len, formatType);
					if (Debug::isTracing("New.Configuration"))
						Debug::trace("New.Configuration", "    HfcObject.create() - CfgFrequentShopper " + util_string::ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case CfgScaleOperationWrite:
				case CfgScaleOperationSave:
					newObject = new CfgScale(data, len, NormalScaleOpFormat);
					if (Debug::isTracing("New.Configuration"))
						Debug::trace("New.Configuration", "    HfcObject.create() - CfgScaleOperation " + util_string::ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case CfgOperatorAccessWrite:
				case CfgOperatorAccessSave:
					newObject = new CfgOperatorAccess(data, len, formatType);
					if (Debug::isTracing("New.Configuration"))
						Debug::trace("New.Configuration", "    HfcObject.create() - CfgOperatorAccess " + util_string::ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case CfgPrintedFieldsWrite:
				case CfgPrintedFieldsSave:
					newObject = new CfgPrintedFields(data, len, formatType);
					if (Debug::isTracing("New.Configuration"))
						Debug::trace("New.Configuration", "    HfcObject.create() - CfgPrintedFields " + util_string::ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case CfgStoreInfoWrite:
				case CfgStoreInfoSave:
				case CfgStoreInfoRestore:
					newObject = new CfgStoreInfo(data, len, formatType);
					if (Debug::isTracing("New.Configuration"))
						Debug::trace("New.Configuration", "    HfcObject.create() - CfgStoreInfo " + util_string::ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case CfgCurrencyRead:
				case CfgCurrencyWrite:
				case CfgCurrencySave:
					newObject = new CfgCurrency(data, len, formatType);
					if ( Debug::isTracing("New.Configuration") )
						Debug::trace("New.Configuration", "    HfcObject.create() - CfgCurrency " + util_string::ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case CfgScaleUpgradeWrite:
				case CfgScaleUpgradeSave:
					newObject = new CfgScale(data, len, UpgradeScaleFormat);
					if (Debug::isTracing("New.Configuration"))
						Debug::trace("New.Configuration", "    HfcObject.create() - CfgScaleUpgrade " + util_string::ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case CfgEmailWrite:
				case CfgEmailSave:
					newObject = new CfgEmail(data, len, formatType);
					if (Debug::isTracing("New.Configuration"))
						Debug::trace("New.Configuration", "    HfcObject.create() - CfgEmail " + util_string::ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case CfgSynchronizationWrite:
				case CfgSynchronizationSave:
				case CfgSynchronizationRestore:
					newObject = new CfgSynchronization(data, len, formatType);
					if (Debug::isTracing("New.Configuration"))
						Debug::trace("New.Configuration", "    HfcObject.create() - CfgSynchronization " + util_string::ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case CfgSyncListWrite:
				case CfgSyncListSave:
				case CfgSyncListRestore:
					newObject = new CfgSynchronization(data, len, SyncListFormat);
					if (Debug::isTracing("New.Configuration"))
						Debug::trace("New.Configuration", "    HfcObject.create() - CfgSynchronization - List " + util_string::ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case CfgFTPWrite:
				case CfgFTPSave:
					newObject = new CfgFTP(data, len, formatType);
					if (Debug::isTracing("New.Configuration"))
						Debug::trace("New.Configuration", "    HfcObject.create() - CfgEmail " + util_string::ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case CfgCommInterfaceWrite:
				case CfgCommInterfaceSave:
					newObject = new CfgCommInterface(data, len, formatType);
					if (Debug::isTracing("New.Configuration"))
						Debug::trace("New.Configuration", "    HfcObject.create() - CfgCommInterface " + util_string::ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case CfgCommIntfaceScheduledWrite:
				case CfgCommIntfaceScheduledSave:
					newObject = new CfgCommScheduled(data, len, formatType);
					if (Debug::isTracing("New.Configuration"))
						Debug::trace("New.Configuration", "    HfcObject.create() - CfgCommScheduled " + util_string::ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case CfgSelfServiceWrite:
				case CfgSelfServiceSave:
					newObject = new CfgSelfService(data, len, formatType);
					if (Debug::isTracing("New.Configuration"))
						Debug::trace("New.Configuration", "    HfcObject.create() - CfgSelfService " + util_string::ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case CfgPrepackWrite:
				case CfgPrepackSave:
					newObject = new CfgPrepack(data, len, formatType);
					if (Debug::isTracing("New.Configuration"))
						Debug::trace("New.Configuration", "    HfcObject.create() - CfgPrepack " + util_string::ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case CfgMarqueeWrite:
				case CfgMarqueeSave:
					newObject = new CfgMarquee(data, len, formatType);
					if (Debug::isTracing("New.Configuration"))
						Debug::trace("New.Configuration", "    HfcObject.create() - CfgMarquee " + util_string::ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case CfgAccumulateItemsWrite:
				case CfgAccumulateItemsSave:
					newObject = new CfgAccumulateItems(data, len, formatType);
					if (Debug::isTracing("New.Configuration"))
						Debug::trace("New.Configuration", "    HfcObject.create() - CfgAccumulateItems " + util_string::ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case CfgSupportedLabelLengthsWrite:
				case CfgSupportedLabelLengthsSave:
					newObject = new CfgSupportedLabelLengths(data, len, formatType);
					if (Debug::isTracing("New.Configuration"))
						Debug::trace("New.Configuration", "    HfcObject.create() - CfgSupportedLabelLengths " + util_string::ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case DtRestore:
				case DtBatchApplyRestore:
					newObject = new CfgAll(data, len, formatType);
					if (Debug::isTracing("New.Configuration"))
						Debug::trace("New.Configuration", "    HfcObject.create() - CfgAll " + util_string::ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case CfgScaleManagementWrite:
				case CfgScaleManagementSave:
					newObject = new CfgScale(data, len, ScaleManagementFormat);
					if (Debug::isTracing("New.Configuration"))
						Debug::trace("New.Configuration", "    HfcObject.create() - CfgScaleManagement " + HobString(Hex::toString((long long) newObject)) + HobString(" - HfcObject::create()"));
					break;

				case CfgRotationOverrideWrite:
				case CfgRotationOverrideSave:
					newObject = new CfgPrepack(data, len, RotationOverrideFormat);
					if (Debug::isTracing("New.Configuration"))
						Debug::trace("New.Configuration", "    HfcObject.create() - CfgRotationOverride " + util_string::ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case CfgReportsSetupWrite:
				case CfgReportsSetupSave:
					newObject = new CfgScale(data, len, ReportsSetupFormat);
					if (Debug::isTracing("New.Configuration"))
						Debug::trace("New.Configuration", "    HfcObject.create() - CfgReportsSetup " + HobString(Hex::toString((long long) newObject)) + " - HfcObject::create()");
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: ConfigurationDev - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;
		}

		case BackupRestore:
			switch (evType)
			{
				case Backup:
				case Restore:
				case BadRecordScan:
					newObject = new BackupRestoreSelections(data, len, formatType);
					break;

				case InitiateReadAll:
					newObject = new BackupRestoreInitiateReadAllMsg(data);
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: BackupRestore - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;

		case ImportDev:
			switch (evType)
			{
				case ImportStart: 
						newObject = new ImportSelections(data, len, formatType);
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: Import - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;

		case ReportManagerDev:
		{
			switch ( evType )
			{
				case StartReport:
				{
					if (Debug::isTracing("ReportManager.event"))
						Debug::trace("ReportManager.event", 20, "Received start report message");

					newObject = new HfcStartReport(data, len, formatType);

					break;
				}
				default:
				{
					if ( Debug::isTracing("ReportManager.event") )
					{
						Debug::trace("ReportManager.event", 10,
								"Received a generic (?) ReportManager event with event type " + HobString(Integer::toString(evType)));
					}

					newObject = new GenericHfcObject(data, len, formatType);

					break;
				}
			}

			break;
		}

		case Error:
			if ( formatType == SynchronizationTaggedFormat )
			{
				newObject = new HfcError(data, len, formatType);

				if ( Debug::isTracing("New.Error") )
					Debug::trace("New.Error", "    HfcError: " + util_string::ptrtostring(newObject) + " - HfcObject::create()");
			}
			else
			{
				newObject = new HfcResponse(data, len, formatType);

				if (newObject && evType == ErrRemoveAnyModalDialog)
					((HfcResponse *)newObject)->setDismissed(true);
				if (Debug::isTracing("New.Error"))
					Debug::trace("New.Error", "    HfcResponse: " + ptrtostring(newObject) + " - HfcObject::create()");
			}

			break;

		case TouchScreen:
			switch (evType)
			{
				case TsCalibrateNextStep:
				{
					newObject = new TouchScreenCalibrationStep(data, len, formatType);
					if (Debug::isTracing("New.TouchScreen"))
						Debug::trace("New.TouchScreen", "    TouchScreen: " + ptrtostring(newObject) + " - HfcObject::create()");
				}
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: TouchScreen - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;

		case GenericTextDev:
			switch (evType)
			{
				case DtRead:
				case DtWrite:
				case DtDelete:
				case DtSave:
				case DtRestore:
				case DtFirst:
				case DtLast:
				case DtCreateNew:
					newObject = new GenericLabelText(data, len, formatType);
					if (Debug::isTracing("New.GenericTextDev"))
						Debug::trace("New.GenericTextDev", "    GenericLabelText: " + ptrtostring(newObject) + " - HfcObject::create()");
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: Class - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;

		case TimerDev:
			switch (evType)
			{
				case TimerExpired:
					newObject = (formatType == InternalFormat) ? new HfcTimeout(data) : new HfcTimeout(data, len, formatType);
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: TimerDev - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;

		case CategoryDev:
			switch (evType)
			{
				case DtRead:
				case DtWrite:
				case DtDelete:
				case DtBatchApplyDelete:
				case DtSave:
				case DtRestore:
				case DtCreateNew:
					newObject = new Category(data, len, formatType);
					if (Debug::isTracing("New.Category"))
						Debug::trace("New.Category", "    Category: " + ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case CategoryReadSoftKeyList:
				case CategoryReadSubCategoriesList:
				{
					newObject = new ReadSoftKeyParams(data, len, formatType);
					if (Debug::isTracing("New.ReadSoftKeyParams"))
						Debug::trace("New.ReadSoftKeyParams", "    : " + ptrtostring(newObject) + " - HfcObject::create()");
				}
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: CategoryDev - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;

		case SpecialsDev:
			switch (evType)
			{
				case DtRead:
				case DtWrite:
				case DtDelete:
				case DtBatchApplyDelete:
				case DtSave:
				case DtRestore:
				case DtCreateNew:
					newObject = new Special(data, len, formatType);
					if (Debug::isTracing("New.SpecialsDev"))
						Debug::trace("New.SpecialsDev", "    Specials: " + ptrtostring(newObject) + " - HfcObject::create()");
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: SpecialsDev - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;

		case PresentationMethodDev:
			switch (evType)
			{
				case DtRead:
				case DtWrite:
				case DtDelete:
				case DtSave:
				case DtRestore:
				case DtCreateNew:
					newObject = new PresentationMethod(data, len, formatType);
					if (Debug::isTracing("New.PresentationMethodDev"))
						Debug::trace("New.PresentationMethodDev", "    PresentationMethod: " + ptrtostring(newObject) + " - HfcObject::create()");
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: PresentationMethod - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;

		case StreamingGraphicDev:
			switch (evType)
			{
				case DtRead:
				case DtWrite:
				case DtDelete:
				case DtSave:
				case DtRestore:
				case DtCreateNew:
				case StreamingGraphicSelectSlide:
					newObject = new StreamingGraphic(data, len, formatType);
					if (Debug::isTracing("New.StreamingGraphicDev"))
						Debug::trace("New.StreamingGraphicDev", "    StreamingGraphic: " + ptrtostring(newObject) + " - HfcObject::create()");
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: StreamingGraphicDev - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;

		case ProductStatisticsDev:
			switch (evType)
			{
				case DtRead:
				case DtWrite:
				case DtDelete:
				case DtSave:
				case DtRestore:
				case DtCreateNew:
					newObject = new ProductStatistic(data, len, formatType);
					if (Debug::isTracing("New.ProductStatisticsDev"))
						Debug::trace("New.ProductStatisticsDev", "    ProductStatistic: " + ptrtostring(newObject) + " - HfcObject::create()");
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: ProductStatisticsDev - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;

		case NetworkDev:
		{
			switch (evType)
			{
				case NetworkReqPing:
					newObject = new HfcRequestPing(data, len, formatType);
					break;

				case CfgTcpIpWrite:
				case CfgTcpIpSave:
					newObject = new CfgNetwork(data, len, NetworkTcpIpFormat);
					if (Debug::isTracing("New.NetworkDev"))
						Debug::trace("New.NetworkDev", "    HfcObject.create() - CfgNetworkTcpIpMsg " + ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case CfgDnsWrite:
				case CfgDnsSave:
					newObject = new CfgNetwork(data, len, NetworkDnsFormat);
					if (Debug::isTracing("New.NetworkDev"))
						Debug::trace("New.NetworkDev", "    HfcObject.create() - CfgNetworkDnsMsg " + ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case CfgWirelessCardWrite:
				case CfgWirelessCardSave:
					newObject = new CfgNetwork(data, len, NetworkWirelessCardFormat);
					if (Debug::isTracing("New.NetworkDev"))
						Debug::trace("New.NetworkDev", "    HfcObject.create() - CfgWirelessCardMsg " + ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case CfgWirelessCardActivateWEP:
					newObject = new CfgNetwork(data, len, NetworkWirelessCardWEPEnableFormat);
					if (Debug::isTracing("New.NetworkDev"))
						Debug::trace("New.NetworkDev", "    HfcObject.create() - CfgNetworkEncryptionKey " + ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case CfgWirelessCardSetActiveKey:
				case CfgWirelessCardSetEncryptionKey:
					newObject = new CfgNetwork(data, len, NetworkWirelessCardEncryptionKeyFormat);
					if (Debug::isTracing("New.NetworkDev"))
						Debug::trace("New.NetworkDev", "    HfcObject.create() - CfgNetworkEncryptionKey " + ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case NetworkTftpUpgradeWrite:
				case NetworkTftpUpgradeSave:
					newObject = new CfgNetwork(data, len, NetworkTftpSetupFormat);
					if (Debug::isTracing("New.NetworkDev"))
						Debug::trace("New.NetworkDev", "    HfcObject.create() - NetworkTftpUpgrade " + ptrtostring(newObject) + " - HfcObject::create()");
					break;

				case cfgLEAPWrite:
				case cfgLEAPSave:
					newObject = new CfgNetwork(data, len, NetworkLeapFormat);
					if (Debug::isTracing("New.NetworkDev"))
						Debug::trace("New.NetworkDev", "    HfcObject.create() - CfgNetworkDnsMsg " + ptrtostring(newObject) + " - HfcObject::create()");
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: NetworkDev - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;
		}

		case VersionManagerDev:
		{
			switch (evType)
			{
				case FirmwareUpdateStatus:
					newObject = new HfcVmStatusMsg(data);
					break;

				case InitiateUpgradeProcess:
				case UpgradeComplete:
				case CancelUpgrade:
				case BeginUpgrade:
				case RemoveWrapperSWComponents:
				case RemoveCLASWComponents://Alex Zavala 10/3
				{
					newObject = (formatType == InternalFormat) ? new HfcUpgradePeripheral(data) : new HfcUpgradePeripheral(evType, data, len, formatType);
					if (newObject && ((evType == CancelUpgrade) || (evType == BeginUpgrade)))
					{
						Debug::trace("UpgradePeripheral", "HfcObject::create(" + inttostring(evType) + "):  show UpgradePeripheral*******");
						dynamic_cast<UpgradePeripheral *> (newObject)->show();
					}
					break;
				}

				case UpgradeXferStatus:
					newObject = new HfcUpgradeTransferStatus(data);
					break;

				case UsbWrapperUpgradeStatus:
					newObject = new HfcCUEvolWrapperStatus(data);
					break;

				case PeripheralUpgradeList:
					Debug::trace("HfcUpgradeList", "HfcObject::create(HfcUpgradeList)");
					newObject = new HfcUpgradeList(data);
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: NetworkDev - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;
		}

		case EmailAlertsDev:
		{
			switch (evType)
			{
				case DtDelete:
				case DtBatchApplyDelete:
				case DtRestore:
				case DtSave:
					Debug::trace("Clare", HobString("HfcObject(): EmailAlertsDev - evType = ") + HobString(Integer::toString(evType)) + HobString("   data = ") + HobString(data) + HobString("   formatType = ") + HobString(Integer::toString(formatType)));
					newObject = new HobEmailAlert(data, len, formatType);
					break;
				default:
					if (Debug::isTracing("New.EmailManagerDev"))
						Debug::trace("New.EmailManagerDev", HobString(" WARNING: EmailAlertsDev - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;
		}

		case COOLDev:
		{
			switch (evType)
			{
				case COOLDevReadCountry:
				case COOLDevWriteCountry:
				case COOLDevDeleteCountry:
				case COOLDevCountryBatchApplyDelete:
				case COOLDevSaveCountry:
				case COOLDevRestoreCountry:
				case COOLDevCreateNewCountry:
				case COOLDevAddCountryToCategory:
				case COOLDevDeleteCountryFromCategory:
				case COOLDevCreateNewCountryAndSave:
					newObject = new COOLCountry(data, len, formatType);
					break;
				case COOLDevReadAllCountries:
					newObject = new GenericHfcObject(data, len, formatType);
					break;
				case COOLDevReadByCategoryOnlyCountries:
				case COOLDevReadByAllAndByCategoryCountries:
					newObject = new AllCoolCountries(data, len, formatType);
					break;

				case COOLDevReadProductionStep:
				case COOLDevWriteProductionStep:
				case COOLDevDeleteProductionStep:
				case COOLDevProductionStepBatchApplyDelete:
				case COOLDevSaveProductionStep:
				case COOLDevRestoreProductionStep:
				case COOLDevCreateNewProductionStep:
				case COOLDevAddProductionStepToCategory:
				case COOLDevDeleteProductionStepFromCategory:
				case COOLDevCreateNewProductionStepAndSave:
					newObject = new COOLProductionStep(data, len, formatType);
					break;
				case COOLDevReadAllProductionSteps:
					newObject = new GenericHfcObject(data, len, formatType);
					break;
				case COOLDevReadByCategoryOnlyProductionSteps:
				case COOLDevReadByAllAndByCategoryProductionSteps:
					newObject = new AllCoolProductionSteps(data, len, formatType);
					break;

				case COOLDevReadPretext:
				case COOLDevWritePretext:
				case COOLDevDeletePretext:
				case COOLDevPretextBatchApplyDelete:
				case COOLDevSavePretext:
				case COOLDevRestorePretext:
				case COOLDevCreateNewPretext:
				case COOLDevAddPretextToCategory:
				case COOLDevDeletePretextFromCategory:
					newObject = new COOLPretext(data, len, formatType);
					break;
				case COOLDevReadAllPretexts:
					newObject = new GenericHfcObject(data, len, formatType);
					break;
				case COOLDevReadByCategoryOnlyPretexts:
				case COOLDevReadByAllAndByCategoryPretexts:
					newObject = new AllCoolPretexts(data, len, formatType);
					break;

				case COOLDevReadCategory:
				case COOLDevWriteCategory:
				case COOLDevDeleteCategory:
				case COOLDevCategoryBatchApplyDelete:
				case COOLDevSaveCategory:
				case COOLDevRestoreCategory:
				case COOLDevCreateNewCategory:
					newObject = new COOLCategory(data, len, formatType);
					break;
				case COOLDevReadAllCategories:
					newObject = new GenericHfcObject(data, len, formatType);
					break;
				case COOLDevReadAllCategoriesByCountry:
				case COOLDevReadAllCategoriesByProdStep:
				case COOLDevReadAllCategoriesByPretext:
					newObject = new AllCoolCategories(data, len, formatType);
					break;


				case COOLDevRestoreCoolText:
					newObject = new COOLText(data, len, formatType);
					if (newObject && ((COOLText *)newObject)->getTextType() == COOLText::COOLTextType_Pretext)
					{
						// We created a COOLText object, but it really should be a COOLPretext object.
						delete newObject;
						newObject = new COOLPretext(data, len, formatType);
					}
					break;

				case COOLDevReadCoolText:
				case COOLDevWriteCoolText:
				case COOLDevDeleteCoolText:
				case COOLDevCoolTextBatchApplyDelete:
				case COOLDevSaveCoolText:
				case COOLDevCreateNewCoolText:
					newObject = new COOLText(data, len, formatType);
					break;
				case COOLDevReadAllCoolText:
					newObject = new GenericHfcObject(data, len, formatType);
					break;
				case COOLDevShortlistReadAllCoolText:
					newObject = new AllCoolTexts(data, len, formatType);
					break;
				case COOLDevRestoreShortlist:
					if (formatType == SynchronizationTaggedFormat)
						newObject = new COOLList(data, len, formatType);
					else
						newObject = new COOLShortlist(data, len, formatType);
					break;
				case COOLDevRestoreShortlistForUltima:
				case COOLDevShortlistAddText:
				case COOLDevShortlistDeleteText:
					newObject = new COOLShortlist(data, len, formatType);
					break;
				case COOLDevRestoreShortlistText:
					newObject = new COOLListText(data, len, formatType);
					break;
				case COOLDevReadCategoryCountry:
				case COOLDevWriteCategoryCountry:
				case COOLDevDeleteCategoryCountry:
				case COOLDevBatchApplyDeleteCategoryCountry:
				case COOLDevRestoreCategoryCountry:
					newObject = new COOLCategoryCountry(data, len, formatType);
					break;

				case COOLDevReadCategoryProductionStep:
				case COOLDevWriteCategoryProductionStep:
				case COOLDevDeleteCategoryProductionStep:
				case COOLDevBatchApplyDeleteCategoryProductionStep:
				case COOLDevRestoreCategoryProductionStep:
					newObject = new COOLCategoryProdStep(data, len, formatType);
					break;

				case COOLDevReadCategoryPretext:
				case COOLDevWriteCategoryPretext:
				case COOLDevDeleteCategoryPretext:
				case COOLDevBatchApplyDeleteCategoryPretext:
				case COOLDevRestoreCategoryPretext:
					newObject = new COOLCategoryPretext(data, len, formatType);
					break;

				default:
					if (Debug::isTracing("New.COOLDev"))
					{
						HobString myDebug(" WARNING: COOLDev - Going into GenericHfcObject() - evType = ");
						myDebug.append(Integer::toString(evType));
						Debug::trace("New.COOLDev", myDebug);
					}
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;
		}
		case Numeric:
		{
			switch (evType)
			{

				case NmTest:
					newObject = new HfcNumericTest(data, len, formatType);
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: Numeric - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;
		}
		case NumericSystem:
		{
			switch (evType)
			{

				case SysStatus:
					newObject = (formatType == InternalFormat) ? new HfcNumericSystemStatus(data) : new HfcNumericSystemStatus(data, len, formatType);
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: NumericSystem - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;
		}
		case Marquee:
		{
			switch (evType)
			{

				case MqTest:
					newObject = new HfcMarqueeTest(data, len, formatType);
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: Marquee - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;
		}
		case MarqueeSystem:
		{
			switch (evType)
			{

				case SysStatus:
					newObject = (formatType == InternalFormat) ? new HfcMarqueeSystemStatus(data) : new HfcMarqueeSystemStatus(data, len, formatType);
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: MarqueeSystem - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;
		}

		case GCDSessionDev:
		{
			switch (evType)
			{

			case SysStatus:
				newObject = new HfcGCDSystemStatus(data);
				break;

			default:
				if (Debug::isTracing("New.GenericHfcObject"))
					Debug::trace("New.GenericHfcObject", HobString(" WARNING: GCDSessionDev - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
				newObject = new GenericHfcObject(data, len, formatType);
				break;
			}
			break;
		}

		case GCDDev:
		{
			switch (evType)
			{
			case GcdStatus:
				newObject = new HfcGCDDevStatus(data, len);
				break;

			case GcdFileInfo:
				newObject = new HfcGCDDevFileInfo(data, len);
				break;

			case GcdRequestFileUpload:
				newObject = new HfcGCDDevRequestFileUpload(data, len);
				break;

			case GcdError:
				newObject = new HfcGCDDevError(data, len);
				break;

			default:
				if (Debug::isTracing("New.GenericHfcObject"))
					Debug::trace("New.GenericHfcObject", HobString(" WARNING: GCDDev - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
				newObject = new GenericHfcObject(data, len, formatType);
				break;
			}
			break;
		}

		case SoftkeyDev:
		{
			switch (evType)
			{
				case ExecuteSoftkey:
				{
					newObject = new Softkey(data, len, ExecuteSoftkeyFormat);
					if (Debug::isTracing("Softkey"))
					{
						HobString myDebug("HfcObject::create():  newObject = ");
						myDebug.append(ptrtostring(newObject));
						Debug::trace("Softkey", myDebug);
					}
					break;
				}

				case ReadSoftkey:
				case WriteSoftkey:
				case DeleteSoftkey:
				case BatchApplyDeleteSoftkey:
				case SaveSoftkey:
				case RestoreSoftkey:
				case CreateNewSoftkey:
				case RestoreFlashkeyAsSoftkey:
				{
					newObject = new Softkey(data, len, formatType);
					if (Debug::isTracing("Softkey"))
					{
						HobString myDebug("HfcObject::create():  newObject = ");
						myDebug.append(ptrtostring(newObject));
						Debug::trace("Softkey", myDebug);
					}
					break;
				}

				case ReadSoftkeyFeature:
				case WriteSoftkeyFeature:
				case DeleteSoftkeyFeature:
				case BatchApplyDeleteSoftkeyFeature:
				case SaveSoftkeyFeature:
				case RestoreSoftkeyFeature:
				case CreateNewSoftkeyFeature:
				{
					newObject = new SoftkeyFeature(data, len, formatType);
					if (Debug::isTracing("SoftkeyFeature"))
					{
						HobString myDebug("HfcObject::create():  newObject = ");
						myDebug.append(ptrtostring(newObject));
						Debug::trace("SoftkeyFeature", myDebug);
					}
					break;
				}

				case ReadSoftkeyStyle:
				case WriteSoftkeyStyle:
				case DeleteSoftkeyStyle:
				case BatchApplyDeleteSoftkeyStyle:
				case SaveSoftkeyStyle:
				case RestoreSoftkeyStyle:
				case CreateNewSoftkeyStyle:
				{
					newObject = new SoftkeyStyle(data, len, formatType);
					if (Debug::isTracing("SoftkeyStyle"))
					{
						HobString myDebug("HfcObject::create():  newObject = ");
						myDebug.append(ptrtostring(newObject));
						Debug::trace("SoftkeyStyle", myDebug);
					}
					break;
				}

				case ReadSoftkeyGroup:
#if 0
				case ReadAllSoftkeyGroups:
#endif
				case WriteSoftkeyGroup:
				case DeleteSoftkeyGroup:
				case BatchApplyDeleteSoftkeyGroup:
				case DeleteAllSoftkeyGroups:
				case SaveSoftkeyGroup:
				case RestoreSoftkeyGroup:
				case CreateNewSoftkeyGroup:
				{
					newObject = new SoftkeyGroup(data, len, formatType);
					if (Debug::isTracing("SoftkeyGroup"))
					{
						HobString myDebug("HfcObject::create():  newObject = ");
						myDebug.append(ptrtostring(newObject));
						Debug::trace("SoftkeyGroup", myDebug);
					}
					break;
				}

				case LoadSoftkeyGroup:
				case LoadSoftkeyGroupForEditor:
				{
					newObject = new SoftkeyGroup(data, len, LoadGroupFormat);
					if (Debug::isTracing("SoftkeyGroup"))
					{
						HobString myDebug("HfcObject::create():  LoadSoftkeyGroup, newObject = ");
						myDebug.append(ptrtostring(newObject));
						Debug::trace("SoftkeyGroup", myDebug);
					}
					break;
				}

				case ListCameraSystemPredictions:
					newObject = new hfc::ListCameraSystemPredictionsMessage(data, len, formatType);
					break;

				default:
					if (Debug::isTracing("New.SoftkeyDev"))
					{
						HobString myDebug(" WARNING: SoftkeyDev - Going into GenericHfcObject() - evType = ");
						myDebug.append(Integer::toString(evType));
						Debug::trace("New.SoftkeyDev", myDebug);
					}
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;
		}

		case PeripheralDev:
			switch (evType)
			{
				case DtRead:
				case DtWrite:
				case DtDelete:
				case DtSave:
				case DtRestore:
				case DtCreateNew:
				case RemoveCheckingLostPeripheral:
				case ReadCreatePeripheral:
					newObject = new Peripheral(data, len, formatType);
					if (Debug::isTracing("New.PeripheralDev"))
						Debug::trace("New.PeripheralDev", "    Peripheral: " + ptrtostring(newObject) + " - HfcObject::create()");
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: PeripheralDev - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;

		case ScaleDev:
		{
			if (evType == DtRestore)
			{
				newObject = new hfc::Scale(data, len, formatType);

			}

		}
		break;
		case BatchDev:
		{
			if (evType == DtRestore)
			{
				newObject = new hfc::Batch(data, len, formatType);

			}

		}
		break;

		case NetworkConnectivityDev:
		{
			if (CurrentStatus == evType || NetworkType == evType)
				newObject = (formatType == InternalFormat) ? new HfcNetworkStatus(data) : new HfcNetworkStatus(data, len, formatType);
			break;
		}

		case SynchronizationDev:
		{
			switch ( evType )
			{
				case synchronization::ReadBatches:
				{
					newObject = new hfc::ReadBatchesRecord(data, len, formatType);

					break;
				}
				case synchronization::WriteBatch:
				{
					newObject = new hfc::Batch(data, len, formatType);

					break;
				}
				case synchronization::WriteSynchronizationChange:
				{
					newObject = new hfc::SynchronizationChange(data, len, formatType);

					break;
				}
				default:
				{
					if ( Debug::isTracing("New.GenericHfcObject") )
						Debug::trace("New.GenericHfcObject", " WARNING: SynchronizationDev - Going into GenericHfcObject() - evType = " +
								HobString(Integer::toString(evType)));

					newObject = new GenericHfcObject(data, len, formatType);

					break;
				}
			}

			break;
		}
		case SynchronizationChangeDev:
		{
			if (evType == DtRestore)
				newObject = new hfc::SynchronizationChange(data, len, formatType);
			break;
		}


		case TransactionDev:
		{
			switch ( evType )
			{
				case TransactionReadNonVoidGroup:
				{
					newObject = new hfc::ReadTransactionGroupMessage(data, len, formatType);

					break;
				}
				case TransactionVoidTransactions:
				{
					newObject = new hfc::VoidTransactionsMessage(data, len, formatType);

					break;
				}
				case DtRestore:
				{
					newObject = new hfc::Transaction(data, len, formatType);

					if ( newObject && Debug::isTracing("New.Transaction") )
					{
						Debug::trace("New.Transaction", "HfcObject::create(): Event = " + util_string::uinttostring(evType) );
					}
					break;
				}
				default:
				{
					if ( Debug::isTracing("New.GenericHfcObject") )
						Debug::trace("New.GenericHfcObject", " WARNING: TransactionDev - Going into GenericHfcObject() - evType = " +
								util_string::uinttostring(evType));

					newObject = new GenericHfcObject(data, len, formatType);
					break;
				}
			}
			break;
		}

		case TransactionTypeDev:
		{
			switch ( evType )
			{
				case DtRead:
				case DtWrite:
				case DtDelete:
				case DtBatchApplyDelete:
				case DtSave:
				case DtRestore:
				case DtFirst:
				case DtLast:
				case DtNext:
				case DtPrevious:
				{
					newObject = new hfc::TransactionType(data, len, formatType);
					if ( newObject && Debug::isTracing("New.TransactionType") )
					{
						hfc::TransactionType * newTT = (hfc::TransactionType *)newObject;
						Debug::trace("New.TransactionType", "HfcObject::create(): Event = " + util_string::uinttostring(evType) +
							"  TransactionType: " + util_string::ptrtostring(newObject) + "  TransactionType # = " +
							longtostring(newTT->getNumber()) + "  TransactionType name1 = " + newTT->getName1());
					}

					break;
				}
//				case TransactionTypeSetCurrentType:
//				case TransactionTypeSetCurrentRunModeType:
				case TransactionTypeWriteCurrentType:
				case TransactionTypeRefreshCurrentType:
				default:
				{
					Debug::trace("New.GenericHfcObject",
						"WARNING: TransactionTypeDev - Going into GenericHfcObject() - evType = " + util_string::uinttostring(evType));

					newObject = new GenericHfcObject(data, len, formatType);

					break;
				}
			}

			break;
		}

		case HourlyTotalDev:
		{
			switch ( evType )
			{
				case hfc::ReadFilteredHourlyTotals:
				{
					newObject = new hfc::ReadFilteredHourlyTotalsMessage(data, len, formatType);

					break;
				}

				default:
				{
					Debug::trace("New.GenericHfcObject",
						"WARNING: HourlyTotalDev - Going into GenericHfcObject() - evType = " + util_string::uinttostring(evType));

					newObject = new GenericHfcObject(data, len, formatType);

					break;
				}
			}

			break;
		}

		case OperatorTotalDev:
		{
			switch ( evType )
			{
				case hfc::ReadFilteredOperatorTotals:
				{
					newObject = new hfc::ReadFilteredOperatorTotalsMessage(data, len, formatType);

					break;
				}

				default:
				{
					Debug::trace("New.GenericHfcObject",
						"WARNING: OperatorTotalDev - Going into GenericHfcObject() - evType = " + util_string::uinttostring(evType));

					newObject = new GenericHfcObject(data, len, formatType);

					break;
				}
			}

			break;
		}

		case ProductTotalDev:
		{
			switch ( evType )
			{
				case hfc::ReadFilteredProductTotals:
				{
					newObject = new hfc::ReadFilteredProductTotalsMessage(data, len, formatType);

					break;
				}

				default:
				{
					Debug::trace("New.GenericHfcObject",
						"WARNING: ProductTotalDev - Going into GenericHfcObject() - evType = " + util_string::uinttostring(evType));

					newObject = new GenericHfcObject(data, len, formatType);

					break;
				}
			}

			break;
		}

		case OperatorChangesDev:
		{
			switch ( evType )
			{
				case hfc::ReadOperatorChangeSummary:
				{
					newObject = new hfc::ReadOperatorChangeSummaryMessage(data, len, formatType);

					break;
				}

				case hfc::ReadOperatorChangesGroup:
				{
					newObject = new hfc::ReadOperatorChangeGroupMessage(data, len, formatType);

					break;
				}

				default:
				{
					Debug::trace("New.GenericHfcObject",
						"WARNING: OperatorChangesDev - Going into GenericHfcObject() - evType = " + util_string::uinttostring(evType));

					newObject = new GenericHfcObject(data, len, formatType);

					break;
				}
			}

			break;
		}

		case WindowsConfigurationDev:
		{
			switch ( evType )
			{
				case backupWirelessProfiles:
				{
					newObject = new WirelessProfilesSelections(data, len, formatType);

					break;
				}
			}
			break;
		}

		case CutTestDev:
		{
			switch ( evType )
			{
				case DtRead:
				case DtRestore:
				case BeginCuttingTest:
				case CancelCuttingTest:
				case DtDelete:
				case CuttingTestDetailsRead:
				case SuspendCuttingTest:          // UI sends CutTest data
				case DeleteCuttingTest:           // UI sends cutTestNum as first param.
				case ResumeCuttingTest:           // UI sends cutTestNum as first param.
				case SaveCuttingTestToUsb:        // UI sends cutTestNumber, filePath
				case CompleteCuttingTest:         // UI sends after final comments entered

					newObject = new CuttingTest(data, len, formatType);
					break;
			}
			break;
		}

#if defined ( __linux__ )
		case BoardSite:
		{
			switch( evType )
			{
				case BrdSiteBoardTestCompleteUI: {
					newObject = new HfcBoardSiteTestCompleteUI( data );
					break;
				}
				case BrdSitePercentCompete: {
					newObject = new HfcBoardSitePercentComplete( data );
					break;
				}
				case BrdSiteTestStatus: {
					newObject = new HfcBoardSiteTestStatus( data );
					break;
				}	
			}
			break;			
		}
		
		case HIDDev:
		{
			newObject = new HfcHidDeviceEvent(data, len, formatType);
			break;
		}
#endif
		case MfgTestDev:
		{
			switch (evType)
			{
				case WriteMfgData: {
					newObject = new HfcWriteFpmManData(data, len, formatType);
					break;
				}
				case ReadMfgData: {
					newObject = new HfcWriteFpmManData(data, len, formatType);
					break;
				}
				case RequestPrinterLabelCount: {
					newObject = new HfcRequestPrinterLabelCount(data, len, formatType);
					break;
				}
				default: {
					newObject = new GenericHfcObject(data, len, formatType);
					break;
				}
			}
			break;
		}
		case RecallDev:
		{
			switch (evType)
			{
				case DtRead:
				case DtWrite:
				case DtDelete:
				case DtBatchApplyDelete:
				case DtSave:
				case DtRestore:
				case DtCreateNew:
				case AcknowledgeRecall:

					newObject = new Recall(data, len, formatType);
					if (Debug::isTracing("New.Recall"))
						Debug::trace("New.Recall", "    Recall: " + ptrtostring(newObject) + "  Recall number:" "???" " - HfcObject::create()");
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: Recall - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;
		}

		case DeviceDev:
		{
			switch (evType)
			{
				case DtRead:
				case DtWrite:
				case DtDelete:
				case DtSave:
				case DtRestore:
				case DtCreateNew:

					newObject = new Device(data, len, formatType);
					if (Debug::isTracing("New.Device"))
						Debug::trace("New.Device", "    Device: " + ptrtostring(newObject) + " - HfcObject::create()");
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: Device - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;
		}

		case ProductTextCfgDev:
		{
			switch (evType)
			{
				case DtRead:
				case DtWrite:
				case DtDelete:
				case DtSave:
				case DtRestore:
				case DtCreateNew:

					newObject = new ProductTextCfg(data, len, formatType);
					if (Debug::isTracing("New.ProductTextCfg"))
						Debug::trace("New.ProductTextCfg", "    ProductTextCfg: " + ptrtostring(newObject) + " - HfcObject::create()");
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: ProductTextCfg - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;
		}

		case BarcodeScannerDev:
		{
			switch (evType)
			{
				case DtRead:
				case DtWrite:
				case DtDelete:
				case DtSave:
				case DtRestore:
				case DtCreateNew:
				case BarcodeScannerEnable:
				case BarcodeScannerDisable:
				case BarcodeReadScannerCode:
					newObject = (formatType == InternalFormat) ? new HfcScannerProductRead(data) : new HfcScannerProductRead(data, len, formatType);
					//newObject = new HfcScannerProductRead(data, len, formatType);
					if (Debug::isTracing("New.HfcScannerProductRead"))
						Debug::trace("New.HfcScannerProductRead", "    HfcScannerProductRead: " + ptrtostring(newObject) + " - HfcObject::create()");
					break;

				default:
					if (Debug::isTracing("New.GenericHfcObject"))
						Debug::trace("New.GenericHfcObject", HobString(" WARNING: HfcScannerProductRead - Going into GenericHfcObject() - evType = ") + HobString(Integer::toString(evType)));
					newObject = new GenericHfcObject(data, len, formatType);
					break;
			}
			break;
		}

		case Diagnostics:
			if ( DiagConfigurationReq == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsConfigurationReq>( data, len, formatType );
			else if ( DiagConfiguration == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsConfiguration>( data, len, formatType );

			else if ( DiagStatusReq == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsStatusReq>( data, len, formatType );
			else if ( DiagStatus == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsStatus>( data, len, formatType );

			else if ( DiagAuditLogReq == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsAuditLogReq>( data, len, formatType );
			else if ( DiagAuditLog == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsAuditLog>( data, len, formatType );
			else if ( DiagAuditLogRecordReq == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsAuditLogRecordReq>( data, len, formatType );
			else if ( DiagAuditLogRecord == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsAuditLogRecord>( data, len, formatType );

			else if ( DiagAuditLogServiceVisitStart == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsAuditLogServiceVisitStart>( data, len, formatType );
			else if ( DiagAuditLogServiceVisitStop == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsAuditLogServiceVisitStop>( data, len, formatType );
			else if ( DiagAuditLogServiceVisitStatus == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsAuditLogServiceVisitStatus>( data, len, formatType );

			else if ( DiagCheckErrorHandlerReq == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsCheckErrorHandlerReq>( data, len, formatType );
			else if ( DiagCheckErrorHandler == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsCheckErrorHandler>( data, len, formatType );
			else if ( DiagStartErrorHandler == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsStartErrorHandler>( data, len, formatType );
			else if ( DiagStartErrorHandlerFromMaintRecord == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsStartErrorHandlerFromMaintRecord>( data, len, formatType );
			else if ( DiagErrorHandlerFailed == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsErrorHandlerFailed>( data, len, formatType );

			else if ( DiagShowWizard == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsShowWizard>( data, len, formatType );
			else if ( DiagInitWizardReq == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsInitWizardReq>( data, len, formatType );
			else if ( DiagInitWizard == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsInitWizard>( data, len, formatType );
			else if ( DiagPerformWizardTestingReq == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsPerformWizardTestingReq>( data, len, formatType );
			else if ( DiagPerformWizardTesting == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsPerformWizardTesting>( data, len, formatType );
			else if ( DiagExitWizard == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsExitWizard>( data, len, formatType );

			else if ( DiagServiceMaintLogReq == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsServiceMaintLogReq>( data, len, formatType );
			else if ( DiagServiceMaintLog == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsServiceMaintLog>( data, len, formatType );
			else if ( DiagClearServiceMaintRecord == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsClearServiceMaintRecord>( data, len, formatType );
			else if ( DiagPrintServiceMaintLog == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsPrintServiceMaintLog>( data, len, formatType );

			else if ( DiagServiceActionsReq == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsServiceActionsReq>( data, len, formatType );
			else if ( DiagServiceActions == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsServiceActions>( data, len, formatType );

			else if ( DiagExportDatabase == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsExportDatabase>( data, len, formatType );
			else if ( DiagExportDatabaseComplete == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsExportDatabaseComplete>( data, len, formatType );

			else if ( DiagSendDatabaseEmail == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsSendDatabaseEmail>( data, len, formatType );
			else if ( DiagSendDatabaseGatewayDevice == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsSendDatabaseGatewayDevice>( data, len, formatType );
			else if ( DiagSendDatabaseComplete == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsSendDatabaseComplete>( data, len, formatType );

			else if ( DiagReport == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsReport>( data, len, formatType );

			else if ( DiagPrintLabelStats == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsPrintLabelStats>( data, len, formatType );

			else if ( DiagValueMax == evType )
				newObject = new HfcDiagnosticsMessage<DiagnosticsValueMax>( data, len, formatType );

			else
			{
				if ( Debug::isTracing( "New.GenericHfcObject" ) )
					Debug::trace( "New.GenericHfcObject", " WARNING: HfcDiagnosticsMessage - Going into GenericHfcObject() - evType = " + Integer::toString( evType ) );
				newObject = new GenericHfcObject( data, len, formatType );
			}
			break;

		case UnknownDevice:
		default:
			// Don't know what this is, but I'll create a generic HfCObject for you.
			if (Debug::isTracing("New.GenericHfcObject"))
			{
				HobString myDebug(" WARNING: UnknownDevice - Going into GenericHfcObject() - evType = ");
				myDebug.append(Integer::toString(evType));
				Debug::trace("New.GenericHfcObject", myDebug);
			}
			newObject = new GenericHfcObject(data, len, formatType);
			break;
	}
	return newObject;
}


int HfcObject::getFormatType() const
{
	return formatType;
}
void HfcObject::setFormatType(int formatType)
{
	this->formatType = formatType;
}


bool HfcObject::isReceived(int fieldOffset) const
{
	if (fieldOffset == 0 || fieldOffset > maxFieldOffsetReceivedBytes * 8)
	{
		Debug::trace("HfcObject.exception", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		Debug::trace("HfcObject.exception", HobString("HfcObject::setReceived: fieldOffset too big: ") + HobString(Integer::toString(fieldOffset)));
		Debug::trace("HfcObject.exception", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return false;
	}

	short byteOffset = fieldOffset / 8;
	if (byteOffset >= 0 && byteOffset < maxFieldOffsetReceivedBytes)
	{
		short bitOffset = fieldOffset % 8;
		unsigned char byte = receivedFlags[byteOffset] >> bitOffset;

		if (Debug::isTracing("HfcObject.received"))
			Debug::trace("HfcObject.received", HobString("isReceived(") + HobString(Integer::toString(fieldOffset)) +
															HobString(")  ") + HobString(Hex::toString((char *)receivedFlags, maxFieldOffsetReceivedBytes)) +
															HobString("  result: ") + HobString(Bool::toString(byte & 0x01)));
		return byte & 0x01;
	}
	else
		return false;
}

void HfcObject::setReceived(int fieldOffset, bool received)
{
	bool tracing = (Debug::isTracing("HfcObject.received")) ? true : false;

	if (fieldOffset == 0 || fieldOffset > maxFieldOffsetReceivedBytes * 8)
	{
		Debug::trace("HfcObject.exception", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		Debug::trace("HfcObject.exception", HobString("HfcObject::setReceived: fieldOffset too big: ") + HobString(Integer::toString(fieldOffset)));
		Debug::trace("HfcObject.exception", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return;
	}

	short byteOffset = fieldOffset / 8;
	if (byteOffset >= 0 && byteOffset < maxFieldOffsetReceivedBytes)
	{
		short bitOffset = fieldOffset % 8;
		unsigned char byte = 0x01 << bitOffset;

		if (tracing)
			Debug::trace("HfcObject.received", HobString("before setReceived(offset:") + HobString(Integer::toString(fieldOffset)) +
															HobString(", flag:") + HobString(Bool::toString(received)) + HobString(") result: ") +
															HobString(Hex::toString((char *)receivedFlags, maxFieldOffsetReceivedBytes)));
		if (received)
			receivedFlags[byteOffset] |= byte;
		else
			receivedFlags[byteOffset] &= ~byte;

		if (tracing)
			Debug::trace("HfcObject.received", HobString("after  setReceived(offset:") + HobString(Integer::toString(fieldOffset)) +
															HobString(", flag:") + HobString(Bool::toString(received)) + HobString(") result: ") +
															HobString(Hex::toString((char *)receivedFlags, maxFieldOffsetReceivedBytes)));
	}
}
void HfcObject::setAllReceived(bool received)
{
	memset(receivedFlags, (received ? 0xFF : 0), maxFieldOffsetReceivedBytes);
	if (Debug::isTracing("HfcObject.allreceived"))
		Debug::trace("HfcObject.allreceived", HobString("setAllReceived(") + HobString(Bool::toString(received)) + HobString(") result: ") +
														HobString(Hex::toString((char *)receivedFlags, maxFieldOffsetReceivedBytes)));
}


const char *HfcObject::serialize(int format) const
{
	/* Check to see if this HfcObject instance is overriding the 'default' format
	 * being passed in.  If the HfcObject's formatType is not Unkown, then the
	 * programmer is overriding the format being passed in, so use it.
	 */
	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	serializedData = HobString("HfcObject::serialize - format:") + HobString(Integer::toString(format));
	return serializedData.c_str();
}

const HobString &HfcObject::getSerializedData() const
{
	return serializedData;
}

UINT32 HfcObject::getSerializedLength() const
{
	// todo - If serialize() hasn't yet been called, should this function call it first;
	// otherwise, the length will be invalid, yet the programmer may not have known to
	// call serialize() first.
	return (UINT32)serializedData.length();
}

const char *HfcObject::toString()
{
	return serialize(BrowserFormat);
}


HfcError *HfcObject::firstError()
{
	if (!errorList || errorList->empty())
		return (HfcError *)NULL;
	else
	{
		errorListIterator = errorList->begin();
		return (errorListIterator == errorList->end()) ? (HfcError *)NULL : *errorListIterator;
	}
}

HfcError *HfcObject::nextError()
{
	if (!errorList || errorList->empty())
		return (HfcError *)NULL;
	else
	{
		if (errorListIterator != errorList->end())
			errorListIterator++;
		return (errorListIterator == errorList->end()) ? (HfcError *)NULL : *errorListIterator;
	}
}

bool HfcObject::hasErrors()
{
	bool ret = true;
	if (!errorList)
		ret = false;
	else if (errorList->empty())
		ret = false;

	if (Debug::isTracing("HfcObject.error"))
		Debug::trace("HfcObject.error", HobString("hasErrors() - returning ") + HobString(Bool::toString(ret)));
	return ret;
}

void HfcObject::addError(int errorCode, const HobString &failedTag)
{
	bool tracing = (Debug::isTracing("HfcObject.error")) ? true : false;

	if (!errorList)
	{
		errorList = new std::vector<HfcError *>(0);
		if (errorList)
		{
			if (tracing)
			{
				Debug::trace("HfcObject.error", "addError() - created errorList ");
				Debug::trace("HfcObject.error", HobString("addError() - initial errorList size is ") + HobString(Integer::toString((int)errorList->size())));
			}
		}
		else
		{
			Debug::trace("HfcObject.error", "addError() - tried to create errorList but couldn't ");
			return;
		}
	}

	HobString keyVal = getMyKeyValue();
	HfcError *newError = new HfcError(errorCode, getMyEvolDevice(), failedTag, keyVal);
	if (newError)
	{
		if (tracing)
			Debug::trace("HfcObject.error", HobString("addError() - adding to errorList  errorcode: ") + HobString(Integer::toString(errorCode)) + HobString("  tag: ") + failedTag +
														HobString("  keyValue: ") + keyVal + HobString("  device: ") + HobString(Integer::toString(getMyEvolDevice())));
		errorList->push_back(newError);
	}
}

void HfcObject::clearErrors()
{
	bool tracing = Debug::isTracing("HfcObject.error");

	if (errorList)
	{
		if (tracing)
			Debug::trace("HfcObject.error", "errorList object is not NULL");

		if (!errorList->empty())
		{
			if (tracing)
				Debug::trace("HfcObject.error", HobString("Deleting HfcErrors in the errorList - list size: ") + HobString(Long::toString((long)errorList->size())));

			errorListIterator = errorList->begin();
			while (errorListIterator != errorList->end())
			{
				HfcError *temp = dynamic_cast<HfcError *> (*errorListIterator);
				if (temp)
				{
					if (tracing)
					{
						Debug::trace("HfcObject.error", HobString("Deleting an HfcError in the errorList - errorCode: ") + HobString(Integer::toString((int)temp->getErrorCode())) +
																	HobString("  tag: ") + temp->getFailedTag() +
																	HobString("  keyval: ") + temp->getKeyValue() +
																	HobString("  dev: ") + HobString(Integer::toString((int)temp->getDevice())));
					}
					delete temp;
				}
				errorList->erase(errorListIterator);
				errorListIterator = errorList->begin();
			}

			if (tracing)
				Debug::trace("HfcObject.error", HobString("Deleting HfcErrors in the errorList - list size is now: ") + HobString(Long::toString((long)errorList->size())));
		}
		else if (tracing)
			Debug::trace("HfcObject.error", "errorList is empty");

		delete errorList;
		errorList = (std::vector<HfcError *> *)NULL;
	}
	else
		if (tracing)
		 	Debug::trace("HfcObject.error", "HfcErrors errorList is NULL");
}


// virtual, basic functionality provided by HfcObject, but should be overridden by derived class.
HobString HfcObject::getMyKeyValue() const
{
	return HobString(Long::toString((long) RECORD_NOT_FOUND));
}

// virtual, basic functionality provided by HfcObject, but should be overridden by derived class.
EvolDeviceType HfcObject::getMyEvolDevice() const
{
	return UnknownDevice;
}


// protected
HfcObject::HfcObject()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	serializedData = "";
	formatType = UnknownFormat;
	setAllReceived(false);
	errorList = (std::vector<HfcError *> *)NULL;
/*
	if (Debug::isTracing("HfcObject.create"))
	{
		Debug::trace("HfcObject.create", HobString("Creating HfcObject from default constructor - this: ") + HobString(Hex::toString((long) this)));
		Debug::trace("HfcObject.create", HobString("Creating HfcObject from default constructor - hasErrors: ") + HobString(Bool::toString(hasErrors())));
	}
*/
}

// protected
// Objects of this class can only be created by the static create() Factory
// method in the parent class, SessionManager.
HfcObject::HfcObject(const char * /* msgData */, int /* msgLen */)
{
/*
	bool tracing = (Debug::isTracing("HfcObject.create")) ? true : false;
	if (tracing)
		Debug::trace("HfcObject.create", HobString("Creating HfcObject from char * - this: ") + HobString(Hex::toString((long) this)));
*/
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	serializedData = "";
	formatType = UnknownFormat;
	setAllReceived(false);
	errorList = (std::vector<HfcError *> *)NULL;

	// Parse the data string, reconstructing a Message.
	// EvolMessage,data
/*
	if (tracing)
	{
		if (msgLen >= 0)
		{
			HobString hMsgData(msgData, msgLen);
			Debug::trace("HfcObject.create", HobString("Constructing HfcObject from argument: ") + hMsgData);
		}
	}

	if (tracing)
		Debug::trace("HfcObject.create", HobString("Creating HfcObject from char * - hasErrors: ") + HobString(Bool::toString(hasErrors())));
*/
}

HfcObject::HfcObject(const HfcObject& newObj)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	serializedData = newObj.serializedData;
	formatType = newObj.formatType;

	for (int i = 0; i < maxFieldOffsetReceivedBytes; i++)
		receivedFlags[i] = newObj.receivedFlags[i];

	errorList = (std::vector<HfcError *> *)NULL;

}

// protected
HfcObject::~HfcObject()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	bool tracing = (Debug::isTracing("HfcObject.delete")) ? true : false;
	if (tracing)
		Debug::trace("HfcObject.delete", "Deleting HfcObject " + ptrtostring(this));

	clearErrors();
}
