<?php
function login($email=null,$password=null){
	//brian@sufferhub.com
	//BiteMe123!
	//set url for curl	
	$url="https://www.strava.com/api/v2/authentication/login";
	//set post fields
	$fields = array(
		'email'=>urlencode($email),
		'password'=>urlencode($password)    		
	);
foreach($fields as $key=>$value) { $fields_string .= $key.'='.$value.'&'; }
rtrim($fields_string,'&');
//set curl options
$options = array(
		CURLOPT_RETURNTRANSFER => true,     // return web page
		CURLOPT_HEADER         => false,    // don't return headers
		CURLOPT_FOLLOWLOCATION => true,     // follow redirects
		CURLOPT_ENCODING       => "",       // handle all encodings
		CURLOPT_USERAGENT      => "Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)", // who am i
		CURLOPT_AUTOREFERER    => true,     // set referer on redirect
		CURLOPT_CONNECTTIMEOUT => 120,      // timeout on connect
		CURLOPT_TIMEOUT        => 120,      // timeout on response
		CURLOPT_MAXREDIRS      => 10,       // stop after 10 redirects
		CURLOPT_SSL_VERIFYPEER => false,
		CURLOPT_POST		   => true,
		CURLOPT_POSTFIELDS	   => $fields_string,
	);
	//get curl results
	$ch      = curl_init( $url );
	curl_setopt_array( $ch, $options );
	$content = curl_exec( $ch );
	$err     = curl_errno( $ch );
	$errmsg  = curl_error( $ch );
	$header  = curl_getinfo( $ch );
	curl_close( $ch );
	
	//decode result
	$data=json_decode($content);
	if(isset($data->token)){//check if the authentication was a success
		//save auth data in a fsession
		$_SESSION['id']=$data->athlete->id;
		$_SESSION['token']=$data->token;
		return true;
	}else{
		return false;
	}
}

?>