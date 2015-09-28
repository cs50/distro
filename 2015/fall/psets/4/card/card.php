#!/usr/bin/env php
<?php

    // ensure convert is installed
    if (empty(shell_exec("which convert")))
    {
        die("convert not installed\n");
    }

    // ensure proper usage
    if ($argc < 2)
    {
        die("Usage: php card.php infile [...]\n");
    }

    // open outfile
    $card = "card.raw";
    if (file_exists($card))
    {
        die("{$card} already exists\n");
    }
    $outfile = fopen($card, "wb");
    if ($outfile === false)
    {
        die("could not create {$card}\n");
    }

    // write a block of all 0s to outfile
    for ($i = 0; $i < 512; $i++)
    {
        if (fwrite($outfile, chr(0)) === false)
        {
            die("could not write zeroes to {$card}\n");
        }
    }
    print("Wrote block with zeroes to {$card}.\n");

    // write easter egg to outfile
    $egg = " bit.ly/18gECvy ";
    if (fwrite($outfile, $egg) === false)
    {
        die("could not write easter egg to {$card}\n");
    }
    for ($i = 0, $n = 512 - strlen($egg) % 512; $i < $n; $i++)
    {
        if (fwrite($outfile, chr(0)) === false)
        {
            die("could not write zeroes to {$card}\n");
        }
    }
    print("Wrote block(s) with easter egg to {$card}.\n");

    // iterate over infiles
    $files = array_slice($argv, 1);
    foreach ($files as $file)
    {
        // include *.jpg and *.jpeg, case-insensitively
        if (preg_match("/\.(jpg|jpeg)$/i", $file))
        {
            $temp = tempnam(sys_get_temp_dir(), "");
            if ($temp === false)
            {
                die("could not create temporary file\n");
            }
            if (system(escapeshellcmd("convert {$file} -strip {$temp}")) === false)
            {
                die("could not strip metadata from {$file}\n");
            }
            $size = filesize($temp);
            if ($size === false)
            {
                die("could not determine size of temporary file\n");
            }
            $infile = fopen($temp, "rb");
            if ($infile === false)
            {
                die("could not open temporary file for reading\n");
            }
            $contents = fread($infile, $size);
            if ($contents === false)
            {
                die("could not read temporary file\n");
            }
            if (fclose($infile) === false)
            {
                die("could not close {$file}\n");
            }
            if (fwrite($outfile, $contents, $size) === false)
            {
                die("could not write contents of {$file} to {$card}\n");
            }
            for ($i = 0, $n = 512 - $size % 512; $i < $n; $i++)
            {
                if (fwrite($outfile, chr(0)) === false)
                {
                    die("could not write zeroes to {$card}\n");
                }
            }
            print("Wrote {$file} to {$card}.\n");
        }
        else
        {
            print("Skipped {$file}.\n");
        }
    }

    // close outfile
    if (fclose($outfile) === false)
    {
        die("could not close {$card}\n");
    }

?>
