<?php

    require("../includes/config.php");

    // ensure proper usage
    if (empty($_GET["geo"]))
    {
        http_response_code(400);
        exit;
    }

    // pattern to match against, with a wildcard appended
    $pat = $_GET["geo"] . "%";

    // select rows that match pattern
    $rows = query("SELECT postal_code, place_name, latitude, longitude FROM places WHERE postal_code LIKE ? ORDER BY place_name", $pat);

    // get places
    $places = [];
    foreach ($rows as $row)
    {
        $places[] = [
            "postal_code" => $row["postal_code"],
            "place_name" => $row["place_name"],
            "latitude" => floatval($row["latitude"]),
            "longitude" => floatval($row["longitude"])
        ];
    }

    // output places as JSON (pretty-printed for debugging convenience)
    header("Content-type: application/json");
    print(json_encode($places, JSON_PRETTY_PRINT));

?>
