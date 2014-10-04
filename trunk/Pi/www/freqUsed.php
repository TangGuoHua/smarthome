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
2014-SEP-05 黄长浩  增加客厅吊顶射灯控制器
2014-SEP-14 黄长浩  客厅吊顶射灯实现“全开全关”
2014-OCT-04 黄长浩  电视背景墙控制器增加电视、电信机顶盒的控制
                    调整显示顺序按DisplayOrder排
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

//客厅西墙（电视墙）插座
function chkLivingTVWallSocketClicked( socketNum, obj )
{
	//开，tmp=1；关，tmp=0
	var tmp;
	tmp = obj.checked?1:0;

	if( socketNum == 1 )
	{
		$.post("/api/sendData.php", { nodeID: "51", data2: tmp } );
	}
	else if( socketNum == 3 )
	{
		$.post("/api/sendData.php", { nodeID: "51", data4: tmp } );
	}
	else if( socketNum == 4 )
	{
		$.post("/api/sendData.php", { nodeID: "51", data5: tmp } );
	}
	else if( socketNum == 5 )
	{
		$.post("/api/sendData.php", { nodeID: "51", data6: tmp } );
	}
	else if( socketNum == 6 )
	{
		$.post("/api/sendData.php", { nodeID: "51", data7: tmp } );
	}
	else if( socketNum == 7 )
	{
		$.post("/api/sendData.php", { nodeID: "51", data8: tmp } );
	}
	else if( socketNum == 8 )
	{
		$.post("/api/sendData.php", { nodeID: "51", data9: tmp } );
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


// 客厅吊顶射灯
function chkLivingRoomCeilingLightclicked( val, obj )
{
	//alert( val + ' obj.selected=' + obj.checked );

	//开，tmp=1；关，tmp=0
	var tmp;
	tmp = obj.checked?1:0;

	if( val == 'E' ) //East
	{
		$.post("/api/sendData.php", { nodeID: "52", data2: tmp } );
	}
	else if( val == 'S' )
	{
		$.post("/api/sendData.php", { nodeID: "52", data4: tmp } );
	}
	else if( val == 'W' )
	{
		$.post("/api/sendData.php", { nodeID: "52", data3: tmp } );
	}
	else if( val == 'N' )
	{
		$.post("/api/sendData.php", { nodeID: "52", data5: tmp } );
	}
	else if(val == 'ALLON' )
	{
		$.post("/api/sendData.php", { nodeID: "52", data2:1, data3:1, data4:1, data5:1 } );
		$("#chkLivingRoomLightEast").attr("checked",true).checkboxradio("refresh");
		$("#chkLivingRoomLightWest").attr("checked",true).checkboxradio("refresh");
		$("#chkLivingRoomLightNorth").attr("checked",true).checkboxradio("refresh");
		$("#chkLivingRoomLightSouth").attr("checked",true).checkboxradio("refresh");
	}
	else if(val == 'ALLOFF' )
	{
		$.post("/api/sendData.php", { nodeID: "52", data2:0, data3:0, data4:0, data5:0 } );
		$("#chkLivingRoomLightEast").attr("checked",false).checkboxradio("refresh");
		$("#chkLivingRoomLightWest").attr("checked",false).checkboxradio("refresh");
		$("#chkLivingRoomLightNorth").attr("checked",false).checkboxradio("refresh");
		$("#chkLivingRoomLightSouth").attr("checked",false).checkboxradio("refresh");
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
$sql = "select * from tabDataToNode order by fldDisplayOrder";
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
		    	<legend>电视背景墙</legend>

				<input type="checkbox" name="chkLivingTVWallSocket4" id="chkLivingTVWallSocket4" onclick="chkLivingTVWallSocketClicked( 4, this);" <?php echo $row["fldData5"]==1?"checked":"";?> />
				<label for="chkLivingTVWallSocket4">台灯</label>

				<input type="checkbox" name="chkLivingTVWallSocket3" id="chkLivingTVWallSocket3" onclick="chkLivingTVWallSocketClicked( 3, this);" <?php echo $row["fldData4"]==1?"checked":"";?> />
				<label for="chkLivingTVWallSocket3">电视机</label>

				<input type="checkbox" name="chkLivingTVWallSocket1" id="chkLivingTVWallSocket1" onclick="chkLivingTVWallSocketClicked( 1, this);" <?php echo $row["fldData2"]==1?"checked":"";?> />
				<label for="chkLivingTVWallSocket1">白机顶盒</label>

		    	<input type="checkbox" name="chkLivingTVWallSocket5" id="chkLivingTVWallSocket5" onclick="chkLivingTVWallSocketClicked( 5, this);" <?php echo $row["fldData6"]==1?"checked":"";?> />
				<label for="chkLivingTVWallSocket5">天猫盒子</label>

		    	<input type="checkbox" name="chkLivingTVWallSocket6" id="chkLivingTVWallSocket6" onclick="chkLivingTVWallSocketClicked( 6, this);" <?php echo $row["fldData7"]==1?"checked":"";?> />
				<label for="chkLivingTVWallSocket6">功放</label>
				
				<input type="checkbox" name="chkLivingTVWallSocket" id="chkLivingTVWallSocket7" onclick="chkLivingTVWallSocketClicked( 7, this);" <?php echo $row["fldData8"]==1?"checked":"";?> />
				<label for="chkLivingTVWallSocket7">熔岩灯</label>

				<input type="checkbox" name="chkLivingTVWallSocket8" id="chkLivingTVWallSocket8" onclick="chkLivingTVWallSocketClicked( 8, this);" <?php echo $row["fldData9"]==1?"checked":"";?> />
				<label for="chkLivingTVWallSocket8">风扇</label>
		    </fieldset>
		</li>
	<?php
	}
	elseif( $row["fldNodeID"]==52 )	// 客厅吊顶射灯
	{
	?>
		<li data-role="fieldcontain">
		    <fieldset data-role="controlgroup" data-type="horizontal">
		    	<legend>客厅顶射灯</legend>
		    	<input type="checkbox" name="chkLivingRoomLightEast" id="chkLivingRoomLightEast" onclick="chkLivingRoomCeilingLightclicked('E', this);" <?php echo $row["fldData2"]==1?"checked":"";?> />
				<label for="chkLivingRoomLightEast">东</label>
				<input type="checkbox" name="chkLivingRoomLightSouth" id="chkLivingRoomLightSouth" onclick="chkLivingRoomCeilingLightclicked('S', this);" <?php echo $row["fldData4"]==1?"checked":"";?> />
				<label for="chkLivingRoomLightSouth">南</label>
				<input type="checkbox" name="chkLivingRoomLightWest" id="chkLivingRoomLightWest" onclick="chkLivingRoomCeilingLightclicked('W', this);" <?php echo $row["fldData3"]==1?"checked":"";?> />
				<label for="chkLivingRoomLightWest">西</label>
				<input type="checkbox" name="chkLivingRoomLightNorth" id="chkLivingRoomLightNorth" onclick="chkLivingRoomCeilingLightclicked('N', this);" <?php echo $row["fldData5"]==1?"checked":"";?> />
				<label for="chkLivingRoomLightNorth">北</label>
				<input type="radio" name="rdoLivingRoomLight" id="chkLivingRoomLightAllOn" onclick="chkLivingRoomCeilingLightclicked('ALLON', this);" />
				<label for="chkLivingRoomLightAllOn">全开</label>
				<input type="radio" name="rdoLivingRoomLight" id="chkLivingRoomLightAllOff" onclick="chkLivingRoomCeilingLightclicked('ALLOFF', this);" />
				<label for="chkLivingRoomLightAllOff">全关</label>
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
					<input type="radio" name="radioLEDGreen" id="radioLEDGreen0" onclick="rdoStudyRoomDeskLampClicked(0);" <?php echo $row["fldData1"]==0?"checked":"";?> />
					<label for="radioLEDGreen0">关</label>
					<input type="radio" name="radioLEDGreen" id="radioLEDGreen3" onclick="rdoStudyRoomDeskLampClicked(5);" <?php echo $row["fldData1"]==5?"checked":"";?> />
					<label for="radioLEDGreen3">弱</label>
					<input type="radio" name="radioLEDGreen" id="radioLEDGreen6" onclick="rdoStudyRoomDeskLampClicked(7);" <?php echo $row["fldData1"]==7?"checked":"";?> />
					<label for="radioLEDGreen6">中</label>
					<input type="radio" name="radioLEDGreen" id="radioLEDGreen10" onclick="rdoStudyRoomDeskLampClicked(10);" <?php echo $row["fldData1"]==10?"checked":"";?> />
					<label for="radioLEDGreen10">强</label>
					<input type="radio" name="radioLEDGreen" id="radioLEDGreen20" onclick="rdoStudyRoomDeskLampClicked(20);" <?php echo $row["fldData1"]==20?"checked":"";?> />
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