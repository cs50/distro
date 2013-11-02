<?php

    // configuration
    require("../includes/config.php"); 
    
    // get user's history
    $rows = query("SELECT * FROM history WHERE id = ?", $_SESSION["id"]);
    if ($rows === false)
    {
        apologize("Can't find your history.");
    }

    // render history
    render("history.php", ["title" => "Portfolio", "transactions" => $rows]);

?>
