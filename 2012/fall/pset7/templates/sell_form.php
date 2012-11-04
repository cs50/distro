<form action="sell.php" method="post">
    <fieldset>
        <div class="control-group">
            <select name="symbol">
                <option value=''></option>
                <?php 
                    
                    foreach ($symbols as $symbol)
                    {
                        $s = htmlspecialchars($symbol);
                        printf("<option value='%s'>%s</option>", $s, $s);
                    }

                ?>
            </select>
        </div>
        <div class="control-group">
            <button type="submit" class="btn">Sell</button>
        </div>
    </fieldset>
</form>
