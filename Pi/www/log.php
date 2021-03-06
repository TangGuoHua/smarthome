<html>
<head>
<title>智能家</title>
<meta http-equiv="content-type" content="text/html; charset=utf-8"></meta>
<style type="text/css">
body { width:1600px; margin:auto; font-family:Arial; font-size: 9px; }
table{ border-collapse:collapse;  border-spacing:0;  }
td {font-family:Arial; font-size: 9px; padding: 3; }
</style>
</head>
<body>

<?php

/*
日期        作者    备注
------------------------------------------
2014-SEP-23 黄长浩  增加tabDataSent，并修改一些页面样式
2014-SEP-23 黄长浩  修改页面以适应新的tabDataToNode和tabDataSent表结构
2014-OCT-05 黄长浩  tabDataSent表增加fldRequestor字段
*/

//phpinfo( );

/**
 * Simple example of extending the SQLite3 class and changing the __construct
 * parameters, then using the open method to initialize the DB.
 */
class MyDB extends SQLite3
{
    function __construct()
    {
        $this->open('/var/www/db/smarthome.sqlite3');
		$this->busyTimeout(3000); // set timeout to 3000ms
    }
}

$db = new MyDB();



//$db->exec('CREATE TABLE foo (bar STRING)');
//$db->exec("INSERT INTO foo (bar) VALUES ('This is a test')");

//$result = $db->query('SELECT bar FROM foo');
//var_dump($result->fetchArray());
$query = "select * from tabDataToNode order by fldNodeID";
$results = $db->query($query);

echo("<b>DataToNode</b><br>");
echo("<table border='1'>");
echo( "<tr><td>NodeID</td><td>DispOrder</td><td>Channel</td><td>RFPower</td><td>MaxRetry</td><td>A.Len</td><td>A1</td><td>A2</td><td>A3</td><td>A4</td><td>A5</td><td>D.Len</td><td>D1</td><td>D2</td><td>D3</td><td>D4</td><td>D5</td><td>D6</td><td>D7</td><td>D8</td><td>D9</td><td>D10</td><td>LastResult</td><td>Updated By</td><td>Updated On</td><td>Remarks</td></tr>");
while ($row = $results->fetchArray()) {  
    //var_dump($row);
	//echo( $row[0] );
	//echo( "<br>");
	printf( "<tr><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%s</td><td>%s</td><td>%s</td></tr>", $row["fldNodeID"], $row["fldDisplayOrder"], $row["fldRFChannel"], $row["fldRFPower"], $row["fldMaxRetry"], $row["fldAddrLength"], $row["fldAddr1"], $row["fldAddr2"], $row["fldAddr3"], $row["fldAddr4"], $row["fldAddr5"],$row["fldDataLength"], $row["fldData1"], $row["fldData2"], $row["fldData3"], $row["fldData4"], $row["fldData5"], $row["fldData6"], $row["fldData7"], $row["fldData8"], $row["fldData9"], $row["fldData10"], $row["fldLastSentResult"], $row["fldUpdatedBy"], $row["fldUpdatedOn"], $row["fldNodeRemarks"]);
} 
echo ("</table><br>");

echo ("<table><tr><td>");

	$query = "select * from tabDataRecved order by fldID desc limit 40";
	$results = $db->query($query);

	echo("<b>DataRecved</b><br>" );
	echo("<table border='1'>");
	echo( "<tr><td>ID</td><td>NodeID</td><td>D1</td><td>D2</td><td>D3</td><td>D4</td><td>D5</td><td>D6</td><td>D7</td><td>D8</td><td>D9</td><td>D10</td><td>D11</td><td>D12</td><td>D13</td><td>D14</td><td>D15</td><td>Created On</td></tr>");
	while ($row = $results->fetchArray()) {  
		//var_dump($row);
		//echo( $row[0] );
		//echo( "<br>");
		printf( "<tr><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%s</td></tr>", $row["fldID"], $row["fldNodeID"], $row["fldData1"], $row["fldData2"], $row["fldData3"], $row["fldData4"], $row["fldData5"], $row["fldData6"], $row["fldData7"], $row["fldData8"], $row["fldData9"], $row["fldData10"], $row["fldData11"], $row["fldData12"], $row["fldData13"], $row["fldData14"], $row["fldData15"], $row["fldCreatedOn"]);
	} 
	echo ("</table>");

echo ("</td><td width=40>&nbsp;</td><td>");

	$query = "select * from tabDataSent order by fldID desc limit 40";
	$results = $db->query($query);

	echo("<b>DataSent</b><br>" );
	echo("<table border='1'>");
	echo( "<tr><td>ID</td><td>ToNodeID</td><td>D1</td><td>D2</td><td>D3</td><td>D4</td><td>D5</td><td>D6</td><td>D7</td><td>D8</td><td>D9</td><td>D10</td><td>Requestor</td><td>Sent On</td><td>SentResult</td></tr>");
	while ($row = $results->fetchArray()) {  
		//var_dump($row);
		//echo( $row[0] );
		//echo( "<br>");
		printf( "<tr><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%s</td><td>%s</td><td>%d</td></tr>", $row["fldID"], $row["fldToNodeID"], $row["fldData1"], $row["fldData2"], $row["fldData3"], $row["fldData4"], $row["fldData5"], $row["fldData6"], $row["fldData7"], $row["fldData8"], $row["fldData9"], $row["fldData10"], $row["fldRequestor"], $row["fldSentOn"], $row["fldSentResult"]);
	} 
	echo ("</table>");

echo ("</td></tr></table>");

$db->close();
?>
</body>
</html>