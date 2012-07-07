<?php
session_start();
if(isset($_FILES['fitfile']['tmp_name'])&&$_FILES['fitfile']['tmp_name']!=''){
	include("post.php");
	$res=postfit();
}
if(isset($_POST['email'])){
	include("auth.php");
	$logres=login($_POST['email'],$_POST['password']);
}
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>Untitled Document</title>
</head>

<body>
<?php
	if($res=='Upload Error'){?>
	<div align="center">Your file has been not posted successfully. Please try again</div>
	<?php }elseif($res=='File Error'){
	?>
	<div align="center">You have uploaded an invalid file. Please try again</div>
	<?php }elseif(isset($res)){?>
	<div align="center">Your file has been posted successfully!</div>
	<?php }?>
    <?php if(isset($logres)){
		if($logres){
		?>
	<div align="center">You have logged in successfully!</div>
	<?php }else{?>
    <div align="center">Login failed! Please try again</div>
    <?php }}?>
	<form action="" method="post" enctype="multipart/form-data" name="form1" id="form1">
      <?php if(!isset($_SESSION['token'])){?>
      <div align="center">You need to authenticate the account before you can upload files.</div>
	  <table width="100%" border="0" align="center" cellpadding="5">
	    <tr>
	      <td align="right">Email Address</td>
	      <td><label for="email"></label>
          <input type="text" name="email" id="email" /></td>
        </tr>
	    <tr>
	      <td align="right">Password:</td>
	      <td><label for="password"></label>
          <input type="password" name="password" id="password" /></td>
        </tr>
	    <tr>
	      <td align="right">&nbsp;</td>
	      <td>&nbsp;</td>
        </tr>
	    <tr>
	      <td align="right">&nbsp;</td>
	      <td><input type="submit" name="button2" id="button2" value="Login" /></td>
        </tr>
      </table>
      <?php }else{?>
	  <p>
	    <input type="file" name="fitfile" id="fitfile" />
      </p>
	  <p>
		<input type="submit" name="button" id="button" value="Submit" />
	  </p>
	</form>
    <p><a href="logout.php">logout</a></p>
    <?php }?>

</body>
</html>