<?php

    require(__DIR__ . "/../includes/config.php");

    // ensure proper usage
    if (!isset($_GET["geo"]) || strlen($_GET["geo"]) === 0)
    {
        http_response_code(400);
        exit;
    }

    // select rows that match pattern, ignoring country_code,
    // which wouldn't meet default ft_min_word_len, per
    // http://dev.mysql.com/doc/refman/5.5/en/server-system-variables.html#sysvar_ft_min_word_len
    $rows = CS50::query("SELECT * FROM places WHERE MATCH(postal_code,place_name,admin_name1,admin_name2,admin_name3) AGAINST (?) LIMIT 50", $_GET["geo"]);

    // output places as JSON (pretty-printed for debugging convenience)
    header("Content-type: application/json");
    print(json_encode($rows, JSON_PRETTY_PRINT));

?>
