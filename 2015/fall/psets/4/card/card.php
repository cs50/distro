#!/usr/bin/env php
<?php

    // ensure proper usage
    if ($argc < 3)
    {
        die("Usage: php card.php outfile infile [...]\n");
    }

    // open outfile
    if (!preg_match("/\.raw$/i", $argv[1]))
    {
        die("outfile must end in .raw\n");
    }
    $outfile = fopen($argv[1], "wb");

    // write a block of all 0s to outfile
    for ($i = 0; $i < 512; $i++)
    {
        fwrite($outfile, chr(0));
    }
    print("Wrote block of zeroes.\n");

    // write easter egg to outfile
    $egg = " bit.ly/18gECvy ";
    fwrite($outfile, $egg);
    for ($i = 0, $n = 512 - strlen($egg) % 512; $i < $n; $i++)
    {
        fwrite($outfile, chr(0));
    }
    print("Wrote block(s) with easter egg.\n");

    // iterate over infiles
    $files = array_slice($argv, 2);
    foreach ($files as $file)
    {
        if (preg_match("/\.(jpg|jpeg)$/i", $file))
        {
            $temp = tempnam(sys_get_temp_dir(), "");
            system(escapeshellcmd("convert {$file} -strip {$temp}"));
            $size = filesize($temp);
            $infile = fopen($temp, "rb");
            $contents = fread($infile, $size);
            fclose($infile);
            fwrite($outfile, $contents, $size);
            for ($i = 0, $n = 512 - $size % 512; $i < $n; $i++)
            {
                fwrite($outfile, chr(0));
            }
            print("Wrote {$file}.\n");
        }
        else
        {
            print("Skipped {$file}.\n");
        }
    }

    // close outfile
    fclose($outfile);

?>
