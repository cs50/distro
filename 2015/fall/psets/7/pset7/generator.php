#!/bin/env php
<?php

    printf("INSERT INTO users (username, hash) VALUES('%s', '%s');\n", "andi", crypt("Berkeley", '$1$HA$'));
    printf("INSERT INTO users (username, hash) VALUES('%s', '%s');\n", "caesar", crypt("13", '$1$50$'));
    printf("INSERT INTO users (username, hash) VALUES('%s', '%s');\n", "eli", crypt("yale", '$1$50$'));
    printf("INSERT INTO users (username, hash) VALUES('%s', '%s');\n", "hdan", crypt("boola", '$1$50$'));
    printf("INSERT INTO users (username, hash) VALUES('%s', '%s');\n", "jason", crypt("Silliman", '$1$50$'));
    printf("INSERT INTO users (username, hash) VALUES('%s', '%s');\n", "john", crypt("harvard", '$1$50$'));
    printf("INSERT INTO users (username, hash) VALUES('%s', '%s');\n", "levatich", crypt("PBJ", '$1$50$'));
    printf("INSERT INTO users (username, hash) VALUES('%s', '%s');\n", "rob", crypt("password", '$1$50$'));
    printf("INSERT INTO users (username, hash) VALUES('%s', '%s');\n", "skroob", crypt("12345", '$1$50$'));
    printf("INSERT INTO users (username, hash) VALUES('%s', '%s');\n", "zamyla", crypt("password", '$1$HA$'));

?>
