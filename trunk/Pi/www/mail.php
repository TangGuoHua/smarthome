<html>
<head>
	<title>智能家</title>
	<meta http-equiv="content-type" content="text/html; charset=utf-8"></meta>
	<style type="text/css">
		body { width:1300px; margin:8px; }

	</style>

</head>
<body>


<?php

require("include/class.phpmailer.php"); //下载的文件必须放在该文件所在目录

$mail = new PHPMailer(); //建立邮件发送类

$mail->IsSMTP(); // 使用SMTP方式发送//
$mail->Host = "smtp.189.cn"; // 您的企业邮局域名
$mail->SMTPAuth = true; // 启用SMTP验证功能
$mail->Username = "huangchanghao@189.cn"; // 邮局用户名(请填写完整的email地址)
$mail->Password = "3303272q"; // 邮局密码
$mail->Port=25;
$mail->From = "huangchanghao@189.cn"; //邮件发送者email地址
$mail->FromName = "Changhao";
$mail->AddAddress("hchanghao@163.com", "a");//收件人地址，可以替换成任何想要接收邮件的email信箱,格式是AddAddress("收件人email","收件人姓名")
//$mail->AddReplyTo("", "");
//$mail->AddAttachment("/var/tmp/file.tar.gz"); // 添加附件
//$mail->IsHTML(true); // set email format to HTML //是否使用HTML格式

$mail->Subject = "PHPMailer测试邮件"; //邮件标题
$mail->Body = "Hello,这是测试邮件"; //邮件内容
//$mail->AltBody = "This is the body in plain text for non-HTML mail clients"; //附加信息，可以省略
if(!$mail->Send())
{
	echo "邮件发送失败. <p>";
	echo "错误原因: " . $mail->ErrorInfo;
	exit;
}

echo "邮件发送成功";
?>

</BODY>
</HTML>	