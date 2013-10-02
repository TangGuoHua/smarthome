<?php
header('cache-control: no-cache'); // preventing page from being cached.
include "../include/utils.php";

if( isset($_POST["nodeID"]))
{
	$nodeID=$_POST["nodeID"];

	echo "received data:". $nodeID ;

	if( $nodeID<>"" )
	{
		$isFirstParam = true;
		$sql = "UPDATE tabDataToNode SET ";
		
		for($i=1; $i<=10; $i++)
		{
			$paramName = "data".$i;
			if( isset($_POST[$paramName]) )
			{
				if( $isFirstParam )
				{
					//如果是第一个参数，无需加逗号分割，且要把标志设为false了
					$isFirstParam = false;
				}
				else
				{
					//如果不是第一个参数，则需要加逗号来分割
					$sql = $sql . ",";
				}
				$sql = $sql . "fldData".$i . "=" . $_POST[$paramName];
			}
		}
		
		if( $isFirstParam )
		{
			//如果isFirstParam还为真，那么说明data1, data2, ..., data10 一个都没有POST过来
			echo "Error!  At least one paramater (data1-10) must be posted.";
		}
		else
		{
			$sql = $sql . ", fldUpdatedBy='WEB_UI', fldUpdatedOn=datetime('now', 'localtime') WHERE fldNodeID=".$nodeID;
			//$sql = $sql . ", fldUpdatedBy='robot', fldUpdatedOn=datetime('now', 'localtime') WHERE fldNodeID1=".$nodeID1." AND fldNodeID2=".$nodeID2;
			echo $sql;
			
			$db = new DBHelper();
			$db->exec($sql);// or die("Error: " .sqlite_error_string(sqlite_last_error($conn)));
			$db->close();
			echo "OK";
		}
	}
}
else
{
	echo "Error, [nodeID] must be set";
}
?>