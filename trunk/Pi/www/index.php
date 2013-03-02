<?php
$gPageTitle = "智能家主页";
include "include/templateHeader.php";
?>
<ul data-role="listview" data-inset="true">
	<li data-role="list-divider">功能菜单</li>
		<li><a href="control.php" data-ajax="false">控制</a></li>
		<li><a href="realtimeStatus.php">实况</a></li>
		
	<!--li data-role="list-divider">区域菜单</li>
		<li><a data-ajax="false" href="section/entrance.php">玄关</a></li>
		<li><a data-ajax="false" href="section/dining.php">餐厅</a></li>
		<li><a data-ajax="false" href="section/living.php">客厅</a></li>
		<li><a data-ajax="false" href="section/masterBed.php">主卧</a></li>
		<li><a data-ajax="false" href="section/secondaryBed.php">次卧</a></li>
		<li><a data-ajax="false" href="section/study.php">书房</a></li>

		<li><a data-ajax="false" href="section/outdoor.php">室外</a></li-->
</ul>
<?
include "include/templateFooter.php";
?>