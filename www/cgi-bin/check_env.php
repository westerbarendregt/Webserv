<html>
<body BGCOLOR=#ABCDEF>
<h1>TEST-CGI</h1>
<?php
#echo "<br>\n";
##echo "Date is " . date('Y-m-d') . "<br>\n";
echo "<br><br>META VARIABLE PASSED<br><br>\n";

$meta_var = var_export($_SERVER, true);
$meta_var = str_replace ("\n", "<br>", $meta_var);

echo $meta_var;
echo "<br>";


?>
</body>
</html>

