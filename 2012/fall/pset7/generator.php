#!/bin/env php
<?php

    printf("INSERT INTO users (username, hash) VALUES('%s', '%s');\n", "caesar", crypt("13", '$1$50$'));
    printf("INSERT INTO users (username, hash) VALUES('%s', '%s');\n", "cs50", crypt("12345678", '$1$50$'));
    printf("INSERT INTO users (username, hash) VALUES('%s', '%s');\n", "jharvard", crypt("crimson", '$1$50$'));
    printf("INSERT INTO users (username, hash) VALUES('%s', '%s');\n", "malan", crypt("crimson", '$1$HA$'));
    printf("INSERT INTO users (username, hash) VALUES('%s', '%s');\n", "nate", crypt("bacon", '$1$50$'));
    printf("INSERT INTO users (username, hash) VALUES('%s', '%s');\n", "rbowden", crypt("password", '$1$50$'));
    printf("INSERT INTO users (username, hash) VALUES('%s', '%s');\n", "skroob", crypt("12345", '$1$50$'));
    printf("INSERT INTO users (username, hash) VALUES('%s', '%s');\n", "tmacwilliam", crypt("n00b", '$1$50$'));
    printf("INSERT INTO users (username, hash) VALUES('%s', '%s');\n", "zamyla", crypt("l33t", '$1$50$'));

?>
