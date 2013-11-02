#!/bin/env php
<?php

    printf("INSERT INTO users (username, hash) VALUES('%s', '%s');\n", "caesar", crypt("13", '$1$50$'));
    printf("INSERT INTO users (username, hash) VALUES('%s', '%s');\n", "hirschhorn", crypt("password", '$1$50$'));
    printf("INSERT INTO users (username, hash) VALUES('%s', '%s');\n", "jharvard", crypt("crimson", '$1$50$'));
    printf("INSERT INTO users (username, hash) VALUES('%s', '%s');\n", "malan", crypt("crimson", '$1$HA$'));
    printf("INSERT INTO users (username, hash) VALUES('%s', '%s');\n", "milo", crypt("1337", '$1$HA$'));
    printf("INSERT INTO users (username, hash) VALUES('%s', '%s');\n", "skroob", crypt("12345", '$1$50$'));
    printf("INSERT INTO users (username, hash) VALUES('%s', '%s');\n", "zamyla", crypt("1337", '$1$50$'));

?>
