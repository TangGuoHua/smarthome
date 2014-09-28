#!/bin/sh
# cd /home/pi/smartHome/clock
# echo '**' >> log.txt
# echo $(date) >> log.txt
sqlite3 /var/www/db/smarthome.sqlite3 "update tabDataToNode set fldData1=strftime('%H','now','localtime'), fldData2=strftime('%M', 'now', 'localtime'), fldData3=strftime('%S', 'now', 'localtime'), fldUpdatedOn=datetime('now', 'localtime'), fldUpdatedBy='script' where fldID=6"
