#!/bin/sh
cd /home/pi/smartHome/dnsget
echo '**' >> log.txt
echo $(date) >> log.txt
wget -q --output-document=myip.txt http://myip.dnsdynamic.com/
cat myip.txt >> log.txt
echo -n '  ' >> log.txt
echo 'https://www.dnsdynamic.org/api/?hostname=hao.dnsget.org&myip='$(cat myip.txt)> url.txt
wget -q --http-user='hao@yahoo.com' --http-password='3303_dat66' --append-output=log.txt --output-document=result.txt --input-file=url.txt 
cat result.txt >> log.txt

