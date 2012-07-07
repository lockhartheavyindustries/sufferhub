<?php

class ActivityDataTrackAccess {
  /**
   * Each point contains an array of float latitude, longitude.
   */
  const LOCATION = 'location';
  
  /**
   * Each point contains the integer elevation meters.
   */
  const ELEVATION = 'elevation';
  
  /**
   * Each point contains the integer distance meters.
   */
  const DISTANCE = 'distance';
  
  /**
   * Each point contains the integer heart rate beats per minute.
   */
  const HEARTRATE = 'heartrate';
  
  /**
   * Each point contains the integer cadence revolutions per minute.
   */
  const CADENCE = 'cadence';
   
  /**
   * Each point contains the integer power watts.
   */
  const POWER = 'power';
  
   /**
    * Each point contains the float celsius temperature.
    */
  const TEMPERATURE = 'temperature';
  
  /**
   * Each point contains the meters per second speed.
   */
  const SPEED = 'speed';
  
  /**
   * Each point contains the seconds per meters.
   */
  const PACE = 'pace';
  
  /**
   * Construct an accessor for the specified activity.
   */
  function __construct($activity_id, $type = '') {
    $this->activityId = $activity_id;
    $this->type = $type;
  }
  
  public static function getAllDataTrackTypes() {
    return array(
      self::LOCATION,
      self::ELEVATION,
      self::DISTANCE,
      self::HEARTRATE,
      self::CADENCE,
      self::POWER,
      self::TEMPERATURE,
      self::SPEED,
    );
  }
  
  // TODO: All these track specific functions and the tracks themselves should be defined in the database, except maybe distance, elevation and location
  public static function getMeasurementInfo($track_type) {
    global $user;
    $units_none = array(
      'unit_id' => 'none',
      'unit_single' => '', 'unit_plural' => '', 'unit_symbol' => '', 
      'unit_decimals' => 0, 'conversion_factor' => 1, 'conversion_offset' => 0);
    switch ($track_type) {
      case self::ELEVATION:
        return OpenFitMeasurement::getConversionInfo(OpenFitUserSetting::get($user->uid, OpenFitUserSetting::TYPE_ELEVATION_UNITS));
      case self::DISTANCE:
        return OpenFitMeasurement::getConversionInfo(OpenFitUserSetting::get($user->uid, OpenFitUserSetting::TYPE_DISTANCE_UNITS));
      case self::TEMPERATURE:
        return OpenFitMeasurement::getConversionInfo(OpenFitUserSetting::get($user->uid, OpenFitUserSetting::TYPE_TEMPERATURE_UNITS));
      case self::SPEED:
        return OpenFitMeasurement::getConversionInfo(OpenFitUserSetting::get($user->uid, OpenFitUserSetting::TYPE_SPEED_UNITS));
      case self::PACE: {
        $distance_info = OpenFitMeasurement::getConversionInfo(OpenFitUserSetting::get($user->uid, OpenFitUserSetting::TYPE_DISTANCE_UNITS));
        $distance_info->unit_symbol = '/' . $distance_info->unit_symbol;
        $distance_info->time = true;
        return $distance_info;
      }
      default:
        return $units_none;
    }
  }
  
  public static function getTitle($track_type) {
    if (isset(self::$trackTypesInfo[$track_type])) return self::$trackTypesInfo[$track_type]['title'];
    return t('Data');
  }
  
  public static function getColor($track_type) {
    if (isset(self::$trackTypesInfo[$track_type])) return self::$trackTypesInfo[$track_type]['color'];
    return '#A05050';
  }
  
  public static function getDataSize($track_type) {
    if (isset(self::$trackTypesInfo[$track_type])) return self::$trackTypesInfo[$track_type]['data_size'];
    return strlen(pack(self::getDataFormat($track_type),0));
  }
  
  public static function getDataFormat($track_type) {
    if (isset(self::$trackTypesInfo[$track_type])) return self::$trackTypesInfo[$track_type]['data_format'];
    return 'd';
  }
  
  protected $activityId;
  protected $type;
  
  // TODO: Move this schema into the database.
  protected static $trackTypesInfo = array(
    self::LOCATION => array (
      'id' => 1,
      'version' => 1,
      'data_size' => 8,
      'data_format' => 'f2', // TODO: Should be machine independent ordering to support data portability, always pick one endian type
      'title' => 'Route',
      'color' => '#0080FF',
    ),
    self::ELEVATION => array (
      'id' => 2,
      'version' => 1,
      'data_size' => 4,
      'data_format' => 'f', // TODO: Should be machine independent ordering to support data portability, always pick one endian type
      'title' => 'Elevation',
      'color' => '#AD7118',
    ),
    self::DISTANCE => array (
      'id' => 3,
      'version' => 1,
      'data_size' => 4,
      'data_format' => 'f', // TODO: Should be machine independent ordering to support data portability, always pick one endian type
      'title' => 'Distance',
      'color' => '#204A87',
    ),
    self::HEARTRATE => array (
      'id' => 4,
      'version' => 1,
      'data_size' => 1,
      'data_format' => 'C',
      'title' => 'Heart Rate',
      'color' => '#CC0010',
    ),
    self::CADENCE => array (
      'id' => 5,
      'version' => 1,
      'data_size' => 1,
      'data_format' => 'C',
      'title' => 'Cadence',
      'color' => '#9DC744',
    ),
    self::POWER => array (
      'id' => 6,
      'version' => 1,
      'data_size' => 1,
      'data_format' => 'C',
      'title' => 'Power',
      'color' => '#9B5AAD',
    ),
    self::TEMPERATURE => array (
      'id' => 7,
      'version' => 1,
      'data_size' => 2,
      'data_format' => 's', // TODO: This should be machine independent
      'data_multiple' => 10,
      'title' => 'Temperature',
      'color' => '#A0A0A0',
    ),
    self::SPEED => array (
      'title' => 'Speed',
      'color' => '#377AEB',
    ),
    self::PACE => array (
      'title' => 'Pace',
      'color' => '#68D8D8',
    ),
  );
  
  protected function getFilename() {
    $ext = '.data';
    if (strlen($this->type) > 0) $ext = '-' . $this->type . $ext;
    return DRUPAL_ROOT. '/openfit/data_tracks/' . $this->activityId . $ext;
  }
  
  const FILE_VERSION = 1;
}

class ActivityDataTrackWriter extends ActivityDataTrackAccess {

  /**
   * Construct a writer for the specified activity.
   */
  function __construct($activity_id, $type = '') {
    parent::__construct($activity_id, $type);
  }
  
  /**
   * Write the data tracks.
   *
   * @param $tracks
   *   An array of arrays indexed by track type. The value array contains two elements:
   *    - header: an array of header info,
   *    - data: an array of 2*N elements: offset[0], value[0] .. offset[n], value[n]
   *      Where offset is the time difference since the prior point. Offset must be 255 seconds or less.
   *
   *   The header element consists of the following fields:
   *    - offset: The number of seconds offset from the start of the workout.
   */
  public function writeTracks($tracks) {
    $filename = $this->getFilename();
    
    $file = fopen($filename, 'wb');
    if (!$file) throw new Exception('Could not open file for writing.');
   
    // Discard tracks whose type we don't know or don't have header info or point data.
    $orig_tracks = $tracks;
    $tracks = array();
    foreach ($orig_tracks as $track_type => &$track) {
      // Skip unknown track types.
      if (!isset(self::$trackTypesInfo[$track_type])) continue;
      // Skip tracks with missing data.
      if (!isset($track['header']) || !isset($track['header']['offset']) || !isset($track['data'])) continue;
      $point_count = count($track['data']);
      // Skip tracks with less than one data point.
      if ($point_count < 2) continue;
      // If header offset is negative, discard data before zero.
      $start = 0;
      $track_data = $track['data'];
      if ($track['header']['offset'] < 0) {
        $header_offset = -$track['header']['offset'];
        $prior_elapsed = null;
        $prior_value = null;
        // Find the first value beyond zero and interpolate the zero value if necessary
        for($p = 0; $p < $point_count; $p += 2) {
          $elapsed = $track_data[$p] + (isset($prior_elapsed) ? $prior_elapsed : 0);
          $value = $track_data[$p+1];
          
          if ($elapsed == $header_offset) {
            break;
          } else if ($elapsed > $header_offset) {
            if (isset($prior_elapsed) && isset($prior_value)) {
              $diff = $elapsed - $prior_elapsed;
              $amt = $header_offset - $prior_elapsed;
              if (is_array($value)) {
                $slope = array();
                $count_values = count($value);
                for($i = 0; $i < $count_values; $i++) $new_value[$i] = $prior_value[$i] + (($value[$i] - $prior_value[$i]) * $amt) / $diff;
              } else {
                $new_value = $prior_value + (($value - $prior_value) * $amt) / $diff;
                if ($track_type == self::DISTANCE) $new_value = 0;
              }
              $start--;
              $track['data'][$p - 2] = $header_offset;
              $track['data'][$p - 1] = $new_value;
              $track['data'][$p] -= $amt;
            }
            break;
          }
          $prior_elapsed = $elapsed;
          $prior_value = $value;
          $start++;
        }
      }
      // If we skipped all the data points, discard.
      if (($start * 2) >= $point_count) continue;
      
      $track['header']['start'] = $start;
      $tracks[$track_type] = $track;
    }
    
    $num_tracks = count($tracks);
    
    // Count the track size and byte offset
    $tracks_info = array();
    $track_start = 2 + $num_tracks * 5; // 2 bytes header + (1 byte + 1 int) per track
    foreach ($tracks as $track_type => $track) {
      $type_info = self::$trackTypesInfo[$track_type];
      $num_points = count($track['data']) / 2 - $track['header']['start'];
      $tracks_info[$track_type]['start_byte'] = $track_start;
      $tracks_info[$track_type]['num_points'] = $num_points;
      // Offset start by: 1 byte (version) + 4 bytes (num points) + 4 bytes (time offset) + data points
      $track_start += 9 + $num_points * (1 + $type_info['data_size']);
    }

    // Write the file header
    fwrite($file, chr(self::FILE_VERSION));
    fwrite($file, chr($num_tracks));
    
    // Write the track headers
    foreach ($tracks_info as $track_type => $track_info) {
      $type_info = self::$trackTypesInfo[$track_type];
      fwrite($file, chr($type_info['id']));
      fwrite($file, pack('V', $track_info['start_byte']));
    }

    // Write the track data
    foreach ($tracks_info as $track_type => $track_info) {
      $type_info = self::$trackTypesInfo[$track_type];
      $track = $tracks[$track_type];
      
      $data_format = isset($type_info['data_format']) ? $type_info['data_format'] : 'C';
      $multiple = isset($type_info['data_multiple']) ? $type_info['data_multiple'] : 1;

      $header = $track['header'];
      $data = $track['data']; 
      $track_offset = $header['offset'];
      
      fwrite($file, chr($type_info['version']));
      fwrite($file, pack('V', $track_info['num_points']));
      fwrite($file, pack('V', max($track_offset, 0)));
      if ($track_offset >= 0) $track_offset = null;
      
      for ($p = $header['start']; $p < $track_info['num_points']; $p++) {
        $offset = $data[$p*2];
        if (isset($track_offset)) {
          $offset += $track_offset;
          $track_offset = null;
        }
        fwrite($file, chr($offset));
        $value = $data[$p*2+1];
        if ($multiple != 1) $value *= $multiple;
        if ($data_format == 'C') {
          fwrite($file, chr($value));
        } else {
          if (is_array($value)) {
            $copy = $value;
            array_unshift($copy, $data_format);
            $bytes = call_user_func_array('pack', $copy);
            fwrite($file, $bytes);
          } else {          
            fwrite($file, pack($data_format, $value));
          }
        }
      }
    }
    fclose($file);    
  }  
}

class ActivityDataTrackReader extends ActivityDataTrackAccess {

  /**
   * Construct a reader for the specified activity.
   */
  function __construct($activity_id, $type = '') {
    parent::__construct($activity_id, $type);
  }
  
  /**
   * Read the data tracks.
   *
   * @param $track_types
   *   An array of track types or null to get all.
   * @param $start
   *   A starting cutoff. Values before this will not be returned.
   * @param $end
   *   An ending cutoff. Values after this will not be returned.
   * @return
   *   An associative array by track type of:
   *   offset[0], value[0] .. offset[n], value[n]
   *   Where offset is the elapsed time since the start of the track.
   */
  public function readTracks($track_types = null, $start = null, $end = null) {
    $filename = $this->getFilename();
    
    if (!file_exists($filename)) return array();

    // Read track data
    $file = fopen($filename, 'rb');
    if (!$file) throw new Exception('Could not open file for reading.');

    $tracks_info = $this->readTracksInfo($file);
    
    if ($track_types == null) $track_types = array_keys($tracks_info);
    $tracks = array();    
    foreach ($track_types as $track_type) {
      if (!isset($tracks_info[$track_type])) continue;
      if (!isset(self::$trackTypesInfo[$track_type])) continue;
      $type_info = self::$trackTypesInfo[$track_type];
      
      fseek($file, $tracks_info[$track_type]);
      // Read the track header
      $header = array();
      $version = ord(fread($file, 1));
      $num_points = unpack('V', fread($file, 4));
      $num_points = $num_points[1];
      $offset = unpack('V', fread($file, 4));
      $offset = $offset[1];
      $header['offset'] = $offset;
      $data_size = $type_info['data_size'];
      $data_format = isset($type_info['data_format']) ? $type_info['data_format'] : 'C';
      $multiple = isset($type_info['data_multiple']) ? $type_info['data_multiple'] : 1;

      // Read the points
      $points = array();
      
      //$tracks[$track_type] = array('header' => $header, 'data' => $points);
      //continue;
      
      $time = 0;
      for ($p = 0; $p < $num_points; $p++) {
        $offset = ord(fread($file, 1));
        $time += $offset;
        $after_end = isset($end) && $time > $end;
        if ($after_end) break; // Early exit
        
        $value = fread($file, $data_size);
        
        $before_start = isset($start) && $time < $start;

        if (!$before_start) {
          if ($data_format == 'C') {
            $value = ord($value);
          } else {
            $value = unpack($data_format, $value);
            if (count($value) == 1) $value = $value[1];
          }
          if ($multiple != 1) $value /= $multiple;
        
          $points[] = $time;
          $points[] = $value;
        }
      }
      $header['total_time'] = $time;
      
      $tracks[$track_type] = array('header' => $header, 'data' => $points);
    }
    
    fclose($file);
    
    return $tracks;
  }
  
  /**
   * Read the file header section and return an associative array of track type > byte offset.
   *
   * @param $file
   *   A file handle.
   */
  private function readTracksInfo($file) {
    // Read the track header
    $version = ord(fread($file, 1));
    $num_tracks = ord(fread($file, 1));

    $type_for_ids = array();
    foreach (self::$trackTypesInfo as $type => $type_info) {
      if (!isset($type_info['id'])) continue;
      $type_for_ids[$type_info['id']] = $type;
    }
    
    // Read the track info
    $tracks_info = array();
    for ($i = 0; $i < $num_tracks; $i++) {
      $type_id = ord(fread($file, 1));
      $start_byte = unpack('V', fread($file, 4));
      if (!isset($type_for_ids[$type_id])) continue;
      
      $start_byte = $start_byte[1];
      $tracks_info[$type_for_ids[$type_id]] = $start_byte;
    }
    
    return $tracks_info;
  }
}
?>