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
  `OpNum` text,
  `SFNum` int NOT NULL,
  `SKHSize` int DEFAULT NULL,
  `SKVSize` int DEFAULT NULL,
  `SKBgClr` int DEFAULT NULL,
  `SKTxClr` int DEFAULT NULL,
  `SKName1` varchar(256) DEFAULT NULL,
  `SKName2` varchar(256) DEFAULT NULL,
  `SKFNBCl` tinyint DEFAULT NULL,
  `SKFNBIm` tinyint DEFAULT NULL,
  `SKFNTxt` tinyint DEFAULT NULL,
  `SKLkPPl` tinyint DEFAULT NULL,
  `SKActn` int DEFAULT NULL,
  `SKActSb` int DEFAULT NULL,
  `SKPrNum` int DEFAULT NULL,
  `SKActPI` int DEFAULT NULL,
  `SKActPS` varchar(256) DEFAULT NULL,
  `SKBgImage` varchar(256) DEFAULT NULL,
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
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sftkeys`
--

LOCK TABLES `sftkeys` WRITE;
/*!40000 ALTER TABLE `sftkeys` DISABLE KEYS */;
/*!40000 ALTER TABLE `sftkeys` ENABLE KEYS */;
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
