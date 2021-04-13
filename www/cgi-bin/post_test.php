<html>
<body BGCOLOR=#ABCDEF>
<h1>TEST-CGI</h1>
<?php


echo "processing post-request\n";
#echo "Date is " . date('Y-m-d') . "<br>\n";
$rawdata = file_get_contents("php://input");
echo "raw data $rawdata\n";

echo "<br><br>META VARIABLE PASSED<br><br>\n";
$meta_var = var_export($_SERVER, true);
$meta_var = str_replace ("\n", "<br>", $meta_var);
echo $meta_var;
echo "<br>";

$var=$_SERVER["CONTENT_TYPE"];
if (!empty($var) && ($var == "application/x-www-form-urlencoded" || $var == "multipart/form-data") )
	foreach ($_POST as $key => $val) 
		echo  "$key => $val \n";
?>
</body>
</html>

