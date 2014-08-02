<?php
/*
日期        作者    备注
------------------------------------------
2014-MAR-30 黄长浩  初始版本
2014-JUN-29 黄长浩  增加客厅电视背景墙控制器
2014-JUL-01 黄长浩  去掉餐厅射灯（北）
2014-JUL-13 黄长浩  增加卫生间热水器
2014-JUL-15 黄长浩  增加餐厅灯带
2014-JUL-30 黄长浩  增加阳台卷帘
2014-JUL-31 黄长浩  增加书房调光台灯
2014-AUG-02 黄长浩  阳台卷帘覆盖度支持20%、40%等值
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

/*
// document ready之后就执行这里的一些函数
$(document).ready(function() {
	//隐藏设定按钮

	showButton( "btnSetStudyroomLamp", false );

});
*/


/***********各个区域控制***********/

function rdoLivingRoomTVWallControllerClicked(ind, val) //客厅电视墙控制器
{
	if( ind == 4 ) //台灯
	{
		$.post("/api/sendData.php", { nodeID: "51", data4: val } );
	}
	else if( ind == 5 ) //天猫盒子
	{
		$.post("/api/sendData.php", { nodeID: "51", data5: val } );
	}
	else if( ind == 8 ) //风扇
	{
		$.post("/api/sendData.php", { nodeID: "51", data8: val } );
	}
}

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


//书房调光台灯
function rdoStudyRoomDeskLampClicked(val)
{
	//alert(val);
	$.post("/api/sendData.php", { nodeID: "31", data1: val } );
}

/*
function btnSetStudyroomLampClicked()
{
	var setVal = document.getElementById('rangeStudyroomLamp').value;

	if( setVal>=0 && setVal <=255 )
	{
		$.post("/api/sendData.php", { nodeID: "31", data1:setVal } );
	}

	showButton( 'btnSetStudyroomLamp', false );
}
*/

//卫生间热水器
function rdoBathroomHeaterClicked( val )
{
	$.post("/api/sendData.php", { nodeID: "91", data5: val } );
}

//阳台卷帘
function rdoBalconyCurtainClicked( val )
{
	$.post("/api/sendData.php", { nodeID: "62", data4: val } );
}

</script>

<a href="index.php" data-role="button" data-icon="arrow-l">返回</a>
<a href="freqUsed.php" data-role="button" data-icon="refresh" data-ajax="false">更新</a>

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
				<label for="chkDinningLightNorth">灯带</label>
		    </fieldset>
		</li>
	<?php
	}
	elseif( $row["fldNodeID"]==51 )	// 客厅电视墙
	{
	?>
		<li data-role="fieldcontain">
		    <fieldset data-role="controlgroup" data-type="horizontal">
		    	<legend>客厅台灯</legend>
					<input type="radio" name="rdoLivingRoomDeskLamp" id="rdoLivingRoomDeskLamp0" value="0" onclick="rdoLivingRoomTVWallControllerClicked(4, '0');" <?php echo $row["fldData4"]==0?"checked":"";?> />
					<label for="rdoLivingRoomDeskLamp0">关</label>
					<input type="radio" name="rdoLivingRoomDeskLamp" id="rdoLivingRoomDeskLamp1" value="1" onclick="rdoLivingRoomTVWallControllerClicked(4, '1');" <?php echo $row["fldData4"]==1?"checked":"";?> />
					<label for="rdoLivingRoomDeskLamp1">开</label>
		    </fieldset>
		</li>
		<li data-role="fieldcontain">
		    <fieldset data-role="controlgroup" data-type="horizontal">
		    	<legend>客厅天猫盒子</legend>
					<input type="radio" name="rdoLivingRoomTMallBox" id="rdoLivingRoomTMallBox0" value="0" onclick="rdoLivingRoomTVWallControllerClicked(5, '0');" <?php echo $row["fldData5"]==0?"checked":"";?> />
					<label for="rdoLivingRoomTMallBox0">关</label>
					<input type="radio" name="rdoLivingRoomTMallBox" id="rdoLivingRoomTMallBox1" value="1" onclick="rdoLivingRoomTVWallControllerClicked(5, '1');" <?php echo $row["fldData5"]==1?"checked":"";?> />
					<label for="rdoLivingRoomTMallBox1">开</label>
		    </fieldset>
		</li>
		<li data-role="fieldcontain">
		    <fieldset data-role="controlgroup" data-type="horizontal">
		    	<legend>客厅风扇</legend>
					<input type="radio" name="rdoLivingRoomFan" id="rdoLivingRoomFan0" value="0" onclick="rdoLivingRoomTVWallControllerClicked(8, '0');" <?php echo $row["fldData8"]==0?"checked":"";?> />
					<label for="rdoLivingRoomFan0">关</label>
					<input type="radio" name="rdoLivingRoomFan" id="rdoLivingRoomFan1" value="1" onclick="rdoLivingRoomTVWallControllerClicked(8, '1');" <?php echo $row["fldData8"]==1?"checked":"";?> />
					<label for="rdoLivingRoomFan1">开</label>
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

	<?php
	}
	elseif( $row["fldNodeID"]==62 )	// 阳台卷帘
	{
	?>
		<li data-role="fieldcontain">
		    <fieldset data-role="controlgroup" data-type="horizontal">
		    	<legend>阳台卷帘覆盖度</legend>
					<input type="radio" name="rdoBalconyCurtain" id="rdoBalconyCurtain0" onclick="rdoBalconyCurtainClicked(0);" <?php echo $row["fldData4"]==0?"checked":"";?> />
					<label for="rdoBalconyCurtain0">全开</label>

					<input type="radio" name="rdoBalconyCurtain" id="rdoBalconyCurtain20" onclick="rdoBalconyCurtainClicked(20);" <?php echo $row["fldData4"]==20?"checked":"";?> />
					<label for="rdoBalconyCurtain20">20%</label>

					<input type="radio" name="rdoBalconyCurtain" id="rdoBalconyCurtain40" onclick="rdoBalconyCurtainClicked(40);" <?php echo $row["fldData4"]==40?"checked":"";?> />
					<label for="rdoBalconyCurtain40">40%</label>

					<input type="radio" name="rdoBalconyCurtain" id="rdoBalconyCurtain60" onclick="rdoBalconyCurtainClicked(60);" <?php echo $row["fldData4"]==60?"checked":"";?> />
					<label for="rdoBalconyCurtain60">60%</label>

					<input type="radio" name="rdoBalconyCurtain" id="rdoBalconyCurtain85" onclick="rdoBalconyCurtainClicked(85);" <?php echo $row["fldData4"]==85?"checked":"";?> />
					<label for="rdoBalconyCurtain85">85%</label>

					<input type="radio" name="rdoBalconyCurtain" id="rdoBalconyCurtain100" onclick="rdoBalconyCurtainClicked(100);" <?php echo $row["fldData4"]==100?"checked":"";?> />
					<label for="rdoBalconyCurtain100">全关</label>

		    </fieldset>
		</li>

	<?php
	}
	elseif( $row["fldNodeID"]==91 )	// 卫生间
	{
	?>
		<!--li data-role="list-divider">卫生间</li-->
		<li data-role="fieldcontain">
			<fieldset data-role="controlgroup" data-type="horizontal">
				<legend>卫生间热水器</legend>
					<input type="radio" name="rdoBathroomHeater" id="rdoBathroomHeater0" value="0" onclick="rdoBathroomHeaterClicked('0');" <?php echo $row["fldData5"]==0?"checked":"";?> />
					<label for="rdoBathroomHeater0">关</label>
					<input type="radio" name="rdoBathroomHeater" id="rdoBathroomHeater1" value="1" onclick="rdoBathroomHeaterClicked('1');" <?php echo $row["fldData5"]==1?"checked":"";?> />
					<label for="rdoBathroomHeater1">开</label>
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
	elseif( $row["fldNodeID"]==31)
	{
	?>
		<li data-role="fieldcontain">
			<fieldset data-role="controlgroup" data-type="horizontal">
				<legend>书房台灯</legend>
					<input type="radio" name="radioLEDGreen" id="radioLEDGreen0" value="0" onclick="rdoStudyRoomDeskLampClicked(0);" <?php echo $row["fldData1"]==0?"checked":"";?> />
					<label for="radioLEDGreen0">关</label>
					<input type="radio" name="radioLEDGreen" id="radioLEDGreen3" value="3" onclick="rdoStudyRoomDeskLampClicked(3);" <?php echo $row["fldData1"]==3?"checked":"";?> />
					<label for="radioLEDGreen3">弱</label>
					<input type="radio" name="radioLEDGreen" id="radioLEDGreen6" value="6" onclick="rdoStudyRoomDeskLampClicked(6);" <?php echo $row["fldData1"]==6?"checked":"";?> />
					<label for="radioLEDGreen6">中</label>
					<input type="radio" name="radioLEDGreen" id="radioLEDGreen10" value="10" onclick="rdoStudyRoomDeskLampClicked(10);" <?php echo $row["fldData1"]==10?"checked":"";?> />
					<label for="radioLEDGreen10">强</label>
					<input type="radio" name="radioLEDGreen" id="radioLEDGreen20" value="20" onclick="rdoStudyRoomDeskLampClicked(20);" <?php echo $row["fldData1"]==20?"checked":"";?> />
					<label for="radioLEDGreen20">延时关</label>
			</fieldset>
		</li>
		<!--li data-role="fieldcontain">
			<label for="rangeStudyroomLamp">书房台灯</label>
			<input type="range" name="rangeStudyroomLamp" id="rangeStudyroomLamp" onchange="showButton( 'btnSetStudyroomLamp', true );" value="<?php echo $row["fldData1"];?>" min="0" max="10" step="1" data-highlight="true" />
			<button id="btnSetStudyroomLamp" data-icon="check" onclick="btnSetStudyroomLampClicked();">设定</button>
		</li-->
	<?php
	}
}
$db->close();
?>


</ul>
<?php
include "include/templateFooter.php";
?>