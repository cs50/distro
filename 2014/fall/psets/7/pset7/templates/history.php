<table class="table table-striped">

    <thead>
        <tr>
            <th>Transaction</th>
            <th>Date/Time</th>
            <th>Symbol</th>
            <th>Shares</th>
            <th>Price</th>
        </tr>
    </thead>

    <tbody>
    <?php 
    
        foreach ($transactions as $transaction)
        {
            printf("<tr>");
            printf("<td>%s</td>", $transaction["type"]);
            printf("<td>%s</td>", date("n/j/y, g:ia", strtotime($transaction["datetime"])));
            printf("<td>%s</td>", htmlspecialchars($transaction["symbol"]));
            printf("<td>%s</td>", number_format($transaction["shares"]));
            printf("<td>$%s</td>", number_format($transaction["price"], 2));
            printf("</tr>");
        }

    ?>
    </tbody>

</table>
