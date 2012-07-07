<?php 

class OpenFitUserSetting {

  const TABLE_SETTING = 'openfit_user_setting';
  
  const TYPE_FULLNAME = 'fullname';
  const TYPE_LOCATION = 'location';
  const TYPE_DATE_OF_BIRTH = 'dateofbirth';
  const TYPE_SEX = 'sex';
  const TYPE_LANGUAGE = 'language';
  const TYPE_COUNTRY = 'country';
  const TYPE_TIMEZONE = 'timezone';
  const TYPE_UNITS = 'units';
  const TYPE_DISTANCE_UNITS = 'units-distance';
  const TYPE_SPEED_UNITS = 'units-speed';
  const TYPE_ELEVATION_UNITS = 'units-elevation';
  const TYPE_WEIGHT_UNITS = 'units-weight';
  const TYPE_TEMPERATURE_UNITS = 'units-temperature';
  const TYPE_CALORIE_UNITS = 'units-calorie';
  const TYPE_WEEK_START = 'week-start';
  const TYPE_DEFAULT_SHARING = 'default-sharing';
  
  const SEX_MALE = 'male';
  const SEX_FEMALE = 'female';
  
  const UNITS_METRIC = 'metric';
  const UNITS_US = 'us';
  
  public static function getSchema(&$schema) {
    $schema[self::TABLE_SETTING] = array(
      'description' => 'A table which contains settings for users.  Combo unique-index of uid and settings prevents double settings for a single user.',
      'fields' => array(
        'uid' => array(
          'description' => 'User id of the setting',
          'type' => 'int',
          'not null' => TRUE,
          'unsigned' => TRUE,
        ),
        'name' => array(
          'description' => 'Name of the setting',
          'type' => 'varchar',
          'not null' => TRUE,
          'length' => 32,
        ),
        'value' => array(
          'description' => 'Value of the setting',
          'type' => 'blob',
          'size' => 'big',
          'not null' => TRUE,
          'serialize' => TRUE,
        ),
      ),
      'indexes' => array(
        'uid' => array('uid'),
      ),
      'unique keys' => array(),
      'primary key' => array('uid', 'name'),
    );
  }
  
  // A cache of settings indexed by uid
  private static $cache = array();
  
  /**
   * Set the specified user setting value
   * @param int $uid User id
   * @param string $name Name of the setting
   * @param mixed $value The value
   */
  public static function set($uid, $name, $value) {
    if ($name == self::TYPE_LANGUAGE || $name == self::TYPE_TIMEZONE) {
      // Save setting to user data
      global $user;
      $set_user = $user;
      if ($set_user->uid != $uid) $set_user = user_load($uid);
      user_save($user, array($name => $value));
    } else {
      // Save setting to OpenFit
      $num_updated = db_update(self::TABLE_SETTING)->condition('uid', $uid)->condition('name', $name)->fields(array('value' => $value))->execute();
      if ($num_updated == 0) {
        $result = db_query('SELECT `value` FROM {' . self::TABLE_SETTING . '} WHERE `name` = :name AND `uid` = :user_id', array(':name' => $name, ':user_id' => $uid));
        if ($result->rowCount() == 0) {
          db_insert(self::TABLE_SETTING)->fields(array('uid' => $uid, 'name' => $name, 'value' => $value))->execute();
        }
      }
    }
    if (isset(self::$cache[$uid])) self::$cache[$uid][$name] = $value;
  }
  
  /**
   * Returns the specified named setting(s) for the user.
   * @param int $uid  User id
   * @param string $name Name of the setting
   * @returns mixed If name is specified, either the value or the default. If name is null, an sparse associative array indexed by name.
   */  
  public static function get($uid, $name = null, $default = null) {
    if (!isset(self::$cache[$uid])) {
      // Use system defaults first
      $settings = self::defaults();
      // Read stored user settings
      $result = db_query('SELECT `value`, `name` FROM {' . self::TABLE_SETTING . '} WHERE `uid` = :user_id', array(':user_id' => $uid));
      while ($row = $result->fetchObject()) {
        $settings[$row->name] = $row->value;
      }
      // Expand units not specified individually based on unit system selected
      $measurement_system = $settings[self::TYPE_UNITS];
      $systems = OpenFitMeasurement::getMeasurementSystems();
      if (isset($systems[$measurement_system])) {
        $system = $systems[$measurement_system];
        $mapping = array (
          self::TYPE_DISTANCE_UNITS => OpenFitMeasurement::MEASUREMENT_DISTANCE,
          self::TYPE_SPEED_UNITS => OpenFitMeasurement::MEASUREMENT_SPEED,
          self::TYPE_ELEVATION_UNITS => OpenFitMeasurement::MEASUREMENT_ELEVATION,
          self::TYPE_WEIGHT_UNITS => OpenFitMeasurement::MEASUREMENT_WEIGHT,
          self::TYPE_TEMPERATURE_UNITS => OpenFitMeasurement::MEASUREMENT_TEMPERATURE,
          self::TYPE_CALORIE_UNITS => OpenFitMeasurement::MEASUREMENT_CALORIE,
        );
        foreach ($mapping as $key => $value) {
          if (!isset($settings[$key])) $settings[$key] = $system->units[$value];
        }
      }
      self::$cache[$uid] = $settings;
    }
    
    // Always load language and timezone from user data
    $language = '';
    $timezone = '';
    global $user;
    $get_user = $user;
    if ($get_user->uid != $uid) $get_user = user_load($uid);
    if (isset($get_user) && $get_user->uid != 0) {
      $language = $get_user->language;
      $timezone = $get_user->timezone;
    }
    // Default from system if not set
    if ($language == '') {
      $default_language = language_default();
      if (isset($default_language)) $language = $default_language->language;
    }
    if ($timezone == '') $timezone = variable_get('date_default_timezone', $timezone);
    // Set the cache
    if (isset($language) && strlen($language) > 0) self::$cache[$uid][self::TYPE_LANGUAGE] = $language;
    if (isset($timezone) && strlen($timezone) > 0) self::$cache[$uid][self::TYPE_TIMEZONE] = $timezone;

    if ($name == null) {
      return self::$cache[$uid];
    } else if (isset(self::$cache[$uid][$name])) {
      return self::$cache[$uid][$name];
    } else {
      return $default;
    }
  }
  
  public static function getCurrentUserLocale() {
    global $user;
    $settings = self::get($user->uid);
    return $settings[self::TYPE_LANGUAGE] . '-' . $settings[self::TYPE_COUNTRY];
  }
  
  private static function defaults() {
    $language = '';
    $default_language = language_default();
    if (isset($default_language)) $language = $default_language->language;
    return array(
      self::TYPE_FULLNAME => '',
      self::TYPE_LOCATION => '',
      self::TYPE_DATE_OF_BIRTH => '',
      self::TYPE_SEX => '',
      self::TYPE_LANGUAGE => $language,
      self::TYPE_COUNTRY => variable_get('site_default_country','US'),
      self::TYPE_TIMEZONE => variable_get('date_default_timezone', 'America/New_York'),
      self::TYPE_UNITS => variable_get('openfit_measurement_system','metric'),
      self::TYPE_WEEK_START => variable_get('date_first_day', 1),
      self::TYPE_DEFAULT_SHARING => 0,
    );
  }
}