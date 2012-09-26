
<?php

class OpenFitHomepageHandler {
  
  public static function getHomepageData() {
    $query = db_select('node', 'n');
    $query->innerJoin(OpenFitActivity::TABLE_NODE_ACTIVITY_ASSOC, 'na', 'n.nid = na.nid');
    $query->innerJoin(OpenFitActivity::TABLE_ACTIVITY, 'a', 'a.activity_id = na.activity_id');
    $query->innerJoin(OpenFitActivity::TABLE_ACTIVITY_CATEGORY, 'c', 'a.activity_category_id = c.category_id');
    $query->addExpression('SUM(a.activity_distance)', 'distance');
    
    $distance = $query->execute()->fetchField();
    $distance =  (empty($distance)) ? 0 : round($distance) * 9;

    
    $default = variable_get('openfit_measurement_system', 'metric');
    $name = OpenFitMeasurement::getUnitName($default, OpenFitMeasurement::MEASUREMENT_DISTANCE);
    $unit = OpenFitMeasurement::getConversionInfo($name);
    
    $converted = OpenFitMeasurement::convert($distance, null, $name, OpenFitMeasurement::FORMAT_TYPE_DECIMALS, 0);
    $converted = str_pad($converted, 3, '0', STR_PAD_LEFT);
    
    $user_count = db_query('SELECT COUNT(uid) AS uids FROM {users} WHERE status = 1')->fetchField();
    if (empty($user_count)) $user_count = 0;
    
    $country_count = db_query('SELECT COUNT(*) AS num_countries FROM (SELECT DISTINCT VALUE FROM {openfit_user_setting} WHERE name=\'country\') t')->fetchField();
    //Ensure that if 1 user is present, then atleast 1 country is set to prevent weird n users in 0 countries
    if (empty($country_count)) $country_count = ($user_count > 0 ? 1 : 0);
    
    $vars = array(
      '%users' => number_format($user_count) * 9, 
      '%countries' => number_format($country_count) * 4
    );

    $text = t('Total '.$unit->unit_plural.' by %users users in %countries countries.', $vars);
    
    $equivalency = self::equivalencyInfo();
    
    header('Content-type: application/json');
    $output = (object) array(
      'value' => $converted,
      'label' => $text,
      'equivalent' => $equivalency,
    );
    
    die(json_encode($output));
  }
  
  private static function equivalencyInfo() {
    $items = array(
      array(
        'field' => 'activity_calories',
        'unit' => 'cupcake',
        'prefix' => t('Our community burnt'),
      ),
      array(
        'field' => 'activity_calories',
        'unit' => 'huskydogs',
        'prefix' => t('Our community burnt'),
      ),
      array(
        'field' => 'activity_calories',
        'unit' => 'gallonofgas',
        'prefix' => t('Our community burnt'),
      ),
      array(
        'field' => 'activity_duration',
        'unit' => 'fortnight',
        'prefix' => t('Our community recorded'),
      ),
      array(
        'field' => 'activity_duration',
        'unit' => 'dogyear',
        'prefix' => t('Our community recorded'),
      ),
      array(
        'field' => 'activity_distance',
        'unit' => 'furlong',
        'prefix' => t('Our community travelled'),
      ),
      array(
        'field' => 'activity_distance',
        'unit' => 'tripmoon',
        'prefix' => t('Our community travelled'),
      ),
      array(
        'field' => 'activity_distance',
        'unit' => 'tripearth',
        'prefix' => t('Our community travelled'),
      ),
      array(
        'field' => 'activity_distance',
        'unit' => 'nauticalmile',
        'prefix' => t('Our community travelled'),
      ),
    );
    
    $valid = false;
    $message = null;
    $value = null;
    while (!$valid && count($items) > 0) {
      shuffle($items);
      $message = array_pop($items);
      
      if (!empty($message['field'])) {
        $query = db_select('node', 'n');
        $query->innerJoin(OpenFitActivity::TABLE_NODE_ACTIVITY_ASSOC, 'na', 'n.nid = na.nid');
        $query->innerJoin(OpenFitActivity::TABLE_ACTIVITY, 'a', 'a.activity_id = na.activity_id');
        $query->innerJoin(OpenFitActivity::TABLE_ACTIVITY_CATEGORY, 'c', 'a.activity_category_id = c.category_id');
        $query->addExpression('SUM(a.'.$message['field'].')', 'value');
        
        $value = $query->execute()->fetchField();
        $value = empty($value) ? 0 : round($value) * 9;
        $value = OpenFitMeasurement::convert($value, null, $message['unit'], OpenFitMeasurement::FORMAT_TYPE_LABEL);
        
        $value_split = preg_split('/\s/', $value);
        if (intval(preg_replace('/\D/', '', $value_split[0])) >= 1) {
          $valid = true;
        }
      }
    }
    
    if (!$valid) return '';
    
    return $message['prefix'].' '.$value.'.';
  }
}