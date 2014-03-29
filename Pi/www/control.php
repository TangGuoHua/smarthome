<?php
/*
日期        作者    备注
------------------------------------------
2013-APR-09 黄长浩  增加了调光台灯控制滑条
2013-APR-29 黄长浩  修改蓝绿调光LED为主卧台灯，并增加延时关功能
2013-MAY-17 黄长浩  主卧调光台灯移到书房
2013-OCT-02 黄长浩  根据新家schema做相应修改
2013-Nov-08 黄长浩  增加餐厅顶射灯控制开关
2014-FEB-05 黄长浩  增加卫生间小厨宝控制开关
                    增加南卧油汀控制开关及温度设定
2014-FEB-09 黄长浩  厨房1、2号灯改为分别设定阈值
2014-MAR-29 黄长浩  增加客厅东墙南头控制器（落地灯及插座）
*/

$gPageTitle = "控制";
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

//厨房

function rdoKitchenLightClicked( ind, val )
{
	if( ind == 1 ) //Light 1
	{
		$.post("/api/sendData.php", { nodeID: "22", data3: val } );
	}
	if( ind == 2 ) // Light 2
	{
		$.post("/api/sendData.php", { nodeID: "22", data4: val } );
	}
}

function btnSetKitchenLightOnThresholdClicked( ind )
{
	if( ind == 1)
	{
		var setVal = document.getElementById('rangeKitchenLight1OnThreshold').value;

		if( setVal>=0 && setVal <=255 )
		{
			$.post("/api/sendData.php", { nodeID: "22", data1:setVal } );
		}

		showButton( 'btnSetKitchenLight1OnThreshold', false );
	}
	else if( ind ==2 )
	{
		var setVal = document.getElementById('rangeKitchenLight2OnThreshold').value;

		if( setVal>=0 && setVal <=255 )
		{
			$.post("/api/sendData.php", { nodeID: "22", data2:setVal } );
		}

		showButton( 'btnSetKitchenLight2OnThreshold', false );
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


//卫生间灯
function rdoBathroomLightClicked( val )
{
	$.post("/api/sendData.php", { nodeID: "91", data3: val } );
}

//卫生间灯亮阈值
function btnSetBathroomLightOnThresholdClicked()
{
	var setVal = document.getElementById('rangeBathroomLightOnThreshold').value;

	if( setVal>=0 && setVal <=255 )
	{
		$.post("/api/sendData.php", { nodeID: "91", data2:setVal } );
	}

	showButton( 'btnSetBathroomLightOnThreshold', false );
}

//卫生间热水器
function rdoBathroomHeaterClicked( val )
{
	$.post("/api/sendData.php", { nodeID: "91", data5: val } );
}

//卫生间小厨宝
function rdoBathroomXiaoChuBaoClicked( val )
{
	$.post("/api/sendData.php", { nodeID: "92", data1: val } );
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
	$.post("/api/sendData.php", { nodeID: "201", data1: val } );
}

//南卧油汀插座
function rdoSouthBedroomHeaterClicked(val)
{
	$.post("/api/sendData.php", { nodeID: "82", data1: val } );
}

//南卧温度设定按钮
function btnSetSouthBedroomTempClicked()
{
	var setVal = document.getElementById('sliderSouthBedroomTemp').value;
	setVal *= 10;
	
	if( setVal>=30 && setVal <=200 ) 
	{
		$.post("/api/sendData.php", { nodeID: "82", data2:setVal } );
	}
	else //温度在3度以下时，关闭油汀
	{
		$.post("/api/sendData.php", { nodeID: "82", data1: '0', data2: setVal } );
	}

	showButton( 'btnSetSouthBedroomTemp', false );
}


function btnSetStudyroomLampClicked()
{
	var setVal = document.getElementById('rangeStudyroomLamp').value;

	if( setVal>=0 && setVal <=255 )
	{
		$.post("/api/sendData.php", { nodeID: "201", data1:setVal } );
	}

	showButton( 'btnSetStudyroomLamp', false );
}

// 测试移动节点
function rdoTestMobileLightClicked( val )
{
	$.post("/api/sendData.php", { nodeID: "202", data2: val } );
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

	if( $row["fldNodeID"]==22 )//厨房
	{
	?>
		<!--li data-role="list-divider">厨房</li-->
		<li data-role="fieldcontain">
			<fieldset data-role="controlgroup" data-type="horizontal">
				<legend>厨房吊柜灯</legend>
					<input type="radio" name="rdoKitchenLight1" id="rdoKitchenLight1_0" value="0" onclick="rdoKitchenLightClicked(1, '0');" <?php echo $row["fldData3"]==0?"checked":"";?> />
					<label for="rdoKitchenLight1_0">关</label>
					<input type="radio" name="rdoKitchenLight1" id="rdoKitchenLight1_1" value="1" onclick="rdoKitchenLightClicked(1, '1');" <?php echo $row["fldData3"]==1?"checked":"";?> />
					<label for="rdoKitchenLight1_1">开</label>
					<input type="radio" name="rdoKitchenLight1" id="rdoKitchenLight1_2" value="2" onclick="rdoKitchenLightClicked(1, '2');" <?php echo $row["fldData3"]==2?"checked":"";?> />
					<label for="rdoKitchenLight1_2">自动</label>
			</fieldset>
		</li>
		<li data-role="fieldcontain">
			<label for="rangeKitchenLight1OnThreshold">厨房吊柜灯开灯阈值</label>
			<input type="range" name="rangeKitchenLight1OnThreshold" id="rangeKitchenLight1OnThreshold" onchange="showButton( 'btnSetKitchenLight1OnThreshold', true );" value="<?php echo $row["fldData1"];?>" min="0" max="255" step="1" data-highlight="true" />
			<button id="btnSetKitchenLight1OnThreshold" data-icon="check" onclick="btnSetKitchenLightOnThresholdClicked(1);">设定</button>
		</li>
		<li data-role="fieldcontain">
			<fieldset data-role="controlgroup" data-type="horizontal">
				<legend>厨房顶灯</legend>
					<input type="radio" name="rdoKitchenLight2" id="rdoKitchenLight2_0" value="0" onclick="rdoKitchenLightClicked(2, '0');" <?php echo $row["fldData4"]==0?"checked":"";?> />
					<label for="rdoKitchenLight2_0">关</label>
					<input type="radio" name="rdoKitchenLight2" id="rdoKitchenLight2_1" value="1" onclick="rdoKitchenLightClicked(2, '1');" <?php echo $row["fldData4"]==1?"checked":"";?> />
					<label for="rdoKitchenLight2_1">开</label>
					<input type="radio" name="rdoKitchenLight2" id="rdoKitchenLight2_2" value="2" onclick="rdoKitchenLightClicked(2, '2');" <?php echo $row["fldData4"]==2?"checked":"";?> />
					<label for="rdoKitchenLight2_2">自动</label>
			</fieldset>
		</li>
		<li data-role="fieldcontain">
			<label for="rangeKitchenLight2OnThreshold">厨房顶灯开灯阈值</label>
			<input type="range" name="rangeKitchenLight2OnThreshold" id="rangeKitchenLight2OnThreshold" onchange="showButton( 'btnSetKitchenLight2OnThreshold', true );" value="<?php echo $row["fldData2"];?>" min="0" max="255" step="1" data-highlight="true" />
			<button id="btnSetKitchenLight2OnThreshold" data-icon="check" onclick="btnSetKitchenLightOnThresholdClicked(2);">设定</button>
		</li>
	<?php
	}
	elseif( $row["fldNodeID"]==41 )	// 餐厅
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
	else if( $row["fldNodeID"]==82 ) //南卧
	{
	?>
		<li data-role="fieldcontain">
			<fieldset data-role="controlgroup" data-type="horizontal">
				<legend>南卧油汀</legend>
					<input type="radio" name="rdoSouthBedroomHeater" id="rdoSouthBedroomHeater0" value="0" onclick="rdoSouthBedroomHeaterClicked('0');" <?php echo $row["fldData1"]==0?"checked":"";?> />
					<label for="rdoSouthBedroomHeater0">关</label>
					<input type="radio" name="rdoSouthBedroomHeater" id="rdoSouthBedroomHeater1" value="1" onclick="rdoSouthBedroomHeaterClicked('1');" <?php echo $row["fldData1"]==1?"checked":"";?> />
					<label for="rdoSouthBedroomHeater1">开</label>
					<input type="radio" name="rdoSouthBedroomHeater" id="rdoSouthBedroomHeater2" value="2" onclick="rdoSouthBedroomHeaterClicked('2');" <?php echo $row["fldData1"]==2?"checked":"";?> />
					<label for="rdoSouthBedroomHeater2">温控</label>
			</fieldset>
		</li>
		<li data-role="fieldcontain">
			<label for="sliderSouthBedroomTemp">南卧温度</label>
			<input type="range" name="sliderSouthBedroomTemp" id="sliderSouthBedroomTemp" onchange="showButton( 'btnSetSouthBedroomTemp', true );" value="<?php echo $row["fldData2"]/10;?>" min="0" max="20" step="0.5" data-highlight="true" />
			<button id="btnSetSouthBedroomTemp" data-icon="check" onclick="btnSetSouthBedroomTempClicked();">设定</button>
		</li>
	<?php	
	}
	elseif( $row["fldNodeID"]==91 )	// 卫生间
	{
	?>
		<!--li data-role="list-divider">卫生间</li-->
		<li data-role="fieldcontain">
			<fieldset data-role="controlgroup" data-type="horizontal">
				<legend>卫生间灯</legend>
					<input type="radio" name="rdoBathroomLight" id="rdoBathroomLight0" value="0" onclick="rdoBathroomLightClicked('0');" <?php echo $row["fldData3"]==0?"checked":"";?> />
					<label for="rdoBathroomLight0">关</label>
					<input type="radio" name="rdoBathroomLight" id="rdoBathroomLight1" value="1" onclick="rdoBathroomLightClicked('1');" <?php echo $row["fldData3"]==1?"checked":"";?> />
					<label for="rdoBathroomLight1">开</label>
					<input type="radio" name="rdoBathroomLight" id="rdoBathroomLight2" value="2" onclick="rdoBathroomLightClicked('2');" <?php echo $row["fldData3"]==2?"checked":"";?> />
					<label for="rdoBathroomLight2">自动</label>
			</fieldset>
		</li>
		<li data-role="fieldcontain">
			<label for="rangeBathroomLightOnThreshold">卫生间开灯阈值</label>
			<input type="range" name="rangeBathroomLightOnThreshold" id="rangeBathroomLightOnThreshold" onchange="showButton( 'btnSetBathroomLightOnThreshold', true );" value="<?php echo $row["fldData2"];?>" min="0" max="255" step="1" data-highlight="true" />
			<button id="btnSetBathroomLightOnThreshold" data-icon="check" onclick="btnSetBathroomLightOnThresholdClicked();">设定</button>
		</li>
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
	elseif( $row["fldNodeID"]==92 )	// 卫生间南墙
	{
	?>
		<!--li data-role="list-divider">卫生间</li-->
		<li data-role="fieldcontain">
			<fieldset data-role="controlgroup" data-type="horizontal">
				<legend>卫生间小厨宝</legend>
					<input type="radio" name="rdoBathroomXiaoChuBao" id="rdoBathroomXiaoChuBao0" value="0" onclick="rdoBathroomXiaoChuBaoClicked('0');" <?php echo $row["fldData1"]==0?"checked":"";?> />
					<label for="rdoBathroomXiaoChuBao0">关</label>
					<input type="radio" name="rdoBathroomXiaoChuBao" id="rdoBathroomXiaoChuBao1" value="1" onclick="rdoBathroomXiaoChuBaoClicked('1');" <?php echo $row["fldData1"]==1?"checked":"";?> />
					<label for="rdoBathroomXiaoChuBao1">开</label>

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
	elseif( $row["fldNodeID"]==201)
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
		<li data-role="fieldcontain">
			<label for="rangeStudyroomLamp">书房台灯</label>
			<input type="range" name="rangeStudyroomLamp" id="rangeStudyroomLamp" onchange="showButton( 'btnSetStudyroomLamp', true );" value="<?php echo $row["fldData1"];?>" min="0" max="10" step="1" data-highlight="true" />
			<button id="btnSetStudyroomLamp" data-icon="check" onclick="btnSetStudyroomLampClicked();">设定</button>
		</li>
	<?php
	}
	elseif( $row["fldNodeID"]==202)
	{
	?>
		<li data-role="fieldcontain">
			<fieldset data-role="controlgroup" data-type="horizontal">
				<legend>测试移动灯</legend>
					<input type="radio" name="rdoTestMobileLight" id="rdoTestMobileLight0" value="0" onclick="rdoTestMobileLightClicked('0');" <?php echo $row["fldData2"]==0?"checked":"";?> />
					<label for="rdoTestMobileLight0">关</label>
					<input type="radio" name="rdoTestMobileLight" id="rdoTestMobileLight1" value="1" onclick="rdoTestMobileLightClicked('1');" <?php echo $row["fldData2"]==1?"checked":"";?> />
					<label for="rdoTestMobileLight1">开</label>
			</fieldset>
		</li>
	<?php
	}
}
$db->close();
?>


</ul>
<?php
include "include/templateFooter.php";
?>