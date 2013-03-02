<?php
$gPageTitle = "次卧";
include "../include/templateHeader.php";
include "../include/utils.php";
?>

<a href="../index.php" data-role="button" data-icon="arrow-l">返回</a>
<a href="masterBed.php" data-role="button" data-icon="refresh" data-ajax="false">更新</a>

<ul data-role="listview" data-inset="true" >
	<li data-role="list-divider">实况</li>
	<?php
	$db = new DBHelper();
	$query = "select * from tabDataHistory where fldNodeID2=1 order by fldID desc limit 1";
	$results = $db->query($query);

	if ($row = $results->fetchArray()) 
	{  
		printf( "<li>室温 %d.%d度 <span class='ui-li-count'>%s</span></li>", $row["fldData3"], $row["fldData4"], $row["fldCreatedOn"]);
		printf( "<li>湿度 %d%% <span class='ui-li-count'>%s</span></li>", $row["fldData1"], $row["fldCreatedOn"]);
	}

	$db->close();
	?>
</ul>
<?
include "../include/templateFooter.php";
?>