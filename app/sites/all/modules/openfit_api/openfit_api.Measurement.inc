<?php 

class OpenFitMeasurement{

  const TABLE_UNIT = 'openfit_unit';
  
  const SYSTEM_METRIC = 'metric';
  const SYSTEM_US = 'us';
  
  const MEASUREMENT_DISTANCE = 'distance';
  const MEASUREMENT_SPEED = 'speed';
  const MEASUREMENT_ELEVATION = 'elevation';
  const MEASUREMENT_WEIGHT = 'weight';
  const MEASUREMENT_TEMPERATURE = 'temperature';
  const MEASUREMENT_CALORIE = 'calorie';
  
  const DEFAULT_DISTANCE_UNITS = 'kilometer';
  const DEFAULT_SPEED_UNITS = 'kilometer-per-hour';
  const DEFAULT_ELEVATION_UNITS = 'meter';
  const DEFAULT_WEIGHT_UNITS = 'kilogram';
  const DEFAULT_TEMPERATURE_UNITS = 'celsius';
  const DEFAULT_CALORIE_UNITS = 'kilojoule';
  
  const FORMAT_TYPE_NONE = 'none';
  const FORMAT_TYPE_DECIMALS = 'decimals';
  const FORMAT_TYPE_LABEL = 'label';
  const FORMAT_TYPE_LABEL_CAPS = 'caps';
  const FORMAT_TYPE_SYMBOL = 'symbol';
  
  const UNITS_CALORIE = 'calorie';
  
  public static function getSchema(&$schema) {
    $schema[self::TABLE_UNIT] = array(
      'description' => 'A table that relates a unit type in various systems to their names & conversion factors.',
      'fields' => array(
        'unit_id' => array(
          'description' => 'Machine readable unique id.',
          'type' => 'varchar',
          'not null' => TRUE,
          'length' => 32,
        ),
        'unit_type' => array(
          'description' => 'Type of measurement.',
          'type' => 'varchar',
          'not null' => TRUE,
          'length' => 32,
        ),
        'unit_single' => array(
          'description' => 'Localizable singular name.',
          'type' => 'varchar',
          'not null' => TRUE,
          'length' => 64,
        ),
        'unit_plural' => array(
          'description' => 'Localizable plural name.',
          'type' => 'varchar',
          'not null' => TRUE,
          'length' => 64,
        ),
        'unit_symbol' => array(
          'description' => 'Localizable shorthand symbol (abbreviation).',
          'type' => 'varchar',
          'not null' => TRUE,
          'length' => 64,
        ),        
        'unit_decimals' => array(
          'description' => 'Default number of digits after the decimal.',
          'type' => 'int',
          'not null' => TRUE,
          'size' => 'small',
        ),        
        'conversion_factor' => array(
          'description' => 'Factor to multiply the value by to convert to SI units',
          'type' => 'float',
          'not null' => TRUE,
          'default' => 0,
        ),
        'conversion_offset' => array(
          'description' => 'Factor to add to the value to convert to SI units',
          'type' => 'float',
          'not null' => TRUE,
          'default' => 0,
        ),
      ),
      'primary key' => array('unit_id'),
    );
  }

  /**
   * Get information about the measurement systems (eg "US" or "Metric").
   *
   * Returns an associative array indexed by a SYSTEM_* constant that consists of:
   *   - title: A non-localized system title
   *   - units: An associative array indexed by MEASUREMENT_* which indicates the default units.
   */
  public static function getMeasurementSystems() {
    return array(
      self::SYSTEM_METRIC => (object)array(
        'title' => 'Metric',
        'units' => array(
          self::MEASUREMENT_DISTANCE => 'kilometer',
          self::MEASUREMENT_SPEED => 'kilometer-per-hour',
          self::MEASUREMENT_ELEVATION => 'meter',
          self::MEASUREMENT_WEIGHT => 'kilogram',
          self::MEASUREMENT_TEMPERATURE => 'celsius',
          self::MEASUREMENT_CALORIE => 'kilojoule',
        ),
      ),
      self::SYSTEM_US => (object)array(
        'title' => 'US',
        'units' => array(
          self::MEASUREMENT_DISTANCE => 'mile',
          self::MEASUREMENT_SPEED => 'mile-per-hour',
          self::MEASUREMENT_ELEVATION => 'foot',
          self::MEASUREMENT_WEIGHT => 'pound',
          self::MEASUREMENT_TEMPERATURE => 'fahrenheit',
          self::MEASUREMENT_CALORIE => 'calorie',
        ),
      ),
    );
  }
  
  /**
    * Get the measurement units for the system.
    */
  public static function getUnitName($system, $unit) {
    $systems = self::getMeasurementSystems();
    return $systems[$system]->units[$unit];
  }
  
  /**
   * Convert a value from one unit system to another and optionally format the output.
   *
   * @param $value
   *   The value to convert from.
   * @param $units_from
   *   The units to convert from.
   * @param $units_to
   *   The units to convert to.
   * @param $format
   *   Optional parameter to format the output by rounding decimals and adding a unit label (eg "1.00 mi").
   * @param $decimals
   *   Number of decimals to output if $format is something other than FORMAT_TYPE_NONE. If null, use the default unit decimals.
   */
  public static function convert($value, $units_from, $units_to, $format = self::FORMAT_TYPE_NONE, $decimals = null) {
    if (!is_numeric($value)) return null;

    // TODO: Cache entire table to avoid database queries??
    
    // Fetch the from and to unit types
    $rows = db_query(
      'SELECT
        `unit_id`,`unit_single`,`unit_plural`,`unit_symbol`,`unit_decimals`,`conversion_factor`,`conversion_offset`
      FROM {' . self::TABLE_UNIT . '} 
        WHERE `unit_id` = :from OR `unit_id` = :to',
      array(':from' => $units_from, ':to' => $units_to))
      ->fetchAllAssoc('unit_id');
      
    // Check for valid units. If either fail keep processing with raw values
    if (!isset($rows[$units_from])) watchdog('openfit_api','Cannot find unit id "' . $units_from . '" to convert from.', null, WATCHDOG_ERROR);
    if (!isset($rows[$units_to])) watchdog('openfit_api','Cannot find unit id "' . $units_to . '" to convert to.', null, WATCHDOG_ERROR);
    
    // Convert the incoming value to SI units
    if (isset($rows[$units_from])) {
      if ($rows[$units_from]->conversion_factor != 1 || $rows[$units_from]->conversion_offset != 0) {
        $value -= $rows[$units_from]->conversion_offset;
        $value *= $rows[$units_from]->conversion_factor;
      }
    }
    
    // Convert from SI units to the outgoing value
    if (isset($rows[$units_to])) {
      if ($rows[$units_to]->conversion_factor != 1 || $rows[$units_to]->conversion_offset != 0) {
        $value /= $rows[$units_to]->conversion_factor;
        $value += $rows[$units_to]->conversion_offset;
      }
    }
      
    // Round to the appropriate decimals
    if (!isset($decimals) && isset($rows[$units_to])) {
      $decimals = $rows[$units_to]->unit_decimals;
    }
    if (isset($decimals) && $format != self::FORMAT_TYPE_NONE) {
      $pattern =  '#,##0';
      if ($decimals > 0) {
        $pattern .= '.' . str_repeat('0', $decimals);
      }
      $fmt = new NumberFormatter(OpenFitUserSetting::getCurrentUserLocale(), NumberFormatter::DECIMAL);
      $fmt->setPattern($pattern);
      $value = $fmt->format($value);
    }
    
    // Add the units label
    if (isset($rows[$units_to])) {
      $label = null;
      switch ($format) {
        case self::FORMAT_TYPE_LABEL:
        case self::FORMAT_TYPE_LABEL_CAPS:
          if ($value == 1) {
            $label = $rows[$units_to]->unit_single;
          } else {
            $label = $rows[$units_to]->unit_plural;
          }
          if ($format == self::FORMAT_TYPE_LABEL_CAPS) $label = ucfirst($label);
          break;
        case self::FORMAT_TYPE_SYMBOL:
          $label = $rows[$units_to]->unit_symbol;
          break;
      }
      // If $label is set $value is changed from a numeric to a string
      if (isset($label)) $value .= ' ' . $label;
    }
    
    return $value;
  }
  
    public static function getConversionInfo($units) {
      // TODO: Cache entire table to avoid database queries??
      
      // Fetch the from and to unit types
      $rows = db_query(
        'SELECT
          `unit_id`,`unit_single`,`unit_plural`,`unit_symbol`,`unit_decimals`,`conversion_factor`,`conversion_offset`
        FROM {' . self::TABLE_UNIT . '} 
          WHERE `unit_id` = :units',
        array(':units' => $units))
        ->fetchAllAssoc('unit_id');
        
      // Check for valid units. If either fail keep processing with raw values
      if (!isset($rows[$units])) watchdog('openfit_api','Cannot find unit id "' . $units . '" to convert from.', null, WATCHDOG_ERROR);
      
      return $rows[$units];      
    }
    
  /**
   * Get the label for the specified units in the specified format.
   *
   * @param $value
   *   The value used to determine if a single or plural label is returned.
   * @param $units
   *   The unit id.
   * @param $format
   *   The format type
   * @return
   *   The formatted label type for the units or blank if not found.
   */
  public static function getLabel($value, $units, $format) {
    if ($format == self::FORMAT_TYPE_NONE || $format == self::FORMAT_TYPE_DECIMALS) return '';
    
    // TODO: Cache entire table to avoid database queries??
    
    // Fetch the unit type
    $rows = db_query(
      'SELECT
        `unit_id`,`unit_single`,`unit_plural`,`unit_symbol`
      FROM {' . self::TABLE_UNIT . '} 
        WHERE `unit_id` = :id ',
      array(':id' => $units))
      ->fetchAllAssoc('unit_id');
      
    // Check for valid units.
    if (!isset($rows[$units])) {
      watchdog('openfit_api','Cannot find label for unit id "' . $units . '".', null, WATCHDOG_ERROR);
      return '';
    }
    
    switch ($format) {
      case self::FORMAT_TYPE_LABEL:
      case self::FORMAT_TYPE_LABEL_CAPS:
        if ($value == 1) {
          $label = $rows[$units]->unit_single;
        } else {
          $label = $rows[$units]->unit_plural;
        }
        if ($format == self::FORMAT_TYPE_LABEL_CAPS) $label = ucfirst($label);
        return t($label);
      case self::FORMAT_TYPE_SYMBOL:
        return t($rows[$units]->unit_symbol);
    }
    return '';
  }
  
  public static function getCalorieTitle($calorie_units) {
    if ($calorie_units != self::UNITS_CALORIE) {
      $symbol = self::getLabel(1, $calorie_units, self::FORMAT_TYPE_SYMBOL);
      return t('Energy (@units)', array('@units' => $symbol));
    } else {
      return t('Calories');
    }
  }
  
  /**
   * Return an associative array of an attempt to parse the specified text into a measurement value.
   *
   * Array elements:
   *   - valid: A boolean indicating whether the parsing was valid
   *   - value: The value in the unit type specified.
   *
   * @param $text
   *   The text to parse.
   * @param $value_units
   *   The unit type to return the value in. If null the value is returned as-is without conversion.
   * @param $default_units
   *   The default unit type if the user does not explicitly specify it. Modified to return the value the user enters.
   */
  public static function parse($text, $value_units = null, &$default_units = null) {
    $values = array('valid' => TRUE, 'value' => 0);
    $text = trim($text);
    // Fix for PHP bug in NumberFormatter::parse() when using locales which have space for thousands separator (French).
    $text = str_replace(' ','',$text); 
    $len = strlen($text);
    
    // Blank will always return zero.
    if ($len == 0) return $values;
    
    $revtext = strrev($text);
    $matches = array();
    $num_match = preg_match('/[0-9,.]/', $revtext, $matches, PREG_OFFSET_CAPTURE);
    
    // If there are no numeric digits return zero.
    if ($num_match == 0 || count($matches) == 0) return $values;
    
    $last_digit = $len - 1 - $matches[0][1];
    $meters = 0;
    if ($last_digit != $len - 1) {
      $unit_text = substr($text, $last_digit + 1);
      $text = substr($text, 0, $last_digit + 1);
      if (isset($value_units) && isset($default_units)) {
        // TODO: Cache entire table to avoid database queries??
        $rows = db_query('SELECT `unit_id`, `unit_type` FROM {' . self::TABLE_UNIT . '} WHERE `unit_id` = :unit', array(':unit' => $value_units))
          ->fetchAllAssoc('unit_id');
        if (isset($rows[$value_units])) {
          $unit_type = $rows[$value_units]->unit_type;
          $unit_id = self::parseUnits($unit_text, $unit_type);
          if (isset($unit_id)) $default_units = $unit_id;
        }
      }
    }
    $fmt = new NumberFormatter(OpenFitUserSetting::getCurrentUserLocale(), NumberFormatter::DECIMAL);
    $value = $fmt->parse($text);
    if ($value === false) {
      $values['valid'] = FALSE;
      return $values;
    }
    if (isset($value_units) && isset($default_units)) $value = self::convert($value, $default_units, $value_units);
    $values['value'] = $value;
    return $values;
  }

  /**
   * Return the unit id of the text or null if not found.
   *
   * Do a case-insensitive lookup of the text against unit measurements for the specified type in this order:
   *   - localized symbol text (eg "mi")
   *   - localized single text (eg "mile")
   *   - localized plural text (eg "miles")
   *
   * @param $text
   *   The text to look up.
   * @param $unit_type
   *   A MEASUREMENT_* constant (eg MEASUREMENT_DISTANCE, MEASUREMENT_WEIGHT, etc).
   * @return
   *   If found, the unit id of the specified text, otherwise null.
   */   
  private static function parseUnits($text, $unit_type) {
    $text = strtolower(trim($text));
    if ($unit_type == self::MEASUREMENT_DISTANCE || $unit_type == self::MEASUREMENT_ELEVATION) $unit_type = 'length';
    
    // TODO: Cache entire table to avoid database queries??
    $rows = db_query(
      'SELECT
        `unit_id`,`unit_single`,`unit_plural`,`unit_symbol`
      FROM {' . self::TABLE_UNIT . '} 
        WHERE `unit_type` = :type',
      array(':type' => $unit_type))
      ->fetchAllAssoc('unit_id');
    
    foreach ($rows as $unit_id => $row) {
      if (strcasecmp(t($row->unit_symbol), $text) == 0) return $row->unit_id;
    }
    foreach ($rows as $unit_id => $row) {
      if (strcasecmp(t($row->unit_single), $text) == 0) return $row->unit_id;
    }
    foreach ($rows as $unit_id => $row) {
      if (strcasecmp(t($row->unit_plural), $text) == 0) return $row->unit_id;
    }
    return null;
  }  
}