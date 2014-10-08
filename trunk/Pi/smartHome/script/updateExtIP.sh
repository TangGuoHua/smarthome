#!/bin/sh

# This script updates the external IP address of the system

cd /home/pi/smartHome/script
echo '**' >> log.txt
echo $(date) >> log.txt

curl http://changhao.sinaapp.com/smartHome/updateIP.php?xcode=haohome >> log.txt
