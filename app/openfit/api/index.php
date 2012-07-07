<?php

$base_url = (!empty($_SERVER['HTTPS']) && $_SERVER['HTTPS'] != 'off') ? 'https://' : 'http://';
$base_url .= $_SERVER['SERVER_NAME'];
if ($_SERVER['SERVER_PORT'] != 80) $base_url .= ':' . $_SERVER['SERVER_PORT'];

define('DRUPAL_ROOT', realpath(getcwd() . '/../..'));
require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
require_once DRUPAL_ROOT . '/includes/common.inc'; //Fixes module_load_include, etc.
drupal_bootstrap(DRUPAL_BOOTSTRAP_SESSION);
drupal_language_initialize();  //Fixes null global language issue

$operations = array(
  'export' => array(
    'file' => 'OpenFitExportHandler.class.php',
    'class' => 'OpenFitExportHandler',
    'method' => 'handle',
  ),
  'get_chart_data' => array(
    'file' => 'GetTrackDataHandler.class.php',
    'class' => 'GetTrackDataHandler',
    'method' => 'getChartData',
  ),
  'get_route_data' => array(
    'file' => 'GetTrackDataHandler.class.php',
    'class' => 'GetTrackDataHandler',
    'method' => 'getRouteData',
  ),
  'set_node_value' => array(
    'file' => 'NodeAccessHandler.class.php',
    'class' => 'NodeAccessHandler',
    'method' => 'setNodeValue',
  ),
  'get_shared' => array(
    'file' => 'OpenFitShareHandler.class.php',
    'class' => 'OpenFitShareHandler',
    'method' => 'getShared',
  ),
  'get_homepage_data' => array(
    'file' => 'OpenFitHomepageHandler.class.php',
    'class' => 'OpenFitHomepageHandler',
    'method' => 'getHomepageData',
  ),
);

header('Cache-control: no-cache, must-revalidate');
header('Expires: Mon, 01 Jan 2000 00:00:00 GMT');

$op = isset($_GET['op']) ? $_GET['op'] : (isset($_POST['op']) ? $_POST['op'] : null);

if (isset($operations[$op])) {
  $file = $operations[$op]['file'];
  $class = $operations[$op]['class'];
  $method = $operations[$op]['method'];
  
  try {
    include_once($file);
    call_user_func(array($class, $method));
  } catch (Exception $e) {
    switch ($e->getCode()) {
      case  403:
        header('HTTP/1.0 403 Forbidden');
        die($e->getMessage());
      default:
        header('HTTP/1.0 500 Internal Server Error');
        die('Exception encountered while handling request.  Code: '.$e->getCode().' Message: '.$e->getMessage());
    }
  }
} else {
    header('HTTP/1.0 500 Internal Server Error');
    die('Invalid op parameter: '.$op);
}
?>