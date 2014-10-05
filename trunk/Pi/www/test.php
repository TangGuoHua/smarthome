<?php


$output = shell_exec('crontab -l');
//file_put_contents('/tmp/phpcrontab.txt', $output.'* * * * * NEW_CRON'.PHP_EOL);
//echo exec('crontab /tmp/crontab.txt');

echo $output;
echo "hello<br>hi<br>";

$output = "";
$output = $output . "# Report external IP address to web portal" . PHP_EOL;
$output = $output . "0,30 * * * * /home/pi/smartHome/script/updateExtIP.sh" . PHP_EOL;
$output = $output . PHP_EOL;

$output = $output . "# Kitchen ceiling light AUTO at 4:30 every day" . PHP_EOL;
$output = $output . "30 4 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData4=2, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=22\"" . PHP_EOL;
$output = $output . PHP_EOL;

file_put_contents('/tmp/phpcrontab.txt', $output);

echo exec('crontab -r');
//echo exec('crontab /tmp/phpcrontab.txt');
echo "done<br>";
?>