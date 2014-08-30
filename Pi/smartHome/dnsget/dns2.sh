#!/bin/bash
cd /home/pi/smartHome/dnsget


old_ip_file="./old_ip.txt"
new_ip_file="./new_ip.txt"

if [ ! -f "$old_ip_file" ]; then
	touch "$old_ip_file"
fi

old_ip=$(head -1 $old_ip_file)
wget -q --output-document=$new_ip_file http://myip.dnsdynamic.com/
new_ip=$(head -1 $new_ip_file)

if [ ! -n "$old_ip" ]
then
	echo $new_ip > $old_ip_file
elif [ -n "$old_ip" ] && [ "$old_ip" != "$new_ip" ] 
then
	echo $new_ip > $old_ip_file
	
	echo '**' >> log.txt
	echo $(date) >> log.txt
	echo "IP is changed." >> log.txt
	#wget -q --http-user='hchanghao' --http-password='X260APA' --output-file=result.txt http://dynupdate.no-ip.com/nic/update?hostname=haohome.myftp.org&myip=$new_ip
	curl --user hchanghao:X260APA http://dynupdate.no-ip.com/nic/update?hostname=haohome.myftp.org&myip=$new_ip
	echo "Done." >> log.txt

fi

#echo "Done." >> log.txt