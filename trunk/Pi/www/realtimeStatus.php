<?php
/*
日期        作者    备注
------------------------------------------
2014-JAN-27 黄长浩  增加卫生间亮度

*/

$gPageTitle = "全家实况";
include "include/templateHeader.php";
include "include/utils.php";
?>

<a href="index.php" data-role="button" data-icon="arrow-l">返回</a>
<a href="realtimeStatus.php" data-role="button" data-icon="refresh" data-ajax="false">更新</a>

<ul data-role="listview" data-inset="true" >
<?php

$db = new DBHelper();


$query = "select * from tabDataRecved where fldNodeID=54 order by fldID desc limit 1";
$results = $db->query($query);
if ($row = $results->fetchArray()) 
{
	echo "<li data-role='list-divider'>客厅</li>";
	printf( "<li><a href='chart.php?param=54'>温度 %s%d.%d度 <span class='ui-li-count'>%s</span></a></li>", $row["fldData6"]==1?"-":"", $row["fldData4"], $row["fldData5"], $row["fldCreatedOn"]);
	printf( "<li><a href='chart.php?param=54'>湿度 %d.%d%% <span class='ui-li-count'>%s</span></a></li>", $row["fldData2"], $row["fldData3"], $row["fldCreatedOn"]);
}

$query = "select * from tabDataRecved where fldNodeID=22 order by fldID desc limit 1";
$results = $db->query($query);
if ($row = $results->fetchArray()) 
{
	echo "<li data-role='list-divider'>厨房</li>";
	printf( "<li><a href='chart.php?param=22'>亮度 %d <span class='ui-li-count'>%s</span></a></li>", $row["fldData2"], $row["fldCreatedOn"]);
	printf( "<li><a href='chart.php?param=22'>温度 %s%d.%d度 <span class='ui-li-count'>%s</span></a></li>", $row["fldData6"]==1?"-":"", $row["fldData7"], $row["fldData8"], $row["fldCreatedOn"]);
}

$query = "select * from tabDataRecved where fldNodeID=91 order by fldID desc limit 1";
$results = $db->query($query);
if ($row = $results->fetchArray()) 
{
	echo "<li data-role='list-divider'>卫生间</li>";
	printf( "<li><a href='chart.php?param=91'>亮度 %d <span class='ui-li-count'>%s</span></a></li>", $row["fldData2"], $row["fldCreatedOn"]);
	//printf( "<li><a href='chart.php?param=91'>温度 %s%d.%d度 <span class='ui-li-count'>%s</span></a></li>", $row["fldData6"]==1?"-":"", $row["fldData7"], $row["fldData8"], $row["fldCreatedOn"]);
}

$db->close();
?>
</ul>
<?php
include "include/templateFooter.php";
?>