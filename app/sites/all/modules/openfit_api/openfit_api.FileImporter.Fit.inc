<?php
require_once('openfit_api.ActivityDataTrack.inc');

class FitFileImporter {
  public function __construct() {
    $this->machine_endian = (pack ('L', 0x6162797A) == pack ('V', 0x6162797A));
    $this->geo_coord_multiplier = 180 / pow(2, 31);
    $this->base_types = array(
      self::BASE_TYPE_ENUM => array('null' => pack('H*', 'FF'), 'endian' => false),
      self::BASE_TYPE_SINT8 => array('null' => pack('H*', '7F'), 'endian' => false),
      self::BASE_TYPE_UINT8 => array('null' => pack('H*', 'FF'), 'endian' => false),
      self::BASE_TYPE_SINT16 => array('null' => pack('H*', '7FFF'), 'unpack' => 's'),
      self::BASE_TYPE_UINT16 => array('null' => pack('H*', 'FFFF'), 'unpack' => 'S'),
      self::BASE_TYPE_SINT32 => array('null' => pack('H*', '7FFFFFFF'), 'unpack' => 'l'),
      self::BASE_TYPE_UINT32 => array('null' => pack('H*', 'FFFFFFFF'),),
      self::BASE_TYPE_STRING => array('unpack' => 'a', 'endian' => false),
      self::BASE_TYPE_FLOAT32 => array('null' => pack('H*', 'FFFFFFFF'), 'unpack' => 'f'),
      self::BASE_TYPE_FLOAT64 => array('null' => pack('H*', 'FFFFFFFFFFFFFFFF'), 'unpack' => 'd'),
      self::BASE_TYPE_UINT8Z => array('null' => pack('H*', '00'), 'endian' => false),
      self::BASE_TYPE_UINT16Z => array('null' => pack('H*', '0000'), 'unpack' => 'S'),
      self::BASE_TYPE_UINT32Z => array('null' => pack('H*', '00000000'),),
      self::BASE_TYPE_BYTE => array(),
    );
    if ($this->machine_endian) {
      foreach ($this->base_types as $key => &$value) {
        if (isset($value['null'])) $value['null'] = strrev($value['null']);
      }
    }
  }
  
  public function importFile($filename) {
    $import_data = $this->readFile($filename);
    
    $import_results = array();
    // Set the import metadata.
    $import_results['metadata'] = array(
      'type' => 'file',
      'file' => array('name' => $filename),
    );
    if (isset($import_data['file_id'])) {
      $file_id = $import_data['file_id'];
      if (isset($file_id['time_created'])) $import_results['metadata']['time_created'] = $file_id['time_created'];
      
      $serial = isset($file_id['serial_number']) ? $file_id['serial_number'] : '';
      $manufacturer = t('Unknown');
      $model = t('Unknown');
      if (isset($file_id['manufacturer']) && isset(self::$productNames[$file_id['manufacturer']])) {
        $products = self::$productNames[$file_id['manufacturer']];
        $manufacturer = $products['manufacturer_name'];
        if (isset($file_id['product']) && isset($products['products'][$file_id['product']])) {
          $model = $products['products'][$file_id['product']];
        }
      }
      $import_results['metadata']['device'] = array(
        'manufacturer' => $manufacturer,
        'model' => $model,
        'serial' => $serial,
      );
    }
    
    // Activities.
    $import_results['activities'] = $import_data['activities'];
    // TODO: weight_scale_readings
    
    return $import_results;
  }
  
  private function readFile($filename) {
    $data = array();
    $data['file_header'] = array();
    $data['file_id'] = array();
    $data['file_creator'] = array();
    $data['device_info'] = array();
    $data['activities'] = array();
    $data['weight_scale_readings'] = array();
    
    $file = fopen($filename, 'rb');
    if (!$file) throw new Exception('Could not open file for reading.');
   
    // Read the header block. 12 bytes (or more) indicating information about the file. 
    $header_size = ord(fread($file, 1));
    $data['file_header']['protocol_version'] = ord(fread($file, 1));
    $data['file_header']['minor_version'] = ord(fread($file, 1));
    $data['file_header']['major_version'] = ord(fread($file, 1));
    $file_data_size = unpack('V', fread($file, 4));
    $file_data_size = $file_data_size[1];
    $data['file_header']['data_size'] = $file_data_size;
    $filetype = unpack('a4', fread($file, 4));
    $filetype = $filetype[1];
    fseek($file, $header_size);
    
    $compressed_time_reference = null;
    $zero_next_compressed_rollover = false;
    $compressed_offset_rollover = 0;
    $prior_compressed_offset = 0;
              
    $sessions = array();
    $laps = array();
    $pool_lengths = array();
    $records = array();
    
    // Loop through each record in the file.
    $reader_position= 0;
    $definitions = array();
    while ($reader_position < $file_data_size) {
      $record_header_byte = ord(fread($file, 1));      
      if (($record_header_byte & 0x80) != 0) {
        $record_header_type = self::HEADER_TYPE_COMPRESSED_TIMESTAMP_DATA;
      } else if (($record_header_byte & 0x60) != 0) {
        $record_header_type = self::HEADER_TYPE_DEFINITION;
      } else {
        $record_header_type = self::HEADER_TYPE_DATA;
      }
      if (($record_header_byte & 0x80) != 0) {
        $local_message_type = ($record_header_byte & 0x60) >> 5; // bits 5-6
      } else {
        $local_message_type = ($record_header_byte & 0x0F); // bits 0-3
      }
      $reader_position++;
      if ($record_header_type == self::HEADER_TYPE_DEFINITION) {
        $definition = array();
        // Reserved byte.
        fread($file, 1);
        $reader_position++;
        // Architecture.
        $little_endian = (ord(fread($file, 1)) == 0);
        $reader_position++;
        // Global message number.
        $pack_format = $little_endian ? 'v' : 'n';
        $global_message_number = unpack($pack_format, fread($file, 2));
        $global_message_number = $global_message_number[1];
        $reader_position += 2;
        // Number of fields.
        $number_of_fields = ord(fread($file, 1));
        $reader_position++;
        // Field definitions.
        $total_data_size = 0;
        $fields = array();
        for($i = 0; $i < $number_of_fields; $i++) {
          $number = ord(fread($file, 1));
          $data_size = ord(fread($file, 1));
          $base_type = ord(fread($file, 1));
          $reader_position += 3;
          $fields[] = array('number' => $number, 'data_size' => $data_size, 'base_type' => $base_type);
          $total_data_size += $data_size;
        }
        $definitions[$local_message_type] = array(
          'endian' => $little_endian,
          'msg' => $global_message_number,
          'fields' => $fields,
          'data_size' => $total_data_size,
        );
      } else if (isset($definitions[$local_message_type])) {
        $defn = $definitions[$local_message_type];
        $endian = $defn['endian'];
        $msg = $defn['msg'];
        
        // For messages we don't care about: read, discard, continue loop.
        if (!isset($this->message_fields[$msg])) {
          fread($file, $defn['data_size']);
          $reader_position += $defn['data_size'];        
          //echo  'IGNORED data msg=' . $msg . '<br />';
          continue;
        }
        
        // Read data value for each field from file and unpack based on type.
        $field_values = array();
        foreach ($defn['fields'] as $field_defn) {
          $number = $field_defn['number'];
          $field_value = fread($file, $field_defn['data_size']);
          $base_type = $field_defn['base_type'];
          
          // Use the raw value for base types which are not recognized.
          if (!isset($this->base_types[$base_type])) {
            $field_values[$number] = $field_value;
            continue;
          }
          
          $unpack_endian = !isset($this->base_types[$base_type]['endian']) || !$this->base_types[$base_type]['endian'];
          if ($unpack_endian && $endian != $this->machine_endian) $field_value = strrev($field_value);

          // If the base type has a defined null and it matches the value, set the field value to null.
          if (isset($this->base_types[$base_type]['null']) && $field_value == $this->base_types[$base_type]['null']) {
            $field_values[$number] = null;
            continue;
          }
          
          // Parse the raw binary string depending on base type characteristics.
          switch ($base_type) {
            case self::BASE_TYPE_ENUM:
            case self::BASE_TYPE_UINT8:
            case self::BASE_TYPE_UINT8Z:
              $field_values[$number] = ord($field_value);
              break;
            case self::BASE_TYPE_SINT8:
              $field_value = ord($field_value);
              if ($field_value > 0x7F) $field_value = $field_value - 256;
              $field_values[$number] = $field_value;
              break;
            case self::BASE_TYPE_UINT32:
            case self::BASE_TYPE_UINT32Z:
              $field_value = unpack('L', $field_value);
              $field_value = $field_value[1];
              if ($field_value < 0) $field_value = 0x100000000 + $field_value;
              $field_values[$number] = $field_value;

              break;
            default:
              if (isset($this->base_types[$base_type]['unpack'])) {
                $unpack_format = $this->base_types[$base_type]['unpack'];
                $ov = $field_value;
                $field_value = unpack($unpack_format, $field_value);
                $field_value = $field_value[1];
                $field_values[$number] = $field_value;
              }
              break;
          }
        }

        $reader_position += $defn['data_size'];
        
        // Copy the raw field values into meaningful record values.
        $record = null;
        if (isset($this->message_fields[$msg])) {
          $record = array();
          foreach ($field_values as $number => $field_value) {
            if (isset($this->message_fields[$msg][$number]) && isset($field_value)) {
              $record[$this->message_fields[$msg][$number]] = $field_value;
            }
          }
          if (count($record) == 0) $record = null;
        }
        
        if (isset($record) && $record_header_type == self::HEADER_TYPE_COMPRESSED_TIMESTAMP_DATA) {
          $time_offset = ($record_header_byte & 0x1F); // bits 0-4
          if ($zero_next_compressed_rollover) {
            $zero_next_compressed_rollover = false;
            $compressed_offset_rollover = -$time_offset;
          } else {
            if ($time_offset < $prior_compressed_offset) $compressed_offset_rollover += 32;
          }
          $prior_compressed_offset = $time_offset;
          $offset_seconds = $compressed_offset_rollover + $time_offset;
          $record['timestamp'] = $compressed_time_reference + $offset_seconds;
        }
        
        // If the record has values, process it based on type.
        if (isset($record)) {
          switch ($msg) {
            case self::MESSAGE_FILE_ID:
              if (isset($record['time_created'])) $record['time_created'] = $this->timestampToDateTime($record['time_created']);
              $data['file_id'] = $record;
              break;
            case self::MESSAGE_SESSION:
              if (isset($record['start_time'])) $compressed_time_reference = $record['start_time'];
              $sessions[] = $record;
              if(count($sessions) > 0 && count($laps) > 0) {
                $data['activities'][] = $this->processActivity($sessions, $laps, $records);
              }
              $sessions = array();
              $laps = array();
              $records = array();
              break;
            case self::MESSAGE_LAP:
              // Swimming workouts with zero length laps should be marked as rest.
              if ($record['sport'] == 5 && $record['total_distance'] == 0 && $record['total_cycles'] == 0) $record['intensity'] = 1;
              if (count($pool_lengths) > 0) $record['pool_lengths'] = $pool_lengths;
              $laps[] = $record;
              $pool_lengths = array();
              break;
            case self::MESSAGE_RECORD:
            case self::MESSAGE_EVENT:
              $record['msg'] = $msg;
              $records[] = $record;
              break;
            case self::MESSAGE_COMPRESSED_HEADER_TIME_EVENT:
              $compressed_time_reference = $record['timestamp'];
              $zero_next_compressed_rollover = true;
              break;
            case self::MESSAGE_DEVICE_INFO:
              if (isset($record['timestamp'])) $record['timestamp'] = $this->timestampToDateTime($record['timestamp']);
              $data['device_info'] = $record;
              break;
            case self::MESSAGE_WEIGHT_SCALE:
              if (isset($record['timestamp'])) $record['timestamp'] = $this->timestampToDateTime($record['timestamp']);
              if (isset($record['weight'])) $record['weight'] /= 100;
              if (isset($record['percent_fat'])) $record['percent_fat'] /= 100;
              if (isset($record['percent_hydration'])) $record['percent_hydration'] /= 100;
              if (isset($record['bone_mass'])) $record['bone_mass'] /= 100;
              if (isset($record['muscle_mass'])) $record['muscle_mass'] /= 100;
              if (isset($record['basal_met'])) $record['basal_met'] *= self::KILOJOULE_PER_KCAL/4;
              if (isset($record['active_met'])) $record['active_met'] *= self::KILOJOULE_PER_KCAL/4;
              $data['weight_scale_readings'][] = $record;
              break;
            case self::MESSAGE_ACTIVITY:
              $sessions = array();
              $laps = array();
              $pool_lengths = array();
              $records = array();
              break;
            case self::MESSAGE_FILE_CREATOR:
              $data['file_creator'] = $record;
              break;
            case self::MESSAGE_POOL_LENGTH:
              $pool_lengths[] = $record;
              break;
          }
        } else {
          $field_values['msg'] = $msg;
          $data['unrecognized_records'] = $field_values;
        }
      }
    }
    
    fclose($file);
    
    if(count($sessions) > 0 && count($laps) > 0) {
      $data['activities'][] = $this->processActivity($sessions, $laps, $records);
    }
    
    return $data;
  }
  
  private function processActivity($sessions, $laps, $records) {
    $last_distance16 = 0;
    $accum_distance16 = 0;
    $activity = array();
    $activity_sessions = array();
    $activity_laps = array();
    $timer_stops = array();

    // Process overall information stored in first session record.
    $first_session = $sessions[0];
    $start_timestamp = $first_session['start_time'];
    $activity['start_time'] = $this->timestampToDateTime($start_timestamp);
    $sport = $this->sport_types[0];
    if (isset($this->sport_types[$first_session['sport']])) $sport = $this->sport_types[$first_session['sport']];
    $activity['sport_type'] = $sport;
    
    $summary_fields = array(
      'total' => array(
        'distance' => 'total_distance',
        'clock_duration' => 'total_elapsed_time',
        'duration' => 'total_timer_time',
        'calories' => 'total_calories',
        'elevation_gain' => 'total_ascent',
        'elevation_loss' => 'total_descent',
      ),
      'avgmax' => array(
        'speed' => 'speed',
        'heartrate' => 'heartrate',
        'cadence' => 'cadence',
        'power' => 'power',
      ),
    );

    // Process session records and accumulate activity totals, avg, max.
    $activity_averages = array();
    foreach ($summary_fields['total'] as $to => $from) {
      $activity[$to] = 0;
    }
    foreach ($summary_fields['avgmax'] as $to => $from) {
      $activity_avgmax[$to] = array('avg_value' => 0, 'avg_time' => 0);
    }
    $num_sessions = count($sessions);
    foreach ($sessions as $session) {
      $session_info = array();
      $session_info['start_time'] = $this->timestampToDateTime($session['start_time']);
      $session_info['first_lap'] = isset($session['first_lap_index']) ? $session['first_lap_index'] : 0;
      $session_info['num_laps'] = $session['num_laps'];
      $sport = $this->sport_types[0];
      if (isset($this->sport_types[$session['sport']])) $sport = $this->sport_types[$session['sport']];
      $session_info['sport_type'] = $sport;
      if ($sport != $activity['sport_type']) $activity['sport_type'] = 'other';
                  
      foreach ($summary_fields['total'] as $to => $from) {
        $session_info[$to] = isset($session[$from]) ? $session[$from] : 0;
      }
      foreach ($summary_fields['avgmax'] as $to => $from) {
        $avg = isset($session['avg_' . $from]) ? $session['avg_' . $from] : null;
        $max = isset($session['max_' . $from]) ? $session['max_' . $from] : null;
        if (isset($avg) || isset($max)) {
          $session_info[$to] = array();
          if (isset($avg)) $session_info[$to]['avg'] = $avg;
          if (isset($max)) $session_info[$to]['max'] = $max;
        }
      }
      
      $session_info['distance'] /= 100;
      $session_info['clock_duration'] /= 1000;
      $session_info['duration'] /= 1000;
      $session_info['calories'] *= self::KILOJOULE_PER_KCAL;
      if (isset($session_info['speed']) && isset($session_info['speed']['avg'])) $session_info['speed']['avg'] /= 1000;
      if (isset($session_info['speed']) && isset($session_info['speed']['max'])) $session_info['speed']['max'] /= 1000;
      
      foreach ($summary_fields['total'] as $to => $from) {
        $activity[$to] += $session_info[$to];
      }
      
      foreach ($summary_fields['avgmax'] as $to => $from) {
        if (!isset($session_info[$to])) continue;
        if (isset($session_info[$to]['avg'])) {
          $activity_avgmax[$to]['avg_time'] += $session_info['duration'];
          $activity_avgmax[$to]['avg_value'] += $session_info[$from]['avg'] * $session_info['duration'];
        }
        if (isset($session_info[$to]['max'])) {
          $max = $session_info[$to]['max'];
          if (isset($activity_avgmax[$to]['max'])) $max = max($max, $activity_avgmax[$to]['max']);
          $activity_avgmax[$to]['max'] = $max;
        }
      }
      if ($num_sessions > 0) $activity_sessions[] = $session_info;
    }
    foreach ($activity_avgmax as $field => $info) {
      if ($info['avg_time'] > 0 || isset($info['max'])) {
        $activity[$field] = array();
        if ($info['avg_time'] > 0) $activity[$field]['avg'] = $info['avg_value'] / $info['avg_time'];
        if (isset($info['max'])) $activity[$field]['max'] = $info['max'];
      }
    }

    // Process lap records.
    foreach ($laps as $lap) {
      $lap_info = array();
      $lap_info['start_time'] = $this->timestampToDateTime($lap['start_time']);
      $sport = $this->sport_types[0];
      if (isset($this->sport_types[$lap['sport']])) $sport = $this->sport_types[$lap['sport']];
      $lap_info['sport_type'] = $sport;
      if (isset($lap['intensity']) && isset($this->lap_types[$lap['intensity']])) {
        $lap_info['type'] = $this->lap_types[$lap['intensity']];
      } else {
        $lap_info['type'] = 'active';
      }
      
      foreach ($summary_fields['total'] as $to => $from) {
        $lap_info[$to] = isset($lap[$from]) ? $lap[$from] : 0;
      }
      foreach ($summary_fields['avgmax'] as $to => $from) {
        $avg = isset($lap['avg_' . $from]) ? $lap['avg_' . $from] : null;
        $max = isset($lap['max_' . $from]) ? $lap['max_' . $from] : null;
        if (isset($avg) || isset($max)) {
          $lap_info[$to] = array();
          if (isset($avg)) $lap_info[$to]['avg'] = $avg;
          if (isset($max)) $lap_info[$to]['max'] = $max;
        }
      }
      $lap_info['distance'] /= 100;
      $lap_info['clock_duration'] /= 1000;
      $lap_info['duration'] /= 1000;
      $lap_info['calories'] *= self::KILOJOULE_PER_KCAL;
      if (isset($lap_info['speed']) && isset($lap_info['speed']['avg'])) $lap_info['speed']['avg'] /= 1000;
      if (isset($lap_info['speed']) && isset($lap_info['speed']['max'])) $lap_info['speed']['max'] /= 1000;
      
      if (isset($lap['pool_lengths'])) {
        $pool_lengths_info = array();
        foreach($lap['pool_lengths'] as $pool_length) {
          $length_info = array();
          $length_info['time'] = $pool_length['total_elapsed_time'] / 1000;
          $length_info['strokes'] = $pool_length['num_strokes'];
          $length_info['type'] = 'unspecified';
          if (isset($pool_length['stroke_type']) && isset($this->swim_stroke_types[$pool_length['stroke_type']])) {
            $length_info['type'] = $this->swim_stroke_types[$pool_length['stroke_type']];
          }
          $pool_lengths_info[] = $length_info;
        }
        $lap_info['pool_lengths'] = $pool_lengths_info;
      }
      
      $activity_laps[] = $lap_info;
    }
    
    // Process records for detail tracks
    $data_tracks = array(
      ActivityDataTrackAccess::LOCATION => array(),
      ActivityDataTrackAccess::ELEVATION => array(),
      ActivityDataTrackAccess::DISTANCE => array(),
      ActivityDataTrackAccess::HEARTRATE => array(),
      ActivityDataTrackAccess::CADENCE => array(),
      ActivityDataTrackAccess::POWER => array(),
      ActivityDataTrackAccess::TEMPERATURE => array(),
    );
    $tracks_info = array(
      ActivityDataTrackAccess::LOCATION => array(
        'timestamp' => null),
      ActivityDataTrackAccess::ELEVATION => array(
        'field' => 'altitude', 'timestamp' => null, 'compress' => true, 'last_elapsed' => null, 'last_value' => null),
      ActivityDataTrackAccess::DISTANCE => array(
        'field' => 'distance', 'timestamp' => null),
      ActivityDataTrackAccess::HEARTRATE => array(
        'field' => 'heartrate', 'timestamp' => null, 'compress' => true, 'last_elapsed' => null, 'last_value' => null),
      ActivityDataTrackAccess::CADENCE => array(
        'field' => 'cadence', 'timestamp' => null, 'compress' => true, 'last_elapsed' => null, 'last_value' => null),
      ActivityDataTrackAccess::POWER => array(
        'field' => 'power', 'timestamp' => null, 'compress' => true, 'last_elapsed' => null, 'last_value' => null),
      ActivityDataTrackAccess::TEMPERATURE => array(
        'field' => 'temperature', 'timestamp' => null, 'compress' => true, 'last_elapsed' => null, 'last_value' => null),
    );
    foreach ($tracks_info as $track_type => &$track_info) {
      $track_info['data_format'] = ActivityDataTrackAccess::getDataFormat($track_type);
    }

    $last_timer_stop = null;
    $last_record_time = null;
    $total_session_distance = 0;
    foreach ($records as $record) {
      switch ($record['msg']) {
        // Process a track data record.
        case self::MESSAGE_RECORD: {
          $timestamp = isset($record['timestamp']) ? $record['timestamp'] : $last_record_time;
          if (!isset($last_timer_stop) && (!isset($last_record_time) || $timestamp >= $last_record_time)) {
            foreach ($tracks_info as $track_type => &$track_info) {
              // Read the track value
              $value = null;
              switch ($track_type) {
                case ActivityDataTrackAccess::LOCATION:
                  if (isset($record['position_lat']) && isset($record['position_long'])) {
                    $lat = $record['position_lat'] * $this->geo_coord_multiplier;
                    $long = $record['position_long'] * $this->geo_coord_multiplier;
                    $value = array($lat, $long);
                    $value = pack($track_info['data_format'],$lat,$long);
                  }
                  break;
                case ActivityDataTrackAccess::ELEVATION:
                  $field = $track_info['field'];
                  if (isset($record[$field])) {
                    $value = ($record[$field] / 5) - 500;
                    $value = pack($track_info['data_format'],$value);
                  }
                  break;
                case ActivityDataTrackAccess::DISTANCE:
                  $field = $track_info['field'];
                  if (isset($record[$field])) {
                    if (isset($record['compressed_speed_distance'])) {
                      $byte1 = ($record[$field] >> 8) & 0xFF;
                      $byte2 = ($record[$field] >> 16) & 0xFF;
                      $distance16 = $byte1 >> 4 | (($byte2 << 4) & 0xFF);
                      $accum_distance16 += ($distance16 - $last_distance16) & 0x0FFF;
                      $last_distance16 = $distance16;
                      $value = $accum_distance16 / 16;
                    } else {
                      $value = $record[$field] / 100;
                    }
                    $value += $total_session_distance;
                    $value = pack($track_info['data_format'],$value);
                  }
                  break;
                default:
                  $field = $track_info['field'];
                  if (isset($record[$field])) {
                    $value = $record[$field];                
                    $value = pack($track_info['data_format'],$value);
                  }
                  break;
              }
              if (!isset($value)) continue;
              
              // Calculate elapsed time since last point
              if (!isset($track_info['timestamp'])) {
                $track_info['timestamp'] = $timestamp;
                $data_tracks[$track_type]['start_time'] = $this->timestampToDateTime($timestamp);
                $data_tracks[$track_type]['data'] = '';
              }
              $elapsed = $timestamp - $track_info['timestamp'];
              
              // Add point to track
              if (isset($track_info['compress'])) {
                // Omit points where the value has not changed since the last.
                if ($value != $track_info['last_value']) {
                  if (isset($track_info['last_value'])) {
                    $data_tracks[$track_type]['data'] .= pack('L',$track_info['last_elapsed']) . $track_info['last_value'];
                  }
                  $data_tracks[$track_type]['data'] .= pack('L',$elapsed) . $value;
                }
                $track_info['last_elapsed'] = $elapsed;
                $track_info['last_value'] = $value;
              } else {
                $data_tracks[$track_type]['data'] .= pack('L',$elapsed) . $value;
              }
            }
          }
          $last_record_time = $timestamp;
          break;
        }
        
        // Process timer events.
        case self::MESSAGE_EVENT: {
          if ($record['event'] == self::EVENT_TIMER) {
            if ($record['event_type'] == self::EVENT_TYPE_START) {
              if (isset($last_timer_stop)) {
                $from = $last_timer_stop - $start_timestamp;
                $to = $record['timestamp'] - $start_timestamp;
                $timer_stops[] = array('from' => $from, 'to' => $to);
              }
              $last_timer_stop = null;
            } else if ($record['event_type'] == self::EVENT_TYPE_STOP || $record['event_type'] == self::EVENT_TYPE_STOP_ALL) {
              if (!isset($last_timer_stop)) $last_timer_stop = $record['timestamp'];
            }
          }
          break;
        }
        
        // Process session events.
        case self::MESSAGE_SESSION: {
          $session_distance = $record['total_distance'] / 100;
          $total_session_distance += $session_distance;

          if (isset($tracks_info[ActivityDataTrackAccess::DISTANCE]['timestamp'])) {
            // TODO: If there are any pauses during this session, we need to add them to the session_end_time size total_timer_time doesn't include them
            $session_end_timespan = (int)($record['start_time'] + $record['total_timer_time'] / 1000);
            $distance_track_elapsed = $session_end_timespan - $tracks_info[ActivityDataTrackAccess::DISTANCE]['timestamp'];
            $data_tracks[ActivityDataTrackAccess::DISTANCE][$distance_track_elapsed] = $total_session_distance;
          }
          break;
        }
      }
    }
    $activity['sessions'] = $activity_sessions;
    $activity['laps'] = $activity_laps;
    $activity['data_tracks'] = $data_tracks;
    $activity['timer_stops'] = $timer_stops;
    // Determine the standard category from the sport type
    $category_name = 'Other';
    switch ($activity['sport_type']) {
      case 'running':
        $category_name = 'Running';
        break;
      case 'cycling':
        $category_name = 'Cycling';
        break;
      case 'swimming':
        $category_name = 'Swimming';
        break;
    }
    $activity['category'] = $category_name;
    
    return $activity;
  }

  private function timestampToDateTime($timestamp) {
    $time = new \DateTime('now', new \DateTimeZone('UTC'));
    $time->setTimestamp($timestamp);
    $time->add(new \DateInterval('P19Y11M29D'));
    return $time->format('Y-m-d H:i:s');
  }

  private $machine_endian;
  private $geo_coord_multiplier;
  
  private $base_types;
  
  private static $productNames = array(
    1 => array(
      'manufacturer_name' => 'Garmin',
      'products' => array(
        1 => 'HRM1',
        2 => 'AXH01',
        3 => 'AXB01',
        4 => 'AXB02',
        5 => 'HRM2SS',
        6 => 'DSIALF02',
        717 => 'Forerunner 405',
        782 => 'Forerunner 50',
        988 => 'Forerunner 60',
        1011 => 'DSI ALF 01',
        1018 => 'Forerunner 310XT',
        1036 => 'Edge 500',
        1124 => 'Forerunner 110',
        1169 => 'Edge 800',
        1253 => 'Chirp',
        1325 => 'Edge 200',
        1328 => 'Forerunner 910XT',
        1341 => 'ALF04',
        1345 => 'Forerunner 610',
        1436 => 'Forerunner 70',
        1446 => 'Forerunner 310XT 4T',
        10007 => 'SDM4 footpod',
        20119 => 'Garmin Training Center',
        65534 => 'Garmin Connect website',
      ),
    ),
    2 => array('manufacturer_name' => 'Garmin FR405 ANTFS', 'products' => array()),
    3 => array('manufacturer_name' => 'Zephyr', 'products' => array()),
    4 => array('manufacturer_name' => 'Dayton', 'products' => array()),
    5 => array('manufacturer_name' => 'IDT', 'products' => array()),
    6 => array('manufacturer_name' => 'SRM', 'products' => array()),
    7 => array('manufacturer_name' => 'Quarq', 'products' => array()),
    8 => array('manufacturer_name' => 'iBike', 'products' => array()),
    9 => array('manufacturer_name' => 'Saris', 'products' => array()),
    10 => array('manufacturer_name' => 'Spark HK', 'products' => array()),
    11 => array('manufacturer_name' => 'Tanita', 'products' => array()),
    12 => array('manufacturer_name' => 'Echowell', 'products' => array()),
    13 => array('manufacturer_name' => 'Dynastream OEM', 'products' => array()),
    14 => array('manufacturer_name' => 'Nautilus', 'products' => array()),
    15 => array('manufacturer_name' => 'Dynastream', 'products' => array()),
    16 => array('manufacturer_name' => 'Timex', 'products' => array()),
    17 => array('manufacturer_name' => 'Metrigear', 'products' => array()),
    18 => array('manufacturer_name' => 'Xelic', 'products' => array()),
    19 => array('manufacturer_name' => 'Beurer', 'products' => array()),
    20 => array('manufacturer_name' => 'Cardiosport', 'products' => array()),
    21 => array('manufacturer_name' => 'A & D', 'products' => array()),
    22 => array('manufacturer_name' => 'HMM', 'products' => array()),
    23 => array('manufacturer_name' => 'Suunto', 'products' => array()),
    24 => array('manufacturer_name' => 'Thita Elektronik', 'products' => array()),
    25 => array('manufacturer_name' => 'G Pulse', 'products' => array()),
    26 => array('manufacturer_name' => 'Clean Mobile', 'products' => array()),
    27 => array('manufacturer_name' => 'Pedal Brain', 'products' => array()),
    28 => array('manufacturer_name' => 'Peaksware', 'products' => array()),
    29 => array('manufacturer_name' => 'Saxonar', 'products' => array()),
    30 => array('manufacturer_name' => 'Lemond Fitness', 'products' => array()),
    31 => array('manufacturer_name' => 'Dexcom', 'products' => array()),
    32 => array('manufacturer_name' => 'Wahoo Fitness', 'products' => array()),
    33 => array('manufacturer_name' => 'Octane Fitness', 'products' => array()),
    34 => array('manufacturer_name' => 'Archinoetics', 'products' => array()),
    35 => array('manufacturer_name' => 'The Hurt Box', 'products' => array()),
    36 => array('manufacturer_name' => 'Citizen Systems', 'products' => array()),
    38 => array('manufacturer_name' => 'o-synce', 'products' => array()),
    39 => array('manufacturer_name' => 'Holux', 'products' => array()),
    40 => array('manufacturer_name' => 'Concept2', 'products' => array()),
    42 => array('manufacturer_name' => 'One Giant Leap', 'products' => array()),
    43 => array('manufacturer_name' => 'Ace Sensor', 'products' => array()),
    44 => array('manufacturer_name' => 'Brim Brothers', 'products' => array()),
    45 => array('manufacturer_name' => 'Xplova', 'products' => array()),
    46 => array('manufacturer_name' => 'Perception Digital', 'products' => array()),
    47 => array('manufacturer_name' => 'BF1Systems', 'products' => array()),
    48 => array('manufacturer_name' => 'Pioneer', 'products' => array()),
  );
  
  private $message_fields = array(
    self::MESSAGE_FILE_ID => array(
      0 => 'type',
      1 => 'manufacturer',
      2 => 'product',
      3 => 'serial_number',
      4 => 'time_created',
      5 => 'number',
    ),
    self::MESSAGE_SESSION => array(
      253 => 'timestamp',
      0 => 'event',
      1 => 'event_type',
      2 => 'start_time',
      3 => 'start_position_lat',
      4 => 'start_position_long',
      5 => 'sport',
      6 => 'sub_sport',
      7 => 'total_elapsed_time',
      8 => 'total_timer_time',
      9 => 'total_distance',
      10 => 'total_cycles',
      11 => 'total_calories',
      12 => 'cycle_length',
      13 => 'total_fat_calories',
      14 => 'avg_speed',
      15 => 'max_speed',
      16 => 'avg_heartrate',
      17 => 'max_heartrate',
      18 => 'avg_cadence',
      19 => 'max_cadence',
      20 => 'avg_power',
      21 => 'max_power',
      22 => 'total_ascent',
      23 => 'total_descent',
      24 => 'total_training_effect',
      25 => 'first_lap_index',
      26 => 'num_laps',
      27 => 'event_group',
      28 => 'trigger',
      29 => 'nec_lat',
      30 => 'nec_long',
      31 => 'swc_lat',
      32 => 'swc_long',
      34 => 'normalized_power',
      35 => 'training_stress_score',
      36 => 'intensity_factor',
      37 => 'left_right_balance',
      254 => 'message_index',
    ),
    self::MESSAGE_LAP => array(
      253 => 'timestamp',
      0 => 'event',
      1 => 'event_type',
      2 => 'start_time',
      3 => 'start_position_lat',
      4 => 'start_position_long',
      5 => 'end_position_lat',
      6 => 'end_position_long',
      7 => 'total_elapsed_time',
      8 => 'total_timer_time',
      9 => 'total_distance',
      10 => 'total_cycles',
      11 => 'total_calories',
      12 => 'total_fat_calories',
      13 => 'avg_speed',
      14 => 'max_speed',
      15 => 'avg_heartrate',
      16 => 'max_heartrate',
      17 => 'avg_cadence',
      18 => 'max_cadence',
      19 => 'avg_power',
      20 => 'max_power',
      21 => 'total_ascent',
      22 => 'total_descent',
      23 => 'intensity',
      24 => 'lap_trigger',
      25 => 'sport',
      26 => 'event_group',
      27 => 'nec_lat',
      28 => 'nec_long',
      29 => 'swc_lat',
      30 => 'swc_long',
      33 => 'normalized_power',
      34 => 'left_right_balance',
      40 => 'num_pool_lengths',
      254 => 'message_index',
    ),
    self::MESSAGE_RECORD => array(
      253 => 'timestamp',
      0 => 'position_lat',
      1 => 'position_long',
      2 => 'altitude',
      3 => 'heartrate',
      4 => 'cadence',
      5 => 'distance',
      6 => 'speed',
      7 => 'power',
      8 => 'compressed_speed_distance',
      9 => 'grade',
      10 => 'resistance',
      11 => 'time_from_course',
      12 => 'cycle_length',
      13 => 'temperature',
      17 => 'speed_1s',
      18 => 'cycles',
      19 => 'total_cycles',
      28 => 'compressed_accumulated_power',
      29 => 'accumulated_power',
      30 => 'left_right_balance',
    ),
    self::MESSAGE_EVENT => array(
      253 => 'timestamp',
      0 => 'event',
      1 => 'event_type',
      2 => 'data16',
      3 => 'data',
      4 => 'event_group',
    ),
    self::MESSAGE_COMPRESSED_HEADER_TIME_EVENT => array(
      253 => 'timestamp',
    ),
    self::MESSAGE_DEVICE_INFO => array(
      253 => 'timestamp',
      0 => 'device_index',
      1 => 'device_type',
      2 => 'manufacturer',
      3 => 'serial_number',
      4 => 'product',
      5 => 'software_version',
      6 => 'hardware_version',
      7 => 'cum_operating_time',
      10 => 'battery_voltage',
      11 => 'battery_status',
    ),
    self::MESSAGE_WEIGHT_SCALE => array(
      253 => 'timestamp',
      0 => 'weight',
      1 => 'percent_fat',
      2 => 'percent_hydration',
      3 => 'viceral_fat_mass',
      4 => 'bone_mass',
      5 => 'muscle_mass',
      7 => 'basal_met',
      8 => 'physique_rating',
      9 => 'active_met',
      10 => 'metabolic_age',
      11 => 'viceral_fat_rating',
      12 => 'user_profile_index',
    ),
    self::MESSAGE_ACTIVITY => array(
      253 => 'timestamp',
      0 => 'total_timer_time',
      1 => 'num_sessions',
      2 => 'type',
      3 => 'event',
      4 => 'event_type',
      5 => 'local_timestamp',
      6 => 'event_group',
    ),
    self::MESSAGE_FILE_CREATOR => array(
      0 => 'software_version',
      1 => 'hardware_version',
    ),
    self::MESSAGE_POOL_LENGTH => array(
      253 => 'timestamp',
      2 => 'start_time',
      3 => 'total_elapsed_time',
      4 => 'total_timer_time',
      5 => 'num_strokes',
      7 => 'stroke_type',
      11 => 'calories',
      12 => 'is_active',
      254 => 'message_index',
    ),
  );
  
  private $sport_types = array(
    0 => 'generic',
    1 => 'running',
    2 => 'cycling',
    3 => 'transition',
    4 => 'fitness_equipment',
    5 => 'swimming',
    6 => 'basketball',
    7 => 'soccer',
    8 => 'tennis',
    9 => 'american_football',
    10 => 'training',
    254 => 'all',
  );
  private $sport_sub_types = array(
    0 => 'generic',
    1 => 'treadmill', // running
    2 => 'street', // running
    3 => 'trail', // running
    4 => 'track', // running
    5 => 'spin', // cycling
    6 => 'indoor_cycling',
    7 => 'road',
    8 => 'mountain',
    9 => 'downhill',
    10 => 'recumbent',
    11 => 'cyclocross',
    12 => 'hand_cycling',
    13 => 'track_cycling',
    14 => 'indoor_rowing', // fitness_equipment
    15 => 'elliptical', // fitness_equipment 
    16 => 'stair_climbing', // fitness_equipment
    17 => 'lap_swimming', // swimming
    18 => 'open_water', // swimming
    254 => 'all',
    255 => 'unspecified',
  );
  private $lap_types = array(
    0 => 'active',
    1 => 'rest',
    2 => 'warmup',
    3 => 'cooldown',
  );
  private $session_trigger_types = array(
    0 => 'activity_end',
    1 => 'manual',
    2 => 'auto_multi_sport',
    3 => 'fitness_equipment',
  );
  private $swim_stroke_types = array(
    0 => 'freestyle',
    1 => 'backstroke',
    2 => 'breaststroke',
    3 => 'butterfly',
    4 => 'drill',
    5 => 'mixed',
    254 => 'unspecified',
  );
    
  const HEADER_TYPE_DEFINITION = 1;
  const HEADER_TYPE_DATA = 2;
  const HEADER_TYPE_COMPRESSED_TIMESTAMP_DATA = 3;
  
  const MESSAGE_INVALID = 0x7FFF;
  const MESSAGE_FILE_ID = 0;
  const MESSAGE_CAPABILITIES = 1;
  const MESSAGE_DEVICE_SETTINGS = 2;
  const MESSAGE_USER_PROFILE = 3;
  const MESSAGE_HRM_PROFILE = 4;
  const MESSAGE_SDM_PROFILE = 5;
  const MESSAGE_BIKE_PROFILE = 6;
  const MESSAGE_ZONES_TARGET = 7;
  const MESSAGE_HR_ZONE = 8;
  const MESSAGE_POWER_ZONE = 9;
  const MESSAGE_MET_ZONE = 10;
  const MESSAGE_SPORT = 12;
  const MESSAGE_GOAL = 15;
  const MESSAGE_SESSION = 18;
  const MESSAGE_LAP = 19;
  const MESSAGE_RECORD = 20;
  const MESSAGE_EVENT = 21;
  const MESSAGE_COMPRESSED_HEADER_TIME_EVENT = 22; // A guess from reverse engineering FIT files
  const MESSAGE_DEVICE_INFO = 23;
  const MESSAGE_WORKOUT = 26;
  const MESSAGE_WORKOUT_STEP = 27;
  const MESSAGE_SCHEDULE = 28;
  const MESSAGE_WEIGHT_SCALE = 30;
  const MESSAGE_COURSE = 31;
  const MESSAGE_COURSE_POINT = 32;
  const MESSAGE_TOTALS = 33;
  const MESSAGE_ACTIVITY = 34;
  const MESSAGE_SOFTWARE = 35;
  const MESSAGE_FILE_CAPABILITIES = 37;
  const MESSAGE_MESG_CAPABILITIES = 38;
  const MESSAGE_FIELD_CAPABILITIES = 39;
  const MESSAGE_FILE_CREATOR = 49;
  const MESSAGE_BLOOD_PRESSURE = 51;
  const MESSAGE_SPEED_ZONE = 53;
  const MESSAGE_HRV = 78;
  const MESSAGE_POOL_LENGTH = 101;
  const MESSAGE_PAD = 105;
  
  const BASE_TYPE_ENUM = 0x00; // byte
  const BASE_TYPE_SINT8 = 0x01; // sbyte
  const BASE_TYPE_UINT8 = 0x02; // byte
  const BASE_TYPE_SINT16 = 0x83; // short
  const BASE_TYPE_UINT16 = 0x84; // ushort
  const BASE_TYPE_SINT32 = 0x85; // int
  const BASE_TYPE_UINT32 = 0x86; // uint
  const BASE_TYPE_STRING = 0x07; // byte
  const BASE_TYPE_FLOAT32 = 0x88; // float
  const BASE_TYPE_FLOAT64 = 0x89; // double
  const BASE_TYPE_UINT8Z = 0x0A; // byte
  const BASE_TYPE_UINT16Z = 0x8B; // ushort
  const BASE_TYPE_UINT32Z = 0x8C; // uint
  const BASE_TYPE_BYTE = 0x0D; // byte
  
  const EVENT_INVALID = 0xFF;
  const EVENT_TIMER = 0; // Group 0.  Start / stop_all
  const EVENT_WORKOUT = 3; // start / stop
  const EVENT_WORKOUT_STEP = 4; // Start at beginning of workout.  Stop at end of each step.
  const EVENT_POWER_DOWN = 5; // stop_all group 0
  const EVENT_POWER_UP = 6; // stop_all group 0
  const EVENT_OFF_COURSE = 7; // start / stop group 0
  const EVENT_SESSION = 8; // Stop at end of each session.
  const EVENT_LAP = 9; // Stop at end of each lap.
  const EVENT_COURSE_POINT = 10; // marker
  const EVENT_BATTERY = 11; // marker
  const EVENT_VIRTUAL_PARTNER_PACE = 12; // Group 1. Start at beginning of activity if VP enabled; when VP pace is changed during activity or VP enabled mid activity.  stop_disable when VP disabled.
  const EVENT_HR_HIGH_ALERT = 13; // Group 0.  Start / stop when in alert condition.
  const EVENT_HR_LOW_ALERT = 14; // Group 0.  Start / stop when in alert condition.
  const EVENT_SPEED_HIGH_ALERT = 15; // Group 0.  Start / stop when in alert condition.
  const EVENT_SPEED_LOW_ALERT = 16; // Group 0.  Start / stop when in alert condition.
  const EVENT_CAD_HIGH_ALERT = 17; // Group 0.  Start / stop when in alert condition.
  const EVENT_CAD_LOW_ALERT = 18; // Group 0.  Start / stop when in alert condition.
  const EVENT_POWER_HIGH_ALERT = 19; // Group 0.  Start / stop when in alert condition.
  const EVENT_POWER_LOW_ALERT = 20; // Group 0.  Start / stop when in alert condition.
  const EVENT_RECOVERY_HR = 21; // marker
  const EVENT_BATTERY_LOW = 22; // marker
  const EVENT_TIME_DURATION_ALERT = 23; // Group 1.  Start if enabled mid activity (not required at start of activity). Stop when duration is reached.  stop_disable if disabled.
  const EVENT_DISTANCE_DURATION_ALERT = 24; // Group 1.  Start if enabled mid activity (not required at start of activity). Stop when duration is reached.  stop_disable if disabled.
  const EVENT_CALORIE_DURATION_ALERT = 25; // Group 1.  Start if enabled mid activity (not required at start of activity). Stop when duration is reached.  stop_disable if disabled.
  const EVENT_ACTIVITY = 26; // Group 1..  Stop at end of activity.
  const EVENT_FITNESS_EQUIPMENT = 27; // marker
  
  const EVENT_TYPE_INVALID = 0xFF;
  const EVENT_TYPE_START = 0;
  const EVENT_TYPE_STOP = 1;
  const EVENT_TYPE_CONSECUTIVE_DEPRECIATED = 2;
  const EVENT_TYPE_MARKER = 3;
  const EVENT_TYPE_STOP_ALL = 4;
  const EVENT_TYPE_BEGIN_DEPRECIATED = 5;
  const EVENT_TYPE_END_DEPRECIATED = 6;
  const EVENT_TYPE_END_ALL_DEPRECIATED = 7;
  const EVENT_TYPE_STOP_DISABLE = 8;
  const EVENT_TYPE_STOP_DISABLE_ALL = 9;
  
  const KILOJOULE_PER_KCAL = 4.184;
}

?>