<?php 

class OpenFitDataField {
  const DATA_TYPE_NUMERIC = 'numeric';
  const DATA_TYPE_DISTANCE = 'distance';
  const DATA_TYPE_TIME = 'time';
  const DATA_TYPE_SPEED = 'speed';
  const DATA_TYPE_PACE = 'pace';
  const DATA_TYPE_DATE = 'date';

  /**
   * Get a render array for the form fields.
   *
   * @param $field_info
   *   An array of field schema info indexed by field id.
   * @param $form_data
   *   An array of field data indexed by field id.
   * @param $form_fields
   *   An array of field ids to output. If empty all fields in $form_data are rendered.
   */   
  public static function getFormDataRenderArray($field_info, $form_data, $form_fields = null) {
    if (!isset($form_fields)) {
      foreach ($form_data as $field_id => $field_data) $form_fields[] = $field_id;
    }
    $page = array();
    foreach ($form_fields as $field_id) {
      if (!isset($form_data[$field_id])) continue;
      $field_data = $form_data[$field_id];
      $classes = isset($field_data['class']) ?  $field_data['class'] : array();
      $value = $field_data['data'];
      if (!isset($value) || strlen($value) == 0) $value = '&nbsp;';
      $page += array(
        $field_id => array(
          '#markup' => theme_field(array(
            'classes' => implode($classes, ' '),
            'attributes' => ' id="' . strtr($field_id, '_', '-') . '"',
            'label_hidden' => FALSE,
            'title_attributes' => '',
            'label' => t($field_info[$field_id]['title']),
            'content_attributes' => '',
            'item_attributes' => array(''),
            'items' => array(array('#markup' => $value)),
          )),
        )
      );
    }
    return $page;
  }

  /**
   * Format a datetime database column value for output.
   *
   * The $field_info parameter may contain the following options.
   *   - timezone_field: A field id in the $row which has an integer timezone offset. Default to zero.
   *   - date_format: An IntlDateFormatter format constant. Default to IntlDateFormatter::SHORT.
   *   - time_format: An IntlDateFormatter format constant. Default to IntlDateFormatter::SHORT.
   *
   * @param $row
   *   An datatabase stdClass row keyed by field id.
   * @param $field_id
   *   The field id.
   * @param $field_info
   *   A stdClass of the data field info.
   * @param $value
   *   The column value from the data row.
   * @return
   *   A string cell value.
   */
  public static function formatDatetime($row, $field_id, $field_info, $value) {
    if (!isset($value) || $value == '') return '';
    $date = new DateTime($value, new DateTimeZone('UTC'));
    $offset_hours = 0;
    if (isset($field_info['timezone_field'])) {
      $tz_field = $field_info['timezone_field'];
      if (isset($row->$tz_field)) {
        $offset_hours += $row->$tz_field;
        OpenFitMeasurementDateTime::offsetDate($date, $offset_hours);
      }
    }

    $date_format = isset($field_info['date_format']) ? $field_info['date_format'] : IntlDateFormatter::SHORT;
    $time_format = isset($field_info['time_format']) ? $field_info['time_format'] : IntlDateFormatter::SHORT;
    $fmt = new IntlDateFormatter(OpenFitUserSetting::getCurrentUserLocale(), $date_format, $time_format, 'UTC');
    return $fmt->format($date);
  }
  
  /**
   * Format a distance database column value (stored as meters) for output.
   *
   * The $field_info parameter may contain the following options.
   *   - blank: Text to return if the database value is null. Default to ''.
   *   - units: A type of unit ('kilometer', 'mile', etc). If not present, the current logged in user setting is used.
   *   - format: A OpenFitMeasurement::FORMAT_TYPE_* constant indicating how to format the value. Defaults to FORMAT_TYPE_SYMBOL.
   *   - decimals: The number of decimals to show or null if the default decimals for the unit should be used. Default to null.
   *
   * @param $row
   *   An datatabase stdClass row keyed by field id.
   * @param $field_id
   *   The field id.
   * @param $field_info
   *   A stdClass of the data field info.
   * @param $value
   *   The column value from the data row.
   * @return
   *   A string cell value.
   */
  public static function formatDistance($row, $field_id, $field_info, $value) {
    if (!isset($value) || (is_string($value) && $value == '')) return isset($field_info['blank']) ? $field_info['blank'] : '';
    
    if (isset($field_info['units'])) {
      $units = $field_info['units'];
    } else {
      global $user;
      $preferences = OpenFitUserSetting::get($user->uid);
      $units = $preferences[OpenFitUserSetting::TYPE_DISTANCE_UNITS];
    }
    $format = isset($field_info['format']) ? $field_info['format'] : OpenFitMeasurement::FORMAT_TYPE_SYMBOL;
    $decimals = isset($field_info['decimals']) ? $field_info['decimals'] : null;
    return OpenFitMeasurement::convert($value, 'meter', $units, $format, $decimals);
  }
  
  /**
   * Format a distance database column value (stored as meters) for output.
   *
   * The $field_info parameter may contain the following options.
   *   - blank: Text to return if the database value is null. Default to ''.
   *   - units: A type of unit ('kilometer', 'mile', etc). If not present, the current logged in user setting is used.
   *   - format: A OpenFitMeasurement::FORMAT_TYPE_* constant indicating how to format the value. Defaults to FORMAT_TYPE_SYMBOL.
   *   - decimals: The number of decimals to show or null if the default decimals for the unit should be used. Default to null.
   *
   * @param $row
   *   An datatabase stdClass row keyed by field id.
   * @param $field_id
   *   The field id.
   * @param $field_info
   *   A stdClass of the data field info.
   * @param $value
   *   The column value from the data row.
   * @return
   *   A string cell value.
   */
  public static function formatElevation($row, $field_id, $field_info, $value) {
    if (!isset($value) || (is_string($value) && $value == '')) return isset($field_info['blank']) ? $field_info['blank'] : '';
    
    if (isset($field_info['units'])) {
      $units = $field_info['units'];
    } else {
      global $user;
      $preferences = OpenFitUserSetting::get($user->uid);
      $units = $preferences[OpenFitUserSetting::TYPE_ELEVATION_UNITS];
    }
    $format = isset($field_info['format']) ? $field_info['format'] : OpenFitMeasurement::FORMAT_TYPE_SYMBOL;
    $decimals = isset($field_info['decimals']) ? $field_info['decimals'] : null;
    return OpenFitMeasurement::convert($value, 'meter', $units, $format, $decimals);
  }

  /**
   * Format a time database column value (stored as seconds) for output.
   *
   * Using the digits parameters of $field_info allows the caller to pad the time fields.
   * The default format will look like M:SS.
   * 
   * The $field_info parameter may contain the following options.
   *   - blank: Text to return if the database value is null. Default to ''.
   *   - hour_digits: The number of hour digits to show (0, 1, or 2). Defaults to 0.
   *   - minute_digits: The number of minute digits to show (0, 1, or 2). Defaults to 1.
   *   - second_digits: The number of second digits to show (0, 1, or 2). Defaults to 2.
   *   - millisecond_digits: The number of millisecond digits to show (0, 1, 2, or 3). Defaults to 0.
   *   - show_positive_sign: If true, a + sign is added before positive time values.
   *
   * @param $row
   *   An datatabase stdClass row keyed by field id.
   * @param $field_id
   *   The field id.
   * @param $field_info
   *   A stdClass of the data field info.
   * @param $value
   *   The column value from the data row.
   * @return
   *   A string cell value.
   */
  public static function formatTimespan($row, $field_id, $field_info, $value) {
    if (!isset($value) || (is_string($value) && $value == '')) return isset($field_info['blank']) ? $field_info['blank'] : '';
    $hour_digits = isset($field_info['hour_digits']) ? $field_info['hour_digits'] : 0;
    $minute_digits = isset($field_info['minute_digits']) ? $field_info['minute_digits'] : 1;
    $second_digits = isset($field_info['second_digits']) ? $field_info['second_digits'] : 2;
    $millisecond_digits = isset($field_info['millisecond_digits']) ? $field_info['millisecond_digits'] : 0;
    $show_positive_sign = isset($field_info['show_positive_sign']) ? $field_info['show_positive_sign'] : false;
    return OpenFitMeasurementDateTime::formatTimespan($value, $hour_digits, $minute_digits, $second_digits, $millisecond_digits, $show_positive_sign);
  }
  
  /**
   * Format a calories database column value (stored as kilojoules) for output.
   *
   * The $field_info parameter may contain the following options.
   *   - blank: Text to return if the database value is null. Default to ''.
   *   - units: A type of unit ('calorie', 'kilojoule', etc). If not present, the current logged in user setting is used.
   *   - format: A OpenFitMeasurement::FORMAT_TYPE_* constant indicating how to format the value. Defaults to FORMAT_TYPE_DECIMALS.
   *   - decimals: The number of decimals to show or null if the default decimals for the unit should be used. Default to null.
   *
   * @param $row
   *   An datatabase stdClass row keyed by field id.
   * @param $field_id
   *   The field id.
   * @param $field_info
   *   A stdClass of the data field info.
   * @param $value
   *   The column value from the data row.
   * @return
   *   A string cell value.
   */
  public static function formatCalories($row, $field_id, $field_info, $value) {
    if (!isset($value) || (is_string($value) && $value == '')) return isset($field_info['blank']) ? $field_info['blank'] : '';
    
    if (isset($field_info['units'])) {
      $units = $field_info['units'];
    } else {
      global $user;
      $preferences = OpenFitUserSetting::get($user->uid);
      $units = $preferences[OpenFitUserSetting::TYPE_CALORIE_UNITS];
    }
    $format = isset($field_info['format']) ? $field_info['format'] : OpenFitMeasurement::FORMAT_TYPE_DECIMALS;
    $decimals = isset($field_info['decimals']) ? $field_info['decimals'] : null;
    return OpenFitMeasurement::convert($value, 'kilojoules', $units, $format, $decimals);
  }
  
  /**
   * Format a speed database column value (stored as meters per second) for output.
   *
   * The $field_info parameter may contain the following options.
   *   - blank: Text to return if the database value is null. Default to '&infin;'.
   *   - units: A type of unit ('kilometer-per-hour', 'mile-per-hour', etc). If not present, the current logged in user setting is used.
   *   - format: A OpenFitMeasurement::FORMAT_TYPE_* constant indicating how to format the value. Defaults to FORMAT_TYPE_SYMBOL.
   *   - decimals: The number of decimals to show or null if the default decimals for the unit should be used. Default to null.
   *
   * @param $row
   *   An datatabase stdClass row keyed by field id.
   * @param $field_id
   *   The field id.
   * @param $field_info
   *   A stdClass of the data field info.
   * @param $value
   *   The column value from the data row.
   * @return
   *   A string cell value.
   */
  public static function formatSpeed($row, $field_id, $field_info, $value) {
    if (!isset($value) || (is_string($value) && $value == '')) return isset($field_info['blank']) ? $field_info['blank'] : '&infin;'; // Infinite speed
    
    if (isset($field_info['units'])) {
      $units = $field_info['units'];
    } else {
      global $user;
      $preferences = OpenFitUserSetting::get($user->uid);
      $units = $preferences[OpenFitUserSetting::TYPE_SPEED_UNITS];
    }
    $format = isset($field_info['format']) ? $field_info['format'] : OpenFitMeasurement::FORMAT_TYPE_SYMBOL;
    $decimals = isset($field_info['decimals']) ? $field_info['decimals'] : 1;
    return OpenFitMeasurement::convert($value, 'meter-per-second', $units, $format, $decimals);
  }

  /**
   * Format a pace database column value (stored as meters per second) for output.
   *
   * The $field_info parameter may contain the following options.
   *   - units: A type of distance unit ('kilometer', 'mile', etc). If not present, the current logged in user setting is used.
   *   - hour_digits: The number of hour digits to show (0, 1, or 2). Defaults to 0.
   *   - minute_digits: The number of minute digits to show (0, 1, or 2). Defaults to 1.
   *   - second_digits: The number of second digits to show (0, 1, or 2). Defaults to 2.
   *   - millisecond_digits: The number of millisecond digits to show (0, 1, 2, or 3). Defaults to 0.
   *   - show_positive_sign: If true, a + sign is added before positive time values.
   *
   * @param $row
   *   An datatabase stdClass row keyed by field id.
   * @param $field_id
   *   The field id.
   * @param $field_info
   *   A stdClass of the data field info.
   * @param $value
   *   The column value from the data row.
   * @return
   *   A string cell value.
   */
  public static function formatPace($row, $field_id, $field_info, $value) {
    if ((!isset($value) || (is_string($value) && $value == '')) && isset($field_info['blank'])) return $field_info['blank'];
    if (isset($field_info['units'])) {
      $units = $field_info['units'];
    } else {
      global $user;
      $preferences = OpenFitUserSetting::get($user->uid);
      $units = $preferences[OpenFitUserSetting::TYPE_DISTANCE_UNITS];
    }
    if (isset($value) && $value == 0) {
      $text = '&infin;'; // Infinite pace
    } else {
      if (!isset($value) || $value == '') {
        $seconds_per = 0;
      } else {
        $value = OpenFitMeasurement::convert($value, 'meter', $units);
        $seconds_per = 1 / $value;
      }
      $hour_digits = isset($field_info['hour_digits']) ? $field_info['hour_digits'] : 0;
      $minute_digits = isset($field_info['minute_digits']) ? $field_info['minute_digits'] : 1;
      $second_digits = isset($field_info['second_digits']) ? $field_info['second_digits'] : 2;
      $millisecond_digits = isset($field_info['millisecond_digits']) ? $field_info['millisecond_digits'] : 0;
      $show_positive_sign = isset($field_info['show_positive_sign']) ? $field_info['show_positive_sign'] : false;
      $text = OpenFitMeasurementDateTime::formatTimespan($seconds_per, $hour_digits, $minute_digits, $second_digits, $millisecond_digits, $show_positive_sign);
    }
    return $text . ' /' . OpenFitMeasurement::getLabel(1, $units, OpenFitMeasurement::FORMAT_TYPE_SYMBOL);
  }
  
  public static function formatNumber($row, $field_id, $field_info, $value) {
    if (!isset($value) || (is_string($value) && $value == '') || !is_numeric($value)) return isset($field_info['blank']) ? $field_info['blank'] : '';
    
    if (isset($field_info['units'])) {
      $units = $field_info['units'];
    } else {
      global $user;
      $preferences = OpenFitUserSetting::get($user->uid);
      $units = $preferences[OpenFitUserSetting::TYPE_CALORIE_UNITS];
    }
    $format = isset($field_info['format']) ? $field_info['format'] : true;
    $decimals = isset($field_info['decimals']) ? $field_info['decimals'] : null;
    
    if (isset($decimals) && $format) {
      $pattern =  '#,##0';
      if ($decimals > 0) {
        $pattern .= '.' . str_repeat('0', $decimals);
      }
      $fmt = new NumberFormatter(OpenFitUserSetting::getCurrentUserLocale(), NumberFormatter::DECIMAL);
      $fmt->setPattern($pattern);
      $value = $fmt->format($value);
    }
    
    return $value;
 }
}