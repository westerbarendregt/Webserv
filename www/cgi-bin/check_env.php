<?php
$data["post_data"]=file_get_contents("php://input");
foreach ($_SERVER as $key => $val)
	$data[$key] = $val;
$var=$_SERVER["CONTENT_TYPE"];
if (!empty($var) && ($var == "application/x-www-form-urlencoded" || $var == "multipart/form-data") ) {
	foreach ($_POST as $key => $val)  {
		$data["post_parsed_" . $key]=$val;
	}
}
$json=json_encode($data);
echo $json;
?>
