<?php
class DBHelper extends SQLite3
{
    function __construct()
    {
        // Running on Pi
		$this->open('/var/www/db/smarthome.sqlite3');
		
		// Running on Laptop
		//$this->open('D:/Pri/SourceForgeProjects/SmartHome/Pi/www/db/smarthome.sqlite3');
		
		$this->busyTimeout(3000); // set timeout to 3000ms
    }
}
?>