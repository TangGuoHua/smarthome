<?php
$gPageTitle = "书房";
include "../include/templateHeader.php";
include "../include/utils.php";
?>

<a href="../index.php" data-role="button" data-icon="arrow-l">返回</a>
<a href="study.php" data-role="button" data-icon="refresh" data-ajax="false">更新</a>

<ul data-role="listview" data-inset="true" >
	<li data-role="list-divider">实况</li>
	<?php
	$db = new DBHelper();
	$query = "select * from tabDataHistory where fldNodeID2=11 order by fldID desc limit 1";
	$results = $db->query($query);

	if ($row = $results->fetchArray()) 
	{  
		printf( "<li>室温 %.1f度 <span class='ui-li-count'>%s</span></li>", ($row["fldData3"]*256+$row["fldData4"])/10, $row["fldCreatedOn"]);
		printf( "<li>亮度 %dLux <span class='ui-li-count'>%s</span></li>", $row["fldData11"]*256+$row["fldData12"], $row["fldCreatedOn"]);
		printf( "<li>气压 %d帕 <span class='ui-li-count'>%s</span></li>", $row["fldData6"]*65536+$row["fldData7"]*256+$row["fldData8"], $row["fldCreatedOn"]);
	}
	$db->close();
	?>
</ul>
<?php
include "../include/templateFooter.php";
?>