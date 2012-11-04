<?php

    // configuration
    require("../includes/config.php"); 

    // if form was submitted
    if ($_SERVER["REQUEST_METHOD"] == "POST")
    {
        // validate submission
        if (empty($_POST["symbol"]))
        {
            apologize("You must select a stock to sell.");
        }

        // get user's number of shares
        $rows = query("SELECT shares FROM portfolios WHERE id = ? AND symbol = ?", $_SESSION["id"], $_POST["symbol"]);
        if (count($rows) != 1)
        {
            apologize("You don't own that stock.");
        }
        $shares = $rows[0]["shares"];

        // sell shares
        $stock = lookup($_POST["symbol"]);
        if ($stock !== false)
        {
            // update portfolio
            query("DELETE FROM portfolios WHERE id = ? AND symbol = ?", $_SESSION["id"], $_POST["symbol"]);

            // update cash
            query("UPDATE users SET cash = cash + ? WHERE id = ?", $shares * $stock["price"], $_SESSION["id"]);

            // update history
            query("INSERT INTO history (id, type, symbol, shares, price, datetime)
                VALUES(?, 'SELL', ?, ?, ?, NOW())", $_SESSION["id"], $stock["symbol"],
                $shares, $stock["price"]);

            // redirect user
            redirect("/");
        }
    }
    else
    {
        // get user's portfolio
        $symbols = [];
        $rows = query("SELECT symbol FROM portfolios WHERE id = ? ORDER BY symbol", $_SESSION["id"]);
        if ($rows === false)
        {
            apologize("Could not find your portfolio.");
        }

        // get symbols in portfolio
        foreach ($rows as $row)
        {
            $symbols[] = $row["symbol"];
        }

        // render form
        render("sell_form.php", ["symbols" => $symbols, "title" => "Sell"]);
    }

?>
