<?php
/*
日期        作者    备注
------------------------------------------
2014-SEP-28 黄长浩  初始版本
*/

$gPageTitle = "预设模式";
include "include/templateHeader.php";
include "include/utils.php";
?>

<script>
function setMode( modeName )
{

	if( modeName == 'Dinner' ) //晚餐
	{
		$.post("/api/sendData.php", { nodeID: "41", data1: 1, data3:1 } ); //开餐厅射灯
		$.post("/api/sendData.php", { nodeID: "51", data4:1 } ); //开台灯
		$.post("/api/sendData.php", { nodeID: "53", data1:1 } ); //开客厅落地灯
	}
	else if( modeName == 'WatchTV' ) // 看电视
	{
		$.post("/api/sendData.php", { nodeID: "41", data1:0, data2:0, data3:0, data4:0 } ); //关餐厅射灯
		$.post("/api/sendData.php", { nodeID: "51", data4:1, data5:1 } ); //开台灯 天猫盒子
		$.post("/api/sendData.php", { nodeID: "53", data1:0 } ); //关客厅落地灯
		$.post("/api/sendData.php", { nodeID: "200", data2: 1 } ); //书房LED灯带
	}
	else if( modeName == 'Sleep' ) // 睡觉
	{
		$.post("/api/sendData.php", { nodeID: "41", data1:0, data2:0, data3:0, data4:0 } ); //关餐厅射灯
		$.post("/api/sendData.php", { nodeID: "51", data4:0, data5:0, data6:0, data7:0, data8:0 } ); //关电视墙上的电器
		$.post("/api/sendData.php", { nodeID: "53", data1:0 } ); //关客厅落地灯
		$.post("/api/sendData.php", { nodeID: "200", data2: 0 } ); //关书房LED灯带
		$.post("/api/sendData.php", { nodeID: "31", data1: 0 } ); //关书房台灯
	}
}
</script>

<a href="index.php" data-role="button" data-icon="arrow-l">返回</a>
<a href="presetMode.php" data-role="button" data-icon="refresh" data-ajax="false">更新</a>

<ul data-role="listview" data-inset="true">
	<li data-role="fieldcontain">
		<fieldset data-role="controlgroup" data-type="horizontal">
			<legend>预设场景模式</legend>
				<!--input type="radio" name="rdoPresetMode" id="rdoPresetMode0" value="0" onclick="rdoPresetModeClicked(0);" />
				<label for="rdoPresetMode0">起床</label>
				<input type="radio" name="rdoPresetMode" id="rdoPresetMode1" value="0" onclick="rdoPresetModeClicked(0);" />
				<label for="rdoPresetMode1">早餐</label-->

				<input type="radio" name="rdoMode" id="rdoModeDinner" onclick="setMode('Dinner');" />
				<label for="rdoModeDinner">晚餐</label>

				<input type="radio" name="rdoMode" id="rdoModeWatchTV" onclick="setMode('WatchTV');" />
				<label for="rdoModeWatchTV">看电视</label>

				<input type="radio" name="rdoMode" id="rdoModeSleep" onclick="setMode('Sleep');" />
				<label for="rdoModeSleep">睡觉</label>

				<!--input type="radio" name="rdoPresetMode" id="rdoPresetMode5" value="0" onclick="rdoPresetModeClicked(0);" />
				<label for="rdoPresetMode5">外出</label-->
		</fieldset>
	</li>
</ul>
<?php
include "include/templateFooter.php";
?>