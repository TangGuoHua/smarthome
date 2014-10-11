<?php
/*
日期        作者    备注
------------------------------------------
2014-OCT-11 黄长浩  初始版本
*/

$gPageTitle = "定时任务";
include "include/templateHeader.php";
include "include/utils.php";


$cronList = array(
	"Report IP address to web portal every hour" => "0 * * * * /home/pi/smartHome/script/updateExtIP.sh",

	"Kitchen ceiling light AUTO at 4:30 every day" => "30 4 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData4=2, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=22\"",

	"Balcony curtain 0%-coverage at 16:00 every day" => "0 16 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData4=0, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=62\"",

	"Living room floor lamp ON at 17:20 every day" => "20 17 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData1=1, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=53\"",

	"Living room desk lamp ON at 17:21 every day" => "21 17 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData5=1, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=51\"",

	"Dinning room lights ON at 17:40 every day" => "40 17 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData1=1, fldData3=1, fldData4=1, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=41\"",

	"Dinning room lights OFF at 19:00 every day" => "00 19 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData1=0, fldData3=0, fldData4=0, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=41\"",

	"Kitchen ceiling light OFF at 19:30 every day" => "30 19 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData4=0, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=22\"",

	"Bath room big water heater ON at 21:10 every day" => "10 21 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData5=1, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=91\"",

	"Bath room big water heater OFF at 21:30 every day" => "30 21 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData5=0, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=91\""

	);

?>

<a href="index.php" data-role="button" data-icon="arrow-l">返回</a>
<a href="cron.php?installItems=Y" data-role="button" data-icon="refresh" data-ajax="false">Install</a>

<ul data-role="listview" data-inset="true">
	<?php
	
	$output = "";
	foreach($cronList as $key=>$value)
	{
		//$output = $output . '#' . $key . PHP_EOL;
		$output = $output . $value . PHP_EOL;
		?>
		<li data-role="fieldcontain">
			<?php echo $key; ?>
		</li>
		<?php
	}
	?>
</ul>
<?php

$installItems = $_GET['installItems'];
if(isset($installItems) and $installItems=='Y')
{
	// install cron items
	file_put_contents('/tmp/phpcrontab.txt', $output);
	echo exec('crontab -r');
	echo exec('crontab /tmp/phpcrontab.txt');
	//$output = shell_exec('crontab -l');
}


include "include/templateFooter.php";
?>