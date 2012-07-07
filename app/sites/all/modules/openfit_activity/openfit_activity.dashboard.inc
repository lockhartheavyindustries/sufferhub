<?php

function openfit_activity_dashboard_form_get_totals_panel_markup($title, $field_info, $form_data, $form_fields = null) {
  $page = array(
    'title' => array('#markup' => '<h2>' . $title . '</h2>'),
    'summary' => array(
      '#type' => 'container', 
      '#attributes' => array('class' => array('field-block')),
      OpenFitDataField::getFormDataRenderArray($field_info, $form_data, $form_fields)
    ),
  );
  return drupal_render($page);
}

function openfit_activity_dashboard_form_get_week_chart_data() {
  global $user;
  $week_start = OpenFitUserSetting::get($user->uid, OpenFitUserSetting::TYPE_WEEK_START, variable_get('date_first_day', 1));
  $now = new DateTime('now');
  $now = OpenFitMeasurementDateTime::getFirstDayOfWeekDate($week_start, $now);
  $label_format = 'n/j';
  $statistics = OpenFitActivity::getWeekStatistics(5, $user->uid);
  $data = array();
  foreach ($statistics as $bar) {
    $barData = array('value' => $bar['value']);
    $barData['label'] = strtolower($bar['group']->format($label_format));
    if ($bar['group'] == $now) $barData['color'] = '#3C83BC';
    $data[] = (object)$barData;   
  }
  //die(print_r($data, true));
  return $data;
}

function openfit_activity_dashboard_form_get_month_chart_data() {
  global $user;
  $now = new DateTime('now');
  $now = new DateTime($now->format('Y-m') . '-01');
  $label_format = 'M';
  $statistics = OpenFitActivity::getMonthStatistics(5, $user->uid);
  $data = array();
  foreach ($statistics as $bar) {
    $barData = array('value' => $bar['value']);
    $barData['label'] = strtolower($bar['group']->format($label_format));
    if ($bar['group'] == $now) $barData['color'] = '#3C83BC';
    $data[] = (object)$barData;    
  }
  return $data;
}

/**
 * Method to construct the activity dashboard form.
 */
function openfit_activity_dashboard_form($form, &$form_state) {
  drupal_page_is_cacheable(FALSE);
  global $user;
  $activities = OpenFitActivity::getActivities($user->uid, null, null, 10);  
  $nodes = array();
  if (count($activities) > 0) {
    foreach ($activities as $activity) {
      $node = node_load($activity->nid);
      node_build_content($node, 'teaser');
      $teaser = $node->content;
      $nodes[] = array(
        'item' => $teaser,
      );
    }
  } else {
    $nodes[] = array(
      '#markup' => t('You haven\'t recorded an activity yet.'),
    );
  }
  
  $week_summary_data = OpenFitActivity::getWeekSummaryInfo($user->uid);
  $month_summary_data = OpenFitActivity::getMonthSummaryInfo($user->uid);
  $summary_fields = array('distance', 'duration', 'calories', 'avg-speed', 'avg-pace');
  $summary_field_info = OpenFitActivity::getActivitySummaryFieldInfo();
  $summary_data = OpenFitDataTable::getTableRows($summary_field_info, $summary_fields,
    array($week_summary_data, $month_summary_data));
  
  $chart_label = t('Distance');
  $this_week_markup = openfit_activity_dashboard_form_get_totals_panel_markup(
    t('This Week'), $summary_field_info, $summary_data[0], $summary_fields);
  $this_week_markup .= '<div id="this-week-chart" class="mini-chart"></div><div class="mini-chart-label">' . $chart_label . '</div>';
    
  $this_month_markup = openfit_activity_dashboard_form_get_totals_panel_markup(
    t('This Month'), $summary_field_info, $summary_data[1], $summary_fields);
  $this_month_markup .= '<div id="this-month-chart" class="mini-chart"></div><div class="mini-chart-label">' . $chart_label . '</div>';
  
  drupal_add_js(array('weekChartData' => openfit_activity_dashboard_form_get_week_chart_data()), 'setting');
  drupal_add_js(array('monthChartData' => openfit_activity_dashboard_form_get_month_chart_data()), 'setting');

  $form['sidebar'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('info-sidebar')),
    'totals' => array(
      'this-week' => array(
        '#type' => 'container',
        'block' => array(
          '#markup' => $this_week_markup,
        ),
      ),
      'this-month' => array(
        '#type' => 'container',
        'block' => array(
          '#markup' => $this_month_markup,
        ),
      ),
    ),
  );
  
  $actions = array(
    'add' => array(
      '#attributes' => array('id' => 'add-button'),
      '#left_image' => TRUE,
      '#disabled' => !user_access('create activity content'),
      '#text' => t('Add'),
      '#url' => drupal_get_path_alias('node/add/activity'),    
    ),
    'import' => array(
      '#attributes' => array('id' => 'import-button'),
      '#left_image' => TRUE,
      //'#disabled' => !file_import_types_exist(),
      '#text' => t('Import'),
      '#url' => 'activity/import',
    ),
  );

  $form['header'] = array(
    '#type' => 'container',
    'action-bar' => array(
      '#type' => 'container',
      '#attributes' => array('id' => 'action-bar', 'class' => array('clearfix')),
      'actions' => array(
        '#type' => 'container',
        '#attributes' => array('id' => 'actions'),
        'actions' => theme('commandbar', array('items' => $actions)),
      ),
    ),
    'title' => array(
      '#markup' => '<h2>' . t('Recent activities') . '</h2>',
    ),
  );
    
  $form['items'] = array(
    '#type' => 'container',
    'items' => $nodes,
  );
  $form['#attached']['css'][] = drupal_get_path('module', 'openfit_activity') . '/openfit_activity.css';
  $form['#attached']['js'][] = drupal_get_path('module', 'openfit_activity') . '/js/view.dashboard.js';  
  $form['#attached']['js'][] = drupal_get_path('module', 'openfit_ui') . '/mini-bar-chart.js';
  return $form;
}