<?php


$rawdata = file_get_contents("php://input");
echo "raw data : $rawdata\n";

#$meta_var = var_export($_SERVER, true);
#echo "$meta_var\n";
$qstring = $_SERVER["QUERY_STRING"];
$pinfo =$_SERVER["PATH_INFO"];
echo "\n";
echo "QUERY_STRING = $qstring\n";
echo "PATH_INFO = $pinfo\n";
echo "\n";
$var=$_SERVER["CONTENT_TYPE"];
echo "parsed by script:\n";
if (!empty($var) && ($var == "application/x-www-form-urlencoded" || $var == "multipart/form-data") )
	foreach ($_POST as $key => $val) 
		echo  "$key => $val \n";
?>
