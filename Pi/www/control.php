<?php
/*
日期        谁？   做了什么？
------------------------------------------
2013-APR-09 黄长浩 增加了调光台灯控制滑条
2013-APR-29 黄长浩 修改蓝绿调光LED为主卧台灯，并增加延时关功能
2013-MAY-17 黄长浩 主卧调光台灯移到书房
*/

$gPageTitle = "控制";
include "include/templateHeader.php";
include "include/utils.php";
?>
<script>
// document ready之后就执行这里的一些函数
$(document).ready(function() {
	//隐藏设定按钮
	showButton( "btnLivingTempSet", false );
	showButton( "btnSetSecondaryBedTemp", false );
	showButton( 'btnSetAdjLamp', false );
});

//餐厅射灯
function radioDingingLightClicked( val )
{
	$.post("/api/sendData.php", { nodeID1: "100", nodeID2: "3", data1: val } );
}

//客厅落地灯
function radioLivingLampClicked( val )
{
	$.post("/api/sendData.php", { nodeID1: "100", nodeID2: "7", data1: val } );
}

//客厅油汀
function radioLivingHeaterClicked( val )
{
	$.post("/api/sendData.php", { nodeID1: "100", nodeID2: "7", data2: val } );
}

//LED调光灯radio clicked事件
function radioLEDClicked( GBIndicator, val )
{
	if( GBIndicator == 'G' ) // Green LED
	{
		$.post("/api/sendData.php", { nodeID1: "100", nodeID2: "12", data1: val } );
	}
	else if( GBIndicator == 'B' ) // Blue LED
	{
		$.post("/api/sendData.php", { nodeID1: "100", nodeID2: "12", data2: val } );
	}
}

//调光台灯
function btnSetAdjLampClicked()
{
	var setVal = document.getElementById('sliderAdjLamp').value;
	
	if( setVal>=0 && setVal <=10 ) //设置的值在1－10之间
	{
		$.post("/api/sendData.php", { nodeID1: "100", nodeID2: "12", data1:setVal } );
	}

	showButton( 'btnSetAdjLamp', false );
}


//书房灯Radio Clicked事件
function radioStudyRmLightClicked( val )
{
	$.post("/api/sendData.php", { nodeID1: "100", nodeID2: "17", data2: val } );
}

//次卧床头灯Radio Clicked事件
function radioSecondaryBedLampClicked( val )
{
	$.post("/api/sendData.php", { nodeID1: "100", nodeID2: "18", data1: val } );
}

//次卧温度设定按钮
function btnSetSecondaryBedTempClicked()
{
	var setVal = document.getElementById('sliderSecondaryBedTemp').value;
	setVal *= 10;
	
	if( setVal>=10 && setVal <=200 ) //设置的温度在1－20度之间，进入自动温控模式
	{
		$.post("/api/sendData.php", { nodeID1: "100", nodeID2: "18", data2: "2", data3: setVal, data4:setVal-2 } );
	}
	else //设置的温度在1度以下或20度以上，则关掉加热器
	{
		$.post("/api/sendData.php", { nodeID1: "100", nodeID2: "18", data2: "0", data3: "0", data4:"0" } );
	}
	showButton( 'btnSetSecondaryBedTemp', false );
}


function setHeaterTempClicked()
{
	//alert( document.getElementById('sliderHeater').value );
	showButton( "btnLivingTempSet", false );
}


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

// 给电子钟设定时间
function btnSetClockClicked()
{
	$.post("/api/sendData.php", { nodeID1: "100", nodeID2: "15", data1: "strftime('%H','now','localtime')", data2: "strftime('%M','now','localtime')", data3:"strftime('%S','now','localtime')" } );
}
</script>

<a href="index.php" data-role="button" data-icon="arrow-l">返回</a>
<a href="control.php" data-role="button" data-icon="refresh" data-ajax="false">更新</a>

<ul data-role="listview" data-inset="true">
<?php
$db = new DBHelper();
$sql = "select * from tabDataToNode order by fldDisplayOrder";
$results = $db->query($sql);

while ($row = $results->fetchArray()) 
{
	// 客厅灯
	if( $row["fldNodeID1"]==100 and $row["fldNodeID2"]==3 )
	{
	?>
		<!--li data-role="list-divider">餐厅</li-->
		<li data-role="fieldcontain">
			<fieldset data-role="controlgroup" data-type="horizontal">
				<legend>餐厅射灯</legend>
					<input type="radio" name="radioDiningLight" id="radioDiningLight0" value="0" onclick="radioDingingLightClicked('0');" <?php echo $row["fldData1"]==0?"checked":"";?> />
					<label for="radioDiningLight0">关</label>
					<input type="radio" name="radioDiningLight" id="radioDiningLight1" value="1" onclick="radioDingingLightClicked('1');" <?php echo $row["fldData1"]==1?"checked":"";?> />
					<label for="radioDiningLight1">开</label>
					<input type="radio" name="radioDiningLight" id="radioDiningLight2" value="2" onclick="radioDingingLightClicked('2');" <?php echo $row["fldData1"]==2?"checked":"";?> />
					<label for="radioDiningLight2">自动</label>
			</fieldset>
		</li>
	<?php
	}
	else if( $row["fldNodeID1"]==100 and $row["fldNodeID2"]==7 )
	{
	?>
		<!--li data-role="list-divider">客厅</li-->
		<li data-role="fieldcontain">
			<fieldset data-role="controlgroup" data-type="horizontal">
				<legend>客厅落地灯</legend>
					<input type="radio" name="radioLivingLamp" id="radioLivingLampOff" value="0" onclick="radioLivingLampClicked( '0' );" <?php echo $row["fldData1"]==0?"checked":"";?> />
					<label for="radioLivingLampOff">关</label>
					<input type="radio" name="radioLivingLamp" id="radioLivingLampOn" value="1" onclick="radioLivingLampClicked('1');" <?php echo $row["fldData1"]==1?"checked":"";?> />
					<label for="radioLivingLampOn">开</label>
			</fieldset>
		</li>
		<li data-role="fieldcontain">
			<fieldset data-role="controlgroup" data-type="horizontal">
				<legend>客厅油汀插座</legend>
					<input type="radio" name="radioLivingHeater" id="radioLivingHeaterOff" value="0" onclick="radioLivingHeaterClicked( '0' );" <?php echo $row["fldData2"]==0?"checked":"";?> />
					<label for="radioLivingHeaterOff">关</label>
					<input type="radio" name="radioLivingHeater" id="radioLivingHeaterOn" value="1" onclick="radioLivingHeaterClicked('1');" <?php echo $row["fldData2"]==1?"checked":"";?> />
					<label for="radioLivingHeaterOn">开</label>
			</fieldset>
		</li>
	<?php
	}
	else if( $row["fldNodeID1"]==100 and $row["fldNodeID2"]==17 )
	{
	?>
		<!--li data-role="list-divider">书房</li-->
		
		<li data-role="fieldcontain">
			<fieldset data-role="controlgroup" data-type="horizontal">
				<legend>书房顶灯</legend>
					<input type="radio" name="radioStudyRoomLight" id="radioStudyRoomLight0" value="0" onclick="radioStudyRmLightClicked(0);" <?php echo $row["fldData2"]==0?"checked":"";?> />
					<label for="radioStudyRoomLight0">关</label>
					<input type="radio" name="radioStudyRoomLight" id="radioStudyRoomLight1" value="1" onclick="radioStudyRmLightClicked(1);" <?php echo $row["fldData2"]==1?"checked":"";?> />
					<label for="radioStudyRoomLight1">开</label>
					<input type="radio" name="radioStudyRoomLight" id="radioStudyRoomLight2" value="2" onclick="radioStudyRmLightClicked(2);" <?php echo $row["fldData2"]==2?"checked":"";?> />
					<label for="radioStudyRoomLight2">自动</label>
			</fieldset>
		</li>
	<?php	
	}
	else if( $row["fldNodeID1"]==100 and $row["fldNodeID2"]==18 )
	{
	?>
		<!--li data-role="list-divider">次卧</li-->
		
		<li data-role="fieldcontain">
			<fieldset data-role="controlgroup" data-type="horizontal">
				<legend>次卧床头灯</legend>
					<input type="radio" name="radioSecondaryBedLamp" id="radioSecondaryBedLamp0" value="0" onclick="radioSecondaryBedLampClicked(0);" <?php echo $row["fldData1"]==0?"checked":"";?> />
					<label for="radioSecondaryBedLamp0">关</label>					
					<input type="radio" name="radioSecondaryBedLamp" id="radioSecondaryBedLamp1" value="1" onclick="radioSecondaryBedLampClicked(1);" <?php echo $row["fldData1"]==1?"checked":"";?> />
					<label for="radioSecondaryBedLamp1">开</label>
					<input type="radio" name="radioSecondaryBedLamp" id="radioSecondaryBedLamp2" value="2" onclick="radioSecondaryBedLampClicked(2);" <?php echo $row["fldData1"]==2?"checked":"";?> />
					<label for="radioSecondaryBedLamp2">延时关</label>
			</fieldset>
		</li>
		<li data-role="fieldcontain">
			<label for="sliderSecondaryBedTemp">次卧温度</label>
			<input type="range" name="sliderSecondaryBedTemp" id="sliderSecondaryBedTemp" onchange="showButton( 'btnSetSecondaryBedTemp', true );" value="<?php echo $row["fldData3"]/10;?>" min="0" max="20" step="0.1" data-highlight="true" />
			<button id="btnSetSecondaryBedTemp" data-icon="check" onclick="btnSetSecondaryBedTempClicked();">设定</button>
		</li>
	<?php	
	}
	else if( $row["fldNodeID1"]==100 and $row["fldNodeID2"]==12 )
	{
	?>
		<!--li data-role="list-divider">次卧室</li-->
		
		<li data-role="fieldcontain">
			<fieldset data-role="controlgroup" data-type="horizontal">
				<legend>书房台灯</legend>
					<input type="radio" name="radioLEDGreen" id="radioLEDGreen0" value="0" onclick="radioLEDClicked('G', 0);" <?php echo $row["fldData1"]==0?"checked":"";?> />
					<label for="radioLEDGreen0">关</label>
					<input type="radio" name="radioLEDGreen" id="radioLEDGreen3" value="3" onclick="radioLEDClicked('G', 3);" <?php echo $row["fldData1"]==3?"checked":"";?> />
					<label for="radioLEDGreen3">弱</label>
					<input type="radio" name="radioLEDGreen" id="radioLEDGreen6" value="6" onclick="radioLEDClicked('G', 6);" <?php echo $row["fldData1"]==6?"checked":"";?> />
					<label for="radioLEDGreen6">中</label>
					<input type="radio" name="radioLEDGreen" id="radioLEDGreen10" value="10" onclick="radioLEDClicked('G', 10);" <?php echo $row["fldData1"]==10?"checked":"";?> />
					<label for="radioLEDGreen10">强</label>
					<input type="radio" name="radioLEDGreen" id="radioLEDGreen20" value="20" onclick="radioLEDClicked('G', 20);" <?php echo $row["fldData1"]==20?"checked":"";?> />
					<label for="radioLEDGreen20">延时关</label>
			</fieldset>
		</li>
		<li data-role="fieldcontain">
			<label for="sliderAdjLamp">书房台灯</label>
			<input type="range" name="sliderAdjLamp" id="sliderAdjLamp" onchange="showButton( 'btnSetAdjLamp', true );" value="<?php echo $row["fldData1"];?>" min="0" max="10" step="1" data-highlight="true" />
			<button id="btnSetAdjLamp" data-icon="check" onclick="btnSetAdjLampClicked();">设定</button>
		</li>
	<?php	
	}
}
$db->close();
?>
	<li data-role="fieldcontain">
		<div data-role="fieldcontain">
			<label for="select-native-1">厨房收音机 *</label>
			<select name="select-native-1" id="select-native-1">
				<option value="0">关</option>
				<option value="1">苏州新闻台 91.1MHz</option>
				<option value="2">苏州音乐台 94.8MHz</option>
				<option value="3">苏州生活台 96.5MHz</option>
				<option value="4">苏州台 100MHz</option>
				<option value="5">苏州台 101.1MHz</option>
				<option value="6">苏州台 102.8MHz</option>
				<option value="7">苏州交通台 104.8MHz</option>

			</select>
		</div>
	</li>
	<li data-role="fieldcontain">
		<fieldset data-role="controlgroup" data-type="horizontal">
			<legend>防盗报警器 *</legend>
				<input type="radio" name="radioAlarm" id="radioAlarm0" value="0" onclick="" checked />
				<label for="radioAlarm0">关</label>
				<input type="radio" name="radioAlarm" id="radioAlarm1" value="1" onclick="" />
				<label for="radioAlarm1">警戒</label>
		</fieldset>
	</li>
	<li data-role="fieldcontain">
		<fieldset data-role="controlgroup" data-type="horizontal">
			<legend>煤气泄露报警器 *</legend>
				<input type="radio" name="radioAlarmGasLeak" id="radioAlarmGasLeak0" value="0" onclick="" />
				<label for="radioAlarmGasLeak0">关</label>
				<input type="radio" name="radioAlarmGasLeak" id="radioAlarmGasLeak1" value="1" onclick="" checked />
				<label for="radioAlarmGasLeak1">开</label>
		</fieldset>
	</li>	
	<li data-role="fieldcontain">
		<label for="btnSetClock">无线数字钟</label>
		<button id="btnSetClock" data-icon="gear" onclick="btnSetClockClicked();">设定时间</button>
	</li>

	
	<li data-role="fieldcontain">
		<label for="sliderHeater">客厅温度 *</label>
		<input type="range" name="sliderHeater" id="sliderHeater" onchange="showButton( 'btnLivingTempSet', true );" value="18" min="0" max="25" step="0.5" data-highlight="true" />
		<button id="btnLivingTempSet" data-icon="check" onclick="showButton( 'btnLivingTempSet', false );">设定</button>
	</li>

</ul>
<?php
include "include/templateFooter.php";
?>