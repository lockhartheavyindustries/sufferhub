<?php 
class GetTrackDataHandler {
  
  const MAX_VIEW_TRACK_POINTS = 1000;
  const MAX_VIEW_ROUTE_POINTS = 1000;
  
  public static function getChartData() {
    $node_id = $_GET['nid'];

    $activity = self::loadActivity($node_id);

    module_load_include('inc', 'openfit_api', 'openfit_api.ActivityDataTrack');
    
    // TODO: Store a binary version of this data so it only gets calculated once.
    $reader = new ActivityDataTrackReader($activity['activity_id'], 'full');
    $data = $reader->readTracks();    
    $tracks = self::getViewDerivedTrackData($activity);
    $tracks += self::getViewTrackData($activity, $data);
    if (isset($tracks[ActivityDataTrackAccess::DISTANCE])) {
      $tracks[ActivityDataTrackAccess::DISTANCE]->hidden = true;
    }
    
    // TODO: Sort metric: Speed/Pace/Elevation/HeartRate/Cadence/Power/Temperature/Other

    header('Content-type: application/json');
    $json = (object)$tracks;
    
    die(print_r(json_encode($json), true));
  }

  public static function getRouteData() {
    $node_id = $_GET['nid'];
    
    $activity = self::loadActivity($node_id);

    module_load_include('inc', 'openfit_api', 'openfit_api.ActivityDataTrack');
    
    // TODO: Store a binary version of this data so it only gets calculated once.
    $reader = new ActivityDataTrackReader($activity['activity_id'], 'full');
    $data = $reader->readTracks(array(ActivityDataTrackAccess::LOCATION));
    $data = isset($data[ActivityDataTrackAccess::LOCATION]) ? $data[ActivityDataTrackAccess::LOCATION] : null;
    $route = self::getViewRouteData($activity, $data);

    header('Content-type: application/json');
    $json = (object)array(
      'nid' => (int)$node_id,
      'route' => (object)$route,
    );
    die(print_r(json_encode($json), true));
  }
  
  private static function getViewTrackData($activity, $data) {
    // Create a distance track if its missing
    if (!isset($data[ActivityDataTrackAccess::LOCATION])) {
      $distance_track = null;
      
      $laps = self::loadActivityLaps($activity);
      if (count($laps) > 0) {
        $time = 0;
        $distance = 0;
        $distance_track = array('data' => array());
        
        foreach ($laps as $lap) {
          $rest = $lap->lap_type != 'ACTIVE';
          if ($rest) continue;
          if (!$rest) {
            $time += $lap->lap_duration;
            $distance += $lap->lap_distance;
          }
          
          if (count($distance_track['data']) == 0) {
            $distance_track['data'][] = 0;
            $distance_track['data'][] = 0;
          }
          $distance_track['data'][] = $time;
          $distance_track['data'][] = $distance;
        }

        if ($time >0) {
          $distance_track['header'] = array('offset' => 0, 'total_time' => $time);
        } else {
          $distance_track = null;
        }
      } else if ($activity['activity_duration'] > 0 && $activity['activity_distance'] > 0) {
        $distance_track = array(
          'header' => array('offset' => 0, 'total_time' => $activity['activity_duration']),
          'data' => array(0, 0, $activity['activity_duration'], $activity['activity_distance'])
        );
      }
      if (isset($distance_track)) {
        $data[ActivityDataTrackAccess::DISTANCE] = $distance_track;
      }
    }
  
    // Determine the interval.
    $total_time = 0;
    $total_stopped_time = 0;
    foreach ($data as $track_data) {
      $total_time = max($total_time, $track_data['header']['offset'] + $track_data['header']['total_time']);
    }
    $timer_stops = $activity['activity_timer_stops'];
    foreach ($timer_stops as $timer_stop) {
      $total_stopped_time += max(0, min($total_time, $timer_stop[1]) - $timer_stop[0]);
    }
    $total_time -= $total_stopped_time;
    $interval = max(1, ceil($total_time / self::MAX_VIEW_TRACK_POINTS));
    
    $tracks = array();
    
    foreach ($data as $track_type => $track_data) {
      // Ignore the location track.
      if ($track_type == ActivityDataTrackAccess::LOCATION) continue;
      
      $points = $track_data['data'];
      // Ignore tracks which have less than two points.
      $count = count($points);
      if ($count < 4) continue;
      
      $timer_stops = $activity['activity_timer_stops'];
      $timer_stops_count = count($timer_stops);
      $next_timer_stop = 0;
      $total_stopped_time = 0;
      
      $pt = 2;
      $track_offset = $track_data['header']['offset'];
      $start = array($track_offset + $points[0], $points[1]);
      if ($track_type == ActivityDataTrackAccess::DISTANCE && $start[0] != 0) {
        $start = array(0,0);
        $pt = 0;
      }
      $precision = 1;
      if ($track_type == ActivityDataTrackAccess::DISTANCE) $precision = 3;
      
      while($next_timer_stop < $timer_stops_count && $timer_stops[$next_timer_stop][0] < $start[0]) {
        $total_stopped_time += max(0, $timer_stops[$next_timer_stop][1] - $timer_stops[$next_timer_stop][0]);
        $next_timer_stop++;
      }
      $start[0] = max(0, $start[0] - $total_stopped_time);
      
      $end = array($track_data['header']['offset'] + $track_data['header']['total_time'], $points[$count - 1]);
      $end[0] = min($end[0], $total_time);
      
      $data = array();
      $measurement = ActivityDataTrackAccess::getMeasurementInfo($track_type);
      $title = ActivityDataTrackAccess::getTitle($track_type);
      $color = ActivityDataTrackAccess::getColor($track_type);
      
      $next_sample_time = floor(($start[0] + $interval) / $interval) * $interval;
      
      $prior_time = $start[0];
      $prior_value = $start[1];
      $count -= 2;
      
      while ($pt < $count) {
        $time = $points[$pt] + $track_offset;
        $value = $points[$pt+1];
        
        while($next_timer_stop < $timer_stops_count && $timer_stops[$next_timer_stop][0] < $time) {
          $total_stopped_time += max(0, $timer_stops[$next_timer_stop][1] - $timer_stops[$next_timer_stop][0]);
          $next_timer_stop++;
        }
        $time = max(0, $time - $total_stopped_time);
        if ($time < $prior_time) {
          $pt += 2;
          continue;
        }
        
        if ($interval == -1 || $time == $next_sample_time) {
          $data[] = $value;
          $next_sample_time += $interval;
        } else if ($time > $next_sample_time && $time > $prior_time) {
          $slope = ($value - $prior_value) / ($time - $prior_time);
          while ($time > $next_sample_time && $time < $end[0]) {
            $data[] = round($prior_value + ($slope * ($next_sample_time - $prior_time)), $precision);
            $next_sample_time += $interval;
          }
        }
        $prior_time = $time;
        $prior_value = $value;
        $pt += 2;
      }
           
      $tracks[$track_type] = (object)array(
        'type' => $track_type,
        'interval' => $interval,
        'start' => $start,
        'end' => $end,
        'data' => $data,
        'measurement' => (object)$measurement,
        'title' => $title,
        'color' => $color,
      );

    }
    return $tracks;
  }
  
  private static function getViewDerivedTrackData($activity) {
    $speed = self::getViewDerivedTrackInfo(ActivityDataTrackAccess::SPEED);
    $speed['required'] = ActivityDataTrackAccess::DISTANCE;
    $speed['function'] = '_getSpeedTrack';
    
    $pace = self::getViewDerivedTrackInfo(ActivityDataTrackAccess::PACE);
    $pace['required'] = ActivityDataTrackAccess::DISTANCE;
    $pace['zoommaxavg'] = 3;
    $pace['function'] = '_getPaceTrack';
    
    return array(
      ActivityDataTrackAccess::SPEED => (object)$speed,
      ActivityDataTrackAccess::PACE => (object)$pace,
    );
  }
  
  private static function getViewDerivedTrackInfo($track_type) {
    return array(
      'type' => $track_type,
      'measurement' => (object)ActivityDataTrackAccess::getMeasurementInfo($track_type),
      'title' => ActivityDataTrackAccess::getTitle($track_type),
      'color' => ActivityDataTrackAccess::getColor($track_type),
    );
  }
    
  private static function getViewRouteData($activity, $track_data) {
    if (!isset($track_data)) return array();
    $track_type = ActivityDataTrackAccess::LOCATION;
    
    // Determine the interval.
    // TODO: Base this on distance instead of time and a Douglas-Peucker algorithm.
    // We will also need a elapsed time to distance track to sync route and chart possibly.
    $time = $track_data['header']['offset'] + $track_data['header']['total_time'];
    $interval = max(1, ceil($time / self::MAX_VIEW_ROUTE_POINTS));
    
    $points = $track_data['data'];
    // Ignore routes which have less than two points.
    $count = count($points);
    if ($count < 4) return array();

    $pt = 2;      
    $start = array($track_data['header']['offset'] + $points[0], $points[1][1], $points[1][2]);    
    $end = array($track_data['header']['offset'] + $track_data['header']['total_time'], $points[$count - 1][1], $points[$count - 1][2]);
    $data = array();
    $title = ActivityDataTrackAccess::getTitle($track_type);
    $color = ActivityDataTrackAccess::getColor($track_type);
    
    $next_sample_time = floor(($start[0] + $interval) / $interval) * $interval;
    
    $prior_time = $start[0];
    $prior_value = array(1 => $start[1], 2 => $start[2]);
    $count -= 2;
    
    //$interval = -1; // ALL points
    
    while ($pt < $count) {
      $time = $points[$pt];
      $value = $points[$pt+1];
      
      if ($interval == -1 || $time == $next_sample_time) {
        $data[] = $value[1];
        $data[] = $value[2];
        $next_sample_time += $interval;
      } else if ($time > $next_sample_time && $time > $prior_time) {
        $slope = array(
          ($value[1] - $prior_value[1]) / ($time - $prior_time),
          ($value[2] - $prior_value[2]) / ($time - $prior_time),
        );
        while ($time > $next_sample_time && $time < $end[0]) {
          // TODO: Figure out the number of decimals that Google Maps uses and round to reduce AJAX response size.
          // there is no need to return values with unnecessarily long precision.
          $data[] = $prior_value[1] + ($slope[0] * ($next_sample_time - $prior_time));
          $data[] = $prior_value[2] + ($slope[1] * ($next_sample_time - $prior_time));
          $next_sample_time += $interval;
        }
      }
      $prior_time = $time;
      $prior_value = $value;
      $pt += 2;
    }
         
    $route = array(
      'interval' => $interval,
      'start' => $start,
      'end' => $end,
      'data' => $data,
      'title' => $title,
      'color' => $color,
    );
    
    return $route;
  }
  
  private static function loadActivity($nid) {
    require_once DRUPAL_ROOT . '/includes/common.inc';
    module_load_include('module', 'user');

    $bypass_access = user_access('bypass node access');
    $deny_access = !user_access('access content');
    $published_only_access = !user_access('view own unpublished content');
    if ($deny_access && !$bypass_access) {
      throw new Exception(t('You are not authorized to access this page.'), 403);
    }

    $query = db_select('node', 'n');
    $query->innerJoin(OpenFitActivity::TABLE_NODE_ACTIVITY_ASSOC, 'na', 'n.nid = na.nid');
    $query->innerJoin(OpenFitActivity::TABLE_ACTIVITY, 'a', 'a.activity_id = na.activity_id');
    $query
      ->fields('n', array('nid'))
      ->fields('a', array(
        'activity_timer_stops',
        'activity_id',
        'activity_duration',
        'activity_distance',
      ));

    $query->condition('n.nid', $nid);
    if (!$bypass_access) {
      global $user;
      $me = $user->uid;
      $query->condition(db_or()->condition('n.uid', $me)->condition('n.status',1));
      if ($published_only_access) {
        $query->condition('n.status',1);
      }
    }
    $result = $query->execute();
    if ($result) $result = $result->fetchAssoc();
    if (!$result) {
      throw new Exception(t('You are not authorized to access this page.'), 403);
    }
    $timer_stops = array();
    $timer_stops_count = strlen($result['activity_timer_stops']) / 4;
    if ($timer_stops_count > 0) {
      $timer_stops_elapsed = unpack('V' . $timer_stops_count, $result['activity_timer_stops']);
      $count = count($timer_stops_elapsed) + 1;
      $timer_stops_item = array();
      for($i = 1; $i < $count; $i += 2) {
        $timer_stops[] = array($timer_stops_elapsed[$i], $timer_stops_elapsed[$i + 1]);
      }
    }
    $result['activity_timer_stops'] = $timer_stops;

    return $result;
  }
  
  private static function loadActivityLaps($activity) {
    require_once DRUPAL_ROOT . '/includes/common.inc';

    $query = db_select(OpenFitActivity::TABLE_ACTIVITY_LAP, 'l');
    $query->fields('l', array('lap_type','lap_distance','lap_duration'));

    $query->condition('l.activity_id', $activity['activity_id']);
    $query->orderBy('lap_start', 'ASC');
    $result = $query->execute();
    if (!$result) return array();
    return $result->fetchAll();
  }
}
?>
