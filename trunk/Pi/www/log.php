<html>
<head>
<title>智能家</title>
<meta http-equiv="content-type" content="text/html; charset=utf-8"></meta>
<style type="text/css">
body { width:1200px; margin:auto;  }
a { font-size: 80pt; }

</style>
</head>
<body>

<?php
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

echo("tabDataToNode<br>");
echo("<table border='1'>");
echo( "<tr><td>ID</td><td>NodeID</td><td>Channel</td><td>Addr1</td><td>Addr2</td><td>Addr3</td><td>Len</td><td>D1</td><td>D2</td><td>D3</td><td>D4</td><td>D5</td><td>D6</td><td>Updated By</td><td>Updated On</td><td>Remarks</td></tr>");
while ($row = $results->fetchArray()) {  
    //var_dump($row);
	//echo( $row[0] );
	//echo( "<br>");
	printf( "<tr><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%s</td><td>%s</td><td>%s</td></tr>", $row["fldID"], $row["fldNodeID"], $row["fldRFChannel"], $row["fldAddr1"], $row["fldAddr2"], $row["fldAddr3"], $row["fldDataLength"], $row["fldData1"], $row["fldData2"], $row["fldData3"], $row["fldData4"], $row["fldData5"], $row["fldData6"], $row["fldUpdatedBy"], $row["fldUpdatedOn"], $row["fldNodeRemarks"]);
} 
echo ("</table><br><br>");


$query = "select * from tabDataRecved order by fldID desc limit 50";
$results = $db->query($query);

echo("tabDataRecved<br>" );
echo("<table border='1'>");
echo( "<tr><td>ID</td><td>NodeID</td><td>DATA1</td><td>DATA2</td><td>DATA3</td><td>DATA4</td><td>DATA5</td><td>DATA6</td><td>DATA7</td><td>DATA8</td><td>DATA9</td><td>DATA10</td><td>DATA11</td><td>DATA12</td><td>DATA13</td><td>DATA14</td><td>DATA15</td><td>Created On</td></tr>");
while ($row = $results->fetchArray()) {  
    //var_dump($row);
	//echo( $row[0] );
	//echo( "<br>");
	printf( "<tr><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%s</td></tr>", $row["fldID"], $row["fldNodeID"], $row["fldData1"], $row["fldData2"], $row["fldData3"], $row["fldData4"], $row["fldData5"], $row["fldData6"], $row["fldData7"], $row["fldData8"], $row["fldData9"], $row["fldData10"], $row["fldData11"], $row["fldData12"], $row["fldData13"], $row["fldData14"], $row["fldData15"], $row["fldCreatedOn"]);
} 
echo ("</table>");

$db->close();
?>
</body>
</html>