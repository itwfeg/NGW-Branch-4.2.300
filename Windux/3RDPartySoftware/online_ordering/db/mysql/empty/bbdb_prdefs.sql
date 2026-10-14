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
  `PrName1` varchar(128) DEFAULT NULL,
  `PrName2` varchar(128) DEFAULT NULL,
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
  `PDEANFS` varchar(128) DEFAULT NULL,
  `PDEAN128FS` varchar(128) DEFAULT NULL,
  `PDGS1GTINFS` text,
  `PDGS1AIFS` text,
  `PDQRCODEFS` text,
  `PDDMATRXFS` text,
  `PDEANPr` int DEFAULT NULL,
  `PDDesc` text,
  `PDGrNm1` varchar(256) DEFAULT NULL,
  `PDGrNm2` varchar(256) DEFAULT NULL,
  `PDGrNm3` varchar(128) DEFAULT NULL,
  `PDGrNm4` varchar(256) DEFAULT NULL,
  `PDImage` varchar(256) DEFAULT NULL,
  `PDCOLTN` varchar(128) DEFAULT NULL,
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
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `prdefs`
--

LOCK TABLES `prdefs` WRITE;
/*!40000 ALTER TABLE `prdefs` DISABLE KEYS */;
/*!40000 ALTER TABLE `prdefs` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2021-03-23  8:33:01
