#!/bin/env php
<?php

     printf("INSERT INTO `users` (username, hash) VALUES('%s', '%s');\n", "andi", password_hash("Berkeley", PASSWORD_DEFAULT));
     printf("INSERT INTO `users` (username, hash) VALUES('%s', '%s');\n", "caesar", password_hash("13", PASSWORD_DEFAULT));
     printf("INSERT INTO `users` (username, hash) VALUES('%s', '%s');\n", "eli", password_hash("yale", PASSWORD_DEFAULT));
     printf("INSERT INTO `users` (username, hash) VALUES('%s', '%s');\n", "hdan", password_hash("boola", PASSWORD_DEFAULT));
     printf("INSERT INTO `users` (username, hash) VALUES('%s', '%s');\n", "jason", password_hash("Silliman", PASSWORD_DEFAULT));
     printf("INSERT INTO `users` (username, hash) VALUES('%s', '%s');\n", "john", password_hash("harvard", PASSWORD_DEFAULT));
     printf("INSERT INTO `users` (username, hash) VALUES('%s', '%s');\n", "levatich", password_hash("PBJ", PASSWORD_DEFAULT));
     printf("INSERT INTO `users` (username, hash) VALUES('%s', '%s');\n", "rob", password_hash("password", PASSWORD_DEFAULT));
     printf("INSERT INTO `users` (username, hash) VALUES('%s', '%s');\n", "skroob", password_hash("12345", PASSWORD_DEFAULT));
     printf("INSERT INTO `users` (username, hash) VALUES('%s', '%s');\n", "zamyla", password_hash("password", PASSWORD_DEFAULT));

?>
