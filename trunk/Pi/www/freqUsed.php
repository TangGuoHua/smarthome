<?php
/*
日期        作者    备注
------------------------------------------
2014-MAR-30 黄长浩  初始版本
*/

$gPageTitle = "常用";
include "include/templateHeader.php";
include "include/utils.php";
?>

<script>
/*********** 公用函数 ***********/

// Show or Hide a button
// btnID   : button ID, eg. <button id="btnTest">Test</button>
// visible : ture - show, false - hide
function showButton( btnID, visible )
{
	btnID = "#" + btnID;
	if( visible )
	{
		$(btnID).closest('.ui-btn').show();
	}
	else
	{
		$(btnID).closest('.ui-btn').hide();
	}
}


// document ready之后就执行这里的一些函数
$(document).ready(function() {
	//隐藏设定按钮
	showButton( "btnSetBathroomLightOnThreshold", false );
	showButton( "btnSetKitchenLight1OnThreshold", false );
	showButton( "btnSetKitchenLight2OnThreshold", false );
	showButton( "btnSetStudyroomLamp", false );
	showButton( "btnSetSouthBedroomTemp", false );
});


/***********各个区域控制***********/


//客厅东墙南头控制器  （落地灯、插座）
function rdoLivingRoomEastWallSouthControllerClicked(ind, val)
{
	if( ind == 1 ) //落地灯
	{
		$.post("/api/sendData.php", { nodeID: "53", data1: val } );
	}
	if( ind == 2 ) //插座
	{
		$.post("/api/sendData.php", { nodeID: "53", data2: val } );
	}
}

// 餐厅
function chkDinningRoomLightclicked( val, obj )
{
	//alert( val + ' obj.selected=' + obj.checked );

	//开，tmp=1；关，tmp=0
	var tmp;
	tmp = obj.checked?1:0;

	if( val == 'E' ) //East
	{
		$.post("/api/sendData.php", { nodeID: "41", data1: tmp } );
	}
	else if( val == 'S' )
	{
		$.post("/api/sendData.php", { nodeID: "41", data2: tmp } );
	}
	else if( val == 'W' )
	{
		$.post("/api/sendData.php", { nodeID: "41", data3: tmp } );
	}
	else if( val == 'N' )
	{
		$.post("/api/sendData.php", { nodeID: "41", data4: tmp } );
	}
}


//书房LED灯带
function rdoStudyLEDStripeClicked( val )
{
	$.post("/api/sendData.php", { nodeID: "200", data2: val } );
}



</script>

<a href="index.php" data-role="button" data-icon="arrow-l">返回</a>
<a href="control.php" data-role="button" data-icon="refresh" data-ajax="false">更新</a>

<ul data-role="listview" data-inset="true">
<?php
$db = new DBHelper();
$sql = "select * from tabDataToNode order by fldNodeID";
$results = $db->query($sql);

while ($row = $results->fetchArray()) 
{

	if( $row["fldNodeID"]==41 )	// 餐厅
	{
	?>
		<li data-role="fieldcontain">
		    <fieldset data-role="controlgroup" data-type="horizontal">
		    	<legend>餐厅顶射灯</legend>
		    	<input type="checkbox" name="chkDinningLightEast" id="chkDinningLightEast" onclick="chkDinningRoomLightclicked('E', this);" <?php echo $row["fldData1"]==1?"checked":"";?> />
				<label for="chkDinningLightEast">东</label>
				<input type="checkbox" name="chkDinningLightSouth" id="chkDinningLightSouth" onclick="chkDinningRoomLightclicked('S', this);" <?php echo $row["fldData2"]==1?"checked":"";?> />
				<label for="chkDinningLightSouth">南</label>
				<input type="checkbox" name="chkDinningLightWest" id="chkDinningLightWest" onclick="chkDinningRoomLightclicked('W', this);" <?php echo $row["fldData3"]==1?"checked":"";?> />
				<label for="chkDinningLightWest">西</label>
				<input type="checkbox" name="chkDinningLightNorth" id="chkDinningLightNorth" onclick="chkDinningRoomLightclicked('N', this);" <?php echo $row["fldData4"]==1?"checked":"";?> />
				<label for="chkDinningLightNorth">北</label>
		    </fieldset>
		</li>
	<?php
	}
	elseif( $row["fldNodeID"]==53 )	// 客厅东墙南头（落地灯）
	{
	?>
		<li data-role="fieldcontain">
		    <fieldset data-role="controlgroup" data-type="horizontal">
		    	<legend>客厅落地灯</legend>
					<input type="radio" name="rdoLivingRoomFloorLamp" id="rdoLivingRoomFloorLamp0" value="0" onclick="rdoLivingRoomEastWallSouthControllerClicked(1, '0');" <?php echo $row["fldData1"]==0?"checked":"";?> />
					<label for="rdoLivingRoomFloorLamp0">关</label>
					<input type="radio" name="rdoLivingRoomFloorLamp" id="rdoLivingRoomFloorLamp1" value="1" onclick="rdoLivingRoomEastWallSouthControllerClicked(1, '1');" <?php echo $row["fldData1"]==1?"checked":"";?> />
					<label for="rdoLivingRoomFloorLamp1">开</label>

		    </fieldset>
		</li>
		<li data-role="fieldcontain">
		    <fieldset data-role="controlgroup" data-type="horizontal">
		    	<legend>客厅落地灯旁插座</legend>
					<input type="radio" name="rdoLivingRoomSocketNext2FloorLamp" id="rdoLivingRoomSocketNext2FloorLamp0" value="0" onclick="rdoLivingRoomEastWallSouthControllerClicked(2, '0');" <?php echo $row["fldData2"]==0?"checked":"";?> />
					<label for="rdoLivingRoomSocketNext2FloorLamp0">关</label>
					<input type="radio" name="rdoLivingRoomSocketNext2FloorLamp" id="rdoLivingRoomSocketNext2FloorLamp1" value="1" onclick="rdoLivingRoomEastWallSouthControllerClicked(2, '1');" <?php echo $row["fldData2"]==1?"checked":"";?> />
					<label for="rdoLivingRoomSocketNext2FloorLamp1">开</label>

		    </fieldset>
		</li>
	<?php
	}
	elseif( $row["fldNodeID"]==200 )//书房
	{
	?>
		<!--li data-role="list-divider">书房</li-->
		<li data-role="fieldcontain">
			<fieldset data-role="controlgroup" data-type="horizontal">
				<legend>书房灯带</legend>
					<input type="radio" name="rdoStudyLEDStripe" id="rdoStudyLEDStripe_0" value="0" onclick="rdoStudyLEDStripeClicked('0');" <?php echo $row["fldData2"]==0?"checked":"";?> />
					<label for="rdoStudyLEDStripe_0">关</label>
					<input type="radio" name="rdoStudyLEDStripe" id="rdoStudyLEDStripe_1" value="1" onclick="rdoStudyLEDStripeClicked('1');" <?php echo $row["fldData2"]==1?"checked":"";?> />
					<label for="rdoStudyLEDStripe_1">开</label>

			</fieldset>
		</li>
	<?
	}
}
$db->close();
?>


</ul>
<?php
include "include/templateFooter.php";
?>