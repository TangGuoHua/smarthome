<?php

$output = "";
$output = $output . "# Report external IP address to web portal" . PHP_EOL;
$output = $output . "0 * * * * /home/pi/smartHome/script/updateExtIP.sh" . PHP_EOL;
$output = $output . PHP_EOL;

$output = $output . "# Kitchen ceiling light AUTO at 4:30 every day" . PHP_EOL;
$output = $output . "30 4 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData4=2, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=22\"" . PHP_EOL;
$output = $output . PHP_EOL;

$output = $output . "# Balcony curtain 0%-coverage at 4:35 every day" . PHP_EOL;
$output = $output . "35 4 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData4=0, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=62\"" . PHP_EOL;
$output = $output . PHP_EOL;

$output = $output . "# Balcony curtain 0%-coverage at 16:00 every day" . PHP_EOL;
$output = $output . "0 16 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData4=0, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=62\"" . PHP_EOL;
$output = $output . PHP_EOL;

$output = $output . "# Living room floor lamp ON at 17:20 every day" . PHP_EOL;
$output = $output . "20 17 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData1=1, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=53\"" . PHP_EOL;
$output = $output . PHP_EOL;

$output = $output . "# Living room desk lamp ON at 17:21 every day" . PHP_EOL;
$output = $output . "21 17 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData5=1, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=51\"" . PHP_EOL;
$output = $output . PHP_EOL;

$output = $output . "# Dinning room lights ON at 17:40 every day" . PHP_EOL;
$output = $output . "40 17 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData1=1, fldData3=1, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=41\"" . PHP_EOL;
$output = $output . PHP_EOL;

//$output = $output . "# Balcony curtain 100%-coverage at 18:30 every day" . PHP_EOL;
//$output = $output . "30 18 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData4=100, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=62\"" . PHP_EOL;
//$output = $output . PHP_EOL;

$output = $output . "# Kitchen ceiling light OFF at 19:30 every day" . PHP_EOL;
$output = $output . "30 19 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData4=0, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=22\"" . PHP_EOL;
$output = $output . PHP_EOL;

$output = $output . "# Bath room big water heater ON at 21:10 every day" . PHP_EOL;
$output = $output . "10 21 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData5=1, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=91\"" . PHP_EOL;
$output = $output . PHP_EOL;

$output = $output . "# Bath room big water heater OFF at 21:30 every day" . PHP_EOL;
$output = $output . "30 21 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData5=0, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=91\"" . PHP_EOL;


file_put_contents('/tmp/phpcrontab.txt', $output);

echo exec('crontab -r');
echo exec('crontab /tmp/phpcrontab.txt');


$output = shell_exec('crontab -l');


echo $output;
echo "<br>";

echo "done<br>";
?>