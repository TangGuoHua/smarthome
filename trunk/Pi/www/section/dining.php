<?php
$gPageTitle = "餐厅";
include "../include/templateHeader.php";
include "../include/utils.php";
?>
<script>
	$(document).ready(function() {
		$('#flipDiningLight').change(function() {
			//alert( 'clicked value:'+ $(this).val() );
			if ($(this).val() == "on") 
			{
				$.post("/api/sendData.php", { nodeID1: "100", nodeID2: "3", data1: "1" } );
			}
			else 
			{
				$.post("/api/sendData.php", { nodeID1: "100", nodeID2: "3", data1: "0" } );
			}			
		});
	});
</script>

<a href="../index.php" data-role="button" data-icon="arrow-l">返回</a>
<a href="dining.php" data-role="button" data-icon="refresh" data-ajax="false">更新</a>

<ul data-role="listview" data-inset="true" >
<?php
$db = new DBHelper();
$sql = "select fldNodeID1, fldNodeID2, fldData1, fldUpdatedOn, fldUpdatedBy from tabDataToNode ";
$results = $db->query($sql);

while ($row = $results->fetchArray()) 
{
	// 客厅灯
	if( $row["fldNodeID1"]==100 and $row["fldNodeID2"]==3 )
	{
	?>
		<li data-role="list-divider">控制</li>
		<li data-role="fieldcontain">
			<label for="flipDiningLight">射灯</label>
			<select name="flipDiningLight" id="flipDiningLight" data-role="slider">
				<?php
				if( $row["fldData1"]==0 )
				{
					//当前灯是关的
					echo "<option value='off' selected>关</option><option value='on'>开</option>";
				}
				else
				{
					echo "<option value='off'>关</option><option value='on' selected>开</option>";
				}
				?>
			</select>
			<span class='ui-li-count'><?php echo $row["fldUpdatedOn"];?></span>
		</li>
		<?php
	}
}
?>
		<li data-role="list-divider">实况</li>
			<?php
			$query = "select * from tabDataHistory where fldNodeID2=5 order by fldID desc limit 1";
			$results = $db->query($query);

			if ($row = $results->fetchArray()) 
			{  
				printf( "<li>室温 %d.%d度 <span class='ui-li-count'>%s</span></li>", $row["fldData10"], $row["fldData11"], $row["fldCreatedOn"]);
				printf( "<li>湿度 %d%% <span class='ui-li-count'>%s</span></li>", $row["fldData8"], $row["fldCreatedOn"]);
			}
$db->close();
?>
</ul>
<?php
include "../include/templateFooter.php";
?>