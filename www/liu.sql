-- MySQL dump 10.13  Distrib 5.5.44, for debian-linux-gnu (x86_64)
--
-- Host: localhost    Database: liu
-- ------------------------------------------------------
-- Server version	5.5.44-0ubuntu0.14.04.1

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
-- Table structure for table `home`
--

DROP TABLE IF EXISTS `home`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `home` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(20) COLLATE utf8_bin NOT NULL,
  `another` varchar(10) COLLATE utf8_bin DEFAULT '',
  `father_id` bigint(20) DEFAULT NULL,
  `remarks` varchar(10) COLLATE utf8_bin DEFAULT '',
  `birth` varchar(4) COLLATE utf8_bin DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=36 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `home`
--

LOCK TABLES `home` WRITE;
/*!40000 ALTER TABLE `home` DISABLE KEYS */;
INSERT INTO `home` VALUES (1,'根目录','',1,'',''),(2,'刘咨荣','安淑贞',19,'','1918'),(3,'刘吾成','阴学梅',2,'','1951'),(4,'刘吾斌','毕风鸣',2,'','1954'),(5,'刘吾光','闫秀芬',2,'','1958'),(6,'刘吾明','胡凌霞',2,'','1961'),(7,'刘云霞','雷海光',3,'','1979'),(8,'刘云宝','',3,'','1981'),(9,'刘云升','马轶群',4,'','1981'),(10,'刘云芳','',4,'','1989'),(11,'刘云花','范栋',5,'','1983'),(12,'刘云娟','师世平',5,'','1986'),(13,'刘云宾','',5,'','1992'),(14,'刘云润','李铭珺',6,'','1988'),(15,'刘云辉','',6,'','1990'),(17,'刘月英','梁生跃',2,'','1944'),(18,'刘凤英','邓振业',2,'','1946'),(19,'刘章福','李氏',1,'',''),(22,'刘咨达','冀氏',19,'',''),(23,'刘秀花','尚启昌',19,'段村',''),(24,'刘咨花','梁作金',19,'梁家',''),(25,'刘吾玉','',22,'',''),(26,'刘吾文','',22,'',''),(27,'刘春族','',25,'云林',''),(28,'刘春明','',25,'二小',''),(29,'刘春峰','',25,'全宏',''),(30,'刘云仙','',25,'',''),(31,'刘二姑娘','',25,'',''),(32,'刘三姑娘','',25,'',''),(33,'刘永亮','',26,'',''),(34,'刘永爱','',26,'',''),(35,'刘春林','',25,'柿子','');
/*!40000 ALTER TABLE `home` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2015-10-26 11:46:32
