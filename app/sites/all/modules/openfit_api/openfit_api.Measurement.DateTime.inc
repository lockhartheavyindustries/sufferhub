<?php 

class OpenFitMeasurementDateTime {

  /**
   * Convenience function to offset the date by positive or negative hours (such as a local timezone)
   *
   * @param $date
   *   A DateTime object.
   * @param $hours
   *   The number of hours to offset.
   */
  public static function offsetDate($date, $hours) {
    $minutes_offset = $hours * 60;
    if ($minutes_offset > 0) {
      $date->add(new DateInterval('PT' . $minutes_offset . 'M'));
    } else if ($minutes_offset < 0) {
      $date->sub(new DateInterval('PT' . -$minutes_offset . 'M'));
    }
  }
  
  /**
   * Get the ISO string representation of the timezone hours offset.
   *
   * @param $timezone
   *    The timezone offset from UTC in hours.
   */
  public static function getISOTimezone($timezone) {
    if ($timezone == 0) return 'Z';
    $hours = intval($timezone);
    $minutes = ($timezone - $hours) * 60;
    $sign = ($timezone < 0) ? '-' : '+';
    return $sign . str_pad(abs($hours), 2, '0', STR_PAD_LEFT) . ':' . str_pad($minutes, 2, '0', STR_PAD_LEFT);
  }
  
   /**
   * Return a DateTime which is the first day of the week that $date falls within.
   *
   * @param $week_start
   *    The start of the week.
   * @param $date
   *    The date or null to use today's date.
   */
  public static function getFirstDayOfWeekDate($week_start, $date) {
    if (!isset($date)) $date = new DateTime('now');
    $dow = intval($date->format('w'));
    $start_offset = $dow - $week_start;
    if ($start_offset < 0) $start_offset += 7;
    if ($start_offset > 0) $date->sub(new DateInterval('P' . $start_offset . 'D'));
    return new DateTime($date->format('Y-m-d'));
  }
  
  /**
   * Format a UTC datetime as time of day using the current user's locale setting.
   *
   * @param $datetime_utc
   *   A DateTime object.
   */
  public static function formatTimeOfDay($datetime_utc) {
    $locale = OpenFitUserSetting::getCurrentUserLocale();
    $fmt = new IntlDateFormatter($locale, IntlDateFormatter::NONE, IntlDateFormatter::SHORT, 'UTC');
    return $fmt->format($datetime_utc);
  }
  
  /**
   * Return an associative array of an attempt to parse the specified text into a time of day.
   *
   * Array elements:
   *   - valid: A boolean indicating whether the parsing was valid
   *   - hour: Hours (0 to 23)
   *   - minute: Minutes (0 to 59)
   *   - second: Seconds (0 to 59)
   *   - millisecond: Milliseconds (0 to 999)
   *
   * @param $text
   *   The text to parse.
   */
  public static function parseTimeOfDay($text) {
    $time_separator = ':';
    $decimal_separator = '.';
    $locale = OpenFitUserSetting::getCurrentUserLocale();
    $fmt = new IntlDateFormatter($locale, IntlDateFormatter::NONE, IntlDateFormatter::SHORT);
    $d = new DateTime('2001-01-01T12:34:56.78');
    $txt = $fmt->format($d);
    $pos = strpos($txt,'34');
    if ($pos > 0) $time_separator = substr($txt,$pos-1,1);
    $fmt = new NumberFormatter($locale, NumberFormatter::DECIMAL);
    $txt = $fmt->format(12.34);
    $pos = strpos($txt,'34');
    if ($pos > 0) $decimal_separator = substr($txt,$pos-1,1);

    $values = array('valid' => FALSE, 'hour' => 0, 'minute' => 0, 'second' => 0, 'millisecond' => 0);
    $text = strtolower($text);
    
    $pm = false;
    $am = false;
    if (strpos($text, 'p')) {
      $pm = true;
    } else if (strpos($text, 'a')) {
      $am = true;
    }
    $text = preg_replace('/[apm]/','',$text);
    list($hms, $milis) = explode($decimal_separator,$text . $decimal_separator);
    if (!isset($hms)) return $values;
    if (!isset($milis)) $milis = 0;
    if (strpos($text, $time_separator) === false) return $values;
    list ($hours, $minutes, $seconds) = explode($time_separator, $hms . $time_separator);
    if (!isset($hours) || !isset($minutes)) return $values;
    if (!isset($seconds)) $seconds = 0;
    $hours = intval($hours);
    $minutes = intval($minutes);
    $seconds = intval($seconds);
    $milis = intval($milis);
    if ($milis < 10) $milis *= 10;
    if ($milis < 100) $milis *= 10;
    if ($pm && $hours < 12) $hours += 12;
    if ($am && $hours == 12) $hours -= 12;
    if ($hours > 23 || $hours < 0 || $minutes > 59 || $minutes < 0 || 
      $seconds > 59 || $seconds < 0 || $milis > 999 || $milis < 0) {
      return $values;
    }
    $values['valid'] = TRUE;
    $values['hour'] = $hours;
    $values['minute'] = $minutes;
    $values['second'] = $seconds;
    $values['millisecond'] = $milis;
    return $values;
  }  

  /**
   * Format a value representing seconds in a span of time.
   *
   * @param $value
   *   The number of seconds. May be positive or negative.
   * @param $hour_digits
   *   The number of hour digits to always show.
   * @param $minute_digits
   *   The number of minute digits to always show.
   * @param $second_digits
   *   The number of second digits to always show.
   * @param $millisecond_digits
   *   The number of millisecond digits to always show.
   * @param $show_positive_sign
   *   If true positive timespans are prepended with a '+' sign.
   */
  public static function formatTimespan($value, $hour_digits = 0, $minute_digits = 1, $second_digits = 2, $millisecond_digits = 0, $show_positive_sign = false) {
    if (!is_numeric($value)) return null;
    $sign = $value < 0 ? '-' : $show_positive_sign ? '+' : '';
    $value = abs($value);
    $hours = floor($value/3600);
    $value -= $hours*3600;
    $minutes = $second_digits == 0 ? round($value/60) : floor($value/60);
    $value -= $minutes*60;
    $seconds = $millisecond_digits == 0 ? round($value) : floor($value);
    $value -= $seconds;
    if ($seconds >= 60) {
      $seconds -= 60;
      $minutes += 1;
      if ($minutes >= 60) {
        $minutes -= 60;
        $hours += 1;
      }
    }
    // Avoid using pow()
    switch ($millisecond_digits) {
      case 0: break;
      case 1: $pow = 10; break;
      case 2: $pow = 100; break;
      default: $pow = 1000; $millisecond_digits = 3; break;
    }
    if ($millisecond_digits == 0) {
      $milliseconds = '';
    } else {
      $milliseconds = '.' . str_pad(round($value * $pow), $millisecond_digits,'0',STR_PAD_LEFT);
    }
    
    if ($minute_digits > 2) $minute_digits = 2;
    if ($second_digits > 2) $second_digits = 2;
    
    if ($hours > 0 || $hour_digits > 0) $minute_digits = 2;      
    //if ($minutes > 0 || $minute_digits > 0) $second_digits = 2;
    
    $hours = ($hours > 0 || $hour_digits > 0) ? str_pad($hours,$hour_digits,'0',STR_PAD_LEFT) . ':' : '';
    $minutes = ($minutes > 0 || $minute_digits > 0) ? str_pad($minutes,$minute_digits,'0',STR_PAD_LEFT) : '';
    $seconds = $second_digits > 0 ? ':' . str_pad($seconds,$second_digits,'0',STR_PAD_LEFT) : '';
    
    return $sign . $hours . $minutes . $seconds . $milliseconds;
  }

  /**
   * Return an associative array of an attempt to parse the specified text into a timespan.
   *
   * Array elements:
   *   - valid: A boolean indicating whether the parsing was valid
   *   - value: Number of seconds in the timespan
   *
   * @param $text
   *   The text to parse.
   * @param $allow_positive_only
   *   If true, negative values are converted to positive.
   */
  public static function parseTimespan($text, $allow_positive_only = true) {
    $values = array('valid' => TRUE, 'value' => 0);
    $text = trim($text);
    if ($allow_positive_only) $text = str_replace('-','',$text);
    if (strpos($text, ':') === false) {
      $values['value'] = intval($text) * 60;
      return $values;
    }
    $text = str_replace(',','.',$text);
    list($hms, $milis) = explode('.',$text . '.');
    if (!isset($hms)) return $values;
    if (!isset($milis)) $milis = 0;
    list ($hours, $minutes, $seconds) = explode(':', $hms . ':');
    if (!isset($seconds) || strlen($seconds) == 0) {
      $seconds = $minutes;
      $minutes = $hours;
      $hours = 0;
    }
    $hours = intval($hours);
    $minutes = intval($minutes);
    $seconds = intval($seconds);
    $milis = intval($milis);
    if ($milis < 10) $milis *= 10;
    if ($milis < 100) $milis *= 10;
    $values['value'] = $hours * 3600 + $minutes * 60 + $seconds + $milis / 1000;
    return $values;
  }

  /**
   * Convert a IntlDateFormatter pattern into a jQuery date popup pattern
   *
   * @param $pattern
   *   The IntlDateFormatter pattern.
   * @return
   *   The converted pattern.
   */
  public static function convertPattern($pattern) {
    $pattern = trim(str_replace(array('q', 'Q', 'W', 'F', 'g', 'z', 'Z', 'A', 'v', 'V'), '', $pattern));
    
    $icu_characters = "GyYuQqMLwWdDFgEecahHkKmsSAzZvVW'";
    $split_pattern = preg_split('/([^'.$icu_characters.']+)|(\'[^\']+\')/', $pattern, -1, PREG_SPLIT_NO_EMPTY | PREG_SPLIT_DELIM_CAPTURE);
    
    $converted = array();
    foreach ($split_pattern as $match) {
      $converted[] = self::getDatepickerChars($match);
    }
    
    $date_pattern = trim(implode($converted));
    
    return $date_pattern;
  }
  
  /**
   * List of unconvertable stuff:
   * 
   * G (era marker) goes to the string literal AD
   * u (extended year) goes to yy
   * L (stand alone month) goes to the equivalent M (month)
   * Y (stand alone year) goes to the equivalent y (year)
   * c (stand alone day) goes to the equivalent e (day)
   * k goes to the equivalent h (1-12 or 0-23 depending on timeformatter)
   * H goes to h (see above)
   * S does not round (TODO find if timepicker always returns 4 sig figs for S)
   * 
   * Q (quarter) is unsupported
   * W (week in month) is unsupported
   * F (day of week in month) is unsupported
   * g (modified julian day) is unsupported
   * z (timezone) is unsupported
   * A (ms since day start) is unsupported
   * v (timezone-generic) is unsupported
   * 
   * Everything not listed here should convert fine.
   * 
   * @param $chars the ICU Date Char(s) to convert
   */
  private static function getDatepickerChars($chars) {
    $start_char = $chars[0];
    if ($start_char == '\'') return $chars;
    
    for ($i = 0; $i < strlen($chars); $i++) {
      if ($chars[$i] != $start_char) {
        $unit1 = substr($chars, 0, $i);
        $unit2 = substr($chars, $i);
        $char1 = self::getDatepickerChars($unit1);
        $char2 = self::getDatepickerChars($unit2);
        return $char1.$char2;
      }
    }
    
    $datepicker_chars = '';
    $len = strlen($chars);
    switch ($start_char) {
      case 'a':
        return 'tt';
      case 'G':
        return 'AD'; //Seriously, when are we ever going to need to use BC dates
      case 'y':
      case 'Y':
      case 'u':
        if ($len == 4) { //Four digit year
          return 'Y';
        } else {
          return 'y';
        }
      case 'M':
      case 'L':
        if ($len == 2) { //Numeric month leading zero
          return 'm';
        } else if ($len == 1) { //Numeric month no leading zero
          return 'n';
        } else if ($len == 4) { //Long month name
          return 'F';
        } else { //Short month name
          return 'M';
        }
      case 'd':  //You can pass this one through.
        return $start_char;
      case 'D':
        return 'o';
      case 'e':
      case 'E':
      case 'c':
        if ($len == 4) {  //Long day of week
          return 'l';
        } else {  //Short day of week
          return 'D';
        }
      case 'H':
        return strtolower($chars);
      case 'k':
      case 'K':
        if (strlen($chars) == 1) {
          return 'g';
        } else {
          return 'h';
        }
      case 'S':
        return 'l';
      default:
       return $chars;
    }
  }


}