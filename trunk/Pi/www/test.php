<?php
echo "hello <br>";

//exec("/bin/bash aunchInst.sh  $inst_type $inst_num $type",$results,$ret);
exec( "mpg123 'http://translate.google.com/translate_tts?tl=zh-cn&q=今天下午'",$output, $ret);
echo "Result:{$ret}";
print_r($output);

?>