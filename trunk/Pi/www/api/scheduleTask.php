<?php
header('cache-control: no-cache'); // preventing page from being cached.
include "../include/utils.php";

if( isset($_POST["fldID"]) ) //&& isset($_POST["nodeID2"]))
{
	$fldID=$_POST["fldID"];
	//$nodeID2=$_POST["nodeID2"];

	//echo "received data:". $nodeID1." -- ". $nodeID2 ;

	if( $fldID<>"" ) //&& $nodeID2<>"" )
	{

		$sql = "UPDATE tabScheduledTask SET fldScheduledDateTime=datetime('now', '+90 second', 'localtime'), fldTaskStatus=0, fldUpdatedBy='WEB_UI', fldUpdatedOn=datetime('now', 'localtime') WHERE fldID=".$fldID;
		echo $sql;
			
		$db = new DBHelper();
		$db->exec($sql);// or die("Error: " .sqlite_error_string(sqlite_last_error($conn)));
		$db->close();
		echo "OK";
	}
}
else
{
	echo "Error, fldID must be set";
}
?>