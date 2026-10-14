-- The following statements add default records to the database.

insert into Configs (CoName, CoValue, CoCheck) values ('storeId','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('storeName','',1);
insert into Configs (CoName, CoValue, CoCheck) values ('storeGraphicName',NULL,1);

insert into Configs (CoName, CoValue, CoCheck) values ('primaryCountryCode','US',1);
insert into Configs (CoName, CoValue, CoCheck) values ('primaryLanguageCode','en',1);
insert into Configs (CoName, CoValue, CoCheck) values ('secondaryCountryCode','MX',1);
insert into Configs (CoName, CoValue, CoCheck) values ('secondaryLanguageCode','es',1);
insert into Configs (CoName, CoValue, CoCheck) values ('lengthUnits','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('temperatureUnits','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('dateDisplayFormat','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('timeDisplayFormat','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('securityLblUnitPrice','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('securityLblTtlPrice','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('fractionalPricingEnabled','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('lbsForPricingEnabled','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('byCountCalcMethod','3',1);
insert into Configs (CoName, CoValue, CoCheck) values ('clearPluAfterPrinting','1',1);

insert into Configs (CoName, CoValue, CoCheck) values ('beepOnStableWeight','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('beeperVolumeLevel','5',1);

insert into Configs (CoName, CoValue, CoCheck) values ('fixedWeightUnits','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('weigherAttached','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('tareSetback','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('maxTare','9995',1);
insert into Configs (CoName, CoValue, CoCheck) values ('platterTareEnabled','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('changeZeroTareOnlyOnce','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('changeForcedTareOnlyOnce','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('scaleWeighMode','1',1);	-- default to AVOIR, to preserve the setting of externalWeightPrecision

insert into Configs (CoName, CoValue, CoCheck) values ('externalWeightPrecision','2',1);
insert into Configs (CoName, CoValue, CoCheck) values ('userRezeroWeightLimit','10',1);
insert into Configs (CoName, CoValue, CoCheck) values ('weigherWakeupSensorAttached','1',1);

insert into Configs (CoName, CoValue, CoCheck) values ('loginRequired','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('autoLogoutTime','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('autoLookupProduct','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('autoLookupNumDigits','4',1);
insert into Configs (CoName, CoValue, CoCheck) values ('autoLookupOverrideDigit','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('autoDisplayProductNotes','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('autoDisplaySuggestiveSelling','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('autoDisplayCookingNotes','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('horizLabelSkew1','5',1);
insert into Configs (CoName, CoValue, CoCheck) values ('horizLabelSkew2','5',1);
insert into Configs (CoName, CoValue, CoCheck) values ('horizLabelSkew3','5',1);

insert into Configs (CoName, CoValue, CoCheck) values ('collectTransactions','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('collectTransactionsDays','8',1);
insert into Configs (CoName, CoValue, CoCheck) values ('trackingOperatorChanges','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('opAccessToBrowserMode','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('opAccessToDesktopMode','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('autoLoadOldFirmwareVersions','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('autoSearchNewFirmwareVersions','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('newFirmwareVersionSearchTime','05:00 AM',1);

insert into Configs (CoName, CoValue, CoCheck) values ('forceUniqueProdClassNums','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enablePowerSaverMode','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('powerSaverModeTimeout','10',1);
insert into Configs (CoName, CoValue, CoCheck) values ('defaultBrowserHomePage','defaultBrowserHomepage.html',1);
insert into Configs (CoName, CoValue, CoCheck) values ('defaultRunScreenHomePage','defaultBrowserHomepage.html',1);
insert into Configs (CoName, CoValue, CoCheck) values ('defaultProductRecordType','1',1);

insert into Configs (CoName, CoValue, CoCheck) values ('defaultFilmWidth','3',1);
insert into Configs (CoName, CoValue, CoCheck) values ('defaultFilmTension','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('defaultFilmTensionFrontToBack','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('defaultFilmTensionLeftToRight','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('defaultIntakeMode','2',1);
insert into Configs (CoName, CoValue, CoCheck) values ('defaultLabelPlacement','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('defaultApplierRotation','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('sealerBeltTimeOnSun',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('sealerBeltTimeOffSun',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('sealerBeltTimeOnMon',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('sealerBeltTimeOffMon',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('sealerBeltTimeOnTue',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('sealerBeltTimeOffTue',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('sealerBeltTimeOnWed',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('sealerBeltTimeOffWed',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('sealerBeltTimeOnThu',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('sealerBeltTimeOffThu',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('sealerBeltTimeOnFri',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('sealerBeltTimeOffFri',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('sealerBeltTimeOnSat',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('sealerBeltTimeOffSat',NULL,1);

insert into Configs (CoName, CoValue, CoCheck) values ('dbBackupFrequency','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('autoCompressDB','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('compressDBTime','04:00 AM',1);

insert into Configs (CoName, CoValue, CoCheck) values ('COOLTrackingNumberEnabled','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('dontChangeCurrentCOOLViaComm','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('primaryExpirationPageNote','Check for expiring product in the case daily.',1);
insert into Configs (CoName, CoValue, CoCheck) values ('secondExpirationPageNote','Check for expiring product in the case daily.',1);

insert into Configs (CoName, CoValue, CoCheck) values ('checkingNetworkConnectivity','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('offlineDateTime',NULL,1);

insert into Configs (CoName, CoValue, CoCheck) values ('communicationHistory',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('communicationItemReceived',NULL,1);

insert into Configs (CoName, CoValue, CoCheck) values ('popupKeypadEnabled','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('popupKeypadStyle','2',1);
insert into Configs (CoName, CoValue, CoCheck) values ('popupKeypadLocation','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('operatorLoginKeyboardStyle','2',1);

insert into Configs (CoName, CoValue, CoCheck) values ('capacityBarLocation','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('pingPrimary','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('pingGateway','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('pingTarget','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('pingInterval','00:30',1);
insert into Configs (CoName, CoValue, CoCheck) values ('pingTargetIPAddress','0.0.0.0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('rebootUnprotectedAllowed','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('autoCreateRFIDOperator','1',1);

insert into Configs (CoName, CoValue, CoCheck) values ('ignoreUltimaFormatLabelTypes','1',1);

insert into Configs (CoName, CoValue, CoCheck) values ('reportsOffsetTop','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('reportsOffsetBottom','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('showDeleteAllFiles','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('showPrinterSettings','1',1);

insert into Configs (CoName, CoValue, CoCheck) values ('remoteAccessPermitted','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('remoteAccessRequiresLogin','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('remoteAccessLoginMethod','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('remoteAccessUsername',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('remoteAccessPassword',NULL,1);

insert into Configs (CoName, CoValue, CoCheck) values ('backdateByDays','1',1);


insert into Configs (CoName, CoValue, CoCheck) values ('systemFont',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('startScreenHome','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('browserHasAddressBar','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('defaultSearchMethod','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('resetStatisticsDays','10',1);
insert into Configs (CoName, CoValue, CoCheck) values ('leftRunscreenContent','32',1);
insert into Configs (CoName, CoValue, CoCheck) values ('runscreenDynamicContent','4',1);
insert into Configs (CoName, CoValue, CoCheck) values ('runscreenKeypadContent','2',1);
insert into Configs (CoName, CoValue, CoCheck) values ('rightScreenContent','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('mostRecentPlanogramGraphic',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('mostRecentPlanogramGraphicCRC','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('mostRecentPlanogramAcknowledged','1',1);

insert into Configs (CoName, CoValue, CoCheck) values ('enableDisplayNetworkDetails','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('disableOperatorFlashkeyEditor','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('disableLeftRunscreen','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('mostRecentRunMode','5',1);

insert into Configs (CoName, CoValue, CoCheck) values ('displayTransTypeIndicator','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('nowServingQueueIPAddress','0.0.0.0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('nowServingSysType','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('fsRoundingMethod','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('fsDiscountMethod','9',1);
insert into Configs (CoName, CoValue, CoCheck) values ('fsDiscountType','2',1);
insert into Configs (CoName, CoValue, CoCheck) values ('fsDiscountFormula','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('totalPriceText1',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('totalPriceText2',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('savingsText1',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('savingsText2',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('unitPriceText1',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('unitPriceText2',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('fsRoundingWeightPrecision','2',1);

insert into Configs (CoName, CoValue, CoCheck) values ('priceModifiable','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('byCountModifiable','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('tareModifiable','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('fixedWghtModifiable','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('shelfLifeModifiable','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('prodLifeModifiable','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('labelTypeModifiable','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('dateTimeModifiable','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('portionsModifiable','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('priceAccessLevel','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('byCountAccessLevel','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('tareAccessLevel','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('fixedWghtAccessLevel','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('shelfLifeAccessLevel','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('prodLifeAccessLevel','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('labelTypeAccessLevel','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('dateTimeAccessLevel','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('portionsAccessLevel','1',1);

insert into Configs (CoName, CoValue, CoCheck) values ('sellByDatePrinted','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('sellByDateJulian','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('packedOnDatePrinted','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('packedOnDateJulian','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('prodLifeDatePrinted','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('prodLifeDateJulian','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('unitPricePrinted','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('byCountPrinted','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('barCodePrinted','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('currencySymbolPrinted','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('textNamesPreprinted','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('wghtOnByCountPrinted','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('oneOnByCountPrinted','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('secondCheckDigit','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('printFirstLetterOfTotalType','-1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('netWtOnFixedWtAlwaysPrinted','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('fixedWeightFormat','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('scaleSynchronizationType','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('primaryScale',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncConfigurations','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncEmailAlerts','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncProductDefaults','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncClasses','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncExpandedTexts','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncSpecialMessages','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncNutritionFacts','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncProductNotes','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncMarqueeTexts','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncTransactions','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncTransactionTypes','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncCategories','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncSpecials','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncCoolCategories','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncCoolCountries','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncCoolProductionSteps','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncCoolPreTexts','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncCoolTexts','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncCoolListTexts','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncCoolLists','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncCategoryCountries','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncCategoryProductionSteps','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncCategoryPreTexts','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncProducts','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncExpirationLabels','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncLabels','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncLabelTexts','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncOperatorNotes','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncOperators','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncSoftkeyGroups','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncSoftkeyStyles','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncSoftkeyFeatures','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncSoftkeys','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncLabelGraphics','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncCategoryGraphics','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncProductGraphics','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncSoftkeyGraphics','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('syncSpecialsGraphics','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('enableSellBy','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableBestBefore','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enablePackedOn','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableProdImg','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableUpcNumber','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableFilmWidth','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableFilmTension','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableWrapTypes','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableCountryOfOrigin','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableTrackingNumber','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableRunTotals','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableProductionEntry','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('productionEntryContPrint','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableProductionPlanning','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('productionPlanningFrame',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('productionPlanningUpdate',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableSelfServe','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('pluSearchRange','10',1);
insert into Configs (CoName, CoValue, CoCheck) values ('appendingPluNumberToName','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('numOfDescCharsToUse','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('rotOverrideSetting1','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('rotOverrideValue1','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('rotOverrideSetting2','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('rotOverrideValue2','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('rotOverrideSetting3','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('rotOverrideValue3','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('customerDisplay','2',1);
insert into Configs (CoName, CoValue, CoCheck) values ('gcdLayoutNumber','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('gcdColorSchemeNumber','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('gcdAllowOperReset','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('gcdVolume','60',1);
insert into Configs (CoName, CoValue, CoCheck) values ('gcdPlaybackWaitTime','30',1);
insert into Configs (CoName, CoValue, CoCheck) values ('gcdMediaServerName',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('gcdMediaServerIP','0.0.0.0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('gcdFtpUser',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('gcdFtpPassword',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('gcdFtpDir',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('gcdPlaylistFile','playlist.txt',1);
insert into Configs (CoName, CoValue, CoCheck) values ('gcdAutoSearch','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('gcdSearchHour','12',1);
insert into Configs (CoName, CoValue, CoCheck) values ('gcdSearchMinute','00',1);
insert into Configs (CoName, CoValue, CoCheck) values ('gcdSearchAmOrPm','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('gcdIsDnsEnabled','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('gcdDownloadProtocol','SFTP',1);
insert into Configs (CoName, CoValue, CoCheck) values ('gcdLanguage','4',1);
insert into Configs (CoName, CoValue, CoCheck) values ('printWithoutCustomerDisplay','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('gcdUseTransactionalLayout','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('gcdIdleAdvertType','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('gcdIdleWebsiteAddress',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('gcdProductAdvertType','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('enableAccumItems','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableItemStripMode','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('accumItemsReturnToStartScreen','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableAccumItemsTotalsOnly','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('accumItemsEANFormatString','XXEEEEEPPPPP',1);
insert into Configs (CoName, CoValue, CoCheck) values ('accumItemsEANPrefix','99',1);
insert into Configs (CoName, CoValue, CoCheck) values ('accumItemsEANCode','12345',1);
insert into Configs (CoName, CoValue, CoCheck) values ('accumItemsScaleId','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('accumItemsTransactionNumber','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('accumItemsUPCProductNumber ','6636',1);
insert into Configs (CoName, CoValue, CoCheck) values ('accumBarcodeUseFirstItem','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('readTransactionsCompatibility','0',1);


insert into Configs (CoName, CoValue, CoCheck) values ('maxLabelTexts','20',1);

insert into Configs (CoName, CoValue, CoCheck) values ('gridXElements','5',1);
insert into Configs (CoName, CoValue, CoCheck) values ('gridYElements','4',1);
insert into Configs (CoName, CoValue, CoCheck) values ('numOfMostCommonCategories','5',1);
insert into Configs (CoName, CoValue, CoCheck) values ('imageTypeOnStartScreen','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('initialStreamingGraphicNum','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('productStatsHoursMethod','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('selfServeInactivityTimeout','20',1);
insert into Configs (CoName, CoValue, CoCheck) values ('logoGraphicFileName',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('noGraphicAvailableFileName1','noImage',1);
insert into Configs (CoName, CoValue, CoCheck) values ('noGraphicAvailableFileName2','noImageES',1);
insert into Configs (CoName, CoValue, CoCheck) values ('previousGraphicFileName1','less.png',1);
insert into Configs (CoName, CoValue, CoCheck) values ('previousGraphicFileName2','less.png',1);
insert into Configs (CoName, CoValue, CoCheck) values ('nextGraphicFileName1','more.png',1);
insert into Configs (CoName, CoValue, CoCheck) values ('nextGraphicFileName2','more.png',1);

insert into Configs (CoName, CoValue, CoCheck) values ('from',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('toList',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('smtpIpAddrOrDnsName',NULL,1);

insert into Configs (CoName, CoValue, CoCheck) values ('ClearProdClassTotsClearsTrans','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('ClearHourlyTotsClearsTrans','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('ClearOperatorTotsClearsTrans','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('ClearCOOLTotsClearsTrans','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('ClearOperChangesClearsTrans','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('decimalSeparator','.',1);
insert into Configs (CoName, CoValue, CoCheck) values ('monetaryRule','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('totalPriceTotalDigits','5',1);
insert into Configs (CoName, CoValue, CoCheck) values ('unitPriceTotalDigits','5',1);
insert into Configs (CoName, CoValue, CoCheck) values ('roundingFactor','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('roundingMethod','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('primaryMonetarySymbol','$',1);
insert into Configs (CoName, CoValue, CoCheck) values ('primaryMonetaryDecimalDigits','2',1);
insert into Configs (CoName, CoValue, CoCheck) values ('secondaryMonetarySymbol',NULL,1);
insert into Configs (CoName, CoValue, CoCheck) values ('secondaryMonDecimalDigits','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('permanentMarqueeNumber','999901',1);
insert into Configs (CoName, CoValue, CoCheck) values ('productMarqueeDisplayed','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('productDescMarqueeCounter','0',1);


insert into Configs (CoName, CoValue, CoCheck) values ('virtualPrintingOverride2ndPrinter','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('virtualPrintingOverride3rdPrinter','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('displaySelfServeStartScreen','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('bestSellersOnSelfServeMainScreen','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('enableLabelLength_1_000','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableLabelLength_1_500','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableLabelLength_1_750','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableLabelLength_2_000','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableLabelLength_2_375','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableLabelLength_2_500','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableLabelLength_3_000','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableLabelLength_3_500','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableLabelLength_4_000','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableLabelLength_4_500','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableLabelLength_5_000','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableLabelLength_5_500','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableLabelLength_6_000','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableLabelLength_6_500','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableLabelLength_7_000','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableLabelLength_7_500','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableLabelLength_8_000','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableLabelLength_8_500','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableLabelLength_9_000','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableLabelLength_9_500','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableLabelLength_10_00','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableLabelLength_Continuous','1',1);

insert into Configs (CoName, CoValue, CoCheck) values ('continuousStockIgnoreLabelTaken','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('choseToUseContinuousStock','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('weightRequiredForMeal','1',1);

insert into Configs (CoName, CoValue, CoCheck) values ('suggestiveSellStyle','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('suggestiveSellFrequency','1',1);

insert into Configs (CoName, CoValue, CoCheck) values ('alwaysEnableVirtualKeyboard','1',1);

insert into Configs (CoName, CoValue, CoCheck) values ('showSelectAllWhenVoidingTrans','1',1);

insert into Configs (CoName, CoValue, CoCheck) values ('periodicOfflineBeep','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('enableGlobalPercentOff','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('globalPercentageOff','00000',1);

insert into Configs (CoName, CoValue, CoCheck) values ('enableAccumItemsAndFlashkeys','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('requireFixedWeightVerification','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('autoBackupDBFrequency','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('cutTestOptionEnabled',          '1' ,1);
insert into Configs (CoName, CoValue, CoCheck) values ('maxSuspendedCutTestsOverall',  '40' ,1);
insert into Configs (CoName, CoValue, CoCheck) values ('maxSuspendedCutTestsOperator', '20' ,1);
insert into Configs (CoName, CoValue, CoCheck) values ('secondsToAutoSuspendCutTest',   '0' ,1);
insert into Configs (CoName, CoValue, CoCheck) values ('nextCutTestNumber',             '1' ,1);

insert into Configs (CoName, CoValue, CoCheck) values ('bottomScreenContent',           '0' ,1);

insert into Configs (CoName, CoValue, CoCheck) values ('enableSearchMenuButton','1',1);

insert into Configs (CoName, CoValue, CoCheck) values ('enableHelpPrompts','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableUseCharacterSearching','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('useSecondTare','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('trackingSupervisorChanges','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('variableShelfLifeTime','00:00 AM',1);

insert into Configs (CoName, CoValue, CoCheck) values ('selfServeDisplayTareOnRW','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('digimarcEnabled','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('digimarcDotDensity','40',1);
insert into Configs (CoName, CoValue, CoCheck) values ('digimarcDotSize','1',1);
insert into Configs (CoName, CoValue, CoCheck) values ('digimarcWatermarkRes','75',1);
insert into Configs (CoName, CoValue, CoCheck) values ('digimarcExpandedFreshEnabled','1',1);

insert into Configs (CoName, CoValue, CoCheck) values ('reduceGS1Length','0',1);


insert into Configs (CoName, CoValue, CoCheck) values ('FTPServerIPorHostName','',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPUserName','',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPPassword','',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPSubDirectory','',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPstartTime','',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPUseAutomatic','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPmonday','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPtuesday','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPwednesday','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPthursday','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPfriday','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPsaturday','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPsunday','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPdownloadFirmwareOnly','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPdownloadInstallFirmware','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPdailyUpdates','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPhourlyUpdates','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTP15MinuteUpdates','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPUpdatePLUData','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPUpdateLabelData','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPUpdateFlashkeyData','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPUpdateOperatorData','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPUpdatePlaylists','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPUpdateAlerts','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPUpdateScaleConfiguration','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPUpdateSynchronizationConfiguration','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPUpdateStoreNameNumber','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPUpdateVideos','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPUpdateImages','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPUpdateAudio','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPUpdateFirmware','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPSendPLUData','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPSendLabelData','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPSendFlashkeyData','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPSendOperatorData','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPSendPlaylists','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPSendAlerts','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPSendScaleConfiguration','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPSendSynchronizationConfiguration','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPSendStoreNameNumber','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPSendVideos','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPSendImages','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPSendAudio','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPSendFirmware','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPAllowOperatorSendToDept','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('FTPAllowOperatorUpdateNow','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('selfServeWeightRequiredToSelectItem','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('selfServeByCountItemsAllowed','1',1);

insert into Configs (CoName, CoValue, CoCheck) values ('printer1LabelLength','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('printer2LabelLength','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('printer3LabelLength','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('defaultSoftkeyFontPointSize','12',1);

insert into Configs (CoName, CoValue, CoCheck) values ('diagServiceContactInfo','1.888.4HOBART (1.888.446.2278)',1);
insert into Configs (CoName, CoValue, CoCheck) values ('diagGatewayDeviceAddress','',1);
insert into Configs (CoName, CoValue, CoCheck) values ('diagGatewayDeviceStatusMsgLogLevel','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('valueMaxDiagCollectionDisabled','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('selfServePrintToScreen','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('voidTransactionsViewHours','8',1);

insert into Configs (CoName, CoValue, CoCheck) values ('mostRecentActiveDeptNum','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('useCurrentDeptnumWhenNotReceived','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('unitPricePrintMethod','1',1);

insert into Configs (CoName, CoValue, CoCheck) values ('pick5Active','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('priceChangedLabelIndicator','',1);

insert into Configs (CoName, CoValue, CoCheck) values ('enableSearchOnKeypad','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableEnhancedCenterScreen','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('enhancedCenterHomePage',NULL,1);

insert into Configs (CoName, CoValue, CoCheck) values ('handsFreeDeviceMinLoginPowerLevel','-86',1);

insert into Configs (CoName, CoValue, CoCheck) values ('AllowSupervisorDotWear', '0', 1);
insert into Configs (CoName, CoValue, CoCheck) values ('AllowOperatorDotWear', '0', 1);
insert into Configs (CoName, CoValue, CoCheck) values ('enableCombinedVirtualPrint','0',1);

insert into Configs (CoName, CoValue, CoCheck) values ('selfServeIgnorePrinterErrors', '0' ,1);
insert into Configs (CoName, CoValue, CoCheck) values ('selfServeSearchKeypadFlashkeys','0',1);
insert into Configs (CoName, CoValue, CoCheck) values ('selfServeDisplayPluLabel','0',1);


insert into Depts (DeptNum, DeptName1, DeptName2, DeptCheck) values ( 0, 'Default', 'Default', 1);


insert into SStyles (SSNum, SSDTextL, SSDTextC, SSDGrTxtC, SSDFImgL, SSDBgClr, SSDBgImg, SSImgDir, SSCheck) values (999001, 1,        0, 8421504, 0, 11579568, null, 'images/', 1);
insert into SStyles (SSNum, SSDTextL, SSDTextC, SSDGrTxtC, SSDFImgL, SSDBgClr, SSDBgImg, SSImgDir, SSCheck) values (999002, 1, 16777215, 8421504, 0, 11579568, null, 'images/', 1);
insert into SStyles (SSNum, SSDTextL, SSDTextC, SSDGrTxtC, SSDFImgL, SSDBgClr, SSDBgImg, SSImgDir, SSCheck) values (999003, 1, 16777215, 8421504, 0, 11579568, null, 'images/', 1);
insert into SStyles (SSNum, SSDTextL, SSDTextC, SSDGrTxtC, SSDFImgL, SSDBgClr, SSDBgImg, SSImgDir, SSCheck) values (999004, 1,        0, 8421504, 0,  6724095, null, 'images/', 1);
insert into SStyles (SSNum, SSDTextL, SSDTextC, SSDGrTxtC, SSDFImgL, SSDBgClr, SSDBgImg, SSImgDir, SSCheck) values (999006, 1,        0, 8421504, 0, 11579568, null, 'images/', 1);
insert into SStyles (SSNum, SSDTextL, SSDTextC, SSDGrTxtC, SSDFImgL, SSDBgClr, SSDBgImg, SSImgDir, SSCheck) values (999007, 2, 16777215, 8421504, 0, 11579568, null, 'images/', 1);
insert into SStyles (SSNum, SSDTextL, SSDTextC, SSDGrTxtC, SSDFImgL, SSDBgClr, SSDBgImg, SSImgDir, SSCheck) values (999008, 1,        0, 8421504, 0,     null, null, 'images/', 1);
insert into SStyles (SSNum, SSDTextL, SSDTextC, SSDGrTxtC, SSDFImgL, SSDBgClr, SSDBgImg, SSImgDir, SSCheck) values (999012, 1, 16777215, 8421504, 0, 11579568, null, 'images/', 1);
insert into SStyles (SSNum, SSDTextL, SSDTextC, SSDGrTxtC, SSDFImgL, SSDBgClr, SSDBgImg, SSImgDir, SSCheck) values (999013, 1, 16777215, 8421504, 0, 11579568, null, 'images/', 1);
insert into SStyles (SSNum, SSDTextL, SSDTextC, SSDGrTxtC, SSDFImgL, SSDBgClr, SSDBgImg, SSImgDir, SSCheck) values (999014, 1, 16777215, 8421504, 0, 11579568, null, 'images/', 1);
insert into SStyles (SSNum, SSDTextL, SSDTextC, SSDGrTxtC, SSDFImgL, SSDBgClr, SSDBgImg, SSImgDir, SSCheck) values (999015, 1, 16777215, 8421504, 0, 11579568, null, 'images/', 1);
insert into SStyles (SSNum, SSDTextL, SSDTextC, SSDGrTxtC, SSDFImgL, SSDBgClr, SSDBgImg, SSImgDir, SSCheck) values (999016, 1,        0, 8421504, 0,     null, null, 'images/', 1);
insert into SStyles (SSNum, SSDTextL, SSDTextC, SSDGrTxtC, SSDFImgL, SSDBgClr, SSDBgImg, SSImgDir, SSCheck) values (999017, 1,        0, 8421504, 0,     null, null, 'images/', 1);
insert into SStyles (SSNum, SSDTextL, SSDTextC, SSDGrTxtC, SSDFImgL, SSDBgClr, SSDBgImg, SSImgDir, SSCheck) values (999030, 1, 16777215, 8421504, 0, 11579568, null, 'images/', 1);


insert into SFeatrs (DeptNum, SFNum, SSNum, SGNum, SFDHGrid, SFDVGrid, SFDHSpc, SFDVSpc, SFCheck) values ( 0,  999001, 999001, 999001, 6, 4, 1,  1, 1);
insert into SFeatrs (DeptNum, SFNum, SSNum, SGNum, SFDHGrid, SFDVGrid, SFDHSpc, SFDVSpc, SFCheck) values ( 0,  999002, 999002, 999002, 2, 4, 2,  2, 1);
insert into SFeatrs (DeptNum, SFNum, SSNum, SGNum, SFDHGrid, SFDVGrid, SFDHSpc, SFDVSpc, SFCheck) values ( 0,  999003, 999003, 999003, 5, 3, 2,  2, 1);
insert into SFeatrs (DeptNum, SFNum, SSNum, SGNum, SFDHGrid, SFDVGrid, SFDHSpc, SFDVSpc, SFCheck) values ( 0,  999004, 999004, 999004, 6, 4, 1,  1, 1);
insert into SFeatrs (DeptNum, SFNum, SSNum, SGNum, SFDHGrid, SFDVGrid, SFDHSpc, SFDVSpc, SFCheck) values ( 0,  999006, 999006, 999006, 1, 4, 0, 24, 1);
insert into SFeatrs (DeptNum, SFNum, SSNum, SGNum, SFDHGrid, SFDVGrid, SFDHSpc, SFDVSpc, SFCheck) values ( 0,  999007, 999007, 999007, 6, 4, 2,  2, 1);
insert into SFeatrs (DeptNum, SFNum, SSNum, SGNum, SFDHGrid, SFDVGrid, SFDHSpc, SFDVSpc, SFCheck) values ( 0,  999008, 999008, 999008, 4, 2, 2,  2, 1);
insert into SFeatrs (DeptNum, SFNum, SSNum, SGNum, SFDHGrid, SFDVGrid, SFDHSpc, SFDVSpc, SFCheck) values ( 0,  999012, 999012, 999012, 2, 4, 2,  2, 1);
insert into SFeatrs (DeptNum, SFNum, SSNum, SGNum, SFDHGrid, SFDVGrid, SFDHSpc, SFDVSpc, SFCheck) values ( 0,  999013, 999013, 999013, 2, 6, 2,  2, 1);
insert into SFeatrs (DeptNum, SFNum, SSNum, SGNum, SFDHGrid, SFDVGrid, SFDHSpc, SFDVSpc, SFCheck) values ( 0,  999014, 999014, 999014, 2, 4, 2,  2, 1);
insert into SFeatrs (DeptNum, SFNum, SSNum, SGNum, SFDHGrid, SFDVGrid, SFDHSpc, SFDVSpc, SFCheck) values ( 0,  999015, 999015, 999015, 2, 6, 2,  2, 1);
insert into SFeatrs (DeptNum, SFNum, SSNum, SGNum, SFDHGrid, SFDVGrid, SFDHSpc, SFDVSpc, SFCheck) values ( 0,  999016, 999016, 999016, 2, 5, 2,  2, 1);
insert into SFeatrs (DeptNum, SFNum, SSNum, SGNum, SFDHGrid, SFDVGrid, SFDHSpc, SFDVSpc, SFCheck) values ( 0,  999017, 999017, 999017, 6, 4, 2,  2, 1);
insert into SFeatrs (DeptNum, SFNum, SSNum, SGNum, SFDHGrid, SFDVGrid, SFDHSpc, SFDVSpc, SFCheck) values ( 0,  999030, 999030, 999030, 3, 3, 1,  1, 1);


insert into SKGrps (DeptNum, SGNum, SGHGrid, SGVGrid, SGHSpc, SGVSpc, SGCheck) values ( 0,  999001, 6, 4, 1,  1, 1);
insert into SKGrps (DeptNum, SGNum, SGHGrid, SGVGrid, SGHSpc, SGVSpc, SGCheck) values ( 0,  999002, 1, 5, 0,  0, 1);
insert into SKGrps (DeptNum, SGNum, SGHGrid, SGVGrid, SGHSpc, SGVSpc, SGCheck) values ( 0,  999003, 3, 3, 2,  2, 1);
insert into SKGrps (DeptNum, SGNum, SGHGrid, SGVGrid, SGHSpc, SGVSpc, SGCheck) values ( 0,  999004, 6, 4, 1,  1, 1);
insert into SKGrps (DeptNum, SGNum, SGHGrid, SGVGrid, SGHSpc, SGVSpc, SGCheck) values ( 0,  999006, 1, 4, 0, 24, 1);
insert into SKGrps (DeptNum, SGNum, SGHGrid, SGVGrid, SGHSpc, SGVSpc, SGCheck) values ( 0,  999007, 3, 4, 2,  2, 1);
insert into SKGrps (DeptNum, SGNum, SGHGrid, SGVGrid, SGHSpc, SGVSpc, SGCheck) values ( 0,  999008, 5, 1, 2,  2, 1);
insert into SKGrps (DeptNum, SGNum, SGHGrid, SGVGrid, SGHSpc, SGVSpc, SGCheck) values ( 0,  999012, 1, 5, 0,  1, 1);
insert into SKGrps (DeptNum, SGNum, SGHGrid, SGVGrid, SGHSpc, SGVSpc, SGCheck) values ( 0,  999013, 1, 7, 0,  1, 1);
insert into SKGrps (DeptNum, SGNum, SGHGrid, SGVGrid, SGHSpc, SGVSpc, SGCheck) values ( 0,  999014, 1, 5, 0,  1, 1);
insert into SKGrps (DeptNum, SGNum, SGHGrid, SGVGrid, SGHSpc, SGVSpc, SGCheck) values ( 0,  999015, 1, 7, 0,  1, 1);
insert into SKGrps (DeptNum, SGNum, SGHGrid, SGVGrid, SGHSpc, SGVSpc, SGCheck) values ( 0,  999016, 2, 5, 0,  1, 1);
insert into SKGrps (DeptNum, SGNum, SGHGrid, SGVGrid, SGHSpc, SGVSpc, SGCheck) values ( 0,  999030, 2, 4, 1,  1, 1);


insert into SftKeys (DeptNum, SGNum, SKHPos, SKVPos, CaNum, OpNum, SFNum, SKHSize, SKVSize, SKBgClr, SKTxClr, SKName1, SKName2, SKFNBCl, SKFNBIm, SKFNTxt, SKLkPPl, SKActn, SKActSb, SKPrNum, SKActPI, SKActPS, SKBgImage, SKCheck) values (0,  999002, 0, 0, null, null,   null, 1, 1, null,     null,         'SEARCH',              'BUSCAR', 0, 0, 0, null,    2, null,   null,    4,            null, '182x60_blue.png', 1);
insert into SftKeys (DeptNum, SGNum, SKHPos, SKVPos, CaNum, OpNum, SFNum, SKHSize, SKVSize, SKBgClr, SKTxClr, SKName1, SKName2, SKFNBCl, SKFNBIm, SKFNTxt, SKLkPPl, SKActn, SKActSb, SKPrNum, SKActPI, SKActPS, SKBgImage, SKCheck) values (0,  999003, 0, 0, null, null,   null, 1, 1, null,     null,         'Search',              'Buscar', 0, 0, 0, null,    2, null,   null,    4,            null,    'Dk_Green.png', 1);
insert into SftKeys (DeptNum, SGNum, SKHPos, SKVPos, CaNum, OpNum, SFNum, SKHSize, SKVSize, SKBgClr, SKTxClr, SKName1, SKName2, SKFNBCl, SKFNBIm, SKFNTxt, SKLkPPl, SKActn, SKActSb, SKPrNum, SKActPI, SKActPS, SKBgImage, SKCheck) values (0,  999003, 1, 0, null, null,   null, 1, 1, null,     null,  'Misc By Count',     'Misc\nPor Pieza', 0, 0, 0, null,    8, null, 999990, null,            null,     'DK_Blue.png', 1);
insert into SftKeys (DeptNum, SGNum, SKHPos, SKVPos, CaNum, OpNum, SFNum, SKHSize, SKVSize, SKBgClr, SKTxClr, SKName1, SKName2, SKFNBCl, SKFNBIm, SKFNTxt, SKLkPPl, SKActn, SKActSb, SKPrNum, SKActPI, SKActPS, SKBgImage, SKCheck) values (0,  999003, 2, 0, null, null,   null, 1, 1, null,     null, 'Misc By Weight', 'Misc\nPeso Variable', 0, 0, 0, null,    8, null, 999991, null,            null,     'DK_Blue.png', 1);
insert into SftKeys (DeptNum, SGNum, SKHPos, SKVPos, CaNum, OpNum, SFNum, SKHSize, SKVSize, SKBgClr, SKTxClr, SKName1, SKName2, SKFNBCl, SKFNBIm, SKFNTxt, SKLkPPl, SKActn, SKActSb, SKPrNum, SKActPI, SKActPS, SKBgImage, SKCheck) values (0,  999006, 0, 0, null, null,   null, 1, 1, null,     null,         'Search',              'Buscar', 0, 0, 0, null,    2, null,   null,    4,            null, 'medBlankYEL.jpg', 1);
insert into SftKeys (DeptNum, SGNum, SKHPos, SKVPos, CaNum, OpNum, SFNum, SKHSize, SKVSize, SKBgClr, SKTxClr, SKName1, SKName2, SKFNBCl, SKFNBIm, SKFNTxt, SKLkPPl, SKActn, SKActSb, SKPrNum, SKActPI, SKActPS, SKBgImage, SKCheck) values (0,  999006, 0, 1, null, null, 999007, 1, 1, null,     null, 'Store Specials',          'Especiales', 0, 0, 0, null,   16, null,   null, null,            null, 'medBlankYEL.jpg', 1);
insert into SftKeys (DeptNum, SGNum, SKHPos, SKVPos, CaNum, OpNum, SFNum, SKHSize, SKVSize, SKBgClr, SKTxClr, SKName1, SKName2, SKFNBCl, SKFNBIm, SKFNTxt, SKLkPPl, SKActn, SKActSb, SKPrNum, SKActPI, SKActPS, SKBgImage, SKCheck) values (0,  999006, 0, 2, null, null,   null, 1, 1, null,     null,     'Start over',             'Empezar', 0, 0, 0, null,    2, null,   null, null,            null, 'medBlankYEL.jpg', 1);
insert into SftKeys (DeptNum, SGNum, SKHPos, SKVPos, CaNum, OpNum, SFNum, SKHSize, SKVSize, SKBgClr, SKTxClr, SKName1, SKName2, SKFNBCl, SKFNBIm, SKFNTxt, SKLkPPl, SKActn, SKActSb, SKPrNum, SKActPI, SKActPS, SKBgImage, SKCheck) values (0,  999006, 0, 3, null, null,   null, 1, 1, null,     null,               '',                    '', 0, 0, 0, null, null, null,   null, null,            null,              null, 1);
insert into SftKeys (DeptNum, SGNum, SKHPos, SKVPos, CaNum, OpNum, SFNum, SKHSize, SKVSize, SKBgClr, SKTxClr, SKName1, SKName2, SKFNBCl, SKFNBIm, SKFNTxt, SKLkPPl, SKActn, SKActSb, SKPrNum, SKActPI, SKActPS, SKBgImage, SKCheck) values (0,  999012, 0, 0, null, null,   null, 1, 1, null, 16777215,   'Cut test OFF',        'Cut test OFF', 0, 0, 0, null,   25, null,   null, null,            null, '182x60_blue.png', 1);
insert into SftKeys (DeptNum, SGNum, SKHPos, SKVPos, CaNum, OpNum, SFNum, SKHSize, SKVSize, SKBgClr, SKTxClr, SKName1, SKName2, SKFNBCl, SKFNBIm, SKFNTxt, SKLkPPl, SKActn, SKActSb, SKPrNum, SKActPI, SKActPS, SKBgImage, SKCheck) values (0,  999013, 0, 0, null, null,   null, 1, 1, null, 16777215,   'Cut test OFF',        'Cut test OFF', 0, 0, 0, null,   25, null,   null, null,            null, '182x60_blue.png', 1);
insert into SftKeys (DeptNum, SGNum, SKHPos, SKVPos, CaNum, OpNum, SFNum, SKHSize, SKVSize, SKBgClr, SKTxClr, SKName1, SKName2, SKFNBCl, SKFNBIm, SKFNTxt, SKLkPPl, SKActn, SKActSb, SKPrNum, SKActPI, SKActPS, SKBgImage, SKCheck) values (0,  999014, 0, 0, null, null,   null, 1, 1, null, 16777215,    'Cut test ON',         'Cut test ON', 0, 0, 0, null,   26, null,   null, null,            null, '182x60_blue.png', 1);
insert into SftKeys (DeptNum, SGNum, SKHPos, SKVPos, CaNum, OpNum, SFNum, SKHSize, SKVSize, SKBgClr, SKTxClr, SKName1, SKName2, SKFNBCl, SKFNBIm, SKFNTxt, SKLkPPl, SKActn, SKActSb, SKPrNum, SKActPI, SKActPS, SKBgImage, SKCheck) values (0,  999015, 0, 0, null, null,   null, 1, 1, null, 16777215,        'End run',             'End run', 0, 0, 0, null,   27, null,   null, null,            null, '182x60_blue.png', 1);
insert into SftKeys (DeptNum, SGNum, SKHPos, SKVPos, CaNum, OpNum, SFNum, SKHSize, SKVSize, SKBgClr, SKTxClr, SKName1, SKName2, SKFNBCl, SKFNBIm, SKFNTxt, SKLkPPl, SKActn, SKActSb, SKPrNum, SKActPI, SKActPS, SKBgImage, SKCheck) values (0,  999015, 0, 1, null, null,   null, 1, 1, null, 16777215,    'Cut test ON',         'Cut test ON', 0, 0, 0, null,   26, null,   null, null,            null, '182x60_blue.png', 1);
insert into SftKeys (DeptNum, SGNum, SKHPos, SKVPos, CaNum, OpNum, SFNum, SKHSize, SKVSize, SKBgClr, SKTxClr, SKName1, SKName2, SKFNBCl, SKFNBIm, SKFNTxt, SKLkPPl, SKActn, SKActSb, SKPrNum, SKActPI, SKActPS, SKBgImage, SKCheck) values (0,  999016, 0, 0, null, null, 999017, 1, 1, null,     null,              'A',                   'A', 0, 0, 0, null,   39, null,   null, null,             'A',              null, 1);
insert into SftKeys (DeptNum, SGNum, SKHPos, SKVPos, CaNum, OpNum, SFNum, SKHSize, SKVSize, SKBgClr, SKTxClr, SKName1, SKName2, SKFNBCl, SKFNBIm, SKFNTxt, SKLkPPl, SKActn, SKActSb, SKPrNum, SKActPI, SKActPS, SKBgImage, SKCheck) values (0,  999016, 1, 0, null, null, 999017, 1, 1, null,     null,            'B-C',                 'B-C', 0, 0, 0, null,   39, null,   null, null,           'B,C',              null, 1);
insert into SftKeys (DeptNum, SGNum, SKHPos, SKVPos, CaNum, OpNum, SFNum, SKHSize, SKVSize, SKBgClr, SKTxClr, SKName1, SKName2, SKFNBCl, SKFNBIm, SKFNTxt, SKLkPPl, SKActn, SKActSb, SKPrNum, SKActPI, SKActPS, SKBgImage, SKCheck) values (0,  999016, 0, 1, null, null, 999017, 1, 1, null,     null,            'D-J',                 'D-J', 0, 0, 0, null,   39, null,   null, null, 'D,E,F,G,H,I,J',              null, 1);
insert into SftKeys (DeptNum, SGNum, SKHPos, SKVPos, CaNum, OpNum, SFNum, SKHSize, SKVSize, SKBgClr, SKTxClr, SKName1, SKName2, SKFNBCl, SKFNBIm, SKFNTxt, SKLkPPl, SKActn, SKActSb, SKPrNum, SKActPI, SKActPS, SKBgImage, SKCheck) values (0,  999016, 1, 1, null, null, 999017, 1, 1, null,     null,            'K-M',                 'K-M', 0, 0, 0, null,   39, null,   null, null,         'K,L,M',              null, 1);
insert into SftKeys (DeptNum, SGNum, SKHPos, SKVPos, CaNum, OpNum, SFNum, SKHSize, SKVSize, SKBgClr, SKTxClr, SKName1, SKName2, SKFNBCl, SKFNBIm, SKFNTxt, SKLkPPl, SKActn, SKActSb, SKPrNum, SKActPI, SKActPS, SKBgImage, SKCheck) values (0,  999016, 0, 2, null, null, 999017, 1, 1, null,     null,            'N-O',                 'N-O', 0, 0, 0, null,   39, null,   null, null,           'N,O',              null, 1);
insert into SftKeys (DeptNum, SGNum, SKHPos, SKVPos, CaNum, OpNum, SFNum, SKHSize, SKVSize, SKBgClr, SKTxClr, SKName1, SKName2, SKFNBCl, SKFNBIm, SKFNTxt, SKLkPPl, SKActn, SKActSb, SKPrNum, SKActPI, SKActPS, SKBgImage, SKCheck) values (0,  999016, 1, 2, null, null, 999017, 1, 1, null,     null,              'P',                   'P', 0, 0, 0, null,   39, null,   null, null,             'P',              null, 1);
insert into SftKeys (DeptNum, SGNum, SKHPos, SKVPos, CaNum, OpNum, SFNum, SKHSize, SKVSize, SKBgClr, SKTxClr, SKName1, SKName2, SKFNBCl, SKFNBIm, SKFNTxt, SKLkPPl, SKActn, SKActSb, SKPrNum, SKActPI, SKActPS, SKBgImage, SKCheck) values (0,  999016, 0, 3, null, null, 999017, 1, 1, null,     null,            'Q-S',                 'Q-S', 0, 0, 0, null,   39, null,   null, null,         'Q,R,S',              null, 1);
insert into SftKeys (DeptNum, SGNum, SKHPos, SKVPos, CaNum, OpNum, SFNum, SKHSize, SKVSize, SKBgClr, SKTxClr, SKName1, SKName2, SKFNBCl, SKFNBIm, SKFNTxt, SKLkPPl, SKActn, SKActSb, SKPrNum, SKActPI, SKActPS, SKBgImage, SKCheck) values (0,  999016, 1, 3, null, null, 999017, 1, 1, null,     null,            'T-Z',                 'T-Z', 0, 0, 0, null,   39, null,   null, null, 'T,U,V,W,X,Y,Z',              null, 1);
insert into SftKeys (DeptNum, SGNum, SKHPos, SKVPos, CaNum, OpNum, SFNum, SKHSize, SKVSize, SKBgClr, SKTxClr, SKName1, SKName2, SKFNBCl, SKFNBIm, SKFNTxt, SKLkPPl, SKActn, SKActSb, SKPrNum, SKActPI, SKActPS, SKBgImage, SKCheck) values (0,  999016, 0, 4, null, null, 999017, 2, 1, null,     null,   'Best Sellers',        'Best Sellers', 0, 0, 0, null,   42, null,   null, null,            null,              null, 1);


insert into PrDefs (DeptNum, PDType, PrPrice, PrPrMod, PrIsFrcPr, PrExcPr, PrDscMe, PrDscPr1, PrDscPr2, PrDscPr3, PrTare, PrTareB, PrIsFrcTr, PrPrpTare, PrNetWt, PrByCnt, PrIsFrcBC, PrShlfLfD, PrShlfLfH, PrProdLfD, PrProdLfH, PrIsPrnSL, PrIsPrnPL, PrIsPrnPO, PrBnsPts, PrPrtns, PrIsUsSL, PrInMode, PrLblRot1, PrLblPlc1, PrLblRot2, PrLblPlc2, PrLblRot3, PrLblPlc3, PrName1, PrName2, PrVCOLTxt, PrCOLReq, PrPrPlT, PDClNum, PDTxNum, PDSMNum, PDNFNum, PDMaNum, PDNtNum, PDCaNum, PDLbNum1, PDLbNum2, PDLbNum3, PDSPNum, PDLCNum, PDDXNum, PDLPNum, PDLLNum, PDUPCNS, PDEANFS, PDEAN128FS, PDGS1GTINFS, PDGS1AIFS, PDQRCODEFS, PDDMATRXFS, PDEANPr, PDDesc, PDGrNm1, PDGrNm2, PDGrNm3, PDGrNm4, PDImage, PDCOLTN, PDManualLabelTypeNum1, PDManualLabelTypeNum2, PDManualLabelTypeNum3, PDPrepackLabelTypeNum1, PDPrepackLabelTypeNum2, PDPrepackLabelTypeNum3, PDProdEntryLabelTypeNum1, PDProdEntryLabelTypeNum2, PDProdEntryLabelTypeNum3, PDSelfServLabelTypeNum1, PDSelfServLabelTypeNum2, PDSelfServLabelTypeNum3, PDPick5LabelTypeNum1, PDPick5LabelTypeNum2, PDPick5LabelTypeNum3, PDCheck)
            values (0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, null, 0, 0, 0, 2, 0, 2, 0, 2, null, null, null, null, 0, 999999, null, null, null, null, null, null, null, null, null, null, null, null, null, null, 2, 'XXEEEEEPPPPP', 'XXEEEEEPPPPPWWWWW', null, null, null, null, null,  'Product Defaults - Random Weight', null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, 1);
insert into PrDefs (DeptNum, PDType, PrPrice, PrPrMod, PrIsFrcPr, PrExcPr, PrDscMe, PrDscPr1, PrDscPr2, PrDscPr3, PrTare, PrTareB, PrIsFrcTr, PrPrpTare, PrNetWt, PrByCnt, PrIsFrcBC, PrShlfLfD, PrShlfLfH, PrProdLfD, PrProdLfH, PrIsPrnSL, PrIsPrnPL, PrIsPrnPO, PrBnsPts, PrPrtns, PrIsUsSL, PrInMode, PrLblRot1, PrLblPlc1, PrLblRot2, PrLblPlc2, PrLblRot3, PrLblPlc3, PrName1, PrName2, PrVCOLTxt, PrCOLReq, PrPrPlT, PDClNum, PDTxNum, PDSMNum, PDNFNum, PDMaNum, PDNtNum, PDCaNum, PDLbNum1, PDLbNum2, PDLbNum3, PDSPNum, PDLCNum, PDDXNum, PDLPNum, PDLLNum, PDUPCNS, PDEANFS, PDEAN128FS, PDGS1GTINFS, PDGS1AIFS, PDQRCODEFS, PDDMATRXFS, PDEANPr, PDDesc, PDGrNm1, PDGrNm2, PDGrNm3, PDGrNm4, PDImage, PDCOLTN, PDManualLabelTypeNum1, PDManualLabelTypeNum2, PDManualLabelTypeNum3, PDPrepackLabelTypeNum1, PDPrepackLabelTypeNum2, PDPrepackLabelTypeNum3, PDProdEntryLabelTypeNum1, PDProdEntryLabelTypeNum2, PDProdEntryLabelTypeNum3, PDSelfServLabelTypeNum1, PDSelfServLabelTypeNum2, PDSelfServLabelTypeNum3, PDPick5LabelTypeNum1, PDPick5LabelTypeNum2, PDPick5LabelTypeNum3, PDCheck)
            values (0, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, null, 0, 0, 0, 2, 0, 2, 0, 2, null, null, null, null, 0, 999999, null, null, null, null, null, null, null, null, null, null, null, null, null, null, 0, 'XXEEEEEPPPPP', 'XXEEEEEPPPPPWWWWW', null, null, null, null, null,  'Product Defaults - Fixed Weight', null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, 1);
insert into PrDefs (DeptNum, PDType, PrPrice, PrPrMod, PrIsFrcPr, PrExcPr, PrDscMe, PrDscPr1, PrDscPr2, PrDscPr3, PrTare, PrTareB, PrIsFrcTr, PrPrpTare, PrNetWt, PrByCnt, PrIsFrcBC, PrShlfLfD, PrShlfLfH, PrProdLfD, PrProdLfH, PrIsPrnSL, PrIsPrnPL, PrIsPrnPO, PrBnsPts, PrPrtns, PrIsUsSL, PrInMode, PrLblRot1, PrLblPlc1, PrLblRot2, PrLblPlc2, PrLblRot3, PrLblPlc3, PrName1, PrName2, PrVCOLTxt, PrCOLReq, PrPrPlT, PDClNum, PDTxNum, PDSMNum, PDNFNum, PDMaNum, PDNtNum, PDCaNum, PDLbNum1, PDLbNum2, PDLbNum3, PDSPNum, PDLCNum, PDDXNum, PDLPNum, PDLLNum, PDUPCNS, PDEANFS, PDEAN128FS, PDGS1GTINFS, PDGS1AIFS, PDQRCODEFS, PDDMATRXFS, PDEANPr, PDDesc, PDGrNm1, PDGrNm2, PDGrNm3, PDGrNm4, PDImage, PDCOLTN, PDManualLabelTypeNum1, PDManualLabelTypeNum2, PDManualLabelTypeNum3, PDPrepackLabelTypeNum1, PDPrepackLabelTypeNum2, PDPrepackLabelTypeNum3, PDProdEntryLabelTypeNum1, PDProdEntryLabelTypeNum2, PDProdEntryLabelTypeNum3, PDSelfServLabelTypeNum1, PDSelfServLabelTypeNum2, PDSelfServLabelTypeNum3, PDPick5LabelTypeNum1, PDPick5LabelTypeNum2, PDPick5LabelTypeNum3, PDCheck)
            values (0, 3, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, null, 0, 0, 0, 2, 0, 2, 0, 2, null, null, null, null, 0, 999999, null, null, null, null, null, null, null, null, null, null, null, null, null, null, 2, 'XXEEEEEPPPPP', 'XXEEEEEPPPPPWWWWW', null, null, null, null, null, 'Product Defaults - By Count', null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, 1);
insert into PrDefs (DeptNum, PDType, PrPrice, PrPrMod, PrIsFrcPr, PrExcPr, PrDscMe, PrDscPr1, PrDscPr2, PrDscPr3, PrTare, PrTareB, PrIsFrcTr, PrPrpTare, PrNetWt, PrByCnt, PrIsFrcBC, PrShlfLfD, PrShlfLfH, PrProdLfD, PrProdLfH, PrIsPrnSL, PrIsPrnPL, PrIsPrnPO, PrBnsPts, PrPrtns, PrIsUsSL, PrInMode, PrLblRot1, PrLblPlc1, PrLblRot2, PrLblPlc2, PrLblRot3, PrLblPlc3, PrName1, PrName2, PrVCOLTxt, PrCOLReq, PrPrPlT, PDClNum, PDTxNum, PDSMNum, PDNFNum, PDMaNum, PDNtNum, PDCaNum, PDLbNum1, PDLbNum2, PDLbNum3, PDSPNum, PDLCNum, PDDXNum, PDLPNum, PDLLNum, PDUPCNS, PDEANFS, PDEAN128FS, PDGS1GTINFS, PDGS1AIFS, PDQRCODEFS, PDDMATRXFS, PDEANPr, PDDesc, PDGrNm1, PDGrNm2, PDGrNm3, PDGrNm4, PDImage, PDCOLTN, PDManualLabelTypeNum1, PDManualLabelTypeNum2, PDManualLabelTypeNum3, PDPrepackLabelTypeNum1, PDPrepackLabelTypeNum2, PDPrepackLabelTypeNum3, PDProdEntryLabelTypeNum1, PDProdEntryLabelTypeNum2, PDProdEntryLabelTypeNum3, PDSelfServLabelTypeNum1, PDSelfServLabelTypeNum2, PDSelfServLabelTypeNum3, PDPick5LabelTypeNum1, PDPick5LabelTypeNum2, PDPick5LabelTypeNum3, PDCheck)
            values (0, 4, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, null, 0, 0, 0, 2, 0, 2, 0, 2, null, null, null, null, 0, 999999, null, null, null, null, null, null, null, null, null, null, null, null, null, null, 0, 'XXEEEEEPPPPP', 'XXEEEEEPPPPPWWWWW', null, null, null, null, null, 'Product Defaults - Fixed Volume (Fluid oz.)', null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, 1);


insert into Categrs (DeptNum, CaNum, Cat_CaNum, CaName1, CaName2, CaCdNm1, CaCdNm2, CaBgClr, CaTxClr, CaImage, CaPList, CaCheck) values (0, 999999, 999999, 'Master Category', 'Categoria Mayor', null, null, 16777215, 0, null, null, 1);


insert into Spcials (DeptNum, SPNum, SPName1, SPName2, SPDefL, SPImag1, SPImag2, SPCheck) values (0, 999990, 'New Products',  null, 30,  'new',  'new', 1);
insert into Spcials (DeptNum, SPNum, SPName1, SPName2, SPDefL, SPImag1, SPImag2, SPCheck) values (0, 999991, 'Price Reductions', null, 30, 'sale', 'sale', 1);

insert into Orders (DeptNum, OrderNumber, OrderDetails, OrderStatus, CustomerName, OrderSMS, OrderEmail, OrderOrigin, StoreNumber, OrderCreation_dt, OrderPickup_dt, OrderStart_dt, OrderFinish_dt, OrderTotal_dt, BlockOrder, OperatorNumber, ReasonCanceled, DeniedCancelReq, NumOfTimesPrinted) values ( 0,  26,  "single bagged",            3,  "John D.",  "9379649134",  NULL,  0,  1877,  "2021-03-06 13:22:00",  "2021-03-06 16:50:00",  "2021-03-06 16:35:13",  "2021-03-06 16:40:13",   "2021-03-06 0:5:00",     0,  3,       NULL,        0, 1);
insert into Orders (DeptNum, OrderNumber, OrderDetails, OrderStatus, CustomerName, OrderSMS, OrderEmail, OrderOrigin, StoreNumber, OrderCreation_dt, OrderPickup_dt, OrderStart_dt, OrderFinish_dt, OrderTotal_dt, BlockOrder, OperatorNumber, ReasonCanceled, DeniedCancelReq, NumOfTimesPrinted) values ( 0,  27,  "none",                     2,  "Kathy",    "9373322092",  NULL,  0,  1877,  "2021-03-06 10:30:27",  "2021-03-06 11:20:00",  "2021-03-06 11:10:21",  NULL,                    NULL,                    1,  2,       NULL,        0, 1);
insert into Orders (DeptNum, OrderNumber, OrderDetails, OrderStatus, CustomerName, OrderSMS, OrderEmail, OrderOrigin, StoreNumber, OrderCreation_dt, OrderPickup_dt, OrderStart_dt, OrderFinish_dt, OrderTotal_dt, BlockOrder, OperatorNumber, ReasonCanceled, DeniedCancelReq, NumOfTimesPrinted) values ( 0,  24,  "no labels on zip seal",    1,  "Joe",      "9379649741",  NULL,  0,  1877,  "2021-03-06 11:30:48",  "2021-03-06 12:00:00",  NULL,                   NULL,                    NULL,                    0,  NULL,    NULL,        0, 1);
insert into Orders (DeptNum, OrderNumber, OrderDetails, OrderStatus, CustomerName, OrderSMS, OrderEmail, OrderOrigin, StoreNumber, OrderCreation_dt, OrderPickup_dt, OrderStart_dt, OrderFinish_dt, OrderTotal_dt, BlockOrder, OperatorNumber, ReasonCanceled, DeniedCancelReq, NumOfTimesPrinted) values ( 0,  25,  "thin as possible",         1,  "Cherie",   "9379648741",  NULL,  0,  1877,  "2021-03-06 10:05:00",  "2021-03-06 12:30:00",  NULL,                   NULL,                    NULL,                    0,  NULL,    NULL,        1, 2);
insert into Orders (DeptNum, OrderNumber, OrderDetails, OrderStatus, CustomerName, OrderSMS, OrderEmail, OrderOrigin, StoreNumber, OrderCreation_dt, OrderPickup_dt, OrderStart_dt, OrderFinish_dt, OrderTotal_dt, BlockOrder, OperatorNumber, ReasonCanceled, DeniedCancelReq, NumOfTimesPrinted) values ( 0,  17,  "none",                     5,  "Mark",     "9379649136",  NULL,  1,  1877,  "2021-03-06 09:15:43",  "2021-03-06 10:50:00",  NULL,                   NULL,                    NULL,                    0,  2,       "no showed", 1, 1);
insert into Orders (DeptNum, OrderNumber, OrderDetails, OrderStatus, CustomerName, OrderSMS, OrderEmail, OrderOrigin, StoreNumber, OrderCreation_dt, OrderPickup_dt, OrderStart_dt, OrderFinish_dt, OrderTotal_dt, BlockOrder, OperatorNumber, ReasonCanceled, DeniedCancelReq, NumOfTimesPrinted) values ( 0,  19,  "none",                     3,  "Mary",     "9379642526",  NULL,  0,  1877,  "2021-03-06 11:00:08",  "2021-03-06 11:20:00",  "2021-03-06 11:06:59",  "2021-03-06 11:11:37",   "2021-03-06 0:4:44",     0,  3,       NULL,        0, 1);
insert into Orders (DeptNum, OrderNumber, OrderDetails, OrderStatus, CustomerName, OrderSMS, OrderEmail, OrderOrigin, StoreNumber, OrderCreation_dt, OrderPickup_dt, OrderStart_dt, OrderFinish_dt, OrderTotal_dt, BlockOrder, OperatorNumber, ReasonCanceled, DeniedCancelReq, NumOfTimesPrinted) values ( 0,  20,  "do not mix",               3,  "Judy G",   "9379642645",  NULL,  0,  1877,  "2021-03-06 10:25:31",  "2021-03-06 12:20:00",  "2021-03-06 11:14:42",  "2021-03-06 11:19:15",   "2021-03-06 0:4:33",     0,  3,       NULL,        0, 2);

insert into OrderDetails (DeptNum, OrderNumId, OrderNumber, PluNumber, ProductType, PluPrice, PluByCount, PluWeight, PluThickness, PluDescription) values ( 0, 65,  26, 57428, 1, 469,  1, 1300, 2,     'Capistrano York Ham');
insert into OrderDetails (DeptNum, OrderNumId, OrderNumber, PluNumber, ProductType, PluPrice, PluByCount, PluWeight, PluThickness, PluDescription) values ( 0, 66,  26, 41458, 1, 325,  1, 450,  3,     'Bernina Serrano Ham');
insert into OrderDetails (DeptNum, OrderNumId, OrderNumber, PluNumber, ProductType, PluPrice, PluByCount, PluWeight, PluThickness, PluDescription) values ( 0, 67,  26, 98690, 3, 125,  8, null, null,  'Hamburger Buns 8 Count');
insert into OrderDetails (DeptNum, OrderNumId, OrderNumber, PluNumber, ProductType, PluPrice, PluByCount, PluWeight, PluThickness, PluDescription) values ( 0, 68,  27, 57896, 1, 479,  1, 1000, 3,     'Honey Baked Ham');
insert into OrderDetails (DeptNum, OrderNumId, OrderNumber, PluNumber, ProductType, PluPrice, PluByCount, PluWeight, PluThickness, PluDescription) values ( 0, 69,  24, 12847, 1, 459,  1, 1000, 2,     'Black Forest Ham');
insert into OrderDetails (DeptNum, OrderNumId, OrderNumber, PluNumber, ProductType, PluPrice, PluByCount, PluWeight, PluThickness, PluDescription) values ( 0, 70,  24, 56985, 1, 599,  1, 500,  3,     'Roasted Turkey');
insert into OrderDetails (DeptNum, OrderNumId, OrderNumber, PluNumber, ProductType, PluPrice, PluByCount, PluWeight, PluThickness, PluDescription) values ( 0, 71,  25, 43854, 1, 549,  1, 1000, 2,     'Garlic Chicken');
insert into OrderDetails (DeptNum, OrderNumId, OrderNumber, PluNumber, ProductType, PluPrice, PluByCount, PluWeight, PluThickness, PluDescription) values ( 0, 72,  17, 85475, 3, 100,  4, null, null,  'Pork Egg Roll');
insert into OrderDetails (DeptNum, OrderNumId, OrderNumber, PluNumber, ProductType, PluPrice, PluByCount, PluWeight, PluThickness, PluDescription) values ( 0, 73,  19, 35748, 1, 933,  1, 1500, 4,     'Deluxe Bologna');
insert into OrderDetails (DeptNum, OrderNumId, OrderNumber, PluNumber, ProductType, PluPrice, PluByCount, PluWeight, PluThickness, PluDescription) values ( 0, 74,  19, 26587, 1, 399,  1, 1000, 2,     'American Cheese');
insert into OrderDetails (DeptNum, OrderNumId, OrderNumber, PluNumber, ProductType, PluPrice, PluByCount, PluWeight, PluThickness, PluDescription) values ( 0, 75,  19, 59872, 1, 589,  1, 750,  3,     'Spiced Kentucky Ham');
insert into OrderDetails (DeptNum, OrderNumId, OrderNumber, PluNumber, ProductType, PluPrice, PluByCount, PluWeight, PluThickness, PluDescription) values ( 0, 76,  19, 23785, 1, 399,  1, 750,  2,     'Baby Swiss Cheese');
insert into OrderDetails (DeptNum, OrderNumId, OrderNumber, PluNumber, ProductType, PluPrice, PluByCount, PluWeight, PluThickness, PluDescription) values ( 0, 77,  20, 86745, 1, 179,  1, 2000, 4,     'German Liverwurst');
insert into OrderDetails (DeptNum, OrderNumId, OrderNumber, PluNumber, ProductType, PluPrice, PluByCount, PluWeight, PluThickness, PluDescription) values ( 0, 78,  20, 24587, 1, 139,  1, 1000, 2,     'Limburger Cheese');

insert into BlackBoxConfig (ColName, ColValue, ColCheck) values ('applicationVersion','1.0.0',1);
insert into BlackBoxConfig (ColName, ColValue, ColCheck) values ('databaseVersion','1.0',1);