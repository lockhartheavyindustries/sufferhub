<?php

/**
 * Access check for services retrieve callback.
 *
 * @param $op
 *   Access operator type (ex: 'view')
 * @param $args
 *   Access check arguments. [0] is the node id.
 */
function openfit_api_resource_activity_retrieve_access($op, $args) {
  global $user;
  $nid = $args[0];
  $node = node_load($nid);
  return node_access($op, $node, $user);
}

/**
 * Return a specific activity node.
 *
 * @param $nid
 *   The node id.
 */
function openfit_api_resource_activity_retrieve($nid) {
  $node = node_load($nid);
  if (!node_access("view", $node)) return null;
  
  $activities = OpenFitActivity::getActivities(null, $nid);
  
  if (isset($activities[$nid])) {
    $node->openfit_info['activity'] = $activities[$nid];
    $node->openfit_info['laps'] = OpenFitActivity::getActivityLaps($node->openfit_info['activity']->activity_id);
  }
  
  $row = $node->openfit_info['activity'];
  
  $utc_timezone = new DateTimeZone('UTC');
  $activity_localtime = new DateTime($row->activity_start, $utc_timezone);
  $activity_timezone = $row->activity_timezone;
  $activity_iso_timezone = OpenFitMeasurementDateTime::getISOTimezone($activity_timezone);
  OpenFitMeasurementDateTime::offsetDate($activity_localtime, $activity_timezone);
  $start_time = $activity_localtime->format('Y-m-d') . 'T' . $activity_localtime->format('H:i:s') . $activity_iso_timezone;
  $uri = openfit_api_resource_activity_resource_uri(array('activity', $nid));
  $item = array(
    'start_time' => $start_time,
    'total_distance' => floatval($row->activity_distance),
    'duration' => floatval($row->activity_duration),
    'clock_duration' => floatval($row->activity_clock_duration),
    'type' => $row->category_name,
    'name' => $row->title,
    'calories' => floatval($row->activity_calories),
    'user_id' => intval($row->uid),
    'uri' => $uri,
  );
  if (isset($row->activity_notes) && strlen($row->activity_notes) > 0) $item['notes'] = $row->activity_notes;
  if (isset($row->activity_elevation_gain)) $item['elevation_gain'] = $row->activity_elevation_gain;
  if (isset($row->activity_elevation_loss)) $item['elevation_loss'] = $row->activity_elevation_loss;
  if (isset($row->speed)) $item['avg_speed'] = round($row->speed, 3);
  if (isset($row->activity_max_speed)) $item['max_speed'] = round($row->activity_max_speed, 3);
  if (isset($row->activity_avg_heartrate)) $item['avg_heartrate'] = $row->activity_avg_heartrate;
  if (isset($row->activity_max_heartrate)) $item['max_heartrate'] = $row->activity_max_heartrate;
  if (isset($row->activity_avg_cadence)) $item['avg_cadence'] = $row->activity_avg_cadence;
  if (isset($row->activity_max_cadence)) $item['max_cadence'] = $row->activity_max_cadence;
  if (isset($row->activity_avg_power)) $item['avg_power'] = $row->activity_avg_power;
  if (isset($row->activity_max_power)) $item['max_power'] = $row->activity_max_power;

  $timer_stops_count = strlen($row->activity_timer_stops) / 4;
  if ($timer_stops_count > 0) {
    $timer_stops = unpack('V' . $timer_stops_count, $row->activity_timer_stops);
    $count = count($timer_stops) + 1;
    $timer_stops_item = array();
    for($i = 1; $i < $count; $i += 2) {
      $from = clone $activity_localtime;
      $from->add(new DateInterval('PT' . $timer_stops[$i] . 'S'));
      $from = $from->format('Y-m-d') . 'T' . $from->format('H:i:s') . $activity_iso_timezone;
      $to = clone $activity_localtime;
      $to->add(new DateInterval('PT' . $timer_stops[$i + 1] . 'S'));
      $to = $to->format('Y-m-d') . 'T' . $to->format('H:i:s') . $activity_iso_timezone;
      $timer_stops_item[] = array($from, $to);
    }
    $item['timer_stops'] = $timer_stops_item;
  }
  
  $item['laps'] = array();
  foreach ($node->openfit_info['laps'] as $lap) {
    $lap_localtime = new DateTime($lap->lap_start, $utc_timezone);
    OpenFitMeasurementDateTime::offsetDate($lap_localtime, $activity_timezone);
    $lap_start_time = $lap_localtime->format('Y-m-d') . 'T' . $lap_localtime->format('H:i:s') . $activity_iso_timezone;
    
    $lap_item = array(
      'number' => intval($lap->lap_number),
      'type' => $lap->lap_type,
      'start_time' => $lap_start_time,
      'distance' => floatval($lap->lap_distance),
      'duration' => floatval($lap->lap_duration),
      'clock_duration' => floatval($lap->lap_clock_duration),
      'calories' => floatval($lap->lap_calories),
    );
    if (isset($lap->lap_elevation_gain)) $lap_item['elevation_gain'] = floatval($lap->lap_elevation_gain);
    if (isset($lap->lap_elevation_loss)) $lap_item['elevation_loss'] = floatval($lap->lap_elevation_loss);
    if (isset($lap->speed)) $lap_item['avg_speed'] = round($lap->speed, 3);
    if (isset($lap->activity_max_speed)) $lap_item['max_speed'] = round($lap->activity_max_speed, 3);
    if (isset($lap->activity_avg_heartrate)) $lap_item['avg_heartrate'] = floatval($lap->activity_avg_heartrate);
    if (isset($lap->activity_max_heartrate)) $lap_item['max_heartrate'] = floatval($lap->activity_max_heartrate);
    if (isset($lap->activity_avg_cadence)) $lap_item['avg_cadence'] = floatval($lap->activity_avg_cadence);
    if (isset($lap->activity_max_cadence)) $lap_item['max_cadence'] = floatval($lap->activity_max_cadence);
    if (isset($lap->activity_avg_power)) $lap_item['avg_power'] = floatval($lap->activity_avg_power);
    if (isset($lap->activity_max_power)) $lap_item['max_power'] = floatval($lap->activity_max_power);
    $item['laps'][] = $lap_item;
  }
  
  $reader = new ActivityDataTrackReader($node->openfit_info['activity']->activity_id, 'full');
  $data = $reader->readTracks();
  foreach($data as $type => $track) {
    $track_data = array();
    switch ($type) {
      case ActivityDataTrackAccess::LOCATION: {
        $time = $track['header']['offset'];
        $data = $track['data'];
        $count = count($data);
        for($i = 0; $i < $count; $i += 2) {
           $track_data[] = $data[$i] + $time;
           $track_data[] = $data[$i + 1] = array(round($data[$i + 1][1], 8), round($data[$i + 1][2], 8));
        }
        break;
      }
      default: {
        $time = $track['header']['offset'];
        $data = $track['data'];
        $count = count($data);
        for($i = 0; $i < $count; $i += 2) {
           $track_data[] = $data[$i] + $time;
           $track_data[] = round($data[$i + 1], 1);
        }
        break;
      }
    }
    if (count($track_data)) $item[$type] = $track_data;
  }
  return $item;
}

/**
 * Query to return the list of activities.
 *
 * @param $page
 *   Page number of results to return. Default to 0.
 * @param $page_size
 *   Maximum number of activities to return per query. Default to 25.
 * @param $no_earlier_than
 *   Limit returned activities to those on or after date. Default to null.
 * @param $no_later_than
 *   'Limit returned activities to those on or before date. Default to null.
 */
function openfit_api_resource_activity_index($page = 0, $page_size = 25, $no_earlier_than = null, $no_later_than = null) {
  $page_size = max($page_size, 1);
  $range_start = $page_size * $page;
  $from_date = openfit_api_resource_activity_parse_date($no_earlier_than);
  $to_date = openfit_api_resource_activity_parse_date($no_later_than);
  
  global $user;
  if ($user->uid != 0) $uid = $user->uid;
  
  $total_items = 0;
  $items = array();
  
  if (user_access('access content')) {
    // Total item count
    $query = 'SELECT COUNT(*) FROM {node} n';
    $query .= ' INNER JOIN {' . OpenFitActivity::TABLE_NODE_ACTIVITY_ASSOC . '} na ON n.nid = na.nid';
    if (isset($from_date) || isset($to_date)) $query .= ' INNER JOIN {' . OpenFitActivity::TABLE_ACTIVITY . '} a ON a.activity_id = na.activity_id';
    
    $args = array();
    if (isset($uid)) {
      $query .= ' WHERE n.uid = :uid';
      $args[':uid'] = $uid;
    } else {
      $query .= ' WHERE n.status = :status';
      $args[':status'] = 1;
    }
    if (isset($from_date)) {
      $query .= ' AND a.activity_start >= :from';
      $args[':from'] = $from_date;
    }
    if (isset($to_date)) {
      $query .= ' AND a.activity_start <= :to';
      $args[':to'] = $to_date;
    }
    $total_items = db_query($query, $args)->fetchField();

    // Items for selected page
    $query = db_select('node', 'n');
    $query->range($range_start, $page_size);
    $query->innerJoin(OpenFitActivity::TABLE_NODE_ACTIVITY_ASSOC, 'na', 'n.nid = na.nid');
    $query->innerJoin(OpenFitActivity::TABLE_ACTIVITY, 'a', 'a.activity_id = na.activity_id');
    $query->innerJoin(OpenFitActivity::TABLE_ACTIVITY_CATEGORY, 'c', 'a.activity_category_id = c.category_id');
    $query
      ->fields('n', array('nid', 'title', 'uid'))
      ->fields('a', array('activity_start', 'activity_timezone', 'activity_distance', 'activity_duration'))
      ->fields('c', array('category_id', 'category_name', 'category_noun', 'category_image_url'));
    $query->orderBy('activity_start', 'DESC');
    $query->orderBy('nid', 'DESC');
    if (isset($uid)) {
      $query->condition('n.uid', $uid);
    } else {
      $query->condition('n.status',1);
    }
    if (isset($from_date)) {
      $query->condition('a.activity_start', $from_date, '>=');
    }
    if (isset($to_date)) {
      $query->condition('a.activity_start', $to_date, '<=');
    }
    
    $results = $query->execute();
    while ($row = $results->fetchAssoc()) {
      $row = (object)$row;
      $nid = $row->nid;
      $activity_localtime = new DateTime($row->activity_start, new DateTimeZone('UTC'));
      $activity_timezone = $row->activity_timezone;
      OpenFitMeasurementDateTime::offsetDate($activity_localtime, $activity_timezone);
      $start_time = $activity_localtime->format('Y-m-d') . 'T' . $activity_localtime->format('H:i:s') . OpenFitMeasurementDateTime::getISOTimezone($activity_timezone);
      $uri = openfit_api_resource_activity_resource_uri(array('activity', $nid));
      $item = array(
        'start_time' => $start_time,
        'total_distance' => $row->activity_distance,
        'duration' => $row->activity_duration,
        'type' => $row->category_name,
        'name' => $row->title,
        'user_id' => $row->uid,
        'uri' => $uri,
      );
      $items[] = $item;
    }
  }
  $ret = array('size' => $total_items, 'items' => $items);
  if ($range_start > 0) {
    $ret['previous'] = openfit_api_resource_activity_get_index_page_url($page - 1, $page_size, $no_earlier_than, $no_later_than);
  }
  if ($range_start + $page_size < $total_items) {
    $ret['next'] = openfit_api_resource_activity_get_index_page_url($page + 1, $page_size, $no_earlier_than, $no_later_than);
  }
  return $ret;
}

/**
 * Access check for services create callback.
 */
function openfit_api_resource_activity_create_access() {
  return (node_access('create', OpenFitActivity::NODE_TYPE_ACTIVITY) || user_access('administer content types'));
}

function openfit_api_resource_activity_create($data) {
  $fields = array(
    'copy' => array(
      'category' => 'type',
      'name' => 'name',
      'notes' => 'notes',
    ),
    'ufloat' => array(
      'distance' => 'total_distance',
      'duration' => 'duration',
      'clock_duration' => 'clock_duration',
      'calories' => 'calories',
      'elevation_gain' => 'elevation_gain',
      'elevation_loss' => 'elevation_loss',
    ),
    'avgmax' => array(
      'heartrate' => 'heartrate',
      'cadence' => 'cadence',
      'power' => 'power',
    ),
    'lap_ufloat' => array(
      'distance' => 'distance',
      'duration' => 'duration',
      'clock_duration' => 'clock_duration',
      'calories' => 'calories',
      'elevation_gain' => 'elevation_gain',
      'elevation_loss' => 'elevation_loss',
    ),
    'lap_avgmax' => array(
      'heartrate' => 'heartrate',
      'cadence' => 'cadence',
      'power' => 'power',
    ),
  );
  $avgmax_subfields = array('avg','max');
  
  $activity = array();
  $activity_start_time = null;
  if (isset($data['start_time'])) {
    try {
      $activity_start_time = new DateTime($data['start_time']);
      $activity['timezone'] = $activity_start_time->getOffset() / 3600;
      $activity_start_time->setTimezone(new DateTimeZone('UTC'));
      $activity['start_time'] = $activity_start_time->format('Y-m-d H:i:s');
    } catch (Exception $e) {
      openfit_api_resource_process_error(406, 'Invalid activity start time: ' . $data['start_time']);
    }
  }
  if (!isset($activity_start_time)) openfit_api_resource_process_error(406, 'Missing activity start time');
  
  if (isset($data['max_speed'])) $activity['speed']['max'] = max(0,floatval($data['max_speed']));
  foreach ($fields['copy'] as $to => $from) {
    if (isset($data[$from])) $activity[$to] = $data[$from];
  }
  foreach ($fields['ufloat'] as $to => $from) {
    if (isset($data[$from])) $activity[$to] = max(0,floatval($data[$from]));
  }
  foreach ($fields['avgmax'] as $to => $from) {
    foreach ($avgmax_subfields as $subfield) {
      $from_field = $subfield . '_' . $from;
      if (isset($data[$from_field])) $activity[$to][$subfield] = max(0,floatval($data[$from_field]));
    }
  }
  $data_tracks = array();
  $data_track_types = ActivityDataTrackAccess::getAllDataTrackTypes();
  foreach ($data_track_types as $track_type)
  {
    $track_data = $data[$track_type];
    if (!isset($track_data) || !is_array($track_data)) continue;
    $count = count($track_data);
    if ($count < 2) continue;
    $data_tracks[$track_type] = array();
    for ($i = 0; $i < $count; $i += 2) {
      $data_tracks[$track_type][intval($track_data[$i])] = $track_data[$i+1];
    }
  }
  $activity['data_tracks'] = $data_tracks;
  
  if (isset($data['laps']) && is_array($data['laps'])) {
    $laps = array();
    foreach ($data['laps'] as $lap_data) {
      $lap = array();
      try {
        $lap['start_time'] = new DateTime($lap_data['start_time']);
        $lap['start_time']->setTimezone(new DateTimeZone('UTC'));
        $lap['start_time'] = $lap['start_time']->format('Y-m-d H:i:s');
      } catch (Exception $e) {
        openfit_api_resource_process_error(406, 'Invalid lap start time: ' . $lap_data['start_time']);
      }
      if (isset($lap_data['type'])) $lap['type'] = $lap_data['type'];
      if (isset($lap_data['max_speed'])) $lap['speed']['max'] = max(0,floatval($lap_data['max_speed']));
      foreach ($fields['lap_ufloat'] as $to => $from) {
        if (isset($lap_data[$from])) $lap[$to] = max(0,floatval($lap_data[$from]));
      }
      foreach ($fields['lap_avgmax'] as $to => $from) {
        foreach ($avgmax_subfields as $subfield) {
          $from_field = $subfield . '_' . $from;
          if (isset($lap_data[$from_field])) $lap[$to][$subfield] = max(0,floatval($lap_data[$from_field]));
        }
      }
      $laps[] = $lap;
    }
    $activity['laps'] = $laps;
  }

  if (isset($data['timer_stops']) && is_array($data['timer_stops'])) {
    $timer_stops = array();
    foreach ($data['timer_stops'] as $timer_stop_data) {
      $from = null;
      $to = null;
      try {
        $from = new DateTime($timer_stop_data[0]);
        $from->setTimezone(new DateTimeZone('UTC'));
      } catch (Exception $e) {
        openfit_api_resource_process_error(406, 'Invalid timer pause from time: ' . $timer_stop_data[0]);
      }
      try {
        $to = new DateTime($timer_stop_data[1]);
        $to->setTimezone(new DateTimeZone('UTC'));
      } catch (Exception $e) {
        openfit_api_resource_process_error(406, 'Invalid timer pause to time: ' . $timer_stop_data[1]);
      }
      if (isset($from) && isset($to)) $timer_stops[] = array($from, $to);      
    }
    $elapsed_timer_stops = array();
    foreach ($timer_stops as $timer_stop) {
      $from = $activity_start_time->diff($timer_stop[0]);
      $from = (($from->days * 24 + $from->h) * 60 + $from->i) * 60 + $from->s;
      $to = $activity_start_time->diff($timer_stop[1]);
      $to = (($to->days * 24 + $to->h) * 60 + $to->i) * 60 + $to->s;
      $elapsed_timer_stops[] = array('from' => $from, 'to' => $to);
    }
    $activity['timer_stops'] = $elapsed_timer_stops;
  }
  // TODO: Pool lengths
  
  $nodes = OpenFitActivity::createActivityNodes(array($activity));
  $uris = array();
  foreach ($nodes as $node) {
    OpenFitActivity::insertActivityRecords($node);
    $uris[] = openfit_api_resource_activity_resource_uri(array('activity', $node->nid));
  }
  return array('uris' => $uris);
}

function openfit_api_resource_process_error($code, $message) {
  $data = array('error' => array('code' => $code, 'message' => $message));
  services_error($message, $code, $data);
}

function openfit_api_resource_activity_parse_date($date) {
  if (!isset($date)) return null;
  $parts = explode('-', $date);
  if (count($parts) != 3) return null;
  $year = intval($parts[0]);
  $month = intval($parts[1]);
  $day = intval($parts[2]);
  if ($year < 0) return null;
  if ($month < 1 || $month > 12) return null;
  if ($day < 1 || $day > 31) return null;
  return str_pad(abs($year), 4, '0', STR_PAD_LEFT) . '-' . str_pad(abs($month), 2, '0', STR_PAD_LEFT) . '-' . str_pad(abs($day), 2, '0', STR_PAD_LEFT);
}

function openfit_api_resource_activity_get_index_page_url($page, $page_size, $no_earlier_than, $no_later_than) {
  $url_params = array('page' => $page);
  if ($page_size != 25) $url_params['pageSize'] = $page_size;
  if (isset($no_earlier_than)) $url_params['noEarlierThan'] = $no_earlier_than;
  if (isset($no_later_than)) $url_params['noLaterThan'] = $no_later_than;
  return openfit_api_resource_activity_resource_uri(array('activity'), $url_params);
}

function openfit_api_resource_activity_resource_uri($path, $params = null) {
  $request_type = '';
  $request = parse_url($_SERVER['REQUEST_URI'], PHP_URL_PATH);
  $dot_pos = strrpos($request,'.');
  if (!($dot_pos === FALSE)) $request_type = substr($request,$dot_pos);
  $uri = services_resource_uri($path) . $request_type;
  if (isset($params) && count($params) > 0) $uri.= '?' . http_build_query($params, '', '&');
  return $uri;
}