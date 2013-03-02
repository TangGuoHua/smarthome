<?php
$gPageTitle = "客厅";
include "../include/templateHeader.php";
include "../include/utils.php";
?>

<a href="../index.php" data-role="button" data-icon="arrow-l">返回</a>
<a href="living.php" data-role="button" data-icon="refresh" data-ajax="false">更新</a>

<ul data-role="listview" data-inset="true" >
	<li data-role="list-divider">实况</li>
	<?php
	$db = new DBHelper();
	$query = "select * from tabDataHistory where fldNodeID2=7 and fldData1=0 order by fldID desc limit 1";
	$results = $db->query($query);

	if ($row = $results->fetchArray()) 
	{  
		printf( "<li>室温 %d.%d度 <span class='ui-li-count'>%s</span></li>", $row["fldData4"], $row["fldData5"], $row["fldCreatedOn"]);
		printf( "<li>湿度 %d%% <span class='ui-li-count'>%s</span></li>", $row["fldData2"], $row["fldCreatedOn"]);
	}
	
	/*
	echo "<li data-role='list-divider'>最近15次活动</li>";
	$query = "select fldCreatedOn from tabDataHistory where fldNodeID2=7 and fldData1=5 and fldData2=6 and fldData3=7 order by fldID desc limit 15";
	$results = $db->query($query);

	while ($row = $results->fetchArray()) 
	{ 
		printf( "<li>有人 <span class='ui-li-count'>%s</span></li>", $row["fldCreatedOn"]);
	}
	*/
	
	$db->close();
	?>
</ul>
<?
include "../include/templateFooter.php";
?>