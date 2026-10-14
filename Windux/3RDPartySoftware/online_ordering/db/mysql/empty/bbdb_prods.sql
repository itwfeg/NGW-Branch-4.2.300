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
  `PrName1` varchar(256) DEFAULT NULL,
  `PrName2` varchar(256) DEFAULT NULL,
  `PrVCOLTxt` tinyint DEFAULT NULL,
  `PrCOLReq` tinyint DEFAULT NULL,
  `PrPrPlT` int DEFAULT NULL,
  `PrNum` int NOT NULL,
  `PrType` int DEFAULT NULL,
  `PrBarPref` int DEFAULT NULL,
  `PrBarProN` int DEFAULT NULL,
  `PrBarVenN` int DEFAULT NULL,
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
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci KEY_BLOCK_SIZE=2;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `prods`
--

LOCK TABLES `prods` WRITE;
/*!40000 ALTER TABLE `prods` DISABLE KEYS */;
/*!40000 ALTER TABLE `prods` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2021-03-23  8:33:00
