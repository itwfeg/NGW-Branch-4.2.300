-- MySQL dump 10.13  Distrib 5.6.23, for Linux ()
--
-- Host: localhost    Database: online_ordering
-- ------------------------------------------------------
-- Server version	5.6.23

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Current Database: `online_ordering`
--

/*!40000 DROP DATABASE IF EXISTS `online_ordering`*/;

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `online_ordering` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `online_ordering`;

--
-- Table structure for table `blackboxconfig`
--

DROP TABLE IF EXISTS `blackboxconfig`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `blackboxconfig` (
  `ColName` varchar(128) NOT NULL,
  `ColValue` varchar(128) NOT NULL,
  `ColCheck` int(11) DEFAULT NULL,
  PRIMARY KEY (`ColName`,`ColValue`),
  UNIQUE KEY `BlackBoxConfig_PK` (`ColName`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `blackboxconfig`
--

LOCK TABLES `blackboxconfig` WRITE;
/*!40000 ALTER TABLE `blackboxconfig` DISABLE KEYS */;
INSERT INTO `blackboxconfig` VALUES ('applicationVersion','2.0.0',NULL),('databaseVersion','2.0.0',NULL);
/*!40000 ALTER TABLE `blackboxconfig` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `categrs`
--

DROP TABLE IF EXISTS `categrs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `categrs` (
  `DeptNum` int(11) NOT NULL,
  `CaNum` int(11) NOT NULL,
  `Cat_CaNum` int(11) DEFAULT NULL,
  `CaName1` varchar(128) DEFAULT NULL,
  `CaName2` varchar(128) DEFAULT NULL,
  `CaCdNm1` varchar(128) DEFAULT NULL,
  `CaCdNm2` varchar(128) DEFAULT NULL,
  `CaBgClr` int(11) DEFAULT NULL,
  `CaTxClr` int(11) DEFAULT NULL,
  `CaImage` varchar(256) DEFAULT NULL,
  `CaPList` varchar(256) DEFAULT NULL,
  `CaCheck` int(11) DEFAULT NULL,
  PRIMARY KEY (`DeptNum`,`CaNum`),
  UNIQUE KEY `Categrs_PK` (`DeptNum`,`CaNum`),
  KEY `SubCats_FK` (`DeptNum`,`Cat_CaNum`),
  KEY `CaName1_IN` (`DeptNum`,`CaCdNm1`),
  KEY `CaName2_IN` (`DeptNum`,`CaCdNm2`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `categrs`
--

LOCK TABLES `categrs` WRITE;
/*!40000 ALTER TABLE `categrs` DISABLE KEYS */;
/*!40000 ALTER TABLE `categrs` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `depts`
--

DROP TABLE IF EXISTS `depts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `depts` (
  `DeptNum` int(11) NOT NULL,
  `DeptName1` varchar(128) DEFAULT NULL,
  `DeptName2` varchar(128) DEFAULT NULL,
  `DeptCheck` int(11) DEFAULT NULL,
  PRIMARY KEY (`DeptNum`),
  UNIQUE KEY `Depts_PK` (`DeptNum`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `depts`
--

LOCK TABLES `depts` WRITE;
/*!40000 ALTER TABLE `depts` DISABLE KEYS */;
INSERT INTO `depts` VALUES (0,'Default','Default',0);
/*!40000 ALTER TABLE `depts` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `deptssettings`
--

DROP TABLE IF EXISTS `deptssettings`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `deptssettings` (
  `DeptNum` int(11) NOT NULL,
  `MondayOpen` time DEFAULT NULL,
  `MondayClose` time DEFAULT NULL,
  `TuesdayOpen` time DEFAULT NULL,
  `TuesdayClose` time DEFAULT NULL,
  `WednesdayOpen` time DEFAULT NULL,
  `WednesdayClose` time DEFAULT NULL,
  `ThursdayOpen` time DEFAULT NULL,
  `ThursdayClose` time DEFAULT NULL,
  `FridayOpen` time DEFAULT NULL,
  `FridayClose` time DEFAULT NULL,
  `SaturdayOpen` time DEFAULT NULL,
  `SaturdayClose` time DEFAULT NULL,
  `SundayOpen` time DEFAULT NULL,
  `SundayClose` time DEFAULT NULL,
  `KioskDeptBGImage` varchar(256) DEFAULT NULL,
  `PrepOptions` varchar(255) DEFAULT '',
  PRIMARY KEY (`DeptNum`),
  UNIQUE KEY `deptssettings_PK` (`DeptNum`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `deptssettings`
--

LOCK TABLES `deptssettings` WRITE;
/*!40000 ALTER TABLE `deptssettings` DISABLE KEYS */;
INSERT INTO `deptssettings` VALUES (0,'09:00:00','17:00:00','09:00:00','17:00:00','09:00:00','17:00:00','09:00:00','17:00:00','09:00:00','17:00:00','09:00:00','17:00:00','09:00:00','17:00:00','','Normal');
/*!40000 ALTER TABLE `deptssettings` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `opers`
--

DROP TABLE IF EXISTS `opers`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `opers` (
  `OpNum` varchar(128) NOT NULL,
  `OpRFID` varchar(128) DEFAULT NULL,
  `OpAccLev` int(11) DEFAULT NULL,
  `OpPLang` int(11) DEFAULT NULL,
  `OpSawNote` tinyint(4) DEFAULT NULL,
  `OpName` varchar(128) DEFAULT NULL,
  `OpCheck` int(11) DEFAULT NULL,
  PRIMARY KEY (`OpNum`),
  UNIQUE KEY `Opers_PK` (`OpNum`),
  KEY `OpRFID_IN` (`OpRFID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `opers`
--

LOCK TABLES `opers` WRITE;
/*!40000 ALTER TABLE `opers` DISABLE KEYS */;
INSERT INTO `opers` VALUES ('1010',NULL,1,1,0,'Scale User',1);
/*!40000 ALTER TABLE `opers` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `orderassignments`
--

DROP TABLE IF EXISTS `orderassignments`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `orderassignments` (
  `DeptNum` int(11) NOT NULL,
  `OrderNumber` int(11) NOT NULL,
  `ZoneNum` int(11) NOT NULL DEFAULT '0',
  `PartStatus` int(11) DEFAULT NULL,
  `ItemCount` int(11) NOT NULL DEFAULT '0',
  `PartFinish` timestamp(4) NULL DEFAULT NULL,
  `Operator` int(11) DEFAULT NULL,
  `PartStart` timestamp(4) NULL DEFAULT NULL,
  PRIMARY KEY (`OrderNumber`,`DeptNum`,`ZoneNum`),
  CONSTRAINT `OrderNumber_FK` FOREIGN KEY (`OrderNumber`) REFERENCES `orders` (`OrderNumber`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `orderassignments`
--

LOCK TABLES `orderassignments` WRITE;
/*!40000 ALTER TABLE `orderassignments` DISABLE KEYS */;
/*!40000 ALTER TABLE `orderassignments` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `orderdetails`
--

DROP TABLE IF EXISTS `orderdetails`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `orderdetails` (
  `DeptNum` int(11) NOT NULL,
  `ZoneNum` int(11) DEFAULT NULL,
  `OrderNumId` int(11) NOT NULL,
  `OrderNumber` int(11) NOT NULL,
  `PrNum` int(11) DEFAULT NULL,
  `ProductType` int(11) DEFAULT NULL,
  `PluPrice` int(11) DEFAULT NULL,
  `PluByCount` int(11) DEFAULT NULL,
  `PluWeight` int(11) DEFAULT NULL,
  `PluDescription` varchar(128) DEFAULT NULL,
  `PluInstructions` varchar(50) DEFAULT NULL,
  `ReasonCancelled` text,
  `Cancelled` tinyint(1) DEFAULT '0',
  `Filled` tinyint(1) DEFAULT '0',
  `Substituted` tinyint(1) DEFAULT '0',
  `OriginalPrNum` int(11) DEFAULT NULL,
  `FinalPrice` int(11) DEFAULT NULL,
  `FinalWeight` int(11) DEFAULT NULL,
  `FinalCount` int(11) DEFAULT NULL,
  `AllowSubstitutions` tinyint(1) DEFAULT '0',
  PRIMARY KEY (`OrderNumId`,`OrderNumber`),
  UNIQUE KEY `OrderDetails_PK` (`OrderNumId`,`OrderNumber`),
  KEY `OrderNumber_idx` (`OrderNumber`),
  KEY `OrderNum_FK` (`OrderNumber`),
  KEY `OrderNumber_idx1` (`DeptNum`,`ZoneNum`,`OrderNumber`),
  CONSTRAINT `OrderNumber` FOREIGN KEY (`OrderNumber`) REFERENCES `orders` (`OrderNumber`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `orderdetails`
--

LOCK TABLES `orderdetails` WRITE;
/*!40000 ALTER TABLE `orderdetails` DISABLE KEYS */;
/*!40000 ALTER TABLE `orderdetails` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Temporary view structure for view `orderparts`
--

DROP TABLE IF EXISTS `orderparts`;
/*!50001 DROP VIEW IF EXISTS `orderparts`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE VIEW `orderparts` AS SELECT 
 1 AS `OrderNumber`,
 1 AS `OrderInstructions`,
 1 AS `OrderStatus`,
 1 AS `CustomerName`,
 1 AS `OrderSMS`,
 1 AS `OrderOrigin`,
 1 AS `StoreNumber`,
 1 AS `OrderCreation`,
 1 AS `OrderPickup`,
 1 AS `AllowSubstitutions`,
 1 AS `DeptNum`,
 1 AS `ZoneNum`,
 1 AS `ItemCount`,
 1 AS `Operator`,
 1 AS `PartStatus`,
 1 AS `PartStart`,
 1 AS `PartFinish`*/;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `orders`
--

DROP TABLE IF EXISTS `orders`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `orders` (
  `OrderNumber` int(11) NOT NULL AUTO_INCREMENT,
  `OrderInstructions` varchar(128) DEFAULT NULL,
  `OrderStatus` int(11) DEFAULT NULL,
  `CustomerName` varchar(128) DEFAULT NULL,
  `OrderSMS` varchar(10) DEFAULT NULL,
  `OrderOrigin` int(11) DEFAULT NULL,
  `StoreNumber` int(11) DEFAULT NULL,
  `OrderCreation` timestamp(4) NULL DEFAULT NULL,
  `OrderPickup` timestamp(4) NULL DEFAULT NULL,
  `AllowSubstitutions` tinyint(1) DEFAULT '0',
  PRIMARY KEY (`OrderNumber`)
) ENGINE=InnoDB AUTO_INCREMENT=1000 DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `orders`
--

LOCK TABLES `orders` WRITE;
/*!40000 ALTER TABLE `orders` DISABLE KEYS */;
/*!40000 ALTER TABLE `orders` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `prods`
--

DROP TABLE IF EXISTS `prods`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `prods` (
  `PrNum` int(11) NOT NULL,
  `DeptNum` int(11) NOT NULL,
  `ClNum` int(11) DEFAULT NULL,
  `TxNum` int(11) DEFAULT NULL,
  `PrIsPrnSL` tinyint(4) DEFAULT NULL,
  `DLXNum` int(11) DEFAULT NULL,
  `ProdTxt5Num` int(11) DEFAULT NULL,
  `ProdTxt6Num` int(11) DEFAULT NULL,
  `ProdTxt7Num` int(11) DEFAULT NULL,
  `ProdTxt8Num` int(11) DEFAULT NULL,
  `ProdTxt9Num` int(11) DEFAULT NULL,
  `SMNum` int(11) DEFAULT NULL,
  `NFNum` int(11) DEFAULT NULL,
  `NtNum` int(11) DEFAULT NULL,
  `LbNum1` int(11) DEFAULT NULL,
  `LbNum2` int(11) DEFAULT NULL,
  `LbNum3` int(11) DEFAULT NULL,
  `CaNum` int(11) DEFAULT NULL,
  `SpNum` int(11) DEFAULT NULL,
  `LCNum` int(11) DEFAULT NULL,
  `LPNum` int(11) DEFAULT NULL,
  `LXNum` int(11) DEFAULT NULL,
  `LLID` int(11) DEFAULT NULL,
  `MaNum` int(11) DEFAULT NULL,
  `PrPrice` int(11) DEFAULT NULL,
  `PrPrMod` int(11) DEFAULT NULL,
  `PrIsFrcPr` tinyint(4) DEFAULT NULL,
  `PrExcPr` int(11) DEFAULT NULL,
  `PrDscMe` int(11) DEFAULT NULL,
  `PrDscPr1` int(11) DEFAULT NULL,
  `PrDscPr2` int(11) DEFAULT NULL,
  `PrDscPr3` int(11) DEFAULT NULL,
  `PrTare` int(11) DEFAULT NULL,
  `PrTareB` int(11) DEFAULT NULL,
  `PrIsFrcTr` tinyint(4) DEFAULT NULL,
  `PrPrpTare` int(11) DEFAULT NULL,
  `PrNetWt` int(11) DEFAULT NULL,
  `PrByCnt` int(11) DEFAULT NULL,
  `PrIsFrcBC` tinyint(4) DEFAULT NULL,
  `PrShlfLfD` int(11) DEFAULT NULL,
  `PrShlfLfH` int(11) DEFAULT NULL,
  `PrProdLfD` int(11) DEFAULT NULL,
  `PrProdLfH` int(11) DEFAULT NULL,
  `PrIsPrnPL` tinyint(4) DEFAULT NULL,
  `PrIsPrnPO` tinyint(4) DEFAULT NULL,
  `PrBnsPts` int(11) DEFAULT NULL,
  `PrPrtns` int(11) DEFAULT NULL,
  `PrIsUsSL` tinyint(4) DEFAULT NULL,
  `PrInMode` int(11) DEFAULT NULL,
  `PrLblRot1` int(11) DEFAULT NULL,
  `PrLblPlc1` int(11) DEFAULT NULL,
  `PrLblRot2` int(11) DEFAULT NULL,
  `PrLblPlc2` int(11) DEFAULT NULL,
  `PrLblRot3` int(11) DEFAULT NULL,
  `PrLblPlc3` int(11) DEFAULT NULL,
  `PrName1` varchar(256) DEFAULT NULL,
  `PrName2` varchar(256) DEFAULT NULL,
  `PrVCOLTxt` tinyint(4) DEFAULT NULL,
  `PrCOLReq` tinyint(4) DEFAULT NULL,
  `PrPrPlT` int(11) DEFAULT NULL,
  `PrType` int(11) DEFAULT NULL,
  `PrBarPref` int(11) DEFAULT NULL,
  `PrBarProN` int(11) DEFAULT NULL,
  `PrBarVenN` int(11) DEFAULT NULL,
  `PrEANCd` varchar(100) DEFAULT NULL,
  `PrRawBarC` varchar(256) DEFAULT NULL,
  `PrCdNm1` text,
  `PrCdNm2` text,
  `PrSpcSD` datetime(4) DEFAULT NULL,
  `PrDesc` text,
  `PrGrNm1` varchar(256) DEFAULT NULL,
  `PrGrNm2` varchar(256) DEFAULT NULL,
  `PrGrNm3` varchar(256) DEFAULT NULL,
  `PrGrNm4` varchar(256) DEFAULT NULL,
  `PrImage` varchar(256) DEFAULT NULL,
  `PrCOLTN` text,
  `PrMealSideItemsGroupNum` int(11) DEFAULT NULL,
  `PrMealSideItemsAreFree` tinyint(4) DEFAULT NULL,
  `PrMealNumSidesIncluded` int(11) DEFAULT NULL,
  `PrCutTestMeatSpecSheetGroupNum` int(11) DEFAULT NULL,
  `PrWeightMin` int(11) DEFAULT NULL,
  `PrWeightMax` int(11) DEFAULT NULL,
  `PrTotalPriceMin` int(11) DEFAULT NULL,
  `PrTotalPriceMax` int(11) DEFAULT NULL,
  `PrManualLabelTypeNum1` int(11) DEFAULT NULL,
  `PrManualLabelTypeNum2` int(11) DEFAULT NULL,
  `PrManualLabelTypeNum3` int(11) DEFAULT NULL,
  `PrPrepackLabelTypeNum1` int(11) DEFAULT NULL,
  `PrPrepackLabelTypeNum2` int(11) DEFAULT NULL,
  `PrPrepackLabelTypeNum3` int(11) DEFAULT NULL,
  `PrProdEntryLabelTypeNum1` int(11) DEFAULT NULL,
  `PrProdEntryLabelTypeNum2` int(11) DEFAULT NULL,
  `PrProdEntryLabelTypeNum3` int(11) DEFAULT NULL,
  `PrSelfServLabelTypeNum1` int(11) DEFAULT NULL,
  `PrSelfServLabelTypeNum2` int(11) DEFAULT NULL,
  `PrSelfServLabelTypeNum3` int(11) DEFAULT NULL,
  `PrPick5LabelTypeNum1` int(11) DEFAULT NULL,
  `PrPick5LabelTypeNum2` int(11) DEFAULT NULL,
  `PrPick5LabelTypeNum3` int(11) DEFAULT NULL,
  `PrCheck` int(11) DEFAULT NULL,
  `ZoneNum` int(11) DEFAULT '0',
  PRIMARY KEY (`PrNum`,`DeptNum`),
  UNIQUE KEY `PrNum_PK` (`PrNum`,`DeptNum`),
  KEY `CaProds_FK` (`DeptNum`,`CaNum`),
  KEY `SPProds_FK` (`SpNum`),
  KEY `SPNum_idx` (`DeptNum`,`SpNum`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 KEY_BLOCK_SIZE=2;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `prods`
--

LOCK TABLES `prods` WRITE;
/*!40000 ALTER TABLE `prods` DISABLE KEYS */;
/*!40000 ALTER TABLE `prods` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `settings`
--

DROP TABLE IF EXISTS `settings`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `settings` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `date` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `MondayOpen` time DEFAULT NULL,
  `MondayClose` time DEFAULT NULL,
  `TuesdayOpen` time DEFAULT NULL,
  `TuesdayClose` time DEFAULT NULL,
  `WednesdayOpen` time DEFAULT NULL,
  `WednesdayClose` time DEFAULT NULL,
  `ThursdayOpen` time DEFAULT NULL,
  `ThursdayClose` time DEFAULT NULL,
  `FridayOpen` time DEFAULT NULL,
  `FridayClose` time DEFAULT NULL,
  `SaturdayOpen` time DEFAULT NULL,
  `SaturdayClose` time DEFAULT NULL,
  `SundayOpen` time DEFAULT NULL,
  `SundayClose` time DEFAULT NULL,
  `AccessLevelDelete` int(11) DEFAULT '0',
  `StoreNumber` int(11) NOT NULL,
  `BlockOrders` tinyint(1) DEFAULT '1',
  `OrderListMinutes` int(11) DEFAULT '60',
  `HourTimeSlots` int(11) DEFAULT '6',
  `MinimumOrderWait` int(11) DEFAULT '1',
  `StorePhone` varchar(256) DEFAULT NULL,
  `EnableCustomerSMS` tinyint(1) DEFAULT '1',
  `OrderHistoryDays` int(11) NOT NULL DEFAULT '15',
  `OrderItemWeightMin` decimal(6,3) DEFAULT NULL,
  `OrderItemWeightMax` decimal(6,3) DEFAULT NULL,
  `OrderPriceMax` decimal(6,3) DEFAULT NULL,
  `OrderItemQtyMax` int(11) DEFAULT NULL,
  `MeasurementSystem` varchar(16) DEFAULT 'imperial',
  `NewNowServing` tinyint(1) DEFAULT '1',
  `NowServingWaitTime` int(11) DEFAULT '10',
  `WarningTime` int(11) DEFAULT '20',
  `PermissionDelete` int(11) DEFAULT '5',
  `OrderListMax` int(11) DEFAULT '100',
  `OperatorLogging` tinyint(1) DEFAULT '0',
  `AutorefreshTime` int(11) DEFAULT NULL,
  `OverdueSMSAlert` tinyint(1) DEFAULT NULL,
  `AlertPhone` varchar(256) DEFAULT NULL,
  `KioskBGImage` varchar(256) DEFAULT NULL,
  `KioskBGColor` varchar(256) DEFAULT NULL,
  `KioskLightColor` varchar(256) DEFAULT NULL,
  `KioskDarkColor` varchar(256) DEFAULT NULL,
  `KioskTextColor` varchar(256) DEFAULT NULL,
  `KioskLogo` varchar(256) DEFAULT NULL,
  `SalesOrFrequentShopper` int(11) DEFAULT '0',
  `FrequentShopperType` int(11) DEFAULT '1',
  `FrequentShopperDiscountType` int(11) DEFAULT '2',
  `FrequentShopperDiscountFormula` int(11) DEFAULT '0',
  `KioskTimeout` int(11) DEFAULT NULL,
  `TVMarqueeText` varchar(256) DEFAULT NULL,
  `TVLayout` int(11) DEFAULT '1',
  `TVRegionA` varchar(256) DEFAULT 'W',
  `TVRegionAURL` varchar(256) DEFAULT NULL,
  `TVRegionAPlaylist` varchar(256) DEFAULT NULL,
  `TVRegionB` varchar(256) DEFAULT 'W',
  `TVRegionBURL` varchar(256) DEFAULT NULL,
  `TVRegionBPlaylist` varchar(256) DEFAULT NULL,
  `TVRegionC` varchar(256) DEFAULT 'W',
  `TVRegionCURL` varchar(256) DEFAULT NULL,
  `TVRegionCPlaylist` varchar(256) DEFAULT NULL,
  `OrdersPerSlot` int(11) DEFAULT '4',
  PRIMARY KEY (`id`),
  UNIQUE KEY `Settings_PK` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `settings`
--

LOCK TABLES `settings` WRITE;
/*!40000 ALTER TABLE `settings` DISABLE KEYS */;
INSERT INTO `settings` VALUES (1,'2021-04-29 19:43:34','09:00:00','17:00:00','09:00:00','17:00:00','09:00:00','17:00:00','09:00:00','17:00:00','09:00:00','17:00:00','09:00:00','17:00:00','09:00:00','17:00:00',0,1,1,60,4,10,'1-800-555-5555',1,60,0.250,20.000,200.000,20,'imperial',1,10,20,5,100,1,60,0,NULL,NULL,'linen','#4473c4','#1b4a9c','#fff',NULL,0,1,2,0,120,'',8,'W',NULL,NULL,'W',NULL,NULL,'W',NULL,NULL,4);
/*!40000 ALTER TABLE `settings` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `zones`
--

DROP TABLE IF EXISTS `zones`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `zones` (
  `ZoneNum` int(11) NOT NULL,
  `DeptNum` int(11) NOT NULL,
  `ZoneName` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`ZoneNum`,`DeptNum`),
  UNIQUE KEY `Zone` (`ZoneNum`,`DeptNum`),
  KEY `DeptNum_FK` (`DeptNum`),
  CONSTRAINT `DeptNum_FK` FOREIGN KEY (`DeptNum`) REFERENCES `depts` (`DeptNum`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `zones`
--

LOCK TABLES `zones` WRITE;
/*!40000 ALTER TABLE `zones` DISABLE KEYS */;
INSERT INTO `zones` VALUES (0,0,'Default');
/*!40000 ALTER TABLE `zones` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Current Database: `online_ordering`
--

USE `online_ordering`;

--
-- Final view structure for view `orderparts`
--

/*!50001 DROP VIEW IF EXISTS `orderparts`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = latin1 */;
/*!50001 SET character_set_results     = latin1 */;
/*!50001 SET collation_connection      = latin1_swedish_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`hobart`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `orderparts` AS select `orders`.`OrderNumber` AS `OrderNumber`,`orders`.`OrderInstructions` AS `OrderInstructions`,`orders`.`OrderStatus` AS `OrderStatus`,`orders`.`CustomerName` AS `CustomerName`,`orders`.`OrderSMS` AS `OrderSMS`,`orders`.`OrderOrigin` AS `OrderOrigin`,`orders`.`StoreNumber` AS `StoreNumber`,`orders`.`OrderCreation` AS `OrderCreation`,`orders`.`OrderPickup` AS `OrderPickup`,`orders`.`AllowSubstitutions` AS `AllowSubstitutions`,`orderassignments`.`DeptNum` AS `DeptNum`,`orderassignments`.`ZoneNum` AS `ZoneNum`,`orderassignments`.`ItemCount` AS `ItemCount`,`orderassignments`.`Operator` AS `Operator`,`orderassignments`.`PartStatus` AS `PartStatus`,`orderassignments`.`PartStart` AS `PartStart`,`orderassignments`.`PartFinish` AS `PartFinish` from (`orderassignments` join `orders` on((`orderassignments`.`OrderNumber` = `orders`.`OrderNumber`))) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2022-04-08 15:52:17
