-- MySQL dump 10.13  Distrib 8.0.23, for Win64 (x86_64)
--
-- Host: localhost    Database: bbdb
-- ------------------------------------------------------
-- Server version	8.0.23

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `blackboxconfig`
--

DROP TABLE IF EXISTS `blackboxconfig`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `blackboxconfig` (
  `ColName` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `ColValue` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `ColCheck` int DEFAULT NULL,
  PRIMARY KEY (`ColName`,`ColValue`),
  UNIQUE KEY `BlackBoxConfig_PK` (`ColName`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `blackboxconfig`
--

LOCK TABLES `blackboxconfig` WRITE;
/*!40000 ALTER TABLE `blackboxconfig` DISABLE KEYS */;
INSERT INTO `blackboxconfig` VALUES ('applicationVersion','1.0.0',NULL),('databaseVersion','1.0.0',NULL);
/*!40000 ALTER TABLE `blackboxconfig` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `categrs`
--

DROP TABLE IF EXISTS `categrs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `categrs` (
  `DeptNum` int NOT NULL,
  `CaNum` int NOT NULL,
  `Cat_CaNum` int DEFAULT NULL,
  `CaName1` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `CaName2` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `CaCdNm1` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `CaCdNm2` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `CaBgClr` int DEFAULT NULL,
  `CaTxClr` int DEFAULT NULL,
  `CaImage` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `CaPList` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `CaCheck` int DEFAULT NULL,
  PRIMARY KEY (`DeptNum`,`CaNum`),
  UNIQUE KEY `Categrs_PK` (`DeptNum`,`CaNum`) /*!80000 INVISIBLE */,
  KEY `SubCats_FK` (`DeptNum`,`Cat_CaNum`) /*!80000 INVISIBLE */,
  KEY `CaName1_IN` (`DeptNum`,`CaCdNm1`) /*!80000 INVISIBLE */,
  KEY `CaName2_IN` (`DeptNum`,`CaCdNm2`),
  CONSTRAINT `DeptNum3_fk` FOREIGN KEY (`DeptNum`) REFERENCES `depts` (`DeptNum`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `categrs`
--

LOCK TABLES `categrs` WRITE;
/*!40000 ALTER TABLE `categrs` DISABLE KEYS */;
/*!40000 ALTER TABLE `categrs` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `configs`
--

DROP TABLE IF EXISTS `configs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `configs` (
  `CoName` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `CoValue` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `CoCheck` int DEFAULT NULL,
  PRIMARY KEY (`CoName`,`CoValue`),
  UNIQUE KEY `Configs_PK` (`CoName`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `configs`
--

LOCK TABLES `configs` WRITE;
/*!40000 ALTER TABLE `configs` DISABLE KEYS */;
/*!40000 ALTER TABLE `configs` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `depts`
--

DROP TABLE IF EXISTS `depts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `depts` (
  `DeptNum` int NOT NULL,
  `DeptName1` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `DeptName2` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `DeptCheck` int DEFAULT NULL,
  PRIMARY KEY (`DeptNum`),
  UNIQUE KEY `Depts_PK` (`DeptNum`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `depts`
--

LOCK TABLES `depts` WRITE;
/*!40000 ALTER TABLE `depts` DISABLE KEYS */;
INSERT INTO `depts` VALUES (0,'Default','Default',0),(1,'Deli','Deli',0),(2,'Bakery','Bakery',0);
/*!40000 ALTER TABLE `depts` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `opers`
--

DROP TABLE IF EXISTS `opers`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `opers` (
  `OpNum` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `OpRFID` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `OpAccLev` int DEFAULT NULL,
  `OpPLang` int DEFAULT NULL,
  `OpSawNote` tinyint DEFAULT NULL,
  `OpName` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `OpCheck` int DEFAULT NULL,
  PRIMARY KEY (`OpNum`),
  UNIQUE KEY `Opers_PK` (`OpNum`),
  KEY `OpRFID_IN` (`OpRFID`) /*!80000 INVISIBLE */
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `opers`
--

LOCK TABLES `opers` WRITE;
/*!40000 ALTER TABLE `opers` DISABLE KEYS */;
/*!40000 ALTER TABLE `opers` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `orderdetails`
--

DROP TABLE IF EXISTS `orderdetails`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `orderdetails` (
  `DeptNum` int NOT NULL,
  `OrderNumId` int NOT NULL,
  `OrderNumber` int NOT NULL,
  `PluNumber` int DEFAULT NULL,
  `ProductType` int DEFAULT NULL,
  `PluPrice` int DEFAULT NULL,
  `PluByCount` int DEFAULT NULL,
  `PluWeight` int DEFAULT NULL,
  `PluThickness` int DEFAULT NULL,
  `PluDescription` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `PluInstructions` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `PluTimesPrinted` int DEFAULT NULL,
  `PluReasonCantFill` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `PluInstructions` varchar(50) DEFAULT NULL,
  `PluTimesPrinted` int DEFAULT NULL,
  PRIMARY KEY (`DeptNum`,`OrderNumId`),
  UNIQUE KEY `OrderDetails_PK` (`DeptNum`,`OrderNumId`) /*!80000 INVISIBLE */,
  KEY `OrderNumber_idx` (`OrderNumber`),
  KEY `OrderNum_FK` (`OrderNumber`),
  KEY `OrderNumber_idx1` (`DeptNum`,`OrderNumber`),
  CONSTRAINT `DeptNum1_fk` FOREIGN KEY (`DeptNum`) REFERENCES `depts` (`DeptNum`),
  CONSTRAINT `OrderNumber` FOREIGN KEY (`DeptNum`, `OrderNumber`) REFERENCES `orders` (`DeptNum`, `OrderNumber`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `orderdetails`
--

LOCK TABLES `orderdetails` WRITE;
/*!40000 ALTER TABLE `orderdetails` DISABLE KEYS */;
INSERT INTO `orderdetails` VALUES (0,65,26,57428,1,469,1,1000,2,'Capistrano York Ham','Thickness = 3 and Bag all items in single bag',1,NULL),(0,66,26,41458,1,325,1,1500,3,'Bernina Serrano Ham','Thickness = 4 Bag all items separately',1,NULL),(0,67,26,98690,3,125,8,0,0,'Hamburger Buns 8 Count','None',1,NULL),(0,68,27,57896,1,479,1,1000,3,'Honey Baked Ham','Thickness = 2 Super Thin ',NULL,NULL),(0,69,24,12847,1,459,1,1000,2,'Black Forest Ham','Thickness = 3 chipped ',NULL,NULL),(0,70,24,56985,1,599,1,2000,3,'Roasted Turkey','Thickness = 3',NULL,NULL),(0,71,25,43854,1,549,1,750,2,'Garlic Chicken','Do not cover zipper lock with label',NULL,NULL),(0,72,17,85475,3,100,4,0,0,'Pork Egg Roll','I have a coupon',NULL,NULL),(0,73,19,35748,1,933,1,1000,4,'Deluxe Bologna','Thickness = 4. Is this Turkish bologna? ',1,NULL),(0,74,19,26587,1,399,1,1500,2,'American Cheese','Thickness = 2',1,NULL),(0,75,19,59872,1,589,1,1000,3,'Spiced Kentucky Ham','Thickness = 3. Hotter than Spiced Tenn Ham?',1,NULL),(0,76,19,23785,1,399,1,3000,2,'Baby Swiss Cheese','Thickness = 2',2,NULL),(0,77,20,86745,1,179,1,2000,4,'German Liverwurst','Thickness = 4',2,NULL),(0,78,20,24587,1,139,1,1000,2,'Limburger Cheese','Thickness = 3. Extra stinky',1,NULL);
/*!40000 ALTER TABLE `orderdetails` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `orders`
--

DROP TABLE IF EXISTS `orders`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `orders` (
  `DeptNum` int NOT NULL,
  `OrderNumber` int NOT NULL,
  `OrderDetails` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `OrderStatus` int DEFAULT NULL,
  `CustomerName` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `OrderSMS` varchar(10) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `OrderEmail` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `OrderOrigin` int DEFAULT NULL,
  `StoreNumber` int DEFAULT NULL,
  `OrderCreation` timestamp(4) NULL DEFAULT NULL,
  `OrderPickup` timestamp(4) NULL DEFAULT NULL,
  `OrderStart` timestamp(4) NULL DEFAULT NULL,
  `OrderFinish` timestamp(4) NULL DEFAULT NULL,
  `OrderTotal` int DEFAULT NULL,
  `BlockOrder` int DEFAULT NULL,
  `OperatorNumber` int DEFAULT NULL,
  `ReasonCanceled` text CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci,
  `DeniedCancelReq` int DEFAULT NULL,
  `NumOfTimesPrinted` int DEFAULT NULL,
  `DeniedCancelReq` int DEFAULT NULL,
  `NumOfTimesPrinted` int DEFAULT NULL,
  PRIMARY KEY (`DeptNum`,`OrderNumber`),
  UNIQUE KEY `Orders_PK` (`DeptNum`,`OrderNumber`),
  CONSTRAINT `DeptNum_fk` FOREIGN KEY (`DeptNum`) REFERENCES `depts` (`DeptNum`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `orders`
--

LOCK TABLES `orders` WRITE;
/*!40000 ALTER TABLE `orders` DISABLE KEYS */;
INSERT INTO `orders` VALUES (0,17,'none',5,'Mark S.','9379649136',NULL,0,187,'2021-03-06 14:15:43.0000','2021-03-06 15:50:00.0000',NULL,NULL,NULL,0,2,'no showed',NULL,1),(0,19,'none',3,'Mary M.','9379642526',NULL,0,187,'2021-03-06 16:00:08.0000','2021-03-06 16:20:00.0000','2021-03-06 16:06:59.0000','2021-03-06 16:11:37.0000',987,0,3,NULL,NULL,1),(0,20,'do not mix',3,'Judy G.','9379642645',NULL,0,187,'2021-03-06 15:25:31.0000','2021-03-06 17:20:00.0000','2021-03-06 16:14:42.0000','2021-03-06 16:19:15.0000',766,0,3,NULL,NULL,1),(0,24,'no labels on zip seal',1,'Joe E','9379649741',NULL,0,187,'2021-03-06 16:30:48.0000','2021-03-06 17:00:00.0000',NULL,NULL,NULL,0,0,NULL,NULL,1),(0,25,'thin as possible',1,'Cherie B.','9379648741',NULL,0,187,'2021-03-06 15:05:00.0000','2021-03-06 17:30:00.0000',NULL,NULL,NULL,0,0,NULL,NULL,1),(0,26,'single bagged',3,'John D.','9379649134',NULL,0,187,'2021-03-06 18:22:00.0000','2021-03-06 21:50:00.0000','2021-03-06 21:35:13.0000','2021-03-06 21:40:13.0000',1643,0,3,NULL,NULL,1),(0,27,'none',2,'Kathy A.','9373322092',NULL,0,187,'2021-03-06 15:30:27.0000','2021-03-06 16:20:00.0000','2021-03-06 16:10:21.0000',NULL,NULL,1,2,NULL,NULL,1),(0,28,'none',1,'Scott D.','9373328965',NULL,0,187,NULL,'2021-03-06 17:25:00.0000',NULL,NULL,NULL,0,0,NULL,NULL,1),(0,29,'thin',1,'Randy B.','9373327485',NULL,0,187,NULL,'2021-03-06 17:30:00.0000',NULL,NULL,NULL,0,0,NULL,NULL,1),(0,30,'single bagged',1,'Aaron','9373325681',NULL,0,187,NULL,'2021-03-06 17:35:00.0000',NULL,NULL,NULL,0,0,NULL,NULL,1),(0,31,'in a hurry',1,'Doug A.','9373322552',NULL,1,187,NULL,'2021-03-06 17:40:00.0000',NULL,NULL,NULL,0,0,NULL,NULL,1),(0,32,'937-332-3000',1,'Evan','9373321146',NULL,0,187,NULL,'2021-03-06 17:45:00.0000',NULL,NULL,NULL,0,0,NULL,NULL,1),(0,33,'each in own bag',1,'Larry Pevoar','9373324109',NULL,0,187,NULL,'2021-03-06 17:50:00.0000',NULL,NULL,NULL,0,0,NULL,NULL,1),(0,34,'label each item',1,'Joseph','9373321456',NULL,1,187,NULL,'2021-03-06 17:55:00.0000',NULL,NULL,NULL,0,0,NULL,NULL,1),(0,35,NULL,1,'Carlos','9373327896',NULL,0,187,NULL,'2021-03-06 18:00:00.0000',NULL,NULL,NULL,0,0,NULL,NULL,1),(0,36,NULL,1,'Job E.','9373322587',NULL,0,187,NULL,'2021-03-06 18:05:00.0000',NULL,NULL,NULL,0,0,NULL,NULL,1),(0,37,NULL,1,'Bryan with a Y','9373323000',NULL,1,187,NULL,'2021-03-06 18:10:00.0000',NULL,NULL,NULL,0,0,NULL,NULL,1);
/*!40000 ALTER TABLE `orders` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `prdefs`
--

DROP TABLE IF EXISTS `prdefs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `prdefs` (
  `DeptNum` int NOT NULL,
  `PDType` int NOT NULL,
  `PrPrice` int DEFAULT NULL,
  `PrPrMod` int DEFAULT NULL,
  `PrIsFrcPr` tinyint DEFAULT NULL,
  `PrExcPr` int DEFAULT NULL,
  `PrDscMe` int DEFAULT NULL,
  `PrDscPr1` int DEFAULT NULL,
  `PrDscPr2` int DEFAULT NULL,
  `PrDscPr3` int DEFAULT NULL,
  `PrTare` int DEFAULT NULL,
  `PrTareB` int DEFAULT NULL,
  `PrIsFrcTr` tinyint DEFAULT NULL,
  `PrPrpTare` int DEFAULT NULL,
  `PrNetWt` int DEFAULT NULL,
  `PrByCnt` int DEFAULT NULL,
  `PrIsFrcBC` tinyint DEFAULT NULL,
  `PrShlfLfD` int DEFAULT NULL,
  `PrShlfLfH` int DEFAULT NULL,
  `PrProdLfD` int DEFAULT NULL,
  `PrProdLfH` int DEFAULT NULL,
  `PrIsPrnSL` tinyint DEFAULT NULL,
  `PrIsPrnPL` tinyint DEFAULT NULL,
  `PrIsPrnPO` tinyint DEFAULT NULL,
  `PrBnsPts` int DEFAULT NULL,
  `PrPrtns` int DEFAULT NULL,
  `PrIsUsSL` tinyint DEFAULT NULL,
  `PrInMode` int DEFAULT NULL,
  `PrLblRot1` int DEFAULT NULL,
  `PrLblPlc1` int DEFAULT NULL,
  `PrLblRot2` int DEFAULT NULL,
  `PrLblPlc2` int DEFAULT NULL,
  `PrLblRot3` int DEFAULT NULL,
  `PrLblPlc3` int DEFAULT NULL,
  `PrName1` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `PrName2` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `PrVCOLTxt` tinyint DEFAULT NULL,
  `PrCOLReq` tinyint DEFAULT NULL,
  `PrPrPlT` int DEFAULT NULL,
  `PDClNum` int DEFAULT NULL,
  `PDTxNum` int DEFAULT NULL,
  `PDSMNum` int DEFAULT NULL,
  `PDNFNum` int DEFAULT NULL,
  `PDMaNum` int DEFAULT NULL,
  `PDNtNum` int DEFAULT NULL,
  `PDCaNum` int DEFAULT NULL,
  `PDLbNum1` int DEFAULT NULL,
  `PDLbNum2` int DEFAULT NULL,
  `PDLbNum3` int DEFAULT NULL,
  `PDSPNum` int DEFAULT NULL,
  `PDLCNum` int DEFAULT NULL,
  `PDDXNum` int DEFAULT NULL,
  `PDLPNum` int DEFAULT NULL,
  `PDLLNum` int DEFAULT NULL,
  `PDUPCNS` int DEFAULT NULL,
  `PDEANFS` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `PDEAN128FS` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `PDGS1GTINFS` text CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci,
  `PDGS1AIFS` text CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci,
  `PDQRCODEFS` text CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci,
  `PDDMATRXFS` text CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci,
  `PDEANPr` int DEFAULT NULL,
  `PDDesc` text CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci,
  `PDGrNm1` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `PDGrNm2` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `PDGrNm3` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `PDGrNm4` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `PDImage` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `PDCOLTN` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `PrMealSideItemsGroupNum` int DEFAULT NULL,
  `PrMealSideItemsAreFree` tinyint DEFAULT NULL,
  `PrMealNumSidesIncluded` int DEFAULT NULL,
  `PrCutTestMeatSpecSheetGroupNum` int DEFAULT NULL,
  `PrWeightMin` int DEFAULT NULL,
  `PrWeightMax` int DEFAULT NULL,
  `PDTotalPriceMin` int DEFAULT NULL,
  `PDTotalPriceMax` int DEFAULT NULL,
  `PDManualLabelTypeNum1` int DEFAULT NULL,
  `PDManualLabelTypeNum2` int DEFAULT NULL,
  `PDManualLabelTypeNum3` int DEFAULT NULL,
  `PDPrepackLabelTypeNum1` int DEFAULT NULL,
  `PDPrepackLabelTypeNum2` int DEFAULT NULL,
  `PDPrepackLabelTypeNum3` int DEFAULT NULL,
  `PDProdEntryLabelTypeNum1` int DEFAULT NULL,
  `PDProdEntryLabelTypeNum2` int DEFAULT NULL,
  `PDProdEntryLabelTypeNum3` int DEFAULT NULL,
  `PDSelfServLabelTypeNum1` int DEFAULT NULL,
  `PDSelfServLabelTypeNum2` int DEFAULT NULL,
  `PDSelfServLabelTypeNum3` int DEFAULT NULL,
  `PDPick5LabelTypeNum1` int DEFAULT NULL,
  `PDPick5LabelTypeNum2` int DEFAULT NULL,
  `PDPick5LabelTypeNum3` int DEFAULT NULL,
  `PDCheck` int DEFAULT NULL,
  PRIMARY KEY (`DeptNum`,`PDType`),
  UNIQUE KEY `PrDefs_PK` (`DeptNum`,`PDType`) /*!80000 INVISIBLE */,
  CONSTRAINT `DeptNum2_fk` FOREIGN KEY (`DeptNum`) REFERENCES `depts` (`DeptNum`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `prdefs`
--

LOCK TABLES `prdefs` WRITE;
/*!40000 ALTER TABLE `prdefs` DISABLE KEYS */;
/*!40000 ALTER TABLE `prdefs` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `prods`
--

DROP TABLE IF EXISTS `prods`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `prods` (
  `PrID` int NOT NULL,
  `DeptNum` int NOT NULL,
  `ClNum` int DEFAULT NULL,
  `TxNum` int DEFAULT NULL,
  `Prodscol` int DEFAULT NULL,
  `PrIsPrnSL` tinyint DEFAULT NULL,
  `DLXNum` int DEFAULT NULL,
  `ProdTxt5Num` int DEFAULT NULL,
  `ProdTxt6Num` int DEFAULT NULL,
  `ProdTxt7Num` int DEFAULT NULL,
  `ProdTxt8Num` int DEFAULT NULL,
  `ProdTxt9Num` int DEFAULT NULL,
  `SMNum` int DEFAULT NULL,
  `NFNum` int DEFAULT NULL,
  `NtNum` int DEFAULT NULL,
  `LbNum1` int DEFAULT NULL,
  `LbNum2` int DEFAULT NULL,
  `LbNum3` int DEFAULT NULL,
  `CaNum` int NOT NULL,
  `SPNum` int NOT NULL,
  `LCNum` int DEFAULT NULL,
  `LPNum` int DEFAULT NULL,
  `LXNum` int DEFAULT NULL,
  `LLID` int DEFAULT NULL,
  `MaNum` int DEFAULT NULL,
  `PrPrice` int DEFAULT NULL,
  `PrPrMod` int DEFAULT NULL,
  `PrIsFrcPr` tinyint DEFAULT NULL,
  `PrExcPr` int DEFAULT NULL,
  `PrDscMe` int DEFAULT NULL,
  `PrDscPr1` int DEFAULT NULL,
  `PrDscPr2` int DEFAULT NULL,
  `PrDscPr3` int DEFAULT NULL,
  `PrTare` int DEFAULT NULL,
  `PrTareB` int DEFAULT NULL,
  `PrIsFrcTr` tinyint DEFAULT NULL,
  `PrPrpTare` int DEFAULT NULL,
  `PrNetWt` int DEFAULT NULL,
  `PrByCnt` int DEFAULT NULL,
  `PrIsFrcBC` tinyint DEFAULT NULL,
  `PrShlfLfD` int DEFAULT NULL,
  `PrShlfLfH` int DEFAULT NULL,
  `PrProdLfD` int DEFAULT NULL,
  `PrProdLfH` int DEFAULT NULL,
  `PrIsPrnPL` tinyint DEFAULT NULL,
  `PrIsPrnPO` tinyint DEFAULT NULL,
  `PrBnsPts` int DEFAULT NULL,
  `PrPrtns` int DEFAULT NULL,
  `PrIsUsSL` tinyint DEFAULT NULL,
  `PrInMode` int DEFAULT NULL,
  `PrLblRot1` int DEFAULT NULL,
  `PrLblPlc1` int DEFAULT NULL,
  `PrLblRot2` int DEFAULT NULL,
  `PrLblPlc2` int DEFAULT NULL,
  `PrLblRot3` int DEFAULT NULL,
  `PrLblPlc3` int DEFAULT NULL,
  `PrName1` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `PrName2` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `PrVCOLTxt` tinyint DEFAULT NULL,
  `PrCOLReq` tinyint DEFAULT NULL,
  `PrPrPlT` int DEFAULT NULL,
  `PrNum` int NOT NULL,
  `PrType` int DEFAULT NULL,
  `PrBarPref` int DEFAULT NULL,
  `PrBarProN` int DEFAULT NULL,
  `PrBarVenN` int DEFAULT NULL,
  `PrEANCd` varchar(100) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `PrRawBarC` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `PrCdNm1` text CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci,
  `PrCdNm2` text CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci,
  `PrSpcSD` datetime(4) DEFAULT NULL,
  `PrDesc` text CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci,
  `PrGrNm1` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `PrGrNm2` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `PrGrNm3` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `PrGrNm4` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `PrImage` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `PrCOLTN` text CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci,
  `PrMealSideItemsGroupNum` int DEFAULT NULL,
  `PrMealSideItemsAreFree` tinyint DEFAULT NULL,
  `PrMealNumSidesIncluded` int DEFAULT NULL,
  `PrCutTestMeatSpecSheetGroupNum` int DEFAULT NULL,
  `PrWeightMin` int DEFAULT NULL,
  `PrWeightMax` int DEFAULT NULL,
  `PrTotalPriceMin` int DEFAULT NULL,
  `PrTotalPriceMax` int DEFAULT NULL,
  `PrManualLabelTypeNum1` int DEFAULT NULL,
  `PrManualLabelTypeNum2` int DEFAULT NULL,
  `PrManualLabelTypeNum3` int DEFAULT NULL,
  `PrPrepackLabelTypeNum1` int DEFAULT NULL,
  `PrPrepackLabelTypeNum2` int DEFAULT NULL,
  `PrPrepackLabelTypeNum3` int DEFAULT NULL,
  `PrProdEntryLabelTypeNum1` int DEFAULT NULL,
  `PrProdEntryLabelTypeNum2` int DEFAULT NULL,
  `PrProdEntryLabelTypeNum3` int DEFAULT NULL,
  `PrSelfServLabelTypeNum1` int DEFAULT NULL,
  `PrSelfServLabelTypeNum2` int DEFAULT NULL,
  `PrSelfServLabelTypeNum3` int DEFAULT NULL,
  `PrPick5LabelTypeNum1` int DEFAULT NULL,
  `PrPick5LabelTypeNum2` int DEFAULT NULL,
  `PrPick5LabelTypeNum3` int DEFAULT NULL,
  `PrCheck` int DEFAULT NULL,
  PRIMARY KEY (`PrID`,`DeptNum`),
  UNIQUE KEY `Prods_PK` (`PrID`) /*!80000 INVISIBLE */,
  UNIQUE KEY `PrNum_AK` (`DeptNum`,`PrNum`) /*!80000 INVISIBLE */,
  KEY `CaProds_FK` (`DeptNum`,`CaNum`) /*!80000 INVISIBLE */,
  KEY `SPProds_FK` (`SPNum`) /*!80000 INVISIBLE */,
  KEY `SPNum_idx` (`DeptNum`,`SPNum`) /*!80000 INVISIBLE */,
  CONSTRAINT `CaNum` FOREIGN KEY (`DeptNum`, `CaNum`) REFERENCES `categrs` (`DeptNum`, `CaNum`),
  CONSTRAINT `DeptNum7_fk` FOREIGN KEY (`DeptNum`) REFERENCES `depts` (`DeptNum`),
  CONSTRAINT `SPNum` FOREIGN KEY (`DeptNum`, `SPNum`) REFERENCES `spcials` (`DeptNum`, `SPNum`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci KEY_BLOCK_SIZE=2;
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
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `settings` (
  `id` int NOT NULL AUTO_INCREMENT,
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
  `AccessLevelDelete` int DEFAULT NULL,
  `StoreNumber` int NOT NULL,
  `BlockOrders` tinyint DEFAULT '1',
  `OrderListMinutes` int DEFAULT '60',
  `HourTimeSlots` int DEFAULT '6',
  `MinimumOrderWait` int DEFAULT '1',
  `StorePhone` varchar(256) COLLATE utf8mb4_general_ci DEFAULT NULL,
  `EnableCustomerSMS` tinyint DEFAULT '1',
  `OrderHistoryDays` int NOT NULL DEFAULT '60',
  `OrderItemWeightMin` decimal(6,3) DEFAULT NULL,
  `OrderItemWeightMax` decimal(6,3) DEFAULT NULL,
  `OrderPriceMax` decimal(5,2) DEFAULT NULL,
  `OrderItemQtyMax` int DEFAULT NULL,
  `MeasurementSystem` varchar(16) COLLATE utf8mb4_general_ci DEFAULT 'imperial',
  PRIMARY KEY (`id`),
  UNIQUE KEY `Settings_PK` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `settings`
--

LOCK TABLES `settings` WRITE;
/*!40000 ALTER TABLE `settings` DISABLE KEYS */;
INSERT INTO `settings` VALUES (1,'2021-04-06 22:24:36','09:00:00','17:00:00','09:00:00','17:00:00','09:00:00','17:00:00','09:00:00','17:00:00','09:00:00','17:00:00','09:00:00','17:00:00','09:00:00','17:00:00',1,1,1,60,6,10,'1-800-555-5555',1,60,0.250,25.000,999.99,20,'imperial');
/*!40000 ALTER TABLE `settings` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sfeatrs`
--

DROP TABLE IF EXISTS `sfeatrs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `sfeatrs` (
  `DeptNum` int NOT NULL,
  `SFNum` int NOT NULL,
  `SSNum` int DEFAULT NULL,
  `SGNum` int DEFAULT NULL,
  `SFDHGrid` int DEFAULT NULL,
  `SFDVGrid` int DEFAULT NULL,
  `SFDHSpc` int DEFAULT NULL,
  `SFDVSpc` int DEFAULT NULL,
  `SFCheck` int DEFAULT NULL,
  PRIMARY KEY (`DeptNum`,`SFNum`),
  UNIQUE KEY `SFeatrs_PK` (`DeptNum`,`SFNum`),
  KEY `SSNum_idx` (`SSNum`),
  KEY `DeptNumSGNum_idx` (`DeptNum`,`SGNum`),
  CONSTRAINT `DeptNum6_fk` FOREIGN KEY (`DeptNum`) REFERENCES `depts` (`DeptNum`),
  CONSTRAINT `DeptNumSGNum` FOREIGN KEY (`DeptNum`, `SGNum`) REFERENCES `skgrps` (`DeptNum`, `SGNum`),
  CONSTRAINT `SSNum` FOREIGN KEY (`SSNum`) REFERENCES `sstyles` (`SSNum`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sfeatrs`
--

LOCK TABLES `sfeatrs` WRITE;
/*!40000 ALTER TABLE `sfeatrs` DISABLE KEYS */;
/*!40000 ALTER TABLE `sfeatrs` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sftkeys`
--

DROP TABLE IF EXISTS `sftkeys`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `sftkeys` (
  `DeptNum` int NOT NULL,
  `SGNum` int NOT NULL,
  `SKHPos` int NOT NULL,
  `SKVPos` int NOT NULL,
  `CaNum` int NOT NULL,
  `OpNum` text CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci,
  `SFNum` int NOT NULL,
  `SKHSize` int DEFAULT NULL,
  `SKVSize` int DEFAULT NULL,
  `SKBgClr` int DEFAULT NULL,
  `SKTxClr` int DEFAULT NULL,
  `SKName1` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `SKName2` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `SKFNBCl` tinyint DEFAULT NULL,
  `SKFNBIm` tinyint DEFAULT NULL,
  `SKFNTxt` tinyint DEFAULT NULL,
  `SKLkPPl` tinyint DEFAULT NULL,
  `SKActn` int DEFAULT NULL,
  `SKActSb` int DEFAULT NULL,
  `SKPrNum` int DEFAULT NULL,
  `SKActPI` int DEFAULT NULL,
  `SKActPS` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `SKBgImage` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `SKCheck` int DEFAULT NULL,
  PRIMARY KEY (`DeptNum`,`SGNum`,`SKHPos`,`SKVPos`),
  UNIQUE KEY `SftKeys_PK` (`DeptNum`,`SGNum`,`SKHPos`,`SKVPos`) /*!80000 INVISIBLE */,
  KEY `SGKeys_FK` (`DeptNum`,`SGNum`),
  KEY `CaNum_idx` (`DeptNum`,`CaNum`),
  KEY `SFNum_idx` (`DeptNum`,`SFNum`),
  CONSTRAINT `CaNum2` FOREIGN KEY (`DeptNum`, `CaNum`) REFERENCES `categrs` (`DeptNum`, `CaNum`),
  CONSTRAINT `DeptNum8_fk` FOREIGN KEY (`DeptNum`) REFERENCES `depts` (`DeptNum`),
  CONSTRAINT `SFNum` FOREIGN KEY (`DeptNum`, `SFNum`) REFERENCES `sfeatrs` (`DeptNum`, `SFNum`),
  CONSTRAINT `SGNum` FOREIGN KEY (`DeptNum`, `SGNum`) REFERENCES `skgrps` (`DeptNum`, `SGNum`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sftkeys`
--

LOCK TABLES `sftkeys` WRITE;
/*!40000 ALTER TABLE `sftkeys` DISABLE KEYS */;
/*!40000 ALTER TABLE `sftkeys` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `skgrps`
--

DROP TABLE IF EXISTS `skgrps`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `skgrps` (
  `DeptNum` int NOT NULL,
  `SGNum` int NOT NULL,
  `SGHGrid` int DEFAULT NULL,
  `SGVGrid` int DEFAULT NULL,
  `SGHSpc` int DEFAULT NULL,
  `SGVSpc` int DEFAULT NULL,
  `SGCheck` int DEFAULT NULL,
  PRIMARY KEY (`DeptNum`,`SGNum`),
  UNIQUE KEY `SKGrps_PK` (`DeptNum`,`SGNum`) /*!80000 INVISIBLE */,
  CONSTRAINT `DeptNum5_fk` FOREIGN KEY (`DeptNum`) REFERENCES `depts` (`DeptNum`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `skgrps`
--

LOCK TABLES `skgrps` WRITE;
/*!40000 ALTER TABLE `skgrps` DISABLE KEYS */;
/*!40000 ALTER TABLE `skgrps` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `spcials`
--

DROP TABLE IF EXISTS `spcials`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `spcials` (
  `DeptNum` int NOT NULL,
  `SPNum` int NOT NULL,
  `SPName1` varchar(100) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `SPName2` varchar(100) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `SPDefL` int DEFAULT NULL,
  `SPImag1` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `SPImag2` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `SPCheck` int DEFAULT NULL,
  PRIMARY KEY (`DeptNum`,`SPNum`),
  UNIQUE KEY `Spcials_PK` (`DeptNum`,`SPNum`),
  CONSTRAINT `DeptNum4_fk` FOREIGN KEY (`DeptNum`) REFERENCES `depts` (`DeptNum`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `spcials`
--

LOCK TABLES `spcials` WRITE;
/*!40000 ALTER TABLE `spcials` DISABLE KEYS */;
/*!40000 ALTER TABLE `spcials` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sstyles`
--

DROP TABLE IF EXISTS `sstyles`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `sstyles` (
  `SSNum` int NOT NULL,
  `SSDTextL` int DEFAULT NULL,
  `SSDTextC` int DEFAULT NULL,
  `SSDGrTxtC` int DEFAULT NULL,
  `SSDFImgL` int DEFAULT NULL,
  `SSDBgClr` int DEFAULT NULL,
  `SSDBgImg` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `SSImgDir` varchar(256) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci DEFAULT NULL,
  `SSCheck` int DEFAULT NULL,
  PRIMARY KEY (`SSNum`),
  UNIQUE KEY `SStyles_PK` (`SSNum`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sstyles`
--

LOCK TABLES `sstyles` WRITE;
/*!40000 ALTER TABLE `sstyles` DISABLE KEYS */;
/*!40000 ALTER TABLE `sstyles` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2021-04-06 19:03:04
