<?php

    require("../includes/config.php");

    // ensure each parameter is in lat,lng format
    if (!preg_match("/^-?\d+(?:\.\d+)?,-?\d+(?:\.\d+)?$/", $_GET["sw"]) ||
        !preg_match("/^-?\d+(?:\.\d+)?,-?\d+(?:\.\d+)?$/", $_GET["ne"]))
    {
        http_response_code(400);
        exit;
    }

    // explode southwest corner into two variables
    list($sw_lat, $sw_lng) = explode(",", $_GET["sw"]);

    // explode northeast corner into two variables
    list($ne_lat, $ne_lng) = explode(",", $_GET["ne"]);

    // find 10 cities within view, pseudorandomly chosen if more within view
    // http://stackoverflow.com/questions/4834772/get-all-records-from-mysql-database-that-are-within-google-maps-getbounds
    // TODO: international date line too?
    if ($sw_lng < $ne_lng)
    {
        // northern hemisphere
        $rows = query("SELECT * FROM places WHERE ? <= latitude AND latitude <= ? AND (? <= longitude AND longitude <= ?) GROUP BY place_name ORDER BY RAND() LIMIT 10", $sw_lat, $ne_lat, $sw_lng, $ne_lng);
    }
    else
    {
        // southern hemisphere
        $rows = query("SELECT * FROM places WHERE ? <= latitude AND latitude <= ? AND (? <= longitude OR longitude <= ?) GROUP_BY place_name ORDER BY RAND() LIMIT 10", $sw_lat, $ne_lat, $sw_lng, $ne_lng);
    }

    // output places as JSON (pretty-printed for debugging convenience)
    header("Content-type: application/json");
    print(json_encode($rows, JSON_PRETTY_PRINT));

?>
