<?php
/*
日期        作者    备注
------------------------------------------
2014-JAN-27 黄长浩  增加卫生间亮度
2014-FEB-05 黄长浩  增加卫生间温度
                    增加南卧温湿度
2014-JUL-03 黄长浩  增加厨房、卫生间下的更多参数，如有人？热水器状态等
2014-AUG-30 黄长浩  增加书房温度气压亮度
2014-SEP-08 黄长浩  增加客厅南卧露点
2014-OCT-03 黄长浩  卫生间小厨宝增加模式、延时分钟数等信息
2014-OCT-04 黄长浩  增加电视墙用电器显示
2014-OCT-12 黄长浩  电视改接常闭触点
                    修改客厅温湿度节点
2014-OCT-19 黄长浩  增加室外温度亮度（阳台）
2014-NOV-30 黄长浩  增加客厅取暖器状态
                    增加南卧油汀状态
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


$query = "select * from tabDataRecved where fldNodeID=53 order by fldID desc limit 1";
$results = $db->query($query);
if ($row = $results->fetchArray()) 
{
	//计算露点
	$Tn = 243.12;
	$m =17.62;
	$T=floatval($row["fldData5"])+floatval($row["fldData6"])*0.1;
	$RH= floatval($row["fldData3"])+floatval($row["fldData4"])*0.1;
	$tDew=$Tn*(log($RH/100)+$m*$T/($Tn+$T))/($m-log($RH/100)-$m*$T/($Tn+$T));

	echo "<li data-role='list-divider'>客厅</li>";
	printf( "<li><a href='chart.php?param=53'>温度 %s%.1f度 <span class='ui-li-count'>%s</span></a></li>", $row["fldData7"]==1?"-":"", $T, $row["fldCreatedOn"]);
	printf( "<li><a href='chart.php?param=53'>湿度 %.1f%% <span class='ui-li-count'>%s</span></a></li>", $RH, $row["fldCreatedOn"]);
	printf( "<li>露点 %.1f度 <span class='ui-li-count'>%s</span>", $tDew, $row["fldCreatedOn"]);
	printf( "<li>落地灯 [%s] <span class='ui-li-count'>%s</span></a></li>", $row["fldData8"]==1?"亮":"灭", $row["fldCreatedOn"]);
	printf( "<li>取暖器模式 [%d] <span class='ui-li-count'>%s</span></a></li>", $row["fldData9"], $row["fldCreatedOn"]);
	printf( "<li>取暖器 [%s] <span class='ui-li-count'>%s</span></a></li>", $row["fldData11"]==1?"通电":"断电", $row["fldCreatedOn"]);
	printf( "<li>设定温度 %.1f度 <span class='ui-li-count'>%s</span></a></li>", $row["fldData10"]/10, $row["fldCreatedOn"]);
	//printf( "<li>取暖器继电器2[%s] <span class='ui-li-count'>%s</span></a></li>", $row["fldData12"]==1?"通电":"断电", $row["fldCreatedOn"]);
}


$query = "select * from tabDataRecved where fldNodeID=51 order by fldID desc limit 1";
$results = $db->query($query);
if ($row = $results->fetchArray()) 
{
	echo( "<li>用电器 [收音机" );
	if( $row["fldData4"] == 0 ) echo( "|电视" ); //电视接常闭触点，0开，1关
	if( $row["fldData5"] == 1 ) echo( "|台灯" );
	if( $row["fldData2"] == 1 ) echo( "|白机顶盒" );
	if( $row["fldData6"] == 1 ) echo( "|天猫盒子" );
	if( $row["fldData7"] == 1 ) echo( "|功放" );
	if( $row["fldData8"] == 1 ) echo( "|熔岩灯" );
	if( $row["fldData9"] == 1 ) echo( "|风扇" );
	printf( "] <span class='ui-li-count'>%s</span></li>", $row["fldCreatedOn"]);
}


$query = "select * from tabDataRecved where fldNodeID=31 and fldData1=11 order by fldID desc limit 1";
$results = $db->query($query);
if ($row = $results->fetchArray()) 
{ 
	echo "<li data-role='list-divider'>书房</li>";
	printf( "<li><a href='chart.php?param=31-1'>温度 %.1f度 <span class='ui-li-count'>%s</span></a></li>", ($row["fldData4"]*256+$row["fldData5"])/10, $row["fldCreatedOn"]);
	printf( "<li><a href='chart.php?param=31-1'>气压 %.1f hPa <span class='ui-li-count'>%s</span></a></li>", ($row["fldData7"]*65536+$row["fldData8"]*256+$row["fldData9"])/100, $row["fldCreatedOn"]);
	printf( "<li><a href='chart.php?param=31-2'>亮度 %d Lux <span class='ui-li-count'>%s</span></a></li>", $row["fldData12"]*256+$row["fldData13"], $row["fldCreatedOn"]);	
	printf( "<li>有人？[%s] <span class='ui-li-count'>%s</span></a></li>", $row["fldData14"]==1?"是":"否", $row["fldCreatedOn"]);	
	
}

$query = "select * from tabDataRecved where fldNodeID=82 order by fldID desc limit 1";
$results = $db->query($query);
if ($row = $results->fetchArray()) 
{
	//计算露点
	$Tn = 243.12;
	$m =17.62;
	$T=floatval($row["fldData4"])+floatval($row["fldData5"])*0.1;
	$RH= floatval($row["fldData2"])+floatval($row["fldData3"])*0.1;
	$tDew=$Tn*(log($RH/100)+$m*$T/($Tn+$T))/($m-log($RH/100)-$m*$T/($Tn+$T));

	echo "<li data-role='list-divider'>南卧</li>";
	printf( "<li><a href='chart.php?param=82'>温度 %s%.1f度 <span class='ui-li-count'>%s</span></a></li>", $row["fldData6"]==1?"-":"", $T, $row["fldCreatedOn"]);
	printf( "<li><a href='chart.php?param=82'>湿度 %.1f%% <span class='ui-li-count'>%s</span></a></li>", $RH, $row["fldCreatedOn"]);
	printf( "<li>露点 %.1f度 <span class='ui-li-count'>%s</span></li>", $tDew, $row["fldCreatedOn"]);
	printf( "<li>油汀模式 [%d] <span class='ui-li-count'>%s</span></a></li>", $row["fldData8"], $row["fldCreatedOn"]);
	printf( "<li>油汀 [%s] <span class='ui-li-count'>%s</span></a></li>", $row["fldData7"]==1?"通电":"断电", $row["fldCreatedOn"]);
	printf( "<li>设定温度 %.1f度 <span class='ui-li-count'>%s</span></a></li>", $row["fldData9"]/10, $row["fldCreatedOn"]);

}

$query = "select * from tabDataRecved where fldNodeID=83 order by fldID desc limit 1";
$results = $db->query($query);
if ($row = $results->fetchArray()) 
{
	echo( "<li>射灯 左[" );
	if( $row["fldData3"] == 1 ) echo( "亮" ); else echo( "灭" );

	echo( "], 右[" );
	if( $row["fldData2"] == 1 ) echo( "亮" ); else echo( "灭" );
	printf( "] <span class='ui-li-count'>%s</span></li>", $row["fldCreatedOn"]);
}


$query = "select * from tabDataRecved where fldNodeID=22 order by fldID desc limit 1";
$results = $db->query($query);
if ($row = $results->fetchArray()) 
{
	echo "<li data-role='list-divider'>厨房</li>";
	printf( "<li><a href='chart.php?param=22'>亮度 %d <span class='ui-li-count'>%s</span></a></li>", $row["fldData3"], $row["fldCreatedOn"]);
	printf( "<li><a href='chart.php?param=22'>温度 %s%d.%d度 <span class='ui-li-count'>%s</span></a></li>", $row["fldData10"]==1?"-":"", $row["fldData11"], $row["fldData12"], $row["fldCreatedOn"]);

	printf( "<li>有人？[%s] <span class='ui-li-count'>%s</span></a></li>", $row["fldData2"]==1?"是":"否", $row["fldCreatedOn"]);
	printf( "<li>柜灯 [%s] <span class='ui-li-count'>%s</span></a></li>", $row["fldData8"]==1?"亮":"灭", $row["fldCreatedOn"]);
	printf( "<li>顶灯 [%s] <span class='ui-li-count'>%s</span></a></li>", $row["fldData9"]==1?"亮":"灭", $row["fldCreatedOn"]);
	printf( "<li>柜灯阈值 [%d] <span class='ui-li-count'>%s</span></a></li>", $row["fldData4"], $row["fldCreatedOn"]);
	printf( "<li>顶灯阈值 [%d] <span class='ui-li-count'>%s</span></a></li>", $row["fldData5"], $row["fldCreatedOn"]);
	printf( "<li>柜灯模式 [%d] <span class='ui-li-count'>%s</span></a></li>", $row["fldData6"], $row["fldCreatedOn"]);
	printf( "<li>顶灯模式 [%d] <span class='ui-li-count'>%s</span></a></li>", $row["fldData7"], $row["fldCreatedOn"]);
}


$query = "select * from tabDataRecved where fldNodeID=91 order by fldID desc limit 1";
$results = $db->query($query);
if ($row = $results->fetchArray()) 
{
	echo "<li data-role='list-divider'>卫生间</li>";
	printf( "<li><a href='chart.php?param=91'>亮度 %d <span class='ui-li-count'>%s</span></a></li>", $row["fldData3"], $row["fldCreatedOn"]);
	printf( "<li>开灯阈值 %d <span class='ui-li-count'>%s</span></li>", $row["fldData4"], $row["fldCreatedOn"]);
	printf( "<li>灯 [%s] <span class='ui-li-count'>%s</span></a></li>", $row["fldData5"]==1?"亮":"灭", $row["fldCreatedOn"]);
	printf( "<li>有人？[%s] <span class='ui-li-count'>%s</span></a></li>", $row["fldData2"]==1?"是":"否", $row["fldCreatedOn"]);
	printf( "<li>热水器 [%s] <span class='ui-li-count'>%s</span></a></li>", $row["fldData7"]==1?"开":"关", $row["fldCreatedOn"]);
}

$query = "select * from tabDataRecved where fldNodeID=92 order by fldID desc limit 1";
$results = $db->query($query);
if ($row = $results->fetchArray()) 
{
	printf( "<li>小厨宝模式 [%d] <span class='ui-li-count'>%s</span></a></li>", $row["fldData5"], $row["fldCreatedOn"]);
	printf( "<li>小厨宝延时 [%d分钟] <span class='ui-li-count'>%s</span></a></li>", $row["fldData6"], $row["fldCreatedOn"]);
	printf( "<li>小厨宝 [%s] <span class='ui-li-count'>%s</span></a></li>", $row["fldData7"]==1?"开":"关", $row["fldCreatedOn"]);
	printf( "<li><a href='chart.php?param=92'>温度 %s%d.%d度 <span class='ui-li-count'>%s</span></a></li>", $row["fldData2"]==1?"-":"", $row["fldData3"], $row["fldData4"], $row["fldCreatedOn"]);
}


$query = "select * from tabDataRecved where fldNodeID=62 order by fldID desc limit 1";
$results = $db->query($query);
if ($row = $results->fetchArray()) 
{
	echo "<li data-role='list-divider'>阳台</li>";
	printf( "<li>卷帘覆盖度 %d%% <span class='ui-li-count'>%s</span></a></li>", $row["fldData3"], $row["fldCreatedOn"]);
	printf( "<li>卷帘模式 [%d] <span class='ui-li-count'>%s</span></a></li>", $row["fldData2"], $row["fldCreatedOn"]);
	printf( "<li>卷帘电机状态 [%s] <span class='ui-li-count'>%s</span></a></li>", $row["fldData4"]==1?"运行":"停止", $row["fldCreatedOn"]);

	echo "<li data-role='list-divider'>室外</li>";
	printf( "<li><a href='chart.php?param=62'>温度 %s%d.%d度 <span class='ui-li-count'>%s</span></a></li>", $row["fldData8"]==1?"-":"", $row["fldData9"], $row["fldData10"], $row["fldCreatedOn"]);
	printf( "<li><a href='chart.php?param=62'>亮度 %dLux <span class='ui-li-count'>%s</span></a></li>", $row["fldData6"]*256+$row["fldData7"], $row["fldCreatedOn"]);
}


$db->close();
?>
</ul>
<?php
include "include/templateFooter.php";
?>