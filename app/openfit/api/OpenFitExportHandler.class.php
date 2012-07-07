<?php

class OpenFitTrackIterator implements Iterator {
  private $i;
  private $trackData;
  private $trackHeader;
  
  /**
   * @param $track 
   *    Array that is consistent with the output from ActivityDataTrackReader->readTracks()
   * @param $startTime
   *    optional integer timestamp to add to the key (time)
   */
  public function __construct(Array $track, DateTime $startTime) {
    $this->i = 0;
    $this->trackData = $track['data'];
    $this->trackHeader = $track['header'];
  }
  
  public function valid() {
    if ($this->i < 0 || $this->i > count($this->trackData) - 1) return false;
    return true;
  }
  
  public function current() {
    return $this->trackData[$this->i+1];
  }
  
  public function key() {
    return ($this->trackData[$this->i] + $this->trackHeader['offset']);
  }
  
  public function next() {
    $this->i += 2;
  }
  
  public function rewind(){
    $this->i = 0;
  }
}

class OpenFitExportHandler {
  public static function handle() {
    global $base_url;
    
    require_once DRUPAL_ROOT . '/includes/path.inc'; //Allows use of url()
    module_load_include('inc', 'openfit_api', 'openfit_api.ActivityDataTrack');
    
    $node_id = $_GET['nid'];
    
    $activity = self::loadActivity($node_id);
    if (!isset($activity)) {
      header('Location: ' . $base_url . '/node/' . $node_id . '/export?noauth');
    }
    
    $type = $_GET['type'];
    switch ($type) {
      case 'gpx':
        self::exportGpx($activity);
      break;
      case 'tcx':
        self::exportTcx($activity);
      break;
    }
  }
  
  private static function exportTcx($activity) {
    $info = $activity->openfit_info;
    $utc = new DateTimeZone('UTC');
    $filename = str_replace(' ', '_', $info['activity']->activity_start) . '.tcx';
    header("Content-Type: text/tcx/force-download");
    header("Content-Disposition: attachment; filename=$filename");
    
    $startTime = new DateTime($info['activity']->activity_start, $utc);
    
    $reader = new ActivityDataTrackReader($info['activity']->activity_id, 'full');
    
    $supportedTracks = array(
      ActivityDataTrackAccess::LOCATION, 
      ActivityDataTrackAccess::ELEVATION, 
      ActivityDataTrackAccess::DISTANCE, 
      ActivityDataTrackAccess::HEARTRATE
    );
    
    $data = $reader->readTracks($supportedTracks);
    
    //find the start and end times for each lap. Do some formatting
    $lap_times = array();
    $laps = array();
    foreach ($info['laps'] as $lap_num => $lap) {
        
      $time = new DateTime($info['laps'][$lap_num]->lap_start, $utc);
      $lap->lap_calories = intval(OpenFitMeasurement::convert($lap->lap_calories, null, OpenFitMeasurement::MEASUREMENT_CALORIE));
      $lap->lap_start = $time->format('Y-m-d\TH:i:s\Z');
      
      $lap->points = array();
      $laps[$lap_num] = $lap;
      
      $endTime = clone $time;
      $lap_times[$lap_num] = array($time, $endTime->add(new DateInterval('PT'.intval($info['laps'][$lap_num]->lap_duration).'S')));
    }
    
    //Store hash counters in array to allow for multiple single-timestamp points
    $counters = array();
    foreach($data as $type => $track) {
      $iter = new OpenFitTrackIterator($track, $startTime);
      $current_lap = array_shift(array_keys($laps));
      foreach ($iter as $offsetS => $value) {
        $time = clone $startTime;
        $time->add(new DateInterval('PT'.$offsetS.'S'));
        if (is_object($time)) {
          $formatted_time = $time->format('Y-m-d\TH:i:s\Z');
          
          //Ensure point is inside some lap, figure out which one if so
          if ($time > $lap_times[$current_lap][1]) {
            if (isset($lap_times[$current_lap + 1])) {
              if ($time > $lap_times[$current_lap + 1][0]) {
                ++$current_lap; //Advance lap
              } else {
                continue; //Drop point
              }
            } else {
              continue; //Drop point
            }
          } else if ($time < $lap_times[$current_lap][0]){
            throw new RuntimeException("Something went wrong, time: ".$time->format('Y-m-d\TH:i:s\Z').' is before start of lap.');
          }
          
          //Handles sequentiality for non-timestamped data
          if (isset($laps[$current_lap]->points[$formatted_time])
            && isset($laps[$current_lap]->points[$formatted_time][$type])) {
            if (!isset($counters[$formatted_time])) $counters[$formatted_time] = 1;
            $formatted_time = $formatted_time.$counters[$formatted_time];
            $counters[$formatted_time]++;
          }
          
          if (isset($laps[$current_lap]->points[$formatted_time])) {
            $laps[$current_lap]->points[$formatted_time][$type] = $value;
          } else {
            $laps[$current_lap]->points[$formatted_time] = 
                array($type => $value, 'time' => $formatted_time);
          }
        }
      }
    }
    
    //For debugging purposes, 
    //probably won't matter that points aren't in proper order when really exporting.
    foreach (array_keys($laps) as $lap_num) {
      ksort($laps[$lap_num]->points);
    }
    
    //die(print_r($laps, true));
    
    $out = new XMLWriter();
    $out->openURI('php://output');
    
    $out->startDocument('1.0', 'UTF-8', 'no'); 
    $out->setIndent(true); 
    $out->setIndentString('  ');
    
    $out->startElement('TrainingCenterDatabase');
    $out->writeAttribute('xsi:schemaLocation', 'http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2 http://www.garmin.com/xmlschemas/TrainingCenterDatabasev2.xsd');
    $out->writeAttribute('xmlns', 'http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2');
    $out->writeAttribute('xmlns:xsi', 'http://www.w3.org/2001/XMLSchema-instance');
    
    $out->startElement('Activities');
    $out->startElement('Activity');
    $out->writeAttribute('Sport', $info['activity']->category_name);
    $out->writeElement('Id', $startTime->format('Y-m-d\TH:i:s\Z'));
    if (!empty($info['activity']->activity_notes)) $out->writeElement('Notes', $info['activity']->activity_notes);
    
    foreach ($laps as $lap_num => $lap) {
      $out->startElement('Lap');
      $out->writeAttribute('StartTime', $lap->lap_start);
      $out->writeElement('TotalTimeSeconds', $lap->lap_duration);
      $out->writeElement('DistanceMeters', $lap->lap_distance);
      //$out->writeElement('MaximumSpeed', $lap->lap_max_speed);
      $out->writeElement('Calories', $lap->lap_calories);
      
      $out->startElement('AverageHeartRateBpm');
      $out->writeAttribute('xsi:type', 'HeartRateInBeatsPerMinute_t');
      $out->writeElement('Value', $lap->lap_avg_heartrate);
      $out->endElement();
      
      $out->startElement('MaximumHeartRateBpm');
      $out->writeAttribute('xsi:type', 'HeartRateInBeatsPerMinute_t');
      $out->writeElement('Value', $lap->lap_max_heartrate);
      $out->endElement();
      
      $out->writeElement('Intensity', ucwords(strtolower($lap->lap_type)));
      
      $out->startElement('Track');
      foreach ($lap->points as $point) {
        $out->startElement('Trackpoint');
        $out->writeElement('Time', $point['time']);
        
        if (isset($point[ActivityDataTrackAccess::LOCATION])) {
          $out->startElement('Position');
          $out->writeElement('LatitudeDegrees', $point[ActivityDataTrackAccess::LOCATION][1]);
          $out->writeElement('LongitudeDegrees', $point[ActivityDataTrackAccess::LOCATION][2]);
          $out->endElement();
        }
        
        if (isset($point[ActivityDataTrackAccess::ELEVATION])) 
          $out->writeElement('AltitudeMeters', $point[ActivityDataTrackAccess::ELEVATION]);
        if (isset($point[ActivityDataTrackAccess::DISTANCE])) 
          $out->writeElement('DistanceMeters', $point[ActivityDataTrackAccess::DISTANCE]);
        
        if (isset($point[ActivityDataTrackAccess::HEARTRATE])) {
          $out->startElement('HeartRateBpm');
          $out->writeAttribute('xsi:type', 'HeartRateInBeatsPerMinute_t');
          $out->writeElement('Value', $point[ActivityDataTrackAccess::HEARTRATE]);
          $out->endElement();
        }
        $out->endElement();
      }
      $out->endElement();
      $out->endElement();
    }
    
    $out->endElement();
    $out->endElement();
    
    $out->endElement(); 
    $out->endDocument(); 
    $out->flush();
  }
  private static function exportGpx($activity) {
    $info = $activity->openfit_info;
    
    $filename = str_replace(' ', '_', $info['activity']->activity_start) . '.gpx';
    header("Content-Type: text/gpx/force-download");
    header("Content-Disposition: attachment; filename=$filename");
    
    $startTime = new DateTime($info['activity']->activity_start, new DateTimeZone('UTC'));
    
    $reader = new ActivityDataTrackReader($info['activity']->activity_id, 'full');
    $data = $reader->readTracks(array(ActivityDataTrackAccess::LOCATION, ActivityDataTrackAccess::ELEVATION));
    
    $lIter = new OpenFitTrackIterator($data[ActivityDataTrackAccess::LOCATION], $startTime);
    $eleIter = new OpenFitTrackIterator($data[ActivityDataTrackAccess::ELEVATION], $startTime);
    
    $waypoints = array();
    
    $lastElevation = null;
    while ($lIter->valid()) {
      $output = array();
      
      $output['pos'] = $lIter->current();
      $offset = $lIter->key();
      $time = clone $startTime;
      $time->add(new DateInterval('PT'.$offset.'S'));
      
      $lIter->next();
      
      if ($eleIter->valid()) {
        //This is messy
        $offset = $eleIter->key();
        $eleTime = clone $startTime;
        $eleTime->add(new DateInterval('PT'.$offset.'S'));
        if ($eleTime == $time) {
          $output['ele'] = $eleIter->current();
          $lastElevation = $eleIter->current();
          $eleIter->next();
        } else {
          if (isset($lastElevation)) {
            $output['ele'] = $lastElevation;
          }
        }
      }
      
      $output['time'] = $time->format("Y-m-d\TH:i:s\Z");
      $waypoints[] = $output;
    }
    
    $out = new XMLWriter();
    $out->openURI('php://output');
    
    $out->startDocument('1.0', 'UTF-8'); 
    $out->setIndent(true); 
    $out->setIndentString('  ');
    
    $out->startElement('gpx');
    $out->writeAttribute('xmlns', 'http://www.topografix.com/GPX/1/1');
    $out->writeAttribute('xmlns:xsd', 'http://www.w3.org/2001/XMLSchema');
    $out->writeAttribute('xmlns:xsi', 'http://www.w3.org/2001/XMLSchema-instance');
    $out->writeAttribute('xsi:schemaLocation', 'http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd');
    $out->writeAttribute('version', '1.1');
    $out->writeAttribute('creator', 'OpenFitApi');
    
    $out->startElement('trk'); 
    
    $out->writeElement("name", $info['activity']->title);
    if (!empty($info['activity']->activity_notes)) {
      $out->writeElement('cmt', $info['activity']->activity_notes);
    }
    $out->writeElement('number', 0);
    
    $out->writeElement('type', $info['activity']->category_name);
    
    $out->startElement('trkseg');
    
    foreach($waypoints as $point) {
      $out->startElement('trkpt');
      $out->writeAttribute('lat', $point['pos'][1]);
      $out->writeAttribute('lon', $point['pos'][2]);
      if (isset($point['ele'])) $out->writeElement('ele', $point['ele']);
      $out->writeElement('time', $point['time']);
      $out->endElement();
    
    }
    
    $out->endElement();
    $out->endElement(); 
    
    $out->endElement(); 
    $out->endDocument(); 
    $out->flush();
  }
  
  private static function loadActivity($nid) {
    module_load_include('module', 'node');
    module_load_include('module', 'user');
    module_load_include('inc', 'field', 'field.attach');
    
    $node = node_load($nid);
    if (!node_access("view", $node)) return null;
    
    $activities = OpenFitActivity::getActivities(null, $nid);
    
    if (isset($activities[$nid])) {
      $node->openfit_info['activity'] = $activities[$nid];
      $node->openfit_info['laps'] = OpenFitActivity::getActivityLaps($node->openfit_info['activity']->activity_id);
    }
    return $node;
    // require_once '../../modules/user/user.module';
    // require_once '../../sites/all/modules/openfit_api/openfit_api.Activity.inc';
//     
    // $bypass_access = user_access('bypass node access');
    // $deny_access = !user_access('access content');
    // $published_only_access = !user_access('view own unpublished content');
    // if ($deny_access && !$bypass_access) return null;
// 
    // $query = db_select('node', 'n');
    // $query->innerJoin(OpenFitActivity::TABLE_NODE_ACTIVITY_ASSOC, 'na', 'n.nid = na.nid');
    // $query->innerJoin(OpenFitActivity::TABLE_ACTIVITY, 'a', 'a.activity_id = na.activity_id');
    // $query->innerJoin(OpenFitActivity::TABLE_ACTIVITY_CATEGORY, 'c', 'a.activity_category_id = c.category_id');
    // $query
      // ->fields('n', array('nid', 'title', 'language', 'status'))
      // ->fields('a', array(
        // 'activity_id', 'activity_start', 'activity_timezone', 'activity_distance', 'activity_duration', 
        // 'activity_elevation_gain', 'activity_elevation_loss', 'activity_notes', 'activity_calories'
      // ))
      // ->fields('c', array('category_id', 'category_name', 'category_noun', 'category_image_url'))
      // ->addExpression('activity_distance/activity_duration','speed');
    // $query->condition('n.nid', $node_id);
    // if (!$bypass_access) {
      // global $user;
      // $me = $user->uid;
      // $query->condition(db_or()->condition('n.uid', $me)->condition('n.status',1));
      // if ($published_only_access) {
        // $query->condition('n.status',1);
      // }
    // }
    // $result = $query->execute();
    // if ($result) $result = $result->fetchAssoc();
    // if ($result) return (object)$result;
    // return null;
  }

  private static function getIntervalSeconds(DateInterval $interval) {
     // Day
    $total = $interval->format('%a');
    
    //hour
    $total = ($total * 24) + ($interval->h);
    
    //min
    $total = ($total * 60) + ($interval->i);
    
    //sec
    $total = ($total * 60) + ($interval->s);
    
    return $total;  
  }
}

?>