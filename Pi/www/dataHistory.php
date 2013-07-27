<?php
$gPageTitle = "历史数据";
include "include/templateHeader.php";
include "include/utils.php";

if( isset($_REQUEST["param"]) )
{
	$param=$_REQUEST["param"];
}
else
{
	$param="1";
}
?>
<a href="realtimeStatus.php" data-role="button" data-icon="arrow-l">返回</a>
<a href="dataHistory.php?param=<?php echo $param;?>" data-role="button" data-icon="refresh" data-ajax="false">更新</a>

<ul data-role="listview" data-inset="true">
	
<?php
switch($param)
{
	case "3": // 餐厅PIR
		$query = "select CASE WHEN fldData1 = 0 THEN '无人' WHEN fldData1 = 1 THEN '有人' ELSE 'UNKNOWN' END as s1, fldCreatedOn from tabDataHistory where fldNodeID2=3 order by fldID desc limit 20";
		echo "<li data-role='list-divider'>餐厅有无人最近20条记录</li>";
		break;
	case "18-1": // 次卧床头灯
		$query = "select CASE WHEN fldData1 = 0 THEN '关' WHEN fldData1 = 1 THEN '开' ELSE 'UNKNOWN' END as s1, fldCreatedOn from tabDataHistory where fldNodeID2=18 order by fldID desc limit 20";
		echo "<li data-role='list-divider'>次卧床头灯状态最近20次更新</li>";
		break;
	case "18-2": // 次卧加热器
		$query = "select CASE WHEN fldData2 = 0 THEN '关' WHEN fldData2 = 1 THEN '开' ELSE 'UNKNOWN' END as s1, fldCreatedOn from tabDataHistory where fldNodeID2=18 order by fldID desc limit 20";
		echo "<li data-role='list-divider'>次卧加热器状态最近20次更新</li>";
		break;
	case "55": // 大门
		$query = "select CASE WHEN fldData1 = 0 THEN '关' WHEN fldData1 = 1 THEN '开' ELSE 'UNKNOWN' END as s1, fldCreatedOn from tabDataHistory where fldNodeID2=55 order by fldID desc limit 20";
		echo "<li data-role='list-divider'>大门最近20个动作</li>";
		break;
	case "111": // 书房
		$query = "select CASE WHEN fldData1 = 0 THEN '无人' WHEN fldData1 = 1 THEN '有人' ELSE 'UNKNOWN' END as s1, fldCreatedOn from tabDataHistory where fldNodeID2=111 order by fldID desc limit 20";
		echo "<li data-role='list-divider'>书房有无人最近20条记录</li>";
		break;		
}

$db = new DBHelper();
$results = $db->query($query);
while ($row = $results->fetchArray()) 
{  
	printf( "<li>%s <span class='ui-li-count'>%s</span></li>", $row["s1"], $row["fldCreatedOn"] );
} 
$db->close();
?>

<?php
include "include/templateFooter.php";
?>