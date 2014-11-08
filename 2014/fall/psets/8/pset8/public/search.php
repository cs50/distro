<?php

    require(__DIR__ . "/../includes/config.php");

    // ensure proper usage
    if (!isset($_GET["geo"]) || strlen($_GET["geo"]) === 0)
    {
        http_response_code(400);
        exit;
    }

    // pattern to match against, with a wildcard appended
    $pat = $_GET["geo"] . "%";

    // select rows that match pattern
    $rows = query("SELECT * FROM places WHERE MATCH(country_code,postal_code,place_name,admin_name1,admin_name2,admin_name3) AGAINST (? WITH QUERY EXPANSION) LIMIT 50", $pat);

    // output places as JSON (pretty-printed for debugging convenience)
    header("Content-type: application/json");
    print(json_encode($rows, JSON_PRETTY_PRINT));

?>
