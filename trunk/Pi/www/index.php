<?php
/*
日期        作者    备注
------------------------------------------
2014-MAR-30 黄长浩  增加“常用” 
*/
$gPageTitle = "智能家主页";
include "include/templateHeader.php";
?>
<ul data-role="listview" data-inset="true">
	<li data-role="list-divider">功能菜单</li>
		<li><a href="freqUsed.php">常用</a></li>
		<li><a href="realtimeStatus.php">实况</a></li>
		<li><a href="control.php" data-ajax="false">控制</a></li>
		<li><a href="presetMode.php" data-ajax="false">场景</a></li>
		<li><a href="cron.php" data-ajax="false">定时任务</a></li>
	<!--li data-role="list-divider">区域菜单</li>
		<li><a data-ajax="false" href="section/entrance.php">玄关</a></li>
		<li><a data-ajax="false" href="section/dining.php">餐厅</a></li>
		<li><a data-ajax="false" href="section/living.php">客厅</a></li>
		<li><a data-ajax="false" href="section/masterBed.php">主卧</a></li>
		<li><a data-ajax="false" href="section/secondaryBed.php">次卧</a></li>
		<li><a data-ajax="false" href="section/study.php">书房</a></li>

		<li><a data-ajax="false" href="section/outdoor.php">室外</a></li-->
</ul>
<?php
include "include/templateFooter.php";
?>