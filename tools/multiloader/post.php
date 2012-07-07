<?php
function postfit(){
	//get file extension first. Compatible with php4 and above
	$filext=getfileExt($_FILES['fitfile']['name']);
	//only continue id file extension is .fit or .tcx (case insensitive)
	if($filext=="fit"||$filext=="tcx"){
		//save the uploaded file with the date attached to the name
		$newfilename="uploads/".date("Y-m-d")."_".$_FILES['fitfile']['name'];
		move_uploaded_file($_FILES['fitfile']['tmp_name'],$newfilename);
		//read data from the file
		$handle=fopen($newfilename,'r');
		$data=fread($handle,filesize($newfilename));
		fclose($handle);
		
		
		
		
		//do a curl call with post data
		$url="http://www.strava.com/api/v2/upload";
		$fields = array(
			'token'=>urlencode($_SESSION['token']),
			'id'=>urlencode($_SESSION['id']),
			'data'=>urlencode($data),
			'type'=>urlencode($filext)
		);
		$fields_string='';
	foreach($fields as $key=>$value) { $fields_string .= $key.'='.$value.'&'; }
	rtrim($fields_string,'&');
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
		
		$ch      = curl_init( $url );
		curl_setopt_array( $ch, $options );
		$content = curl_exec( $ch );
		$err     = curl_errno( $ch );
		$errmsg  = curl_error( $ch );
		$header  = curl_getinfo( $ch );
		curl_close( $ch );
		$data=json_decode($content);
		//print_r($data);
		if(isset($data->upload_id)){
			return $data->upload_id;
		}
		else{
			return("Upload Error");
		}
	}
	else{
		return "File Error";
	}
}
function getfileExt($path){
	$pathexp=explode(".",$path);
	$ext=$pathexp[(count($pathexp)-1)];
	return strtolower($ext);
}
?>