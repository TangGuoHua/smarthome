<?php
$gPageTitle = "玄关";
include "../include/templateHeader.php";
include "../include/utils.php";
?>

<a href="../index.php" data-role="button" data-icon="arrow-l">返回</a>
<a href="./entrance.php" data-role="button" data-icon="refresh" data-ajax="false">更新</a>

<ul data-role="listview" data-inset="true">
	<li data-role="list-divider">大门最近20个动作</li>

	<?php
	$db = new DBHelper();
	$query = "select * from tabDataHistory where fldNodeID2=55 order by fldID desc limit 20";
	$results = $db->query($query);
	while ($row = $results->fetchArray()) 
	{ 
		printf( "<li>%s <span class='ui-li-count'>%s</span></li>", ($row["fldData1"]==1)?"开":"关", $row["fldCreatedOn"]);
	} 
	$db->close();
	?>
</ul>
<?php
include "../include/templateFooter.php";
?>