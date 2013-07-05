# python /home/pi/smartHome/dnsget/dnsget.py

import urllib,smtplib
from email.mime.text import MIMEText

def getMyIp():
    try:
        #return urllib.urlopen("http://www.instrument.com.cn/ip.aspx").read()
        return urllib.urlopen("http://myip.dnsdynamic.org/").read()
    except Exception, e:
        return "error"

def sendEmail(ipAddr, result):
    sender = 'haonotifier@yeah.net'
    receiver = 'hchanghao@163.com'
    subject = 'IP changed'
    smtpserver = 'smtp.yeah.net'
    username = 'haonotifier@yeah.net'
    password = 'ocdNew2'
    
    try:
        msg = MIMEText("The result is [" + result + "].\r\n\r\nThe new IP is http://" + ipAddr + ":9033", "plain", "utf-8")
        msg['Subject'] = subject
        msg['To']= receiver
        msg['From']=sender
        smtp = smtplib.SMTP()
        smtp.connect(smtpserver)
        smtp.login(username, password)
        smtp.sendmail(sender, receiver, msg.as_string())
        #print 'a-'+ msg.as_string()
        smtp.quit()
        smtp.close()
    except Exception, e:
        print str(e)

myIp=getMyIp()
if "error" in myIp:
    result = "Error happened when retrieving my IP."
else:
    try:
        result=urllib.urlopen("http://hchanghao:3303272@dynupdate.no-ip.com/nic/update?hostname=hao.myftp.org&myip=" + myIp).read()
        #result=urllib.urlopen("https://hchanghao@163.com:3303272q@www.dnsdynamic.org/api/?hostname=hao.dnsget.org&myip=" + myIp).read()
    except Exception, e:
        result="Error happened when updating IP address."

#print result
#print "good" in result
#print "nochg" in result

if "nochg" in result:
    print "no change"
    #sendEmail(myIp, result)
else:
    print "changed or something wrong"
    sendEmail(myIp, result)

