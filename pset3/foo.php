<?php

$words = file("dictionary.csv");
foreach ($words as $w)
    print strlen($w) . "\n";
