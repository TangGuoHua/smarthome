<?php
/*
日期        作者    备注
------------------------------------------
2013-APR-09 黄长浩  增加了调光台灯控制滑条
2013-APR-29 黄长浩  修改蓝绿调光LED为主卧台灯，并增加延时关功能
2013-MAY-17 黄长浩  主卧调光台灯移到书房
2013-OCT-02 黄长浩  根据新家schema做相应修改
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
	//showButton( "btnSetSecondaryBedTemp", false );
	showButton( "btnSetStudyroomLamp", false );
});


/***********各个区域控制***********/

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


// study room lamp

function rdoStudyRoomDeskLampClicked(val)
{
	//alert(val);
	$.post("/api/sendData.php", { nodeID: "201", data1: val } );
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
$sql = "select * from tabDataToNode";
$results = $db->query($sql);

while ($row = $results->fetchArray()) 
{
	// 卫生间
	if( $row["fldNodeID"]==91 )
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
			<label for="rangeBathroomLightOnThreshold">卫生间灯亮阈值</label>
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