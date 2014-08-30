<?php
/*
日期        作者    备注
------------------------------------------
2014-JAN-27 黄长浩  增加卫生间亮度
2014-FEB-05 黄长浩  增加卫生间温度
                    增加南卧温湿度
                    画图的点数由30增加到33
2014-AUG-30 黄长浩  增加书房温度气压亮度
*/

$gPageTitle = "图";
include "include/templateHeader.php";
include "include/utils.php";
?>

<a href="realtimeStatus.php" data-role="button" data-icon="arrow-l">返回</a>
<!--a href="realtimeStatus.php" data-role="button" data-icon="refresh" data-ajax="false">更新</a-->

<?php

$db = new DBHelper();

if( isset($_REQUEST["param"]) )
{
	$param=$_REQUEST["param"];
}
else
{
	$param="1";
}

switch($param)
{

	
	case "22": //厨房温度亮度
		$query = "select fldData7||'.'||fldData8 as s1, fldData2 as s2, fldCreatedOn from tabDataRecved where fldNodeID=22";
		$chartTitle = "厨房温度亮度";
		$s1Title="温度";
		$s1Unit="度";
		$s2Title="亮度";
		$s2Unit="";
		break;

	case "31-1": //书房温度亮度
		$query = "select (fldData4*256+fldData5)/10.0 as s1, fldData12*256+fldData13 as s2, fldCreatedOn from tabDataRecved where fldNodeID=31 and fldData1=11";
		$chartTitle = "书房温度亮度";
		$s1Title="温度";
		$s1Unit="度";
		$s2Title="亮度";
		$s2Unit="Lux";
		break;

	case "31-2": //书房气压
		$query = "select fldData7*65536+fldData8*256+fldData9 as s1, 0 as s2, fldCreatedOn from tabDataRecved where fldNodeID=31 and fldData1=11";
		$chartTitle = "书房气压";
		$s1Title="气压";
		$s1Unit="帕";
		$s2Title="";
		$s2Unit="";
		break;



	case "54": //客厅温湿度
		$query = "select fldData4||'.'||fldData5 as s1, fldData2||'.'||fldData3 as s2, fldCreatedOn from tabDataRecved where fldNodeID=54";
		$chartTitle = "客厅温湿度";
		$s1Title="温度";
		$s1Unit="度";
		$s2Title="湿度";
		$s2Unit="%";
		break;
	case "82": //南卧温湿度
		$query = "select fldData4||'.'||fldData5 as s1, fldData2||'.'||fldData3 as s2, fldCreatedOn from tabDataRecved where fldNodeID=82";
		$chartTitle = "南卧温湿度";
		$s1Title="温度";
		$s1Unit="度";
		$s2Title="湿度";
		$s2Unit="%";
		break;
	case "91": //卫生间亮度
		$query = "select fldData2 as s1, 0 as s2, fldCreatedOn from tabDataRecved where fldNodeID=91";
		$chartTitle = "卫生间亮度";
		$s1Title="亮度";
		$s1Unit="";
		$s2Title="";
		$s2Unit="";
		break;

	case "92": //卫生间温度
		$query = "select fldData3||'.'||fldData4 as s1, 0 as s2, fldCreatedOn from tabDataRecved where fldNodeID=92";
		$chartTitle = "卫生间温度";
		$s1Title="温度";
		$s1Unit="度";
		$s2Title="";
		$s2Unit="";
		break;


	case "5-2": //室外温度亮度
		$query = "select CASE WHEN fldData7 = 1 THEN '-'||fldData5||'.'||fldData6 ELSE fldData5||'.'||fldData6 END as s1, fldData1 as s2, fldCreatedOn from tabDataHistory where fldNodeID2=5";
		$chartTitle = "室外温度亮度";
		$s1Title="温度";
		$s1Unit="度";
		$s2Title="亮度";
		$s2Unit="";
		break;
	case "55": // 大门
		$query = "select fldData1 as s1, fldData1 as s2, fldCreatedOn from tabDataHistory where fldNodeID2=55";
		$chartTitle = "大门";
		$s1Title="大门";
		$s1Unit="";
		$s2Title="大门";
		$s2Unit="";
		break;
		

		
	case "7-2": //客厅耗电量
		$query = "select (fldData7*16777216+fldData8*65536+fldData9*256+fldData10)/1600.0 as s1, 0 as s2, fldCreatedOn from tabDataHistory where fldNodeID2=7";
		$chartTitle = "油汀落地灯耗电量";
		$s1Title="耗电量";
		$s1Unit="度";
		$s2Title="";
		$s2Unit="";
		break;


		
	case "15": //主卧亮度
		$query = "select fldData3||'.'||fldData4 as s1, fldData1 as s2, fldCreatedOn from tabDataHistory where fldNodeID2=15";
		$chartTitle = "主卧温度亮度";
		$s1Title="温度";
		$s1Unit="度";
		$s2Title="亮度";
		$s2Unit="";
		break;

	case "18": //次卧温度
		$query = "select fldData6||'.'||fldData7 as s1, fldData4 s2, fldCreatedOn from tabDataHistory where fldNodeID2=18";
		$chartTitle = "次卧温度";
		$s1Title="温度";
		$s1Unit="度";
		$s2Title="湿度";
		$s2Unit="%";
		break;
}

//$query = $query." and fldID%3=0 order by fldID desc limit 33";
$query = $query." order by fldID desc limit 33";

$results = $db->query($query);


$datetime = "";
$dataS1 ="";
$dataS2="";
$isFirstRow = true;
$recordCnt = 0;

while ($row = $results->fetchArray()) 
{  
	if( ++$recordCnt > 1 )
	{
		$datetime = ",".$datetime;
		$dataS1 = ",".$dataS1;
		$dataS2 = ",".$dataS2;
	}
		
	$datetime = "'".$row["fldCreatedOn"]."'".$datetime;
	$dataS1 = $row["s1"].$dataS1;
	$dataS2 = $row["s2"].$dataS2;
} 
$db->close();
?>

<!-- 1. Add these JavaScript inclusions in the head of your page -->
<script type="text/javascript" src="js/HighCharts/jquery.min.js"></script>
<script type="text/javascript" src="js/HighCharts/highcharts.js"></script>
	
<!-- 3. Add the container -->
<div style="height: 20px;"></div>
<div id="container" style="width: 480px; height: 360px; margin: 0 auto"></div>

<!-- 2. Add the JavaScript to initialize the chart on document ready -->
<script type="text/javascript">

	var chart;
	$(document).ready(function() {
		chart = new Highcharts.Chart({
			chart: {
				renderTo: 'container'
				//zoomType: 'xy'
			},
			title: {
				text: '<?php echo $chartTitle; ?>'
			},
			subtitle: {
				//text: '<%=txtFromDate%>,<%=txtFromTime%> ~ <%=txtToDate%>,<%=txtToTime%>'
			},

			credits: {
				enabled: false
			},

			xAxis: [{
				//categories: ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec']
				categories:[ <?php echo $datetime; ?> ],
				labels: {   enabled: false,
							step: <?php echo floor( $recordCnt/5 );?>
						},
				//tickWidth: 10,
				showFirstLabel: false,
				tickmarkPlacement: "on"


				//type: 'datetime',
				/*dateTimeLabelFormats: {
										second: '%H:%M:%S',
										minute: '%H:%M',
										hour: '%H:%M',
										day: '%e. %b',
										week: '%e. %b',
										month: '%b \'%y',
										year: '%Y'  
										}*/

			}],
			yAxis: [{ // Secondary yAxis
				
				title: {
					text: '<?php echo $s1Title; ?>',
					style: {
						color: '#4572A7'
					}
				},
				labels: {
					formatter: function() {
						return this.value +'<?php echo $s1Unit;?>';
					},
					style: {
						color: '#4572A7'
					}
				}
				
			},{ // Primary yAxis
				labels: {
					formatter: function() {
						return this.value +'<?php echo $s2Unit;?>';
					},
					style: {
						color: '#89A54E'
					}
				},
				gridLineWidth: 0,
				title: {
					text: '<?php echo $s2Title; ?>',
					style: {
						color: '#89A54E'
					}
				},
				opposite: true
				
			} ],
			tooltip: {
				formatter: function() {
					var unit = {
						'<?php echo $s1Title; ?>': '<?php echo $s1Unit;?>',
						'<?php echo $s2Title; ?>': '<?php echo $s2Unit;?>'
					}[this.series.name];
					
					return ''+
						this.x +', '+ this.y +''+ unit;
				}
			},
			legend: {
				layout: 'vertical',
				align: 'right',
				x: -70,
				verticalAlign: 'top',
				y: -10,
				floating: true,
				backgroundColor: '#FFFFFF'
			},
			series: [{
				name: '<?php echo $s1Title; ?>',
				color: '#4572A7',
				type: 'spline',
				//data: [7.0, 6.9, 9.5, 14.5, 18.2, 21.5, 25.2, 26.5, 23.3, 18.3, 13.9, 9.6]
				data: [ <?php echo $dataS1; ?> ]
				//data: [ [Date.UTC(2010, 0, 1, 13, 0, 30), 3.0], [Date.UTC(2010, 0, 2), 12], [Date.UTC(2010, 0, 3), 15], [Date.UTC(2010, 0, 5), 11] ]
			},{
				name: '<?php echo $s2Title; ?>',
				color: '#89A54E',
				type: 'spline',
				yAxis: 1,
				//data: [99.9, 95, 96.4, 99.2, 94.0, 96.0, 95.6, 98.5, 96.4, 94.1, 95.6, 94.4]		
				data: [<?php echo $dataS2; ?>]

				//data: [ [Date.UTC(2010, 0, 1), 63.0], [Date.UTC(2010, 0, 2), 72], [Date.UTC(2010, 0, 3), 55], [Date.UTC(2010, 0, 4), 71] ]
			
			}]
		});
		
		
	});
		
</script>

<?php
include "include/templateFooter.php";
?>