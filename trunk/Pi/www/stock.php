<?php
$gPageTitle = "每日实况";
include "include/templateHeader.php";
include "include/utils.php";

// 关于如何从Yahoo获取数据，参考下面的链接
// 股票 http://www.gummy-stuff.org/Yahoo-data.htm
// 股票 http://codeyun.com/zonghe/181.html
// 股票(指定日期) http://greenido.wordpress.com/2009/12/22/yahoo-finance-hidden-api/
// 股票 http://www.jarloo.com/yahoo_finance/
// 汇率 http://finance.yahoo.com/d/quotes.csv?s=USDCNY=X&f=d1l1
?>



<a href="stock.php" data-role="button" data-icon="refresh" data-ajax="false">更新</a>

<ul data-role="listview" data-inset="true" >

<li data-role='list-divider'>华夏债C</li>
<li>
<?php
$htmlData = file_get_contents("http://www.chinaamc.com/product/fundLishijingzhi.do?fundcode=001003");

//得到日期
$findme = '<td align="center" width="18%">';
$pos = strpos($htmlData, $findme);

// 注意这里使用的是 ===。简单的 == 不能像我们期待的那样工作，
// 因为 'a' 是第 0 位置上的（第一个）字符。
if ($pos === false) {
    echo "The string '$findme' was not found";
} else {
    // found

	$date = substr( $htmlData, $pos+ strlen($findme), 10 );

	//得到价格
	$findme = '<td align="left" width="16%">&nbsp;';
	$pos = strpos($htmlData, $findme);

	// 注意这里使用的是 ===。简单的 == 不能像我们期待的那样工作，
	// 因为 'a' 是第 0 位置上的（第一个）字符。
	if ($pos === false) {
		echo "The string '$findme' was not found";
	} else {
		// found

		//得到日期
		$findme2 = '</td>';
		$pos2 = strpos($htmlData, $findme2, $pos);

		// 注意这里使用的是 ===。简单的 == 不能像我们期待的那样工作，
		// 因为 'a' 是第 0 位置上的（第一个）字符。
		if ($pos === false) {
			echo "The string '$findme' was not found";
		} else {

			$price = substr( $htmlData, $pos+ strlen($findme), $pos2-$pos- strlen($findme) );

			echo number_format($price*720532.32 - 758000, 2);
			echo "<span class='ui-li-count'>$date</span>";

		}
	}

}
?>
</li>


<li data-role='list-divider'>ORCL</li>

<?php
$htmlData = file_get_contents("http://finance.yahoo.com/d/quotes.csv?s=ORCL&f=d1l1");

//得到日期
$findme = '"';
$pos = strpos($htmlData, $findme,5);

// 注意这里使用的是 ===。简单的 == 不能像我们期待的那样工作，
// 因为 'a' 是第 0 位置上的（第一个）字符。
if ($pos === false) {
    echo "The string '$findme' was not found";
} else {
    // found

	$date = substr( $htmlData, 1, $pos-1 );

	//得到价格
	$findme = ',';
	$pos = strpos($htmlData, $findme);

	// 注意这里使用的是 ===。简单的 == 不能像我们期待的那样工作，
	// 因为 'a' 是第 0 位置上的（第一个）字符。
	if ($pos === false) {
		echo "The string '$findme' was not found";
	} else {
		// found

		//得到日期
		$findme2 = '</td>';
		$pos2 = strpos($htmlData, $findme2, $pos);

		// 注意这里使用的是 ===。简单的 == 不能像我们期待的那样工作，
		// 因为 'a' 是第 0 位置上的（第一个）字符。
		if ($pos === false) {
			echo "The string '$findme' was not found";
		} else {

			$price = trim( substr( $htmlData, $pos+ strlen($findme) ) );
			echo '<li>USD '.$price."<span class='ui-li-count'>$date</span></li>";


			echo '<li>CNY '.number_format( (($price-29.72)*300 + ($price-30.11)*450 + $price*200 )*6.13 , 2);
			echo "<span class='ui-li-count'>$date</span></li>";
			echo "<li data-role='list-divider'>This Year</li>";

			echo '<li>CNY '.number_format( (($price-29.72)*150 + ($price-30.11)*150 )*6.13 , 2);
			echo "<span class='ui-li-count'>$date</span></li>";

		}
	}

}
?>

</ul>
<?php
include "include/templateFooter.php";
?>

