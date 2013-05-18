# python /home/pi/smartHome/dnsget/dnsget.py
import urllib,smtplib
from email.mime.text import MIMEText

def getMyIp():
    return urllib.urlopen("http://www.instrument.com.cn/ip.aspx").read()
    #return urllib.urlopen("http://myip.dnsdynamic.org/").read()

def sendEmail(ipAddr, result):
    sender = 'hchanghao@163.com'
    receiver = 'hchanghao@163.com'
    subject = 'IP changed'
    smtpserver = 'smtp.163.com'
    username = 'hchanghao@163.com'
    password = '3303272q'
    try:
        msg = MIMEText("<html>The new IP is <a href='" + ipAddr + ":9033'>" + ipAddr + ":9033</a><br><br>The result is [" + result + "].<html>", "html", "utf-8")
        msg['Subject'] = subject
        smtp = smtplib.SMTP()
        smtp.connect(smtpserver)
        smtp.login(username, password)
        smtp.sendmail(sender, receiver, msg.as_string())
	#print 'a-'+ msg.as_string()
        smtp.quit()
        return True
    except Exception, e:
        print str(e)
        return False

myIp=getMyIp()
#result=urllib.urlopen("https://hchanghao@163.com:3303272q@www.dnsdynamic.org/api/?hostname=hao.dnsget.org&myip=" + myIp).read()
result=urllib.urlopen("http://hchanghao:3303272@dynupdate.no-ip.com/nic/update?hostname=hao.myftp.org&myip=" + myIp).read()

#print result
#print "good" in result
#print "nochg" in result

if "nochg" in result:
    print "no change"
else:
    print "changed or something wrong"
    sendEmail(myIp, result)

