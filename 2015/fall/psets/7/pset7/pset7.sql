REATE DATABASE IF NOT EXISTS  `pset7`;


--
-- Table structure for table `users`
--

CREATE TABLE IF NOT EXISTS `pset7`.`users` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `username` varchar(255) NOT NULL,
  `hash` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `username` (`username`)
) ENGINE=InnoDB AUTO_INCREMENT=11;

--
-- Dumping data for table `pset7`.`users`
--

INSERT INTO `pset7`.`users` (`id`, `username`, `hash`) VALUES(1, 'andi', '$1$HA$VLmVUwflFXcjO7lA5x/Yl/');
INSERT INTO `pset7`.`users` (`id`, `username`, `hash`) VALUES(2, 'caesar', '$1$50$GHABNWBNE/o4VL7QjmQ6x0');
INSERT INTO `pset7`.`users` (`id`, `username`, `hash`) VALUES(3, 'eli', '$1$50$bc0ppyn1paKwfvSCB1Vqd0');
INSERT INTO `pset7`.`users` (`id`, `username`, `hash`) VALUES(4, 'hdan', '$1$50$UMOz1KE6p/PqftaraS.de1');
INSERT INTO `pset7`.`users` (`id`, `username`, `hash`) VALUES(5, 'jason', '$1$50$Vq4CNGOJzM1vpIazYhsTo1');
INSERT INTO `pset7`.`users` (`id`, `username`, `hash`) VALUES(6, 'john', '$1$50$xuVbD7p.rTf48ShEZcE/k1');
INSERT INTO `pset7`.`users` (`id`, `username`, `hash`) VALUES(7, 'levatich', '$1$50$eqqFT6uMdBI.qIVQAoxLo1');
INSERT INTO `pset7`.`users` (`id`, `username`, `hash`) VALUES(8, 'rob', '$1$50$lJS9HiGK6sphej8c4bnbX.');
INSERT INTO `pset7`.`users` (`id`, `username`, `hash`) VALUES(9, 'skroob', '$1$50$euBi4ugiJmbpIbvTTfmfI.');
INSERT INTO `pset7`.`users` (`id`, `username`, `hash`) VALUES(10, 'zamyla', '$1$HA$l5llES7AEaz8ndmSo5Ig41');
