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

	"每天4:30厨房顶灯设为自动模式" => "30 4 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData4=2, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=22\"",
	
	"每天4:35阳台窗帘全开" => "35 4 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData1=1, fldData2=20, fldData4=0, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=62\"",

	"每周六、日7:30打开热水器" => "30 7 * * 0,6 sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData5=1, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=91\"",

	"每周六、日7:50关闭热水器" => "50 7 * * 0,6 sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData5=0, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=91\"",

	//"每天15:00阳台窗帘全开" => "0 15 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData1=1, fldData2=20, fldData4=0, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=62\"",

	"每天16:45打开客厅落地灯、台灯" => "45 16 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData3=1, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=53; update tabDataToNode set fldData5=1, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=51\"",

	//"每天17:40打开餐厅灯" => "40 17 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData1=1, fldData3=1, fldData4=1, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=41\"",

	//"每天19:00关闭餐厅灯" => "00 19 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData1=0, fldData3=0, fldData4=0, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=41\"",

	"每天20:30关闭厨房顶灯" => "30 20 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData4=0, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=22\"",

	"每天21:50打开热水器" => "50 21 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData5=1, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=91\"",

	"每天22:30关闭热水器" => "30 22 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData5=0, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=91\"",

	"每天22:35阳台窗帘全关" => "35 22 * * * sqlite3 /var/www/db/smarthome.sqlite3 \"update tabDataToNode set fldData1=1, fldData2=20, fldData4=100, fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='cron' where fldNodeID=62\""

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
