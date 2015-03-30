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
2014-OCT-12 黄长浩  电视背景墙控制器去掉电视
                    修改客厅落地灯协议
2014-OCT-12 黄长浩  增加南卧射灯
2014-NOV-30 黄长浩  增加客厅取暖器及温控
2014-DEC-14 黄长浩  增加南卧油汀开关
2014-DEC-28 黄长浩  增加卫生间取暖器
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
//$(document).ready(function() {
	//隐藏设定按钮
	//showButton( "btnSetLivingRoomTemp", false );
//});


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
		$.post("/api/sendData.php", { nodeID: "53", data3: val } );
	}
}


//客厅取暖器插座
function rdoLivingRoomHeaterClicked(val)
{
	$.post("/api/sendData.php", { nodeID: "53", data4: val } );
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
		$.post("/api/sendData.php", { nodeID: "52", data3:1, data4:1, data5:1, data6:1 } );
	}
	else if(val == 'ALLOFF' )
	{
		$.post("/api/sendData.php", { nodeID: "52", data3:0, data4:0, data5:0, data6:0 } );
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
	else if( val == 'DINNERON' )
	{
		$.post("/api/sendData.php", { nodeID: "41", data1:1, data3:1, data4:1 } );
	}
	else if( val == 'ALLOFF' )
	{
		$.post("/api/sendData.php", { nodeID: "41", data1:0, data2:0, data3:0, data4:0  } );
	}
}


//书房LED灯带
function rdoStudyLEDStripeClicked( val )
{
	$.post("/api/sendData.php", { nodeID: "32", data4: val } );
}


//书房调光台灯
function rdoStudyRoomDeskLampClicked(val)
{
	//alert(val);
	$.post("/api/sendData.php", { nodeID: "31", data1: val } );
}


//南卧油汀插座
function rdoSouthBedroomHeaterClicked(val)
{
	$.post("/api/sendData.php", { nodeID: "82", data1: val } );
}


// 南卧衣柜射灯
function southBedroomWardrobeLightClicked( val, obj )
{
	//alert( val + ' obj.selected=' + obj.checked );

	//开，tmp=1；关，tmp=0
	var tmp;
	tmp = obj.checked?1:0;

	if( val == 'E' ) //East
	{
		$.post("/api/sendData.php", { nodeID: "83", data3: tmp } );
	}
	else if( val == 'W' )
	{
		$.post("/api/sendData.php", { nodeID: "83", data4: tmp } );
	}
	else if(val == 'ALLON' )
	{
		$.post("/api/sendData.php", { nodeID: "83", data3:1, data4:1 } );
		$("#chkSouthBedroomWardrobeLightEast").attr("checked",true).checkboxradio("refresh");
		$("#chkSouthBedroomWardrobeLightWest").attr("checked",true).checkboxradio("refresh");
	}
	else if(val == 'ALLOFF' )
	{
		$.post("/api/sendData.php", { nodeID: "83", data3:0, data4:0 } );
		$("#chkSouthBedroomWardrobeLightEast").attr("checked",false).checkboxradio("refresh");
		$("#chkSouthBedroomWardrobeLightWest").attr("checked",false).checkboxradio("refresh");
	}
}


//卫生间热水器
function rdoBathroomHeaterClicked( val )
{
	$.post("/api/sendData.php", { nodeID: "91", data1:1, data2:20, data6: val } );
}


//卫生间取暖器
function rdoBathroomAirHeaterClicked( val )
{
	$.post("/api/sendData.php", { nodeID: "92", data1:1, data2:20, data5: val } );
}


//阳台卷帘
function rdoBalconyCurtainClicked( val )
{
	$.post("/api/sendData.php", { nodeID: "62", data2:20, data4: val } );
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
		    	<legend>餐厅顶灯</legend>
				<input type="radio" name="rdoDinningLight" id="rdoDinningLightAllOff" onclick="chkDinningRoomLightclicked('ALLOFF', this);" <?php echo ($row["fldData1"]==0&$row["fldData2"]==0&$row["fldData3"]==0&$row["fldData4"]==0)?"checked":"";?>/>
				<label for="rdoDinningLightAllOff">关</label>
				<input type="radio" name="rdoDinningLight" id="rdoDinningLightAllOn" onclick="chkDinningRoomLightclicked('DINNERON', this);" <?php echo ($row["fldData1"]==1|$row["fldData2"]==1|$row["fldData3"]==1|$row["fldData4"]==1)?"checked":"";?>/>
				<label for="rdoDinningLightAllOn">开</label>
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
		    	<legend>客厅顶灯</legend>
				<input type="radio" name="rdoLivingRoomLight" id="chkLivingRoomLightAllOff" onclick="chkLivingRoomCeilingLightclicked('ALLOFF', this);" <?php echo ($row["fldData5"]==0&$row["fldData6"]==0&$row["fldData3"]==0&$row["fldData4"]==0)?"checked":"";?>/>
				<label for="chkLivingRoomLightAllOff">关</label>
				<input type="radio" name="rdoLivingRoomLight" id="chkLivingRoomLightAllOn" onclick="chkLivingRoomCeilingLightclicked('ALLON', this);" <?php echo ($row["fldData5"]==1|$row["fldData6"]==1|$row["fldData3"]==1|$row["fldData4"]==1)?"checked":"";?>/>
				<label for="chkLivingRoomLightAllOn">开</label>
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
					<input type="radio" name="rdoLivingRoomFloorLamp" id="rdoLivingRoomFloorLamp0" value="0" onclick="rdoLivingRoomEastWallSouthControllerClicked(1, '0');" <?php echo $row["fldData3"]==0?"checked":"";?> />
					<label for="rdoLivingRoomFloorLamp0">关</label>
					<input type="radio" name="rdoLivingRoomFloorLamp" id="rdoLivingRoomFloorLamp1" value="1" onclick="rdoLivingRoomEastWallSouthControllerClicked(1, '1');" <?php echo $row["fldData3"]==1?"checked":"";?> />
					<label for="rdoLivingRoomFloorLamp1">开</label>

		    </fieldset>
		</li>
		<!--li data-role="fieldcontain">
			<fieldset data-role="controlgroup" data-type="horizontal">
				<legend>客厅取暖器</legend>
					<input type="radio" name="rdoLivingRoomHeater" id="rdoLivingRoomHeater0" value="0" onclick="rdoLivingRoomHeaterClicked('0');" <?php echo $row["fldData4"]==0?"checked":"";?> />
					<label for="rdoLivingRoomHeater0">关</label>
					<input type="radio" name="rdoLivingRoomHeater" id="rdoLivingRoomHeater2" value="2" onclick="rdoLivingRoomHeaterClicked('2');" <?php echo $row["fldData4"]==2?"checked":"";?> />
					<label for="rdoLivingRoomHeater2">恒温<?php echo $row["fldData5"]/10;?>度</label>
			</fieldset>
		</li-->
	<?php
	}
	elseif( $row["fldNodeID"]==62 )	// 阳台卷帘
	{
	?>
		<li data-role="fieldcontain">
		    <fieldset data-role="controlgroup" data-type="horizontal">
		    	<legend>阳台窗帘</legend>
					<input type="radio" name="rdoBalconyCurtain" id="rdoBalconyCurtain0" onclick="rdoBalconyCurtainClicked(0);" <?php echo $row["fldData4"]==0?"checked":"";?> />
					<label for="rdoBalconyCurtain0">全开</label>

					<!--input type="radio" name="rdoBalconyCurtain" id="rdoBalconyCurtain20" onclick="rdoBalconyCurtainClicked(20);" <?php echo $row["fldData4"]==20?"checked":"";?> />
					<label for="rdoBalconyCurtain20">20%</label-->

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
	elseif( $row["fldNodeID"]==820 ) //南卧油汀
	{
	?>
		<li data-role="fieldcontain">
			<fieldset data-role="controlgroup" data-type="horizontal">
				<legend>南卧油汀</legend>
					<input type="radio" name="rdoSouthBedroomHeater" id="rdoSouthBedroomHeater0" value="0" onclick="rdoSouthBedroomHeaterClicked('0');" <?php echo $row["fldData1"]==0?"checked":"";?> />
					<label for="rdoSouthBedroomHeater0">关</label>
					<!--input type="radio" name="rdoSouthBedroomHeater" id="rdoSouthBedroomHeater1" value="1" onclick="rdoSouthBedroomHeaterClicked('1');" <?php echo $row["fldData1"]==1?"checked":"";?> />
					<label for="rdoSouthBedroomHeater1">开</label-->
					<input type="radio" name="rdoSouthBedroomHeater" id="rdoSouthBedroomHeater2" value="2" onclick="rdoSouthBedroomHeaterClicked('2');" <?php echo $row["fldData1"]==2?"checked":"";?> />
					<label for="rdoSouthBedroomHeater2">恒温<?php echo $row["fldData2"]/10;?>度</label>
			</fieldset>
		</li>
	<?php	
	}
	elseif( $row["fldNodeID"]==83 )	// 南卧衣柜射灯
	{
	?>
		<li data-role="fieldcontain">
		    <fieldset data-role="controlgroup" data-type="horizontal">
		    	<legend>南卧射灯</legend>

				<input type="checkbox" id="chkSouthBedroomWardrobeLightWest" onclick="southBedroomWardrobeLightClicked('W', this);" <?php echo $row["fldData4"]==1?"checked":"";?> />
				<label for="chkSouthBedroomWardrobeLightWest">左</label>

		    	<input type="checkbox" id="chkSouthBedroomWardrobeLightEast" onclick="southBedroomWardrobeLightClicked('E', this);" <?php echo $row["fldData3"]==1?"checked":"";?> />
				<label for="chkSouthBedroomWardrobeLightEast">右</label>

				<input type="radio" name="rdoSouthBedroomWardrobeLight" id="rdoSouthBedroomWardrobeLightAllOn" onclick="southBedroomWardrobeLightClicked('ALLON', this);" />
				<label for="rdoSouthBedroomWardrobeLightAllOn">全开</label>
				<input type="radio" name="rdoSouthBedroomWardrobeLight" id="rdoSouthBedroomWardrobeLightAllOff" onclick="southBedroomWardrobeLightClicked('ALLOFF', this);" />
				<label for="rdoSouthBedroomWardrobeLightAllOff">全关</label>
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
					<input type="radio" name="rdoBathroomHeater" id="rdoBathroomHeater0" value="0" onclick="rdoBathroomHeaterClicked('0');" <?php echo $row["fldData6"]==0?"checked":"";?> />
					<label for="rdoBathroomHeater0">关</label>
					<input type="radio" name="rdoBathroomHeater" id="rdoBathroomHeater1" value="1" onclick="rdoBathroomHeaterClicked('1');" <?php echo $row["fldData6"]==1?"checked":"";?> />
					<label for="rdoBathroomHeater1">开</label>
			</fieldset>
		</li>
	<?php
	}
	elseif( $row["fldNodeID"]==32 )//书房
	{
	?>
		<!--li data-role="list-divider">书房</li-->
		<li data-role="fieldcontain">
			<fieldset data-role="controlgroup" data-type="horizontal">
				<legend>书房灯带</legend>
					<input type="radio" name="rdoStudyLEDStripe" id="rdoStudyLEDStripe_0" value="0" onclick="rdoStudyLEDStripeClicked('0');" <?php echo $row["fldData4"]==0?"checked":"";?> />
					<label for="rdoStudyLEDStripe_0">关</label>
					<input type="radio" name="rdoStudyLEDStripe" id="rdoStudyLEDStripe_1" value="1" onclick="rdoStudyLEDStripeClicked('1');" <?php echo $row["fldData4"]==1?"checked":"";?> />
					<label for="rdoStudyLEDStripe_1">开</label>
					<input type="radio" name="rdoStudyLEDStripe" id="rdoStudyLEDStripe_2" value="2" onclick="rdoStudyLEDStripeClicked('2');" <?php echo $row["fldData4"]==2?"checked":"";?> />
					<label for="rdoStudyLEDStripe_2">自动</label>
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
	elseif( $row["fldNodeID"]==920 )	// 卫生间南墙
	{
	?>
		<!--li data-role="list-divider">卫生间</li-->
		<li data-role="fieldcontain">
			<fieldset data-role="controlgroup" data-type="horizontal">
				<legend>卫生间取暖器</legend>
					<input type="radio" name="rdoBathroomAirHeater" id="rdoBathroomAirHeater0" onclick="rdoBathroomAirHeaterClicked('0');" <?php echo $row["fldData5"]==0?"checked":"";?> />
					<label for="rdoBathroomAirHeater0">关</label>

					<input type="radio" name="rdoBathroomAirHeater" id="rdoBathroomAirHeater2" onclick="rdoBathroomAirHeaterClicked('2');" <?php echo $row["fldData5"]==2?"checked":"";?> />
					<label for="rdoBathroomAirHeater2">温控 <?php echo ($row["fldData6"]/10);?>度</label>
			</fieldset>
		</li>
	<?php
	}
}
$db->close();
?>

<li data-role="fieldcontain">
	<fieldset data-role="controlgroup" data-type="horizontal">
		<legend>*厨房收音机</legend>
			<input type="radio" name="rdoKitchenRadio" id="rdoKitchenRadio0" checked/>
			<label for="rdoKitchenRadio0">关</label>
			<input type="radio" name="rdoKitchenRadio" id="rdoKitchenRadio1" />
			<label for="rdoKitchenRadio1">江苏音乐</label>
			<input type="radio" name="rdoKitchenRadio" id="rdoKitchenRadio2" />
			<label for="rdoKitchenRadio2">苏州新闻</label>
			<input type="radio" name="rdoKitchenRadio" id="rdoKitchenRadio3" />
			<label for="rdoKitchenRadio3">苏州交通</label>
			<input type="radio" name="rdoKitchenRadio" id="rdoKitchenRadio4" />
			<label for="rdoKitchenRadio4">中央台</label>
	</fieldset>
</li>

</ul>
<?php
include "include/templateFooter.php";
?>