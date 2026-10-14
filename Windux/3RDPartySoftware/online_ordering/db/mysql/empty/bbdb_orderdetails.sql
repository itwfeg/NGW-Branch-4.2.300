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
  `PluPrice` decimal(5,2) DEFAULT NULL,
  `PluByCount` int DEFAULT NULL,
  `PluWeight` decimal(4,2) DEFAULT NULL,
  `PluThickness` int DEFAULT NULL,
  `PluDescription` varchar(128) DEFAULT NULL,
  PRIMARY KEY (`DeptNum`,`OrderNumId`),
  UNIQUE KEY `OrderDetails_PK` (`DeptNum`,`OrderNumId`) /*!80000 INVISIBLE */,
  KEY `OrderNumber_idx` (`OrderNumber`),
  KEY `OrderNum_FK` (`OrderNumber`),
  KEY `OrderNumber_idx1` (`DeptNum`,`OrderNumber`),
  CONSTRAINT `DeptNum1_fk` FOREIGN KEY (`DeptNum`) REFERENCES `depts` (`DeptNum`),
  CONSTRAINT `OrderNumber` FOREIGN KEY (`DeptNum`, `OrderNumber`) REFERENCES `orders` (`DeptNum`, `OrderNumber`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `orderdetails`
--

LOCK TABLES `orderdetails` WRITE;
/*!40000 ALTER TABLE `orderdetails` DISABLE KEYS */;
INSERT INTO `orderdetails` VALUES (0,65,26,57428,1,469.00,1,1.30,2,'Capistrano York Ham'),(0,66,26,41458,1,325.00,1,0.45,3,'Bernina Serrano Ham'),(0,67,26,98690,3,125.00,8,0.00,0,'Hamburger Buns 8 Count'),(0,68,27,57896,1,479.00,1,1.00,3,'Honey Baked Ham'),(0,69,24,12847,1,459.00,1,1.00,2,'Black Forest Ham'),(0,70,24,56985,1,599.00,1,0.50,3,'Roasted Turkey'),(0,71,25,43854,1,549.00,1,1.00,2,'Garlic Chicken'),(0,72,17,85475,3,100.00,4,0.00,0,'Pork Egg Roll'),(0,73,19,35748,1,933.00,1,1.50,4,'Deluxe Bologna'),(0,74,19,26587,1,399.00,1,1.00,2,'American Cheese'),(0,75,19,59872,1,589.00,1,0.75,3,'Spiced Kentucky Ham'),(0,76,19,23785,1,399.00,1,0.75,2,'Baby Swiss Cheese'),(0,77,20,86745,1,179.00,1,0.75,4,'German Liverwurst'),(0,78,20,24587,1,139.00,1,1.00,2,'Limburger Cheese');
/*!40000 ALTER TABLE `orderdetails` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2021-03-23  8:33:03
