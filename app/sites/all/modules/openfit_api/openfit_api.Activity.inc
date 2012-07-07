<?php 

class OpenFitActivity {

  const NODE_TYPE_ACTIVITY = 'activity';
  const TABLE_NODE_ACTIVITY_ASSOC = 'openfit_node_activity';
  const TABLE_ACTIVITY_CATEGORY = 'openfit_activity_category';
  const TABLE_ACTIVITY = 'openfit_activity';
  const TABLE_ACTIVITY_LAP = 'openfit_activity_lap';

  public static function getSchema(&$schema) {
    $schema[self::TABLE_NODE_ACTIVITY_ASSOC] = array(
      'description' => 'Linked table that maps openfit records to nodes/users in Drupal.',
      'fields' => array(
        'nid' => array(
          'description' => 'Node id for drupal site.',
          'type' => 'int',
          'size' => 'normal',
          'unsigned' => TRUE,
          'not null' => TRUE,
        ),
        'uid' => array(
          'description' => 'Owner of this record.',
          'type' => 'int',
          'size' => 'normal',
          'unsigned' => TRUE,
          'not null' => TRUE,
        ),
        'activity_id' => array(
          'description' => 'Record id in openfit API database.',
          'type' => 'int',
          'size' => 'normal',
          'unsigned' => TRUE,
          'not null' => TRUE,
        ),
      ),
      'primary key' => array('nid'),
      'unique keys' => array('nid' => array('nid')),
    );
    
    /*
    CREATE TABLE `openfit_session` (
      `session_id` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
      `activity_id` INT UNSIGNED NOT NULL ,
      `session_start` DATETIME NOT NULL COMMENT 'Session start date and time in UTC' ,
      `session_duration` DECIMAL(9,2) NOT NULL DEFAULT 0 COMMENT 'Session duration in seconds' ,
      `session_clock_duration` DECIMAL(9,2) NOT NULL DEFAULT 0 COMMENT 'Session clock duration in seconds' ,
      `session_distance` DOUBLE NOT NULL DEFAULT 0 COMMENT 'Session distance in meters' ,
      `session_calories` DECIMAL(9,1) NOT NULL DEFAULT 0 COMMENT 'Session total calories burned in kcal' ,
      `session_type` ENUM('running','cycling','swimming','other') NOT NULL DEFAULT 'other' COMMENT 'The standard session type' ,
      PRIMARY KEY (`session_id`) ,  UNIQUE INDEX `session_id_UNIQUE` (`session_id` ASC) );
      
    CREATE TABLE `openfit_txn` (
      `id` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
      `txn_user_id` INT UNSIGNED NOT NULL ,
      `txn_type` ENUM('autoadd') NOT NULL DEFAULT 'autoadd' COMMENT 'The transaction type' ,
      `txn_data_type` ENUM('activity') NOT NULL COMMENT 'The type of data the transaction refers to' ,
      `txn_data_id` INT UNSIGNED NOT NULL COMMENT 'The data id foreign key' ,
      PRIMARY KEY (`id`) ,
      UNIQUE INDEX `id_UNIQUE` (`id` ASC) );
    */
    
    $schema[self::TABLE_ACTIVITY_CATEGORY] = array(
      'description' => 'OpenFit activity category.',
      'fields' => array(
        'category_id' => array(
          'description' => 'Activity category id.',
          'type' => 'serial',
          'unsigned' => TRUE,
          'not null' => TRUE,
        ),
        'category_name' => array(
          'description' => 'Localizable category name (eg Running).',
          'type' => 'varchar',
          'not null' => TRUE,
          'length' => 60,
        ),        
        'category_noun' => array(
          'description' => 'Localizable category noun (eg run).',
          'type' => 'varchar',
          'not null' => TRUE,
          'length' => 60,
        ),        
        'category_image_url' => array(
          'description' => 'URL to the image for this category.',
          'type' => 'varchar',
          'not null' => TRUE,
          'length' => 256,
        ),
      ),
      'primary key' => array('category_id'),
      'unique keys' => array('category_id' => array('category_id')),
    );
    
    $schema[self::TABLE_ACTIVITY] = array(
      'description' => 'OpenFit activity.',
      'fields' => array(
        'activity_id' => array(
          'description' => 'Activity id',
          'type' => 'serial',
          'unsigned' => TRUE,
          'not null' => TRUE,
        ),
        'activity_start' => array(
          'description' => 'Activity start date and time in UTC.',
          'type' => 'datetime',
          'mysql_type' => 'DATETIME',
          'pgsql_type' => 'timestamp without time zone',
          'sqlite_type' => 'VARCHAR',
          'sqlsrv_type' => 'smalldatetime',
          'not null' => TRUE,
        ),
        'activity_timezone' => array(
          'description' => 'Activity local time UTC offset in hours.',
          'type' => 'numeric',
          'precision' => 3,
          'scale' => 1,
          'not null' => TRUE,
          'default' => 0,
        ),
        'activity_duration' => array(
          'description' => 'Activity duration in seconds.',
          'type' => 'numeric',
          'precision' => 9,
          'scale' => 2,
          'not null' => TRUE,
          'default' => 0,
        ),
        'activity_clock_duration' => array(
          'description' => 'Activity clock duration in seconds.',
          'type' => 'numeric',
          'precision' => 9,
          'scale' => 2,
          'not null' => TRUE,
          'default' => 0,
        ),
        'activity_distance' => array(
          'description' => 'Activity distance in meters.',
          'type' => 'float',
          'size' => 'big',
          'not null' => TRUE,
          'default' => 0,
        ),
        'activity_elevation_gain' => array(
          'description' => 'Activity elevation gain in meters.',
          'type' => 'float',
          'size' => 'big',
          'default' => null,
        ),
        'activity_elevation_loss' => array(
          'description' => 'Activity elevation loss in meters.',
          'type' => 'float',
          'size' => 'big',
          'default' => null,
        ),
        'activity_calories' => array(
          'description' => 'Activity total calories burned in kilojoules.',
          'type' => 'numeric',
          'precision' => 9,
          'scale' => 1,
          'not null' => TRUE,
          'default' => 0,
        ),
        'activity_notes' => array(
          'description' => 'Activity notes.',
          'type' => 'text',
          'not null' => TRUE,
        ),
        'activity_category_id' => array(
          'description' => 'The activity category.',
          'type' => 'int',
          'not null' => TRUE,
        ),
        'activity_timer_stops' => array(
          'description' => 'Array of timer stops (4 byte elapsed start, elapsed finish)',
          'type' => 'blob',
        ),
        'activity_max_speed' => array(
          'description' => 'Activity maximum speed in meters per second.',
          'type' => 'float',
          'default' => null,
        ),
        'activity_avg_heartrate' => array(
          'description' => 'Activity average heart rate.',
          'type' => 'float',
          'default' => null,
        ),
        'activity_max_heartrate' => array(
          'description' => 'Activity maximum heart rate.',
          'type' => 'float',
          'default' => null,
        ),
        'activity_avg_cadence' => array(
          'description' => 'Activity average cadence.',
          'type' => 'float',
          'default' => null,
        ),
        'activity_max_cadence' => array(
          'description' => 'Activity maximum cadence.',
          'type' => 'float',
          'default' => null,
        ),
        'activity_avg_power' => array(
          'description' => 'Activity average power.',
          'type' => 'float',
          'default' => null,
        ),
        'activity_max_power' => array(
          'description' => 'Activity maximum power.',
          'type' => 'float',
          'default' => null,
        ),
      ),
      'primary key' => array('activity_id'),
      'unique keys' => array('activity_id' => array('activity_id')),
    );
    
    $schema[self::TABLE_ACTIVITY_LAP] = array(
      'description' => 'OpenFit activity lap.',
      'fields' => array(
        'lap_id' => array(
          'description' => 'Lap id',
          'type' => 'serial',
          'unsigned' => TRUE,
          'not null' => TRUE,
        ),
        'activity_id' => array(
          'description' => 'The activity category.',
          'type' => 'int',
          'not null' => TRUE,
        ),
        'lap_start' => array(
          'description' => 'Lap start time as elapsed seconds from activity start.',
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
        ),
        'lap_duration' => array(
          'description' => 'Lap duration in seconds.',
          'type' => 'numeric',
          'precision' => 9,
          'scale' => 2,
          'not null' => TRUE,
          'default' => 0,
        ),
        'lap_clock_duration' => array(
          'description' => 'Lap clock duration in seconds.',
          'type' => 'numeric',
          'precision' => 9,
          'scale' => 2,
          'not null' => TRUE,
          'default' => 0,
        ),
        'lap_distance' => array(
          'description' => 'Lap distance in meters.',
          'type' => 'float',
          'size' => 'big',
          'not null' => TRUE,
          'default' => 0,
        ),
        'lap_elevation_gain' => array(
          'description' => 'Lap elevation gain in meters.',
          'type' => 'float',
          'size' => 'big',
          'default' => null,
        ),
        'lap_elevation_loss' => array(
          'description' => 'Lap elevation loss in meters.',
          'type' => 'float',
          'size' => 'big',
          'default' => null,
        ),
        'lap_calories' => array(
          'description' => 'Lap total calories burned in kilojoules.',
          'type' => 'numeric',
          'precision' => 9,
          'scale' => 1,
          'not null' => TRUE,
          'default' => 0,
        ),
        'lap_type' => array(
          'description' => 'Lap type (one of active, rest, warmup, cooldown).',
          'mysql_type' => 'ENUM(\'active\',\'rest\',\'warmup\',\'cooldown\')',
          'not null' => TRUE,
          'default' => 'active',
        ),
        'lap_max_speed' => array(
          'description' => 'Lap maximum speed in meters per second.',
          'type' => 'float',
          'default' => null,
        ),
        'lap_avg_heartrate' => array(
          'description' => 'Lap average heart rate.',
          'type' => 'float',
          'default' => null,
        ),
        'lap_max_heartrate' => array(
          'description' => 'Lap maximum heart rate.',
          'type' => 'float',
          'default' => null,
        ),
        'lap_avg_cadence' => array(
          'description' => 'Lap average cadence.',
          'type' => 'float',
          'default' => null,
        ),
        'lap_max_cadence' => array(
          'description' => 'Lap maximum cadence.',
          'type' => 'float',
          'default' => null,
        ),
        'lap_avg_power' => array(
          'description' => 'Lap average power.',
          'type' => 'float',
          'default' => null,
        ),
        'lap_max_power' => array(
          'description' => 'Lap maximum power.',
          'type' => 'float',
          'default' => null,
        ),
        'lap_pool_lengths' => array(
          'description' => 'Serialized PHP Array of pool length info.',
          'type' => 'blob',
        ),
      ),
      'primary key' => array('lap_id'),
      'unique keys' => array('lap_id' => array('lap_id')),
    );
  }

  /**
   * Get a list of the valid lap types.
   */
  public static function getAllLapTypes() {
    return array ('active' => 0, 'rest' => 1, 'warmup' => 2, 'cooldown' => 3);
  }
  
  /**
   * Get an associative array of localized category name, indexed by id.
   *
   * @param $uid
   *   The user ID to show categories for. Currently ignored.
   */
  public static function getActivityCategories($uid = null) {
    $items = array();
    $query = db_select('openfit_activity_category','c');
    $query->fields('c',array('category_id', 'category_name'));
    $result = $query->execute();
    if ($result) {
      while ($row = $result->fetchAssoc()) {
        $items[$row['category_id']] = $row['category_name'];
      }
      asort($items);
    }      
    if (count($items) == 0) {
      $items[0] = t('Other');
    }
    return $items;
  }
  
  /**
   * Return a new stdClass object initialized to the defaults.
   */
  public static function getNewActivity() {
    global $user;
    $values = array();
    $values['category_id'] = 2;

    $user_timezone = new DateTimeZone(OpenFitUserSetting::get($user->uid, OpenFitUserSetting::TYPE_TIMEZONE));
    $values['activity_timezone'] = $user_timezone->getOffset(new DateTime()) / 3600;
    $now = new DateTime('now', new DateTimeZone('UTC'));
    $values['activity_start'] = $now->format('Y-m-d H:i:s');
    $values['activity_distance'] = 0;
    $values['activity_duration'] = 0;
    $values['activity_clock_duration'] = 0;
    $values['activity_elevation_gain'] = null;
    $values['activity_elevation_loss'] = null;
    $values['activity_notes'] = '';
    $values['activity_calories'] = 0;
    $values['laps'] = array();
    $values['data_tracks'] = array();
    $values['timer_pauses'] = array();
    $values['activity_max_speed'] = null;
    $values['activity_avg_heartrate'] = null;
    $values['activity_max_heartrate'] = null;
    $values['activity_avg_cadence'] = null;
    $values['activity_max_cadence'] = null;
    $values['activity_avg_power'] = null;
    $values['activity_max_power'] = null;
    return (object)$values;
  }
  
  /**
   * Execute a query of activity nodes.
   *
   * This method recognizes the following access permissions from the node module:
   *   - bypass node access: If true, all activities from all users can be returned (no additional conditions are applied to query).
   *   - access content: If false no activities are visible (zero rows are returned).
   *   - view own unpublished content: If false, only public activities are visible (including the athlete's own).
   *
   * @param $uid
   *   The user id to view activities for, or null if all users.
   * @param $nid
   *   The single node id to view an activity for. It is meaningless to specify both $uid and $nid.
   * @param $sort_by_header
   *   If not null, use the header column information and URL params to sort the activities. Otherwise sort by date descending.
   * @param $page_size
   *   If not null, return $page_size number of rows and get the current page from the URL params. Otherwise return all activities.
   * @return An associative array of rows indexed by node id.
   */
  public static function getActivities($uid = null, $nid = null, $sort_by_header = null, $page_size = null) {
    $bypass_access = user_access('bypass node access');
    $deny_access = !user_access('access content');
    $published_only_access = !user_access('view own unpublished content');
    if ($deny_access && !$bypass_access) return null;
    
    // TODO: Optimize column and join selection based on parameter specifying columns
    $query = db_select('node', 'n');
    if (is_array($sort_by_header)) $query = $query->extend('TableSort');
    if (is_numeric($page_size)) {
      $query = $query->extend('PagerDefault');
      $query->limit($page_size);
    }
    $query->innerJoin(self::TABLE_NODE_ACTIVITY_ASSOC, 'na', 'n.nid = na.nid');
    $query->innerJoin(self::TABLE_ACTIVITY, 'a', 'a.activity_id = na.activity_id');
    $query->innerJoin(self::TABLE_ACTIVITY_CATEGORY, 'c', 'a.activity_category_id = c.category_id');
    $query
      ->fields('n', array('nid', 'title', 'language', 'status', 'uid'))
      ->fields('a', array(
        'activity_id', 'activity_start', 'activity_timezone', 'activity_distance', 'activity_duration', 'activity_clock_duration',
        'activity_elevation_gain', 'activity_elevation_loss', 'activity_notes', 'activity_calories',
        'activity_max_speed', 'activity_avg_heartrate', 'activity_max_heartrate',
        'activity_avg_cadence', 'activity_max_cadence', 'activity_avg_power', 'activity_max_power',
        'activity_timer_stops',
      ))
      ->fields('c', array('category_id', 'category_name', 'category_noun', 'category_image_url'))
      ->addExpression('activity_distance/activity_duration','speed');
    if (is_array($sort_by_header)) {
      $query->orderByHeader($sort_by_header);
    } else {
      $query->orderBy('activity_start', 'DESC');
    }
    if (isset($nid)) $query->condition('n.nid', $nid);
    if (isset($uid)) $query->condition('n.uid', $uid);
    if (!$bypass_access) {
      global $user;
      $me = $user->uid;
      if ($me != $uid) {
        $query->condition(db_or()->condition('n.uid', $me)->condition('n.status',1));
      }
      if ($published_only_access) {
        $query->condition('n.status',1);
      }
    }
    $results = $query->execute();
    $activities = array();
    if ($results) {
      while ($row = $results->fetchAssoc()) {
        $activities[$row['nid']] = (object)$row;
      }
    }
    return $activities;    
  }
  
  /**
   * Execute a query of activity laps for the specified activity id in lap number order.
   *
   * To avoid SQL JOINs no node-level access checks are performed.
   *
   * @param $activity_id
   *    The activity id.
   * @return An associative array of rows indexed by lap id.
   */
  public static function getActivityLaps($activity_id) {
    $query = db_select(self::TABLE_ACTIVITY_LAP,'l');
    $query->innerJoin(self::TABLE_ACTIVITY, 'a', 'a.activity_id = l.activity_id');
    $query
      ->fields('a', array('activity_start', 'activity_timezone'))
      ->fields('l', array(
        'lap_id', 'lap_start', 'lap_distance', 'lap_duration', 'lap_clock_duration', 'lap_elevation_gain', 'lap_elevation_loss', 'lap_calories', 'lap_type',
        'lap_max_speed', 'lap_avg_heartrate', 'lap_max_heartrate', 'lap_avg_cadence', 'lap_max_cadence', 'lap_avg_power', 'lap_max_power', 'lap_pool_lengths',
      ))
      ->addExpression('lap_distance/lap_duration','speed');
    $query->orderBy('lap_start', 'ASC');
    $query->condition('l.activity_id', $activity_id);
    $results = $query->execute();
    $laps = array();
    $utc = new DateTimeZone('UTC');
    if ($results) {
      $lap_number = 1;
      while ($row = $results->fetchAssoc()) {
        $row['lap_number'] = $lap_number;
        $start = new DateTime($row['activity_start'], $utc);
        $start->add(new DateInterval('PT' . $row['lap_start'] . 'S'));
        $row['lap_start'] = $start->format('Y-m-d H:i:s');
        if (isset($row['lap_pool_lengths'])) $row['lap_pool_lengths'] = unserialize($row['lap_pool_lengths']);
        $laps[$row['lap_id']] = (object)$row;
        $lap_number++;
      }
    }
    return $laps;
  }
  
  /**
   * Return the summarized totals and averages of activities occuring in the week.
   *
   * @param $uid
   *   The user id, or null for all users.
   * @param $date
   *   The date, or null for today's date.
   * @return An object with the following properties: distance, duration, calories, speed.
   */
  public static function getWeekSummaryInfo($uid = null, $date = null) {
    $week_start = OpenFitUserSetting::get($uid, OpenFitUserSetting::TYPE_WEEK_START, variable_get('date_first_day', 1));
    $this_week = OpenFitMeasurementDateTime::getFirstDayOfWeekDate($week_start, $date);
    $next_week = clone $this_week;
    $next_week->add(new DateInterval('P7D'));    
    return self::getSummaryInfo($uid, $this_week, $next_week);
  }

  public static function getWeekStatistics($num_prior_weeks = 5, $uid = null, $date = null) {
    $week_start = OpenFitUserSetting::get($uid, OpenFitUserSetting::TYPE_WEEK_START, variable_get('date_first_day', 1));
    if (!isset($date)) $date = new DateTime('now');
    $one_week_interval = new DateInterval('P7D');
    $from = OpenFitMeasurementDateTime::getFirstDayOfWeekDate($week_start, $date);
    $to = clone $from;
    $to->add($one_week_interval);
    if ($num_prior_weeks > 0) {
      $from->sub(new DateInterval('P' . $num_prior_weeks*7 . 'D'));
    } else if ($num_prior_weeks < 0) {
      $to->add(new DateInterval('P' . -$num_prior_weeks*7 . 'D'));
    }

    // TODO: Handle pace/speed average, other columns, etc - for now hardcode to distance
    $data_column = 'SUM(a.activity_distance)';

    $date_origin = OpenFitMeasurementDateTime::getFirstDayOfWeekDate($week_start, new DateTime('1900-01-01'));
    $activity_start_sql = 'a.activity_start + INTERVAL 60*a.activity_timezone MINUTE';
    
    $sql = 'SELECT ' .
      'FLOOR(DATEDIFF(' . $activity_start_sql . ',\'' . $date_origin->format('Y-m-d') . ' 00:00:00\')/7) AS week,' .
      $data_column . ' AS value FROM ' . self::TABLE_ACTIVITY . ' a ' .
      'INNER JOIN ' . self::TABLE_NODE_ACTIVITY_ASSOC . ' na ON a.activity_id=na.activity_id ';
    $where = 'WHERE ' . $activity_start_sql . ' >= :from AND ' . $activity_start_sql . ' < :to';
    $params = array(
      ':from' => $from->format('Y-m-d'),
      ':to' => $to->format('Y-m-d')
    );
    if (isset($uid)) {
      $where .= ' AND na.uid=:uid';
      $params[':uid'] = $uid;
    }
    $sql .= $where;
    $sql .= ' GROUP BY week ORDER BY week ';
    
    $rows = array();
    while ($from < $to) {
      $group = clone $from;
      $rows[$group->format('Y-m-d')] = array('group' => $group, 'value' => null);
      $from->add($one_week_interval);
    }
    
    $results = db_query($sql, $params);
    if ($results) {
      while ($row = $results->fetchAssoc()) {
        $weeks = $row['week'];
        $group = clone $date_origin;
        if ($weeks > 0) {
          $group->add(new DateInterval('P' . $weeks*7 . 'D'));
        } else if ($weeks < 0) {
          $group->sub(new DateInterval('P' . -$weeks*7 . 'D'));
        }
        $key = $group->format('Y-m-d');
        $rows[$key] = array('group' => $group, 'value' => $row['value']);
      }
    }    
    return $rows;
  }
  
  /**
   * Return the summarized totals and averages of activities occuring in the month.
   *
   * @param $uid
   *   The user id, or null for all users.
   * @param $date
   *   The date, or null for today's date.
   * @return An object with the following properties: distance, duration, calories, speed.
   */
  public static function getMonthSummaryInfo($uid = null, $date = null) {
    if (!isset($date)) $date = new DateTime('now');
    $this_month = new DateTime($date->format('Y-m') . '-01');
    $next_month = clone $this_month;
    $next_month->add(new DateInterval('P1M'));
    return self::getSummaryInfo($uid, $this_month, $next_month);
  }
  
  public static function getMonthStatistics($num_prior_months = 5, $uid = null, $date = null) {
    if (!isset($date)) $date = new DateTime('now');
    $one_month_interval = new DateInterval('P1M');
    $from = new DateTime($date->format('Y-m') . '-01');
    $to = clone $from;
    $to->add($one_month_interval);
    if ($num_prior_months > 0) {
      $from->sub(new DateInterval('P' . $num_prior_months . 'M'));
    } else if ($num_prior_months < 0) {
      $to->add(new DateInterval('P' . -$num_prior_months . 'M'));
    }

    // TODO: Handle pace/speed average, other columns, etc - for now hardcode to distance
    $data_column = 'SUM(a.activity_distance)';

    $activity_start_sql = 'a.activity_start + INTERVAL 60*a.activity_timezone MINUTE';
    $sql = 'SELECT ' .
      'MONTH(' . $activity_start_sql . ') AS month,' .
      'YEAR(' . $activity_start_sql . ') AS year,' .
      $data_column . ' AS value FROM ' . self::TABLE_ACTIVITY . ' a ' .
      'INNER JOIN ' . self::TABLE_NODE_ACTIVITY_ASSOC . ' na ON a.activity_id=na.activity_id ';
    $where = 'WHERE ' . $activity_start_sql . ' >= :from AND ' . $activity_start_sql . ' < :to';
    $params = array(
      ':from' => $from->format('Y-m-d'),
      ':to' => $to->format('Y-m-d')
    );
    if (isset($uid)) {
      $where .= ' AND na.uid=:uid';
      $params[':uid'] = $uid;
    }
    $sql .= $where;
    $sql .= ' GROUP BY year,month ORDER BY year,month ';
    
    $rows = array();
    while ($from < $to) {
      $group = clone $from;
      $rows[$group->format('Y-m-d')] = array('group' => $group, 'value' => null);
      $from->add($one_month_interval);
    }

    $results = db_query($sql, $params);
    if ($results) {
      while ($row = $results->fetchAssoc()) {
        $group = new DateTime($row['year'] . '-' . $row['month'] . '-01');
        $key = $group->format('Y-m-d');
        $rows[$key] = array('group' => $group, 'value' => $row['value']);
      }
    }      
    return $rows;
  }    
  
  /**
   * Return the summarized totals and averages of a group of activities between two dates.
   *
   * @param $uid
   *   The user id, or null for all users.
   * @param $from
   *   The starting DateTime (inclusive).
   * @param $to
   *   The ending DateTime (exclusive).
   * @return An object with the following properties: distance, duration, calories, speed.
   */
  public static function getSummaryInfo($uid, $from, $to) {
    $activity_start_sql = 'a.activity_start + INTERVAL 60*a.activity_timezone MINUTE';
    $sql = 'SELECT COUNT(*) as count, SUM(activity_distance) AS distance, SUM(activity_duration) AS duration, SUM(activity_calories) AS calories ' .
      'FROM ' . self::TABLE_ACTIVITY . ' a ' .
      'INNER JOIN ' . self::TABLE_NODE_ACTIVITY_ASSOC . ' na ON a.activity_id=na.activity_id ';
    if (isset($from) || isset($to) || isset($uid)) {
      $where = 'WHERE ';
      $params = array();
    }
    if (isset($from) || isset($to)) {
      if (isset($from)) {
        $where .= $activity_start_sql . ' >= :from ';
        $params[':from'] = $from->format('Y-m-d');
      }
      if (isset($to)) {
        if (isset($from)) $where .= 'AND ';
        $where .= $activity_start_sql . ' < :to ';
        $params[':to'] = $to->format('Y-m-d');
      }
    }

    if (isset($uid)) {
      if (isset($from) || isset($to)) $where .= 'AND ';
      $where .= 'na.uid=:uid';
      $params[':uid'] = $uid;
    }
    $sql .= $where;
    $results = db_query($sql, $params);
    $results = $results ? $results->fetchAssoc() : array();
    $results = array_filter($results) + array('distance' => 0, 'duration' => 0, 'calories' => 0);
    if ($results['duration'] > 0) $results['speed'] = $results['distance'] / $results['duration'];
    return (object)$results;
  }
    
  /**
   * Remove the specified activities.
   */
  public static function deleteActivities($nids) {
    $to_delete = array();
    $nodes = node_load_multiple($nids, array());
    foreach ($nodes as $node) if (node_access('delete', $node)) {
      $to_delete[] = $node->nid;
      $node_title = $node->title;
    }
    if (count($to_delete) == 0) return;
    node_delete_multiple($to_delete);
    if (count($to_delete) == 1) {
      $msg = t('Activity !title has been deleted.', array('!title' => '<em class="placeholder">' . $node_title . '</em>'));
    } else {
      $msg = t('Deleted !count activities.', array('!count' => count($to_delete)));
    }
    drupal_set_message($msg);
  }
  
  public static function createActivityNodes($activities) {
    global $user;
    
    $nodes = array();
    
    $user_timezone = new DateTimeZone(OpenFitUserSetting::get($user->uid, OpenFitUserSetting::TYPE_TIMEZONE));
    $categories = OpenFitActivity::getActivityCategories();
    foreach ($activities as &$activity_data) {
    
      self::normalizeActivityData($activity_data);
      
      $node = new stdClass();
      $node->type = OpenFitActivity::NODE_TYPE_ACTIVITY;
      $node->language = OpenFitUserSetting::get($user->uid, OpenFitUserSetting::TYPE_LANGUAGE);
      node_object_prepare($node);
      $node->status = OpenFitUserSetting::get($user->uid, OpenFitUserSetting::TYPE_DEFAULT_SHARING);

      // Find the activity category, default to "other"
      $category = isset($activity_data['category']) ? $activity_data['category'] : 'Other';
      $category_id = count($categories) - 1;
      foreach ($categories as $cat_id => $cat_name) {
        if ($cat_name == $category) {
          $category_id = $cat_id;
          break;
        }
      }
      $start_time = new DateTime(isset($activity_data['start_time']) ? $activity_data['start_time'] : 'now', new DateTimeZone('UTC'));
      $node->title = $category;
      if(isset($activity_data['name'])) $node->title = $activity_data['name'];
      $node->status = OpenFitUserSetting::get($user->uid, OpenFitUserSetting::TYPE_DEFAULT_SHARING);
      $node->openfit_info = array(
        'activity' => OpenFitActivity::getNewActivity(),
      );
      $node->openfit_info['activity']->category_id = $category_id;
      $node->openfit_info['activity']->activity_start = $start_time->format('Y-m-d H:i:s');
      if (isset($activity_data['timezone'])) {
        $node->openfit_info['activity']->activity_timezone = $activity_data['timezone'];
      } else {
        $node->openfit_info['activity']->activity_timezone = $user_timezone->getOffset($start_time) / 3600;
      }
      if (isset($activity_data['distance'])) $node->openfit_info['activity']->activity_distance = $activity_data['distance'];
      if (isset($activity_data['duration'])) $node->openfit_info['activity']->activity_duration = $activity_data['duration'];
      if (isset($activity_data['clock_duration'])) $node->openfit_info['activity']->activity_clock_duration = $activity_data['clock_duration'];
      if (isset($activity_data['elevation_gain'])) $node->openfit_info['activity']->activity_elevation_gain = $activity_data['elevation_gain'];
      if (isset($activity_data['elevation_loss'])) $node->openfit_info['activity']->activity_elevation_loss = $activity_data['elevation_loss'];
      if (isset($activity_data['calories'])) $node->openfit_info['activity']->activity_calories = $activity_data['calories'];
      $node->openfit_info['activity']->activity_notes = isset($activity_data['notes']) ? $activity_data['notes'] : '';
      if (isset($activity_data['speed'])) {
        if (isset($activity_data['speed']['max'])) $node->openfit_info['activity']->activity_max_speed = $activity_data['speed']['max'];
      }
      if (isset($activity_data['heartrate'])) {
        if (isset($activity_data['heartrate']['avg'])) $node->openfit_info['activity']->activity_avg_heartrate = $activity_data['heartrate']['avg'];
        if (isset($activity_data['heartrate']['max'])) $node->openfit_info['activity']->activity_max_heartrate = $activity_data['heartrate']['max'];
      }
      if (isset($activity_data['cadence'])) {
        if (isset($activity_data['cadence']['avg'])) $node->openfit_info['activity']->activity_avg_cadence = $activity_data['cadence']['avg'];
        if (isset($activity_data['cadence']['max'])) $node->openfit_info['activity']->activity_max_cadence = $activity_data['cadence']['max'];
      }
      if (isset($activity_data['power'])) {
        if (isset($activity_data['power']['avg'])) $node->openfit_info['activity']->activity_avg_power = $activity_data['power']['avg'];
        if (isset($activity_data['power']['max'])) $node->openfit_info['activity']->activity_max_power = $activity_data['power']['max'];
      }
      if (isset($activity_data['laps']) && is_array($activity_data['laps'])) {
        $valid_lap_types = self::getAllLapTypes();
        foreach ($activity_data['laps'] as $lap_data) {
          $lap_type = 'active';
          $lap_data_type = isset($lap_data['type']) ? strtolower($lap_data['type']) : $lap_type;
          if (isset($valid_lap_types[$lap_data_type])) $lap_type = $lap_data_type;
          $lap = (object)array(
            'lap_start' => isset($lap_data['start_time']) ? $lap_data['start_time'] : null,
            'lap_duration' => isset($lap_data['duration']) ? $lap_data['duration'] : 0,
            'lap_clock_duration' => isset($lap_data['clock_duration']) ? $lap_data['clock_duration'] : null,
            'lap_distance' => isset($lap_data['distance']) ? $lap_data['distance'] : 0,
            'lap_elevation_gain' => isset($lap_data['elevation_gain']) ? $lap_data['elevation_gain'] : 0,
            'lap_elevation_loss' => isset($lap_data['elevation_loss']) ? $lap_data['elevation_loss'] : 0,
            'lap_calories' => isset($lap_data['calories']) ? $lap_data['calories'] : 0,
            'lap_type' => $lap_type,
            'lap_max_speed' => isset($lap_data['speed']) && isset($lap_data['speed']['max']) ? $lap_data['speed']['max'] : null,
            'lap_avg_heartrate' => isset($lap_data['heartrate']) && isset($lap_data['heartrate']['avg']) ? $lap_data['heartrate']['avg'] : null,
            'lap_max_heartrate' => isset($lap_data['heartrate']) && isset($lap_data['heartrate']['max']) ? $lap_data['heartrate']['max'] : null,
            'lap_avg_cadence' => isset($lap_data['cadence']) && isset($lap_data['cadence']['avg']) ? $lap_data['cadence']['avg'] : null,
            'lap_max_cadence' => isset($lap_data['cadence']) && isset($lap_data['cadence']['max']) ? $lap_data['cadence']['max'] : null,
            'lap_avg_power' => isset($lap_data['power']) && isset($lap_data['power']['avg']) ? $lap_data['power']['avg'] : null,
            'lap_max_power' => isset($lap_data['power']) && isset($lap_data['power']['max']) ? $lap_data['power']['max'] : null,
            'lap_pool_lengths' => isset($lap_data['pool_lengths']) ? $lap_data['pool_lengths'] : null,
          );
          if (!isset($lap->lap_clock_duration)) $lap->lap_clock_duration = $lap->lap_duration;
          if (isset($lap->lap_start) && $lap->lap_duration > 0) {
            $node->openfit_info['activity']->laps[] = $lap;
          }
        }
      }
      
      if (isset($activity_data['data_tracks']) && is_array($activity_data['data_tracks'])) {
        $max_gap = 255;
        foreach ($activity_data['data_tracks'] as $track_type => $data_track) {
          $track_header = array('offset' => 0);
          $track_data = array();
          $prior_elapsed = 0;
          $prior_value = null;
          $saw_numeric = false;
          foreach ($data_track as $key => $value) {
            if ($saw_numeric || is_numeric($key)) {
              $saw_numeric = true;
              $offset = $key - $prior_elapsed;
              if ($key != 0 && $offset <= 0) continue;
              if ($offset > $max_gap) {
                if (!isset($prior_value)) {
                  $before = $track_header['offset'];
                  $track_header['offset'] += $offset;
                  $offset = 0;
                } else {
                  if (is_array($value)) {
                    $slope = array();
                    $count_values = count($value);
                    for($i = 0; $i < $count_values; $i++) $slope[$i] = ($value[$i] - $prior_value[$i]) / $offset;
                  } else {
                    $slope = ($value - $prior_value) / $offset;
                  }
                  $next_pt = $max_gap;
                  while ($offset > $max_gap) {
                    $track_data[] = $max_gap;
                    if (is_array($slope)) {
                      $mid_value = array();
                      for($i = 0; $i < $count_values; $i++) $mid_value[$i] = $prior_value[$i] + ($slope[$i] * $next_pt);
                    } else {
                      $mid_value = $prior_value + ($slope * $next_pt);
                    }
                    $track_data[] = $mid_value;
                    $offset -= $max_gap;
                    $next_pt += $max_gap;
                  }
                }
              }
              $track_data[] = $offset;
              $track_data[] = $value;
              $prior_elapsed = $key;
              $prior_value = $value;
            } else {
              switch ($key) {
                case 'start_time':
                  $track_start = new DateTime($value, new DateTimeZone('UTC'));
                  $track_offset = ($track_start->format('U') - $start_time->format('U'));
                  $track_header['offset'] += $track_offset;
                  break;
              }
            }
          }
          if (count($track_data) > 0) {
            $node->openfit_info['activity']->data_tracks[$track_type] = array(
              'header' => $track_header,
              'data' => $track_data,
            );
          }
        }
      }
      unset($activity_data['data_tracks']);
      
      $node->openfit_info['activity']->timer_stops = 
        isset($activity_data['timer_stops']) && is_array($activity_data['timer_stops']) ? $activity_data['timer_stops'] : array();
        
      // Prevent pathauto processing, we will set alias ourselves.
      $node->path['pathauto'] = 0;
      $node->path['alias'] = '';
      $node = node_submit($node);     
      node_save($node);
      // Code copied from pathauto module. We don't allow pathauto to process alias
      // because it doesn't work with language-specific nodes. Instead create an
      // alias explicitly with the undefined language.
      // @todo Remove the next line when http://drupal.org/node/1025870 is fixed.
      unset($node->uri);
      $node->path['pathauto'] = 1;
      pathauto_node_update_alias($node, 'insert', array('language' => LANGUAGE_NONE));

      $nodes[] = $node;
    }
      
    return $nodes;
  }
  
  public static function insertActivityRecords($node) {
    if ($node->type != OpenFitActivity::NODE_TYPE_ACTIVITY) return;
    $activity = $node->openfit_info['activity'];

    $utc = new DateTimeZone('UTC');
    $activity_start_time = new DateTime($activity->activity_start, $utc);
    
    // Convert timer stops from array to binary array
    if (!isset($activity->timer_stops)) $activity->timer_stops = array();
    if (is_array($activity->timer_stops)) {
      $timer_stops = '';
      foreach ($activity->timer_stops as $stop) {
        $timer_stops .= pack('V', $stop['from']) . pack('V', $stop['to']);
      }
      $activity->timer_stops = $timer_stops;
    }
    $activity->activity_clock_duration = max($activity->activity_duration, $activity->activity_clock_duration);
    // Insert the activity record.
    $activity_id = db_insert('openfit_activity')
      ->fields(array(
        'activity_start' => $activity->activity_start,
        'activity_timezone' => $activity->activity_timezone,
        'activity_duration' => $activity->activity_duration,
        'activity_clock_duration' => $activity->activity_clock_duration,
        'activity_distance' => $activity->activity_distance,
        'activity_elevation_gain' => $activity->activity_elevation_gain,
        'activity_elevation_loss' => $activity->activity_elevation_loss,
        'activity_calories' =>  $activity->activity_calories,
        'activity_notes' => $activity->activity_notes,
        'activity_category_id' => $activity->category_id,
        'activity_timer_stops' => $activity->timer_stops,
        'activity_max_speed' => $activity->activity_max_speed,
        'activity_avg_heartrate' => $activity->activity_avg_heartrate,
        'activity_max_heartrate' => $activity->activity_max_heartrate,
        'activity_avg_cadence' => $activity->activity_avg_cadence,
        'activity_max_cadence' => $activity->activity_max_cadence,
        'activity_avg_power' => $activity->activity_avg_power,
        'activity_max_power' => $activity->activity_max_power,
      ))
      ->execute();
      
    // Insert the laps records.
    if (isset($activity->laps) && is_array($activity->laps)) {
      foreach ($activity->laps as $lap) {
        $lap_start = new DateTime($lap->lap_start, $utc);
        $lap_start = $activity_start_time->diff($lap_start);
        $lap_start = (($lap_start->days * 24 + $lap_start->h) * 60 + $lap_start->i) * 60 + $lap_start->s;
        $lap->lap_clock_duration = max($lap->lap_clock_duration, $lap->lap_clock_duration);
        if ($lap_start >= 0) {
          db_insert('openfit_activity_lap')
            ->fields(array(
              'activity_id' => $activity_id,
              'lap_start' => $lap_start,
              'lap_duration' => $lap->lap_duration,
              'lap_clock_duration' => $lap->lap_clock_duration,
              'lap_distance' => $lap->lap_distance,
              'lap_elevation_gain' => $lap->lap_elevation_gain,
              'lap_elevation_loss' => $lap->lap_elevation_loss,
              'lap_calories' =>  $lap->lap_calories,
              'lap_type' => $lap->lap_type,
              'lap_max_speed' => $lap->lap_max_speed,
              'lap_avg_heartrate' => $lap->lap_avg_heartrate,
              'lap_max_heartrate' => $lap->lap_max_heartrate,
              'lap_avg_cadence' => $lap->lap_avg_cadence,
              'lap_max_cadence' => $lap->lap_max_cadence,
              'lap_avg_power' => $lap->lap_avg_power,
              'lap_max_power' => $lap->lap_max_power,
              'lap_pool_lengths' => serialize($lap->lap_pool_lengths),
            ))
            ->execute();
        }
      }
    }

    // Save the track data
    if (isset($activity->data_tracks) && is_array($activity->data_tracks)) {
      $writer = new ActivityDataTrackWriter($activity_id, 'full');
      $writer->writeTracks($activity->data_tracks);
    }
    
    // Insert the node/activity association record.
    db_insert('openfit_node_activity')
      ->fields(array(
        'nid' => $node->nid,
        'uid' => $node->uid,
        'activity_id' => $activity_id,
      ))
      ->execute();
    unset($node->openfit_info['activity']);
  }
  
  private static function normalizeActivityData(&$activity) {
    if (!isset($activity['data_tracks'])) $activity['data_tracks'] = array();
    if (!isset($activity['timer_stops'])) $activity['timer_stops'] = array();
    foreach ($activity['data_tracks'] as $track_name => $data_track) {
      if (!isset($data_track['data'])) continue;
      $new_data = array();
      if (isset($data_track['start_time'])) $new_data['start_time'] = $data_track['start_time'];
      $data = $data_track['data'];      
      $data_size = ActivityDataTrackAccess::getDataSize($track_name);
      $data_format = ActivityDataTrackAccess::getDataFormat($track_name);
      $data_len = strlen($data);
      $pos = 0;
      while ($pos < $data_len) {
        $time = unpack('L',substr($data,$pos));
        $time = $time[1];
        $value = unpack($data_format,substr($data,$pos+4));
        if (count($value) == 1) {
          $value = $value[1];
        } else {
          $value_array = array();
          foreach ($value as $v) $value_array[] = $v;
          $value = $value_array;
        }
        $new_data[$time] = $value;
        $pos += 4 + $data_size;
      }
      $activity['data_tracks'][$track_name] = $new_data;
    }
            
    // If the distance track is missing, calculate it from the GPS track or lap data.
    if (!isset($activity['data_tracks'][ActivityDataTrackAccess::DISTANCE]) || 
      count($activity['data_tracks'][ActivityDataTrackAccess::DISTANCE]) == 0) {
      self::createDistanceTrackFromGps($activity);
    }
    
    $total_fields = array('distance', 'duration', 'elevation_gain', 'elevation_loss', 'calories');
    $avgmax_fields = array('speed','heartrate','cadence','power');
    $totals = array();
    $avgmax = array();
    foreach ($total_fields as $field) $totals[$field] = 0; 
    foreach ($avgmax_fields as $field) $avgmax[$field] = array('avg' => 0, 'time' => 0, 'max' => null);
    
    // Prefill from laps first, we use everything except max values which may come from data tracks if specified.
    if (isset($activity['laps']) && is_array($activity['laps'])) {          
      foreach ($activity['laps'] as &$lap) {
        if ((isset($lap['type']) && $lap['type'] != 'active') || !isset($lap['duration']) || $lap['duration'] == 0) continue;
        if (isset($lap['distance']) && isset($lap['duration']) && $lap['duration'] > 0) {
          $speed = $lap['distance'] / $lap['duration'];
          if (!isset($lap['speed'])) $lap['speed'] = array();
          $lap['speed']['avg'] = $speed;
        }
        foreach ($total_fields as $field) {
          if (isset($lap[$field])) $totals[$field] += $lap[$field];
        }
        foreach ($avgmax_fields as $field) {
          if (!isset($lap[$field])) continue;
          if (isset($lap[$field]['avg'])) {
            $avgmax[$field]['time'] += $lap['duration'];
            $avgmax[$field]['avg'] += $lap[$field]['avg'] * $lap['duration'];
            if (!isset($lap[$field]['max']) || $lap[$field]['max'] < $lap[$field]['avg']) $lap[$field]['max'] = $lap[$field]['avg'];
          }
          if (isset($lap[$field]['max']) && (!isset($avgmax[$field]['max']) || $avgmax[$field]['max'] < $lap[$field]['max'])) {
            $avgmax[$field]['max'] = $lap[$field]['max'];
          }
        }
      }
    }      
    
    // Total distance is the last value from the distance track.
    if ($totals['distance'] == 0) {
      if (isset($activity['data_tracks'][ActivityDataTrackAccess::DISTANCE])) {
        $totals['distance'] = end($activity['data_tracks'][ActivityDataTrackAccess::DISTANCE]);
        reset($activity['data_tracks'][ActivityDataTrackAccess::DISTANCE]);
      }
    }
    
    // Total time is the longest track time (last key in array) minus timer stops.
    if ($totals['duration'] == 0) {
      $total_track_time = 0;
      foreach ($activity['data_tracks'] as $track_name => $data_track) {
        if (count($data_track) < 2) continue;
        end($data_track);
        $track_time = key($data_track);
        reset($data_track);
        if ($track_time > $totals['duration']) $totals['duration'] = $track_time;
      }
      foreach($activity['timer_stops'] as $timer_stop) {
        $from = max(0,min($timer_stop['from'],$totals['duration']));
        $to = max(0,min($timer_stop['to'],$totals['duration']));
        $totals['duration'] -= max(0,($to-$from));
      }
      $totals['duration'] = max(0, $totals['duration']);
    }
    
    // Elevation gain/loss is more sophisticated because of GPS accuracy - need to consider smoothing
    // and is slightly more expensive to compute. Only do this if the info is not specified in the laps or activity totals.
    $need_elevation = 
      ($totals['elevation_gain'] == 0 && (!isset($activity['elevation_gain']) || $activity['elevation_gain'] == 0)) ||
      ($totals['elevation_loss'] == 0 && (!isset($activity['elevation_loss']) || $activity['elevation_loss'] == 0));
    if ($need_elevation) {
      if (isset($activity['data_tracks'][ActivityDataTrackAccess::ELEVATION])) {
        $elevation_track = $activity['data_tracks'][ActivityDataTrackAccess::ELEVATION];
        // Cheap and relatively fast algorithm which looks a segments longer than 20 seconds
        // where the elevation gain was more than 1 meter.
        // This could be a lot better but would require more calculations; possible issues with user scaling.
        // For now, keep it simple.
        $min_sustained_time = 20;
        $prior_time = null;
        $prior_value = null;
        $saw_numeric = false;
        foreach ($elevation_track as $time => $value) {
          // Skip start_time.
          if (!$saw_numeric && !is_numeric($time)) continue;
          $saw_numeric = true;
          if (isset($prior_value)) {
            $time_diff = $time - $prior_time;
            if ($time_diff < $min_sustained_time) continue;
            $elevation_change = $value - $prior_value;
            $changed = false;
            if ($elevation_change > 1) {
              $totals['elevation_gain'] += $elevation_change;
              $changed = true;
            } else if ($elevation_change < 1) {
              $totals['elevation_loss'] += -$elevation_change;
              $changed = true;
            }
            if ($changed) {
              $prior_time = $time;
              $prior_value = $value;
            }
          } else {
            $prior_time = $time;
            $prior_value = $value;
          }
        }
      }
    }
   
    // Calculate max speed from the distance track and override lap speed if higher. Only do this if not specified on activity.
    $need_max_speed = !isset($activity['speed']) || !isset($activity['speed']['max']);
    if ($need_max_speed && isset($activity['data_tracks'][ActivityDataTrackAccess::DISTANCE])) {
      $distance_track = $activity['data_tracks'][ActivityDataTrackAccess::DISTANCE];
      $min_sustained_time = 10; // Time in seconds that speed must be mainained to "count"
      $prior_time = null;
      $prior_value = null;
      $saw_numeric = false;
      foreach ($distance_track as $time => $value) {
        // Skip start_time.
        if (!$saw_numeric && !is_numeric($time)) continue;
        $saw_numeric = true;
        if (isset($prior_value)) {
          $time_diff = $time - $prior_time;
          if ($time_diff < $min_sustained_time) continue;
          $speed = ($value - $prior_value) / $time_diff;
          if (!isset($avgmax['speed']['max']) || $speed > $avgmax['speed']['max']) {
            $avgmax['speed']['max'] = $speed;
          }
          $prior_time = $time;
          $prior_value = $value;
        } else {
          $prior_time = $time;
          $prior_value = $value;
        }
      }
    }
    
    // Calculate needed avg/max statistics from detail tracks if needed
    $track_fields = array('heartrate','cadence','power');
    foreach ($track_fields as $field) {
      if (!isset($activity['data_tracks'][$field]) || 
        (isset($activity[$field]) && isset($activity[$field]['avg']) && isset($activity[$field]['max']))) continue;
      $has_avg = $avgmax[$field]['time'] > 0;
      $avg_total = 0;
      $time_total = 0;
      $prior_time = null;
      $prior_value = null;
      $saw_numeric = false;
      foreach ($activity['data_tracks'][$field] as $time => $value) {
        // Skip start_time.
        if (!$saw_numeric && !is_numeric($time)) continue;
        $saw_numeric = true;
        if (isset($prior_value)) {
          $time_diff = $time - $prior_time;
          $value = round($value,1);
          $speed = ($value - $prior_value) / $time_diff;
          if (!isset($avgmax[$field]['max']) || $value > $avgmax[$field]['max']) {
            $avgmax[$field]['max'] = $value;
          }
          if (!$has_avg) $avg_total += ($prior_value + $value) * $time_diff;
          $time_total += $time_diff;
        }
        $prior_time = $time;
        $prior_value = $value;
      }
      if (!$has_avg && $time_total > 0) {
        $avgmax[$field]['time'] = $time_total;
        $avgmax[$field]['avg'] = $avg_total / 2;
      }
    }
       
    // Incorporate calculated statistics into the activity where needed
    foreach ($total_fields as $field) {
      if (!isset($activity[$field]) || $activity[$field] == 0) $activity[$field] = $totals[$field];
    }
    foreach ($avgmax_fields as $field) {
      if ((!isset($activity[$field]) || !isset($activity[$field]['avg'])) && $avgmax[$field]['time'] > 0) {
        if (!isset($activity[$field])) $activity[$field] = array();
        $activity[$field]['avg'] = round($avgmax[$field]['avg'] / $avgmax[$field]['time'], 1);
      }
      if ((!isset($activity[$field]) || !isset($activity[$field]['max'])) && isset($avgmax[$field]['max'])) {
        if (!isset($activity[$field])) $activity[$field] = array();
        $activity[$field]['max'] = $avgmax[$field]['max'];
      }
    }
    
    // TODO: Calculate calorie expenditure based on sport, time, distance and athlete weight
    
    // Clear average speed since it's always calculated
    unset($activity['speed']['avg']);                
  }
  
  private static function createDistanceTrackFromGps(&$activity) {
    if (!isset($activity['data_tracks'][ActivityDataTrackAccess::LOCATION])) return;
    $distance = 0;
    $location_track = $activity['data_tracks'][ActivityDataTrackAccess::LOCATION];
    $distance_track = array();
    if (isset($location_track['start_time'])) $distance_track['start_time'] = $location_track['start_time'];
    $prior_time = null;
    $prior_value = null;
    $saw_numeric = false;
    foreach ($location_track as $time => $value) {
      // Skip start_time.
      if (!$saw_numeric && !is_numeric($time)) continue;
      $saw_numeric = true;
      if (isset($prior_value)) {
        $delta_latitude = deg2rad($value[0] - $prior_value[0]);
        $delta_longitude = deg2rad($value[1] - $prior_value[1]);
        $a = sin($delta_latitude / 2) * sin($delta_latitude / 2) + 
          cos(deg2rad($prior_value[0])) * cos(deg2rad($value[0])) *
          sin($delta_longitude / 2) * sin($delta_longitude / 2);
        $c = 2 * atan2(sqrt($a), sqrt(1-$a));
        $segment_distance = self::EARTH_MEAN_RADIUS * $c;
        $distance += $segment_distance;
        $distance_track[$time] = $distance;
      } else {
        $distance_track[$time] = 0;
      }
      $prior_time = $time;
      $prior_value = $value;
    }
    $activity['data_tracks'][ActivityDataTrackAccess::DISTANCE] = $distance_track;
  }
  
  public static function getTableFieldInfo() {
    global $user;
    return array(
      'nid' => array(
        'primary_key' => TRUE,
      ),
      'name' => array(
        'title' => t('Name'),
        'data_field' => 'title',
        'format_class' => 'OpenFitActivity',
        'format_function' => 'formatName',
      ),
      'start' => array(
        'title' => t('Start'),
        'data_field' => 'activity_start',
        'timezone_field' => 'activity_timezone',
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatDatetime',
      ),
      'start-date' => array(
        'title' => t('Start date'),
        'data_field' => 'activity_start',
        'timezone_field' => 'activity_timezone',
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatDatetime',
        'time_format' => IntlDateFormatter::NONE,
      ),
      'start-time' => array(
        'title' => t('Start time'),
        'data_field' => 'activity_start',
        'timezone_field' => 'activity_timezone',
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatDatetime',
        'date_format' => IntlDateFormatter::NONE,
      ),
      'sport' => array(
        'title' => t('Sport'),
        'data_field' => 'category_name',
        // TODO: Need SQL localization to support sorting properly
        //'localize' => true,
      ),
      'sport-noun' => array(
        'title' => t('Sport'),
        'data_field' => 'category_noun',
        // TODO: Need SQL localization to support sorting properly
        //'localize' => true,
      ),
      'distance' => array(
        'title' => t('Distance'),
        'data_field' => 'activity_distance',
        'data_type' => OpenFitDataField::DATA_TYPE_DISTANCE,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatDistance',
      ),
      'time' => array(
        'title' => t('Time'),
        'data_field' => 'activity_duration',
        'data_type' => OpenFitDataField::DATA_TYPE_TIME,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatTimespan',
      ),
      'avg-speed' => array(
        'title' => t('Avg speed'),
        'data_field' => 'speed',
        'data_type' => OpenFitDataField::DATA_TYPE_SPEED,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatSpeed',
      ),
      'avg-pace' => array(
        'title' => t('Avg pace'),
        'data_field' => 'speed',
        'data_type' => OpenFitDataField::DATA_TYPE_PACE,
        'sort_reverse' => TRUE,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatPace',
      ),
      'max-speed' => array(
        'title' => t('Max speed'),
        'data_field' => 'activity_max_speed',
        'data_type' => OpenFitDataField::DATA_TYPE_SPEED,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatSpeed',
      ),
      'max-pace' => array(
        'title' => t('Max pace'),
        'data_field' => 'activity_max_speed',
        'data_type' => OpenFitDataField::DATA_TYPE_PACE,
        'sort_reverse' => TRUE,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatPace',
      ),
      'elev-gain' => array(
        'title' => t('Elev gain'),
        'data_field' => 'activity_elevation_gain',
        'data_type' => OpenFitDataField::DATA_TYPE_DISTANCE,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatElevation',
      ),
      'elev-loss' => array(
        'title' => t('Elev loss'),
        'data_field' => 'activity_elevation_loss',
        'data_type' => OpenFitDataField::DATA_TYPE_DISTANCE,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatElevation',
      ),
      'calories' => array(
        'title' => OpenFitMeasurement::getCalorieTitle(OpenFitUserSetting::get($user->uid, OpenFitUserSetting::TYPE_CALORIE_UNITS)),
        'data_field' => 'activity_calories',
        'data_type' => OpenFitDataField::DATA_TYPE_NUMERIC,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatCalories',
      ),
      'notes' => array(
        'title' => t('Notes'),
        'data_field' => 'activity_notes',
      ),
      'avg-heartrate' => array(
        'title' => t('Avg heartrate'),
        'data_field' => 'activity_avg_heartrate',
        'data_type' => OpenFitDataField::DATA_TYPE_NUMERIC,
        'decimals' => 1,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatNumber',
      ),
      'max-heartrate' => array(
        'title' => t('Max heartrate'),
        'data_field' => 'activity_max_heartrate',
        'data_type' => OpenFitDataField::DATA_TYPE_NUMERIC,
        'decimals' => 1,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatNumber',
      ),
      'avg-cadence' => array(
        'title' => t('Avg cadence'),
        'data_field' => 'activity_avg_cadence',
        'data_type' => OpenFitDataField::DATA_TYPE_NUMERIC,
        'decimals' => 1,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatNumber',
      ),
      'max-cadence' => array(
        'title' => t('Max cadence'),
        'data_field' => 'activity_max_cadence',
        'data_type' => OpenFitDataField::DATA_TYPE_NUMERIC,
        'decimals' => 1,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatNumber',
      ),
      'avg-power' => array(
        'title' => t('Avg power'),
        'data_field' => 'activity_avg_power',
        'data_type' => OpenFitDataField::DATA_TYPE_NUMERIC,
        'decimals' => 1,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatNumber',
      ),
      'max-power' => array(
        'title' => t('Max power'),
        'data_field' => 'activity_max_power',
        'data_type' => OpenFitDataField::DATA_TYPE_NUMERIC,
        'decimals' => 1,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatNumber',
      ),
    );
  }
  
  public static function getLapTableFieldInfo() {
    global $user;
    return array(
      'lap_id' => array(
        'primary_key' => TRUE,
      ),
      'number' => array(
        'title' => '#',
        'data_field' => 'lap_number',
        'data_type' => OpenFitDataField::DATA_TYPE_NUMERIC,
      ),
      'type' => array(
        'title' => t('Type'),
        'data_field' => 'lap_type',
        'localize' => 'true',
      ),
      'start' => array(
        'title' => t('Start'),
        'data_field' => 'lap_start',
        'timezone_field' => 'activity_timezone',
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatDatetime',
      ),
      'start-time' => array(
        'title' => t('Start time'),
        'data_field' => 'lap_start',
        'timezone_field' => 'activity_timezone',
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatDatetime',
        'date_format' => IntlDateFormatter::NONE,
      ),
      'distance' => array(
        'title' => t('Distance'),
        'data_field' => 'lap_distance',
        'data_type' => OpenFitDataField::DATA_TYPE_DISTANCE,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatDistance',
      ),
      'time' => array(
        'title' => t('Time'),
        'data_field' => 'lap_duration',
        'data_type' => OpenFitDataField::DATA_TYPE_TIME,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatTimespan',
      ),
      'avg-speed' => array(
        'title' => t('Avg speed'),
        'data_field' => 'speed',
        'data_type' => OpenFitDataField::DATA_TYPE_SPEED,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatSpeed',
      ),
      'avg-pace' => array(
        'title' => t('Avg pace'),
        'data_field' => 'speed',
        'data_type' => OpenFitDataField::DATA_TYPE_PACE,
        'sort_reverse' => TRUE,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatPace',
      ),
      'max-speed' => array(
        'title' => t('Max speed'),
        'data_field' => 'lap_max_speed',
        'data_type' => OpenFitDataField::DATA_TYPE_SPEED,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatSpeed',
      ),
      'max-pace' => array(
        'title' => t('Max pace'),
        'data_field' => 'lap_max_speed',
        'data_type' => OpenFitDataField::DATA_TYPE_PACE,
        'sort_reverse' => TRUE,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatPace',
      ),
      'elev-gain' => array(
        'title' => t('Elev gain'),
        'data_field' => 'lap_elevation_gain',
        'data_type' => OpenFitDataField::DATA_TYPE_DISTANCE,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatElevation',
      ),
      'elev-loss' => array(
        'title' => t('Elev loss'),
        'data_field' => 'lap_elevation_loss',
        'data_type' => OpenFitDataField::DATA_TYPE_DISTANCE,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatElevation',
      ),
      'calories' => array(
        'title' => OpenFitMeasurement::getCalorieTitle(OpenFitUserSetting::get($user->uid, OpenFitUserSetting::TYPE_CALORIE_UNITS)),
        'data_field' => 'lap_calories',
        'data_type' => OpenFitDataField::DATA_TYPE_NUMERIC,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatCalories',
      ),
      'avg-heartrate' => array(
        'title' => t('Avg heartrate'),
        'data_field' => 'lap_avg_heartrate',
        'data_type' => OpenFitDataField::DATA_TYPE_NUMERIC,
        'decimals' => 1,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatNumber',
      ),
      'max-heartrate' => array(
        'title' => t('Max heartrate'),
        'data_field' => 'lap_max_heartrate',
        'data_type' => OpenFitDataField::DATA_TYPE_NUMERIC,
        'decimals' => 1,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatNumber',
      ),
      'avg-cadence' => array(
        'title' => t('Avg cadence'),
        'data_field' => 'lap_avg_cadence',
        'data_type' => OpenFitDataField::DATA_TYPE_NUMERIC,
        'decimals' => 1,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatNumber',
      ),
      'max-cadence' => array(
        'title' => t('Max cadence'),
        'data_field' => 'lap_max_cadence',
        'data_type' => OpenFitDataField::DATA_TYPE_NUMERIC,
        'decimals' => 1,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatNumber',
      ),
      'avg-power' => array(
        'title' => t('Avg power'),
        'data_field' => 'lap_avg_power',
        'data_type' => OpenFitDataField::DATA_TYPE_NUMERIC,
        'decimals' => 1,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatNumber',
      ),
      'max-power' => array(
        'title' => t('Max power'),
        'data_field' => 'lap_max_power',
        'data_type' => OpenFitDataField::DATA_TYPE_NUMERIC,
        'decimals' => 1,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatNumber',
      ),    
    );
  }
  
  public static function getActivitySummaryFieldInfo() {
  global $user;
    return array(
      'count' => array(
        'title' => t('Total activities'),
        'data_type' => OpenFitDataField::DATA_TYPE_NUMERIC,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatNumber',
      ),
      'distance' => array(
        'title' => t('Total distance'),
        'data_type' => OpenFitDataField::DATA_TYPE_DISTANCE,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatDistance',
      ),
      'duration' => array(
        'title' => t('Total time'),
        'data_type' => OpenFitDataField::DATA_TYPE_TIME,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatTimespan',
      ),
      'avg-speed' => array(
        'title' => t('Avg speed'),
        'data_field' => 'speed',
        'data_type' => OpenFitDataField::DATA_TYPE_SPEED,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatSpeed',
        'blank' => '-',
      ),
      'avg-pace' => array(
        'title' => t('Avg pace'),
        'data_field' => 'speed',
        'data_type' => OpenFitDataField::DATA_TYPE_PACE,
        'sort_reverse' => TRUE,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatPace',
        'blank' => '-',
      ),
      'calories' => array(
        'title' => OpenFitMeasurement::getCalorieTitle(OpenFitUserSetting::get($user->uid, OpenFitUserSetting::TYPE_CALORIE_UNITS)),
        'data_type' => OpenFitDataField::DATA_TYPE_NUMERIC,
        'format_class' => 'OpenFitDataField',
        'format_function' => 'formatCalories',
      ),
    );
  }
  
  public static function formatName($row, $field_id, $field_info, $value) {
    if (!isset($field_info['link']) || $field_info['link'] == '') return $value;
    $options = array('language' => $row->language);
    if (isset($field_info['link_class'])) {
      $options['attributes'] = array('class' => array($field_info['link_class']));
    }
    return l($value, drupal_get_path_alias('node/' . $row->nid), $options);
  }
  
  const EARTH_MEAN_RADIUS = 6371000;
}
