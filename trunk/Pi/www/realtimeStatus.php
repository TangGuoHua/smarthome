<?php
$gPageTitle = "全家实况";
include "include/templateHeader.php";
include "include/utils.php";
?>

<a href="index.php" data-role="button" data-icon="arrow-l">返回</a>
<a href="realtimeStatus.php" data-role="button" data-icon="refresh" data-ajax="false">更新</a>

<ul data-role="listview" data-inset="true" >
<?php

$db = new DBHelper();

$query = "select * from tabDataHistory where fldNodeID2=7 and fldData1=0 order by fldID desc limit 1";
$results = $db->query($query);
if ($row = $results->fetchArray()) 
{
	echo "<li data-role='list-divider'>客厅</li>";
	printf( "<li><a href='chart.php?param=7-1'>温度 %d.%d度 <span class='ui-li-count'>%s</span></a></li>", $row["fldData4"], $row["fldData5"], $row["fldCreatedOn"]);
	printf( "<li><a href='chart.php?param=7-1'>湿度 %d%% <span class='ui-li-count'>%s</span></a></li>", $row["fldData2"], $row["fldCreatedOn"]);
	printf( "<li><a href='chart.php?param=7-2'>耗电 %.1f度 <span class='ui-li-count'>%s</span></a></li>", ($row["fldData7"]*16777216 + $row["fldData8"]* 65536+ $row["fldData9"]*256 + $row["fldData10"])/1600, $row["fldCreatedOn"]);
}

$query = "select * from tabDataHistory where fldNodeID2=15 order by fldID desc limit 1";
$results = $db->query($query);
if ($row = $results->fetchArray()) 
{  
	echo "<li data-role='list-divider'>主卧</li>";
	printf( "<li><a href='chart.php?param=15'>温度 %d.%d度 <span class='ui-li-count'>%s</span></a></li>", $row["fldData3"], $row["fldData4"], $row["fldCreatedOn"]);
	printf( "<li><a href='chart.php?param=15'>亮度 %d <span class='ui-li-count'>%s</span></a></li>", $row["fldData1"], $row["fldCreatedOn"]);
}


$query = "select * from tabDataHistory where fldNodeID2=18 order by fldID desc limit 1";
$results = $db->query($query);
if ($row = $results->fetchArray()) 
{  
	echo "<li data-role='list-divider'>次卧</li>";
	printf( "<li><a href='chart.php?param=18'>温度 %d.%d度 <span class='ui-li-count'>%s</span></a></li>", $row["fldData6"], $row["fldData7"], $row["fldCreatedOn"]);
	printf( "<li><a href='chart.php?param=18'>湿度 %d%% <span class='ui-li-count'>%s</span></a></li>", $row["fldData4"], $row["fldCreatedOn"]);
	printf( "<li><a href='dataHistory.php?param=18-1'>床头灯 %s <span class='ui-li-count'>%s</span></a></li>", $row["fldData1"]==0?"关":"开", $row["fldCreatedOn"]);
	printf( "<li><a href='dataHistory.php?param=18-2'>加热器 %s <span class='ui-li-count'>%s</span></a></li>", $row["fldData2"]==0?"关":"开", $row["fldCreatedOn"]);
}


$query = "select * from tabDataHistory where fldNodeID2=5 order by fldID desc limit 1";
$results = $db->query($query);
if ($row = $results->fetchArray()) 
{ 
	echo "<li data-role='list-divider'>室外</li>";
	printf( "<li><a href='chart.php?param=5-2'>温度 %s%d.%d度 <span class='ui-li-count'>%s</span></a></li>", $row["fldData7"]==1?"-":"", $row["fldData5"], $row["fldData6"], $row["fldCreatedOn"]);
	printf( "<li><a href='chart.php?param=5-2'>亮度 %d <span class='ui-li-count'>%s</span></a></li>", $row["fldData1"], $row["fldCreatedOn"]);

	echo "<li data-role='list-divider'>餐厅</li>";
	printf( "<li><a href='chart.php?param=5-1'>温度 %d.%d度 <span class='ui-li-count'>%s</span></a></li>", $row["fldData10"], $row["fldData11"], $row["fldCreatedOn"]);
	printf( "<li><a href='chart.php?param=5-1'>湿度 %d%% <span class='ui-li-count'>%s</span></a></li>", $row["fldData8"], $row["fldCreatedOn"]);

	//PIR红外探头
	$query = "select * from tabDataHistory where fldNodeID2=3 order by fldID desc limit 1";
	$results = $db->query($query);
	if ($row = $results->fetchArray()) 
	{
		printf( "<li><a href='dataHistory.php?param=3'>%s <span class='ui-li-count'>%s</span></a></li>", $row["fldData1"]==1?"有人":"无人", $row["fldCreatedOn"]);	
	}
}

$query = "select * from tabDataHistory where fldNodeID2=11 order by fldID desc limit 1";
$results = $db->query($query);
if ($row = $results->fetchArray()) 
{ 
	echo "<li data-role='list-divider'>书房</li>";
	printf( "<li><a href='chart.php?param=11-1'>温度 %.1f度 <span class='ui-li-count'>%s</span></a></li>", ($row["fldData3"]*256+$row["fldData4"])/10, $row["fldCreatedOn"]);
	printf( "<li><a href='chart.php?param=11-1'>亮度 %dLux <span class='ui-li-count'>%s</span></a></li>", $row["fldData11"]*256+$row["fldData12"], $row["fldCreatedOn"]);	
	printf( "<li><a href='chart.php?param=11-2'>气压 %d帕 <span class='ui-li-count'>%s</span></a></li>", $row["fldData6"]*65536+$row["fldData7"]*256+$row["fldData8"], $row["fldCreatedOn"]);
	
	//PIR红外探头
	$query = "select * from tabDataHistory where fldNodeID2=111 order by fldID desc limit 1";
	$results = $db->query($query);
	if ($row = $results->fetchArray()) 
	{
		printf( "<li><a href='dataHistory.php?param=111'>%s <span class='ui-li-count'>%s</span></a></li>", $row["fldData1"]==1?"有人":"无人", $row["fldCreatedOn"]);	
	}
	
}

$query = "select * from tabDataHistory where fldNodeID2=55 order by fldID desc limit 4";
$results = $db->query($query);
echo "<li data-role='list-divider'>大门最近4个动作</li>";
while ($row = $results->fetchArray()) 
{
	printf( "<li><a href='dataHistory.php?param=55'>%s <span class='ui-li-count'>%s</span></a></li>", ($row["fldData1"]==1)?"开":"关", $row["fldCreatedOn"]);
} 

$db->close();
?>
</ul>
<?php
include "include/templateFooter.php";
?>