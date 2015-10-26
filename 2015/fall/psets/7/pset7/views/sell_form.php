<form action="sell.php" method="post">
    <fieldset>
        <div class="form-group">
            <select class="form-control" name="symbol">
                <option value=""> </option>
                <?php 
                    
                    foreach ($symbols as $symbol)
                    {
                        $s = htmlspecialchars($symbol);
                        printf("<option value='%s'>%s</option>", $s, $s);
                    }

                ?>
            </select>
        </div>
        <div class="form-group">
            <button class="btn btn-default" type="submit">Sell</button>
        </div>
    </fieldset>
</form>
