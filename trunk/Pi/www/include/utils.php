<?php
class DBHelper extends SQLite3
{
    function __construct()
    {
        $this->open('/var/www/db/smarthome.sqlite3');
		$this->busyTimeout(3000); // set timeout to 3000ms
    }
}
?>