--
-- Database: `pset7`
--

-- --------------------------------------------------------

--
-- Table structure for table `history`
--

CREATE TABLE IF NOT EXISTS `history` (
  `id` int(10) unsigned NOT NULL,
  `type` enum('BUY','SELL') NOT NULL,
  `symbol` varchar(255) NOT NULL,
  `shares` int(11) NOT NULL,
  `price` decimal(65,4) NOT NULL,
  `datetime` datetime NOT NULL
) ENGINE=InnoDB;

-- --------------------------------------------------------

--
-- Table structure for table `portfolios`
--

CREATE TABLE IF NOT EXISTS `portfolios` (
  `id` int(11) NOT NULL,
  `symbol` varchar(255) NOT NULL,
  `shares` int(10) unsigned NOT NULL,
  PRIMARY KEY (`id`,`symbol`)
) ENGINE=InnoDB;

-- --------------------------------------------------------

--
-- Table structure for table `users`
--

CREATE TABLE IF NOT EXISTS `users` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `username` varchar(255) NOT NULL,
  `hash` varchar(255) NOT NULL,
  `cash` decimal(65,4) unsigned NOT NULL DEFAULT '0.0000',
  PRIMARY KEY (`id`),
  UNIQUE KEY `username` (`username`)
) ENGINE=InnoDB AUTO_INCREMENT=12 ;

--
-- Dumping data for table `users`
--

INSERT INTO `pset7`.`users` (`id`, `username`, `hash`, `cash`) VALUES(1, 'caesar', '$1$50$GHABNWBNE/o4VL7QjmQ6x0', 10000.0000);
INSERT INTO `pset7`.`users` (`id`, `username`, `hash`, `cash`) VALUES(2, 'hirschhorn', '$1$50$lJS9HiGK6sphej8c4bnbX.', 10000.0000);
INSERT INTO `pset7`.`users` (`id`, `username`, `hash`, `cash`) VALUES(3, 'jharvard', '$1$50$RX3wnAMNrGIbgzbRYrxM1/', 10000.0000);
INSERT INTO `pset7`.`users` (`id`, `username`, `hash`, `cash`) VALUES(4, 'malan', '$1$HA$azTGIMVlmPi9W9Y12cYSj/', 10000.0000);
INSERT INTO `pset7`.`users` (`id`, `username`, `hash`, `cash`) VALUES(5, 'milo', '$1$HA$6DHumQaK4GhpX8QE23C8V1', 10000.0000);
INSERT INTO `pset7`.`users` (`id`, `username`, `hash`, `cash`) VALUES(6, 'skroob', '$1$50$euBi4ugiJmbpIbvTTfmfI.', 10000.0000);
INSERT INTO `pset7`.`users` (`id`, `username`, `hash`, `cash`) VALUES(7, 'zamyla', '$1$50$uwfqB45ANW.9.6qaQ.DcF.', 10000.0000);
