#!/bin/bash
cd /home/pi/smartHome/dnsget
echo '**' >> log.txt
echo $(date) >> log.txt

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
	echo "IP is changed." >> log.txt
	wget -q --http-user='hchanghao' --http-password='X260APA' http://dynupdate.no-ip.com/nic/update?hostname=haohome.myftp.org&myip=$new_ip
else
	echo "Same IP." >> log.txt
fi

echo "Done." >> log.txt
