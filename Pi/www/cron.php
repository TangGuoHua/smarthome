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

	"每20分钟更新IP地址" => "*/20 * * * * /home/pi/smartHome/script/updateExtIP.sh",

	//"每天2:00电子钟校时" => "0 2 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData1=strftime('%H','now','localtime'),fldData2=strftime('%M','now','localtime'),fldData3=strftime('%S','now','localtime'),fldUpdatedOn=datetime('now','localtime'),fldUpdatedBy='cron' where fldNodeID=203\"",

	"每周一、四2:00电子钟校时" => "0 2 * * 1,4 /home/pi/smartHome/script/adjClock.sh",

	"每天4:30厨房顶灯设为自动模式" => "30 4 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData4=2, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=22\"",
	
	//"每天5:35阳台窗帘全开" => "35 5 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData1=1, fldData2=20, fldData4=0, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=62\"",

	//"每周六、日7:30打开热水器" => "30 7 * * 0,6 sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData6=1, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=91\"",

	//"每周六、日7:50关闭热水器" => "50 7 * * 0,6 sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData6=0, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=91\"",

	//"每天15:00阳台窗帘全开" => "0 15 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData1=1, fldData2=20, fldData4=0, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=62\"",

	//"每天17:35打开客厅落地灯、台灯" => "35 17 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData3=1, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=53; update tabDataToNode set fldData5=1, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=51\"",

	//"每天19:30阳台窗帘全关" => "30 19 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData1=1, fldData2=20, fldData4=100, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=62\"",

	//"每天18:30打开餐厅灯" => "30 18 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData1=1, fldData3=1, fldData4=0, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=41\"",

	//"每天22:30关闭餐厅灯" => "30 22 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData1=0, fldData3=0, fldData4=0, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=41\"",

	"每天20:00关闭厨房顶灯" => "00 20 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData4=0, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=22\"",

	"每天21:10打开热水器" => "10 21 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData6=1, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=91\"",

	"每天21:25关闭热水器" => "25 21 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData6=0, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=91\"",

	//"每天22:00阳台窗帘全关" => "0 22 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData1=1, fldData2=20, fldData4=100, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=62\"",
	
	//"每天22:10打开南卧加热器" => "10 22 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData1=2, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=82\"",

	//"每天21:35打开南卧射灯" => "35 21 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData3=1, fldData4=0, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=83 and (fldData3=0 and fldData4=0)\"",

	//"每天23:00关闭南卧加热器" => "0 23 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData1=0, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=82\"",

	//"每天23:20关闭南卧射灯" => "20 23 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData3=0, fldData4=0, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=83 and (fldData3=1 or fldData4=1)\""

	);

?>

<a href="index.php" data-role="button" data-icon="arrow-l">返回</a>


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

if( isset($_GET['installItems']) )
{
	if( $_GET['installItems']=='Y' )
	{
		// install cron items
		file_put_contents('/tmp/phpcrontab.txt', $output);
		echo exec('crontab -r');
		echo exec('crontab /tmp/phpcrontab.txt');
		//$output = shell_exec('crontab -l');
	}
}
?>

<a href="cron.php?installItems=Y" data-role="button" data-icon="refresh" data-ajax="false">Install</a>

<?php
include "include/templateFooter.php";
?>
