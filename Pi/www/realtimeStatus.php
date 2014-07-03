<?php
/*
日期        作者    备注
------------------------------------------
2014-JAN-27 黄长浩  增加卫生间亮度
2014-FEB-05 黄长浩  增加卫生间温度
                    增加南卧温湿度
2014-JUL-03 黄长浩  增加厨房、卫生间下的更多参数，如有人？热水器状态等
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

	printf( "<li>有人？[%s] <span class='ui-li-count'>%s</span></a></li>", $row["fldData1"]==1?"是":"否", $row["fldCreatedOn"]);
	printf( "<li>柜灯 [%s] <span class='ui-li-count'>%s</span></a></li>", $row["fldData5"]==1?"亮":"灭", $row["fldCreatedOn"]);
	printf( "<li>顶灯 [%s] <span class='ui-li-count'>%s</span></a></li>", $row["fldData6"]==1?"亮":"灭", $row["fldCreatedOn"]);
	printf( "<li>柜灯阈值 [%d] <span class='ui-li-count'>%s</span></a></li>", $row["fldData3"], $row["fldCreatedOn"]);
	printf( "<li>顶灯阈值 [%d] <span class='ui-li-count'>%s</span></a></li>", $row["fldData4"], $row["fldCreatedOn"]);
}

$query = "select * from tabDataRecved where fldNodeID=82 order by fldID desc limit 1";
$results = $db->query($query);
if ($row = $results->fetchArray()) 
{
	echo "<li data-role='list-divider'>南卧</li>";
	printf( "<li><a href='chart.php?param=82'>温度 %s%d.%d度 <span class='ui-li-count'>%s</span></a></li>", $row["fldData6"]==1?"-":"", $row["fldData4"], $row["fldData5"], $row["fldCreatedOn"]);
	printf( "<li><a href='chart.php?param=82'>湿度 %d.%d%% <span class='ui-li-count'>%s</span></a></li>", $row["fldData2"], $row["fldData3"], $row["fldCreatedOn"]);
}


$query = "select * from tabDataRecved where fldNodeID=91 order by fldID desc limit 1";
$results = $db->query($query);
if ($row = $results->fetchArray()) 
{
	echo "<li data-role='list-divider'>卫生间</li>";
	printf( "<li><a href='chart.php?param=91'>亮度 %d <span class='ui-li-count'>%s</span></a></li>", $row["fldData2"], $row["fldCreatedOn"]);
	printf( "<li>开灯阈值 %d <span class='ui-li-count'>%s</span></li>", $row["fldData3"], $row["fldCreatedOn"]);
	printf( "<li>有人？[%s] <span class='ui-li-count'>%s</span></a></li>", $row["fldData1"]==1?"是":"否", $row["fldCreatedOn"]);
	printf( "<li>热水器 [%s] <span class='ui-li-count'>%s</span></a></li>", $row["fldData6"]==1?"开":"关", $row["fldCreatedOn"]);
}

$query = "select * from tabDataRecved where fldNodeID=92 order by fldID desc limit 1";
$results = $db->query($query);
if ($row = $results->fetchArray()) 
{
	printf( "<li>小厨宝 [%s] <span class='ui-li-count'>%s</span></a></li>", $row["fldData1"]==1?"开":"关", $row["fldCreatedOn"]);
	printf( "<li><a href='chart.php?param=92'>温度 %s%d.%d度 <span class='ui-li-count'>%s</span></a></li>", $row["fldData2"]==1?"-":"", $row["fldData3"], $row["fldData4"], $row["fldCreatedOn"]);
}

$db->close();
?>
</ul>
<?php
include "include/templateFooter.php";
?>