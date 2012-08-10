<?php

function openfit_activity_activity_page_top_summary($node, $form_data) {
  $output = array(
    '#type' => 'container', 
    '#attributes' => array('class' => 'top-summary'),
  );
  $output['activity-category'] = array(
    '#markup' => '<div class="activity-category"><img  src="/' . $node->openfit_info['activity']->category_image_url . '"></img></div>',
  );
  $output['summary-info'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => 'summary-info'),
  );
  $output['summary-info']['name'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => 'activity-name'),
    'fields' => array(
      '#type' => 'container',      
      '#attributes' => array('class' => array('field-item')),
      'field' => array(
        '#markup' => htmlentities($form_data['name']['data'], ENT_COMPAT, 'UTF-8'),
      ),
    ),
  );
  $output['summary-info']['start'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => 'activity-start'),
    'fields' => array(
      '#type' => 'container',      
      '#attributes' => array('class' => array('field-item')),
      'field' => array(
        '#markup' => htmlentities($form_data['start']['data'], ENT_COMPAT, 'UTF-8'),
      ),
    ),
  );
  $description_value = t('!time !category for !distance', array(
    '!time' => '<span class="activity-time">' . $form_data['time']['data'] . '</span>',
    '!category' => '<span class="activity-category-name">' . $form_data['sport-noun']['data'] . '</span>',
    '!distance' => '<span class="activity-distance">' . $form_data['distance']['data'] . '</span>',
  ));
  $output['summary-info']['description'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => 'activity-description'),
    'fields' => array(
      '#type' => 'container',      
      '#attributes' => array('class' => array('field-item')),
      'field' => array(
        '#markup' => $description_value,
      ),
    ),
  );  
  return $output;
}

function openfit_activity_activity_page_action_bar($node, $view_mode) {
  $output = array(
    '#type' => 'container',
    '#attributes' => array('id' => 'action-bar', 'class' => array('clearfix')),
    'page-navigation' => array(
      '#type' => 'container', 
      '#attributes' => array('id' => 'page-navigation'),
      'tabs' => openfit_activity_activity_page_tabs($node, $view_mode),
    ),
    'actions' => array(
      '#type' => 'container',
      '#attributes' => array('id' => 'actions'),
      'actions' => openfit_activity_activity_page_actions($node),
    ),
    'top' => array('#markup' => '<div class="clearfix">&nbsp;</div>'),
  );
  if (user_is_logged_in()) {
    $delete_confirm_markup = '<span class="ui-icon ui-icon-alert" style="float:left;"></span>';
    $delete_confirm_markup .= '<span>' . t('Are you sure you want to delete this item?') . '</span>';
    $output['delete-confirm'] = array(
      '#type' => 'container',
      '#attributes' => array('id' => 'delete-confirm', 'title' => t('Confirm delete'), 'style' => 'display:none'),
      'text' => array(
        '#markup' => $delete_confirm_markup,
      ),
    );
  }
  return $output;
}

function openfit_activity_activity_page_tabs($node, $active_page) {
  $tabs = array(
    'full' => array('href' => '', 'title' => t('Summary')),
    'laps' => array('href' => '/laps', 'title' => t('Laps')),
    'map' => array('href' => '/map', 'title' => t('Map')),
    'comments' => array('href' => '/comments', 'title' => t('Comments')),
  );
  $output = array();
  foreach ($tabs as $page_id => $tab_info) {
    $tab = array(
      '#theme' => 'menu_local_task',
      '#link' => array(
        'title' => $tab_info['title'], 
        'href' => drupal_get_path_alias('node/' . $node->nid . $tab_info['href']), 
        'localized_options' => array()
      ),
    );
    if ($active_page == $page_id) $tab['#active'] = TRUE;
    $output[$page_id] = $tab;
  }
  $output = array(
    '#theme' => 'menu_local_tasks',
    '#secondary' => $output,
  );
  return $output;
}

function openfit_activity_activity_page_actions($node) {
  global $base_url;
  $activity_url = drupal_get_path_alias('node/' . $node->nid);
  $items = array();
  if (user_is_logged_in()) {
    $items['edit'] = array(
      '#attributes' => array('id' => 'edit-button'),
      '#left_image' => TRUE,
      '#disabled' => !$node->openfit_info['activity']->access['update'],
      '#text' => t('Edit'),
      '#url' => $activity_url . '/edit',
    );
    $items['delete'] = array(
      '#attributes' => array('id' => 'delete-button'),
      '#left_image' => TRUE,
      '#disabled' => !$node->openfit_info['activity']->access['delete'],
      '#text' => t('Delete'),
      '#url' => $activity_url . '/delete-activity',
    );
  }
  // TODO: Maybe get the export menu options from supported export types
  $export_menu = array();
  $export_menu[] = l(t('GPX'), $activity_url . '/export', array('alias' => TRUE, 'query' => array('type' => 'gpx')));
  $export_menu[] = l(t('TCX'), $activity_url . '/export', array('alias' => TRUE, 'query' => array('type' => 'tcx')));
  $export_menu_markup = '';
  foreach ($export_menu as $menu_item) {
    $export_menu_markup .= '<li>' . $menu_item . '</li>';
  }
  $export_menu_markup = '<ul>' . $export_menu_markup . '</ul>';
  $items['export'] = array(
    '#attributes' => array('id' => 'export-button'),
    '#left_image' => TRUE,
    '#text' => t('Export'),
    '#url' => $activity_url . '/export',
    '#menu' => array(
      '#markup' => $export_menu_markup,
    ),
  );
//  if (user_is_logged_in()) {
//    $selected = $node->openfit_info['activity']->status;
//    $sharing_options = array(
//      0 => array('text' => t('Private'), 'value' => 'private'),
//      1 => array('text' => t('Public'), 'value' => 'public'),
//    );
//    $sharing_menu = array();
//    foreach ($sharing_options as $option_value => $sharing_option) {
//      $selected_class = ($selected == $option_value) ? 'checked' : '';
//      $ajaxurl = $base_url . '/openfit/api/?op=set_node_value&nid=' . $node->nid . //'&f=sharing&v=' . $sharing_option['value'];
//      $url = $activity_url . '/edit/sharing';
//      $label = '<span class="check-img ' . $selected_class . '"></span>';
//      $label .= '<span class="check-text">' . $sharing_option['text'] . '</span>';
//      $label .= '<input name="ajaxurl" type="hidden" value="' . $ajaxurl . '" />';
//      $sharing_menu[] = l($label, $url, array('html' => TRUE, 'alias' => TRUE, //'query' => array('value' => $sharing_option['value'])));
//    }
//    $sharing_menu_markup = '';
//    foreach ($sharing_menu as $menu_item) {
//      $sharing_menu_markup .= '<li class="check-item ajax">' . $menu_item . //'</li>';
//    }
//    $sharing_menu_markup = '<ul>' . $sharing_menu_markup . '</ul>';
//    $items['sharing'] = array(
//      '#attributes' => array('id' => 'sharing-button'),
//      '#left_image' => TRUE,
//      '#right_image' => TRUE,
//      '#disabled' => !$node->openfit_info['activity']->access['update'],
//      '#url' => $activity_url . '/edit/sharing',
//      '#url_options' => array('alias' => TRUE, 'attributes' => array('title' => //t('Sharing'))),
//      '#menu' => array(
//        '#markup' => $sharing_menu_markup,
//      ),
//    );
//  }

  // TODO: Implement settings
  /*
  $items['settings'] = array(
    '#attributes' => array('id' => 'settings-button'),
    '#left_image' => TRUE,
    '#url' => '#',
    '#url_options' => array('attributes' => array('title' => t('Settings'))),
  );
  */
  return theme('commandbar', array('items' => $items));
}

function openfit_activity_read_activity_info($node) {
  $activities= OpenFitActivity::getActivities(null, $node->nid);
  $activity = isset($activities[$node->nid]) ? $activities[$node->nid] : null;
  if (!isset($activity)) {
    // TODO: Need better way to handle error if activity not found by passing some context to the render function.
    drupal_set_message(t('Activity not found'),'error');
    return;
  }
  $activity->access = array('update' => node_access('update', $node), 'delete' => node_access('delete', $node));
  $node->openfit_info['activity'] = $activity;
}

function openfit_activity_view_teaser($node, $active_page, $language) {
  $node->openfit_info = array();
  // Load the activity.
  openfit_activity_read_activity_info($node);
  return $node;
}
function openfit_activity_entity_view_teaser($node, $view_mode, $language) {
  if (!isset($node->openfit_info['activity'])) return;

  $fields = array(
    'name', 'start', 'time', 'sport-noun', 'distance', 'avg-speed', 'max-speed', 'avg-pace', 'max-pace', 'calories',
  );

  $activity = $node->openfit_info['activity'];
  $field_info = OpenFitActivity::getTableFieldInfo();
  $activity_data = OpenFitDataTable::getTableRows($field_info, $fields, array($activity));
  $activity_data = $activity_data[$activity->nid];
  
  $activity_markup = drupal_render(openfit_activity_activity_page_top_summary($node, $activity_data));
  $activity_url = drupal_get_path_alias('node/' . $node->nid);
  
  $page_class = 'page-activity-' . $view_mode;
  $page_id = $page_class . '-' . $node->nid;
  $page_wrap = array(
    '#type' => 'container',
    '#attributes' => array('id' => $page_id, 'class' => array($page_class)),
    'top-summary' => array(
      '#markup' => l($activity_markup, $activity_url, 
        array('alias' => TRUE, 'html' => TRUE, 'attributes' => array('class' => array('activity-item')))
      ),
    ),
  );
  $page = array();  
  $page['#attached']['css'][] = drupal_get_path('module', 'openfit_activity') . '/openfit_activity.css';
  $page[$page_id] = $page_wrap;
  $node->content = $page;
}

function openfit_activity_view_full($node, $active_page, $language) {
  $node->openfit_info = array();
  // Load the activity.
  openfit_activity_read_activity_info($node);
  return $node;
}
function openfit_activity_entity_view_full($node, $view_mode, $language) {
  global $base_url;
  // TODO: Verify this scenario when tempate is done. We still may need to fill the $node->content field with something.
  if (!isset($node->openfit_info['activity'])) return;
  
  $fields = array(
    'name', 'start', 'time', 'sport-noun', 'distance', 'avg-speed', 'max-speed', 'avg-pace', 'max-pace', 'calories', 'elev-gain', 'elev-loss', 
    'notes', 'avg-heartrate', 'max-heartrate', 'avg-cadence', 'max-cadence', 'avg-power', 'max-power','sufferbucks',
  );

  $field_info = OpenFitActivity::getTableFieldInfo();
  $activity_data = OpenFitDataTable::getTableRows($field_info, $fields, array($node->openfit_info['activity']));
  $activity_data = $activity_data[$node->openfit_info['activity']->nid];
  
  $timing_fields = array('speed' => array('avg-speed'), 'pace' => array('avg-pace'));
  if (isset($node->openfit_info['activity']->activity_max_speed)) {
    $timing_fields['speed'][] = 'max-speed';
    $timing_fields['pace'][] = 'max-pace';
  }
  $timing_fields = array_merge($timing_fields['speed'], $timing_fields['pace']);
  $elevation_fields = array();
  if (isset($node->openfit_info['activity']->activity_elevation_gain)) $elevation_fields[] = 'elev-gain';
  if (isset($node->openfit_info['activity']->activity_elevation_loss)) $elevation_fields[] = 'elev-loss';
  
  $page_class = 'page-activity-' . $view_mode;
  $page_id = $page_class . '-' . $node->openfit_info['activity']->nid;
  $page_wrap = array(
    '#type' => 'container',
    '#attributes' => array('id' => $page_id, 'class' => array($page_class)),
  );
  $page_wrap['top-summary'] = openfit_activity_activity_page_top_summary($node, $activity_data);
  $page_wrap['action-bar'] = openfit_activity_activity_page_action_bar($node, $view_mode);
  $page_wrap['detail-area'] = array(
    '#type' => 'container', 
    '#attributes' => array('id' => 'detail-area', 'class' => array('clearfix')),
    'general' => array(
      '#type' => 'container', 
      '#attributes' => array('id' => 'general', 'class' => array('field-block')),
      OpenFitDataField::getFormDataRenderArray($field_info, $activity_data, array('distance', 'time', 'calories'))
    ),
    'timing' => array(
      '#type' => 'container', 
      '#attributes' => array('id' => 'timing', 'class' => array('field-block')),
      OpenFitDataField::getFormDataRenderArray($field_info, $activity_data, $timing_fields)
    ),
  );
  if (count($elevation_fields) > 0) {
    $page_wrap['detail-area']['elevation'] = array(
      '#type' => 'container', 
      '#attributes' => array('id' => 'elevation', 'class' => array('field-block')),
      OpenFitDataField::getFormDataRenderArray($field_info, $activity_data, $elevation_fields)
    );
  }
  
  $track_summary_blocks = array('heartrate','cadence','power');

  foreach ($track_summary_blocks as $track) {
    $avg = 'avg-' . $track;
    $max = 'max-' . $track;
    $classes = array('field-block');
    if ((!isset($activity_data[$avg]['data']) || strlen($activity_data[$avg]['data']) == 0) && 
      (!isset($activity_data[$max]['data']) || strlen($activity_data[$max]['data']) == 0)) {
      $classes[] = 'no-data';
    }
    
    $page_wrap['detail-area'][$track] = array(
      '#type' => 'container', 
      '#attributes' => array('id' => $track, 'class' => $classes),
      OpenFitDataField::getFormDataRenderArray($field_info, $activity_data, array($avg, $max))
    );
  }

  $page_wrap['detail-area']['sufferbucks'] = array(
    '#type' => 'container', 
    '#attributes' => array('id' => 'sufferbucks'),
    OpenFitDataField::getFormDataRenderArray($field_info, $activity_data, array('sufferbucks'))
  );
  

  $page_wrap['detail-area']['notes'] = array(
    '#type' => 'container', 
    '#attributes' => array('id' => 'notes'),
    OpenFitDataField::getFormDataRenderArray($field_info, $activity_data, array('notes'))
  );
  
  /* TODO: Put this in a drupal element - also JS attach below */
  $ajaxurl = $base_url . '/openfit/api/?op=get_chart_data&nid=' . $node->nid;
  $chart_markup = '<div id="activity-chart" class="chart" data-dataurl="' . $ajaxurl . '">

<div class="top-bar">
  <div class="left-actions">
    <ul class="buttonbar chart metrics">
      <li id="activity-chart-metric" class="metric-selector">
        <div class="btn flat">
          <a href="#select">
            <div class="name"></div>
            <span class="arrow">&nbsp;</span>
          </a>
        </div>
      </li>
    </ul>
  </div>

  <div class="right-actions">
    <ul class="buttonbar settings">
      <li id="zoom-out-button" class="disabled">
        <div class="btn">
          <a title="Fit to window" href="#fit">
          <div class="img">&nbsp;</div>
        </a>
        </div>
      </li>
    </ul>
  </div>

</div>

<div class="chart-canvas-container"></div>

<div class="bottom-bar">
  <div class="center-outer">
      <ul class="buttonbar xaxis center-inner">
        <li class="xaxis-selector">
          <div class="btn flat">
          <a href="#select">
              <span class="name"></span>
              <span class="arrow">&nbsp;</span>
            </a>
          </div>          
        </li>
      </ul>
  </div>
  <div style="height:1.0em;padding-bottom:8px">&nbsp;</div>
</div>

</div>
';

  $page_wrap['chart'] = array(
    '#markup' => $chart_markup,
  );

  drupal_add_library('system', 'ui');
  drupal_add_library('system', 'ui.dialog');
  $page = array();  
  $page['#attached']['css'][] = drupal_get_path('module', 'openfit_activity') . '/openfit_activity.css';
  $page['#attached']['js'][] = drupal_get_path('module', 'openfit_activity') . '/js/view.activity.js';  
  $page['#attached']['js'][] = drupal_get_path('module', 'openfit_activity') . '/js/view.activity-' . $view_mode . '.js';  
  /* TODO: Move this into chart element */
  $page['#attached']['js'][] = drupal_get_path('module', 'openfit_ui') . '/flot/jquery.flot.js';  
  $page['#attached']['js'][] = drupal_get_path('module', 'openfit_ui') . '/flot/jquery.flot.resize.js';  
  $page['#attached']['js'][] = drupal_get_path('module', 'openfit_ui') . '/flot/jquery.flot.selection.js';  
  $page['#attached']['js'][] = drupal_get_path('module', 'openfit_ui') . '/jquery.openfit_popupmenus.js';  
  $page['#attached']['js'][] = drupal_get_path('module', 'openfit_ui') . '/jquery.openfit_activitydetailchart.js';  

  $page[$page_id] = $page_wrap;
  $node->content = $page;
}

function openfit_activity_view_laps($node, $view_mode, $language) {
  $node->openfit_info = array();
  // Load the activity.
  openfit_activity_read_activity_info($node);
  if (isset($node->openfit_info['activity'])) {
    $node->openfit_info['laps'] = OpenFitActivity::getActivityLaps($node->openfit_info['activity']->activity_id);
  }
  return $node;
}

function openfit_activity_entity_view_laps($node, $view_mode, $language) {
  // TODO: Verify this scenario when tempate is done. We still may need to fill the $node->content field with something.
  if (!isset($node->openfit_info['activity'])) return;
  
  $fields = array('name', 'start', 'time', 'sport-noun', 'distance');

  $field_info = OpenFitActivity::getTableFieldInfo();
  $activity_data = OpenFitDataTable::getTableRows($field_info, $fields, array($node->openfit_info['activity']));
  $activity_data = $activity_data[$node->openfit_info['activity']->nid];
  
  $table_columns = array('number', 'type', 'start-time', 'distance', 'time', 'avg-speed', 'avg-pace', 'elev-gain', 'calories', 'sufferbucks',);

  $lap_field_info = OpenFitActivity::getLapTableFieldInfo();
  $header = OpenFitDataTable::getTableHeader($lap_field_info, $table_columns);
  $table_rows = OpenFitDataTable::getTableRows($lap_field_info, $table_columns, $node->openfit_info['laps']);
  $table_class = array('activity-table');
  $table_class[] = $table_rows > 0 ? 'data-table' : 'data-table-empty';
 
  $page_class = 'page-activity-' . $view_mode;
  $page_id = $page_class . '-' . $node->openfit_info['activity']->nid;
  $page_wrap = array(
    '#type' => 'container',
    '#attributes' => array('id' => $page_id, 'class' => array($page_class)),
  );
  $page_wrap['top-summary'] = openfit_activity_activity_page_top_summary($node, $activity_data);
  $page_wrap['action-bar'] = openfit_activity_activity_page_action_bar($node, $view_mode);
  $page_wrap['detail-area'] = array(
    '#type' => 'container', 
    '#attributes' => array('id' => 'detail-area'),
  );
  $page_wrap['detail-area']['lap-table'] = array(
    '#markup' => theme('table', array(
      'header' => $header,
      'rows' => $table_rows,
      'empty' => t('No laps'),
      'attributes' => array('class' => $table_class, 'id' => 'lap-table'),
    )),
  );

  drupal_add_library('system', 'ui');
  drupal_add_library('system', 'ui.dialog');
  $page = array();  
  $page['#attached']['css'][] = drupal_get_path('module', 'openfit_activity') . '/openfit_activity.css';
  $page['#attached']['js'][] = drupal_get_path('module', 'openfit_activity') . '/js/view.activity.js';  
  $page['#attached']['js'][] = drupal_get_path('module', 'openfit_activity') . '/js/view.activity-' . $view_mode . '.js';  
  $page['#attached']['js'][] = drupal_get_path('module', 'openfit_ui') . '/jquery.openfit_popupmenus.js';  
  $page[$page_id] = $page_wrap;
  $node->content = $page;
}

function openfit_activity_view_map($node, $view_mode, $language) {
  $node->openfit_info = array();
  // Load the activity.
  openfit_activity_read_activity_info($node);
  if (isset($node->openfit_info['activity'])) {
    //TODO: Read map data
    //$node->openfit_info['laps'] = OpenFitActivity::getActivityLaps($node->openfit_info['activity']->activity_id)->fetchAllAssoc('lap_id');
  }
  return $node;
}

function openfit_activity_entity_view_map($node, $view_mode, $language) {
  global $base_url;
  // TODO: Verify this scenario when tempate is done. We still may need to fill the $node->content field with something.
  if (!isset($node->openfit_info['activity'])) return;
  
  $fields = array('name', 'start', 'time', 'sport-noun', 'distance');

  $field_info = OpenFitActivity::getTableFieldInfo();
  $activity_data = OpenFitDataTable::getTableRows($field_info, $fields, array($node->openfit_info['activity']));
  $activity_data = $activity_data[$node->openfit_info['activity']->nid];
  
  $page_class = 'page-activity-' . $view_mode;
  $page_id = $page_class . '-' . $node->openfit_info['activity']->nid;
  $page_wrap = array(
    '#type' => 'container',
    '#attributes' => array('id' => $page_id, 'class' => array($page_class)),
  );
  $page_wrap['top-summary'] = openfit_activity_activity_page_top_summary($node, $activity_data);
  $page_wrap['action-bar'] = openfit_activity_activity_page_action_bar($node, $view_mode);
  
  // TODO: This should be abstracted to an element/theme
  $ajaxurl = $base_url . '/openfit/api/?op=get_route_data&nid=' . $node->nid;
  $map_markup = '<div id="route" data-dataurl="' . $ajaxurl . '">' .
    '<div id="gmap" class="gmap" style="width:100%; height:600px;margin-top:10px;"></div></div>';
  $page_wrap['map'] = array(
    '#type' => 'markup',
    '#markup' => $map_markup,
  );

  drupal_add_library('system', 'ui');
  drupal_add_library('system', 'ui.dialog');
  $page = array();  
  $page['#attached']['css'][] = drupal_get_path('module', 'openfit_activity') . '/openfit_activity.css';
  $page['#attached']['js'][] = drupal_get_path('module', 'openfit_activity') . '/js/view.activity.js';  
  $page['#attached']['js'][] = drupal_get_path('module', 'openfit_activity') . '/js/view.activity-' . $view_mode . '.js';
  $page['#attached']['js'][] = drupal_get_path('module', 'openfit_ui') . '/jquery.openfit_popupmenus.js';  
  
  $gmap_url = 'http://maps.googleapis.com/maps/api/js?sensor=false';
  $api_key = variable_get('openfit_google_maps_key', '');  
  if (!empty($api_key)) $gmap_url .= '&key='.$api_key;
  
  $page['#attached']['js'][] = $gmap_url;
  $page[$page_id] = $page_wrap;
  $node->content = $page;
}

function openfit_activity_view_comments($node, $view_mode, $language) {
  $node->openfit_info = array();
  // Load the activity.
  openfit_activity_read_activity_info($node);
  if (isset($node->openfit_info['activity'])) {
    //TODO: Read comments data
    //$node->openfit_info['laps'] = OpenFitActivity::getActivityLaps($node->openfit_info['activity']->activity_id)->fetchAllAssoc('lap_id');
  }
  comment_node_view($node, 'full');
  return $node;
}

/**
 * Add a submit handler to redirect to the comments page.
 */
function openfit_activity_form_comment_node_activity_form_alter(&$form, &$form_state, $form_id) {
  if ($form['#node']->type != 'activity') return;

  if (!isset($form['actions']['submit']['#submit'])) $form['actions']['submit']['#submit'] = array();
  $form['actions']['submit']['#submit'][] = 'openfit_activity_comment_submit';
}

/**
 * Add a redirect to the comments page after a comment submission.
 */
function openfit_activity_comment_submit($form, &$form_state) {
  comment_form_submit($form, $form_state);
  $url = drupal_get_path_alias('node/' . $form['#node']->nid) . '/comments';
  $fragment = 'comment-' . $form['#node']->cid;
  $form_state['redirect'] = array($url, array('fragment' => $fragment));
}

function openfit_activity_entity_view_comments($node, $view_mode, $language) {
  // TODO: Verify this scenario when tempate is done. We still may need to fill the $node->content field with something.
  if (!isset($node->openfit_info['activity'])) return;
  
  // Remove "Add new comment" link.
  unset($node->content['links']['comment']);
  // Remove the author label
  unset($node->content['comments']['comment_form']['author']);

  $fields = array('name', 'start', 'time', 'sport-noun', 'distance');

  $field_info = OpenFitActivity::getTableFieldInfo();
  $activity_data = OpenFitDataTable::getTableRows($field_info, $fields, array($node->openfit_info['activity']));
  $activity_data = $activity_data[$node->openfit_info['activity']->nid];
  
  $page_class = 'page-activity-' . $view_mode;
  $page_id = $page_class . '-' . $node->openfit_info['activity']->nid;
  $page_wrap = array(
    '#type' => 'container',
    '#attributes' => array('id' => $page_id, 'class' => array($page_class)),
  );
  $page_wrap['top-summary'] = openfit_activity_activity_page_top_summary($node, $activity_data);
  $page_wrap['action-bar'] = openfit_activity_activity_page_action_bar($node, $view_mode);

  drupal_add_library('system', 'ui');
  drupal_add_library('system', 'ui.dialog');
  $page = array();  
    
  $page['#attached']['css'][] = drupal_get_path('module', 'openfit_activity') . '/openfit_activity.css';
  $page['#attached']['js'][] = drupal_get_path('module', 'openfit_activity') . '/js/view.activity.js';  
  $page['#attached']['js'][] = drupal_get_path('module', 'openfit_activity') . '/js/view.activity-' . $view_mode . '.js';  
  $page['#attached']['js'][] = drupal_get_path('module', 'openfit_ui') . '/jquery.openfit_popupmenus.js';  
  $page[$page_id] = $page_wrap;
  
  $delete_confirm_markup = '<span class="ui-icon ui-icon-alert" style="float:left;"></span>';
  $delete_confirm_markup .= '<span>' . t('Are you sure you want to delete this comment?') . '</span>';
  $page['delete-comment-confirm'] = array(
    '#type' => 'container',
    '#attributes' => array('id' => 'delete-comment-confirm', 'title' => t('Confirm delete'), 'style' => 'display:none'),
    'text' => array(
      '#markup' => $delete_confirm_markup,
    ),
  );

  $node->content = array_merge($page, $node->content);
}

/**
 * Return the form used to import a workout file.
 *
 * @param $form
 *   The form.
 * @param $form_state
 *   The form state.
 **/
function openfit_activity_import_activity_page($form, &$form_state) {
  global $user;
  
  module_load_include('inc', 'openfit_api', 'openfit_api.DatafileImportHandler');
  $importers = OpenFitDatafileImportHandler::getAllFileImporterInfos();
  $extensions = array();
  foreach ($importers as $importer) {
    $extension = '.' . strtoupper($importer['extension']);
    $extensions[$extension] = $extension;
  }
  ksort($extensions);
  $extensions = implode(', ',$extensions);
  
  $form['import-file'] = array(
    '#type' => 'file',
    '#title' => t('Import file'),    
    //'#title_display' => 'invisible',
    //'#required' => true,
    '#size' => 100,
    '#description' => t('Select a file to import (' . $extensions . ').'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Import'),
  );
  return $form;
}

/**
 * Process the import form submit.
 *
 * @param $form
 *   The form.
 * @param $form_state
 *   The form state.
 **/
function openfit_activity_import_activity_page_submit($form, &$form_state) {
  module_load_include('inc', 'openfit_api', 'openfit_api.DatafileImportHandler');
  $source = 'import-file';
  $error_text = null;
  $importer = new OpenFitDatafileImportHandler($source);
  if (!$importer->processFileUpload()) {
    $error_text = $importer->getErrorText();
    if (!isset($error_text)) $error_text = t('Could not import file. An unknown error occurred.');
    form_set_error($source, $error_text);
    return;
  }
}
 
/**
 * Return the form used to add a new activity node type.
 *
 * @param $node
 *   The node to add.
 * @param $form_state
 *   The form state.
 */
function openfit_activity_activity_insert_form($node, &$form_state) {
  return openit_activity_activity_addedit_form($node, $form_state, 'add');
}

/**
 * Handle the form submit handler for the add activity forms.
 *
 * @param $form
 *   The form.
 * @param $form_state
 *   The form state.
 */
function openfit_activity_activity_insert_submit($form, &$form_state) {
  openfit_activity_activity_addedit_submit($form, $form_state, 'add');
}

/**
 * Return the form used to edit an existing activity node type.
 *
 * @param $node
 *   The node to edit.
 * @param $form_state
 *   The form state.
 */
function openfit_activity_activity_update_form($node, &$form_state) {
  return openit_activity_activity_addedit_form($node, $form_state, 'edit');
}

/**
 * Handle the form submit handler for the edit activity forms.
 *
 * @param $form
 *   The form.
 * @param $form_state
 *   The form state.
 */
function openfit_activity_activity_update_submit($form, &$form_state) {
  openfit_activity_activity_addedit_submit($form, $form_state, 'edit');
}

/**
 * Return the form used to add or edit an activity node type.
 *
 * @param $node
 *   The node to add or edit.
 * @param $form_state
 *   The form state.
 * @param $mode
 *   Either 'add' for add or 'edit' for edit.
 */
function openit_activity_activity_addedit_form($node, &$form_state, $mode) {
  global $user;
  $preferences = OpenFitUserSetting::get($user->uid);

  $activity = $node->openfit_info['activity'];
  $category_options = OpenFitActivity::getActivityCategories();

  $date_formatter = new IntlDateFormatter(OpenFitUserSetting::getCurrentUserLocale(), IntlDateFormatter::SHORT, IntlDateFormatter::NONE);
  $date_pattern = OpenFitMeasurementDateTime::convertPattern($date_formatter->getPattern());

  $activity_localtime = new DateTime($activity->activity_start, new DateTimeZone('UTC'));
  OpenFitMeasurementDateTime::offsetDate($activity_localtime, $activity->activity_timezone);
  $start_date = $activity_localtime->format('Y-m-d H:i:s');
  
  $form = array();
  $form['#attached']['css'][] = drupal_get_path('module', 'openfit_activity') . '/openfit_activity.css';
  
  drupal_add_library('system', 'ui');

  if ($mode == 'add') {
    drupal_set_title(t('New Workout'));

    $actions = array();
    $actions['import'] = array(
      '#attributes' => array('id' => 'import-button'),
      '#left_image' => TRUE,
      //'#disabled' => !file_import_types_exist(),
      '#text' => t('Import'),
      '#url' => 'activity/import',
    );
    $form['#attached']['js'][] =drupal_get_path('module', 'openfit_activity') . '/js/view.activity-add.js';
    $form['action-bar'] = array(
      '#type' => 'container',
      '#attributes' => array('id' => 'action-bar', 'class' => array('clearfix')),
      'actions' => array(
        '#type' => 'container',
        '#attributes' => array('id' => 'actions'),
        'actions' => theme('commandbar', array('items' => $actions)),
      ),
    );
  }
  if (user_is_logged_in()) {
    $delete_confirm_markup = '<span class="ui-icon ui-icon-alert" style="float:left;"></span>';
    $delete_confirm_markup .= '<span>' . t('Are you sure you want to delete this item?') . '</span>';
    $output['delete-confirm'] = array(
      '#type' => 'container',
      '#attributes' => array('id' => 'delete-confirm', 'title' => t('Confirm delete'), 'style' => 'display:none'),
      'text' => array(
        '#markup' => $delete_confirm_markup,
      ),
    );
  }

  $form['activity-info'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array('id' => 'activity-info'),
    'sport' => array(
      '#type' => 'select',
      '#title' => t('Sport'),
      '#default_value' => $activity->category_id,
      '#options' => $category_options,
    ),
    'name' => array(
      '#type' => 'textfield',
      '#title' => t('Name'),
      '#default_value' => $node->title,
      '#size' => 255,
    ),
/*
    'start-date' => array(
      '#type' => 'date_popup',
      '#title' => t('Start'),
      '#default_value' => $start_date,
      '#date_format' => $date_pattern,
      '#date_label_position' => 'inline',
      '#required' => TRUE,
    ),
    'start-time' => array(
      '#type' => 'openfit_timeofday',
      '#title' => t('Start time'),
      '#default_value' => $activity_localtime,
      '#required' => TRUE,
    ),
    'time' => array(
      '#type' => 'openfit_duration',
      '#title' => t('Time'),
      '#default_value' => $activity->activity_duration,
      '#required' => TRUE,
    ),
    'distance' => array(
      '#type' => 'openfit_measurement',
      '#title' => t('Distance'),
      '#default_units' => $preferences[OpenFitUserSetting::TYPE_DISTANCE_UNITS],
      '#default_value' => $activity->activity_distance,
      '#required' => TRUE,
      '#value_units' => 'meter',
    ),
    'calories' => array(
      '#type' => 'openfit_measurement',
      '#title' => OpenFitMeasurement::getCalorieTitle($preferences[OpenFitUserSetting::TYPE_CALORIE_UNITS]),
      '#default_units' => $preferences[OpenFitUserSetting::TYPE_CALORIE_UNITS],
      '#default_value' => $activity->activity_calories,
      '#required' => TRUE,
      '#value_format' => OpenFitMeasurement::FORMAT_TYPE_DECIMALS,
      '#value_units' => 'kilojoule',
    ),
	'sufferbucks' => array(
      '#type' => 'openfit_measurement',
      '#title' => OpenFitMeasurement::getSufferbuckTitle($preferences[OpenFitUserSetting::TYPE_SUFFERBUCK_UNITS]),
      '#default_units' => $preferences[OpenFitUserSetting::TYPE_SUFFERBUCK_UNITS],
      '#default_value' => $activity->activity_sufferbucks,
      '#required' => TRUE,
      '#value_format' => OpenFitMeasurement::FORMAT_TYPE_DECIMALS,
      '#value_units' => 'sufferbuck',
    ),
*/
  );
  
  $week_start = $preferences[OpenFitUserSetting::TYPE_WEEK_START];
  $language = $language_value = $preferences[OpenFitUserSetting::TYPE_LANGUAGE];
  // Set the date popup to use the correct week start
  $js = 'jQuery.extend(true,Drupal.settings,{"datePopup":{"edit-dateofbirth-datepicker-popup-0":{"settings":{"firstDay":' . $week_start . '}}}});';
  drupal_add_js($js, array('type' => 'inline', 'scope' => 'footer'));
  if ($language != '' && $language != 'en') drupal_add_js('misc/ui/jquery.ui.datepicker-' . $language . '.js');
  
  //print_r($form);
  return $form;
}

/**
 * Handle the form submit handler for the add and edit activity forms.
 *
 * @param $form
 *   The form.
 * @param $form_state
 *   The form state.
 * @param $mode
 *   Either 'add' for add or 'edit' for edit.
 */
function openfit_activity_activity_addedit_submit($form, &$form_state, $mode) {
  global $user;
  $preferences = OpenFitUserSetting::get($user->uid);
  
  $activity_info = $form_state['values']['activity-info'];
  $node = $form['#node'];
  $activity = $form['#node']->openfit_info['activity'];
  
  $activity->category_id = $activity_info['sport'];

/* not updating these fields during edit, don't pass them along */
/*
  list($year, $month, $day) = explode('-', $activity_info['start-date']);
  $start = $activity_info['start-time'];
  $start->setDate($year, $month, $day);
  OpenFitMeasurementDateTime::offsetDate($start, -$activity->activity_timezone);
  $activity->activity_start =  $start->format('Y-m-d H:i:s'); 
  
  $activity->activity_duration = $activity_info['time'];
  $activity->activity_distance = $activity_info['distance'];
  $activity->activity_calories = $activity_info['calories'];
  $activity->activity_sufferbucks = $activity_info['sufferbucks'];
*/
  $node_title = $activity_info['name'];
  if (strlen($node_title) == 0) {
    $categories = OpenFitActivity::getActivityCategories();
    if (isset($categories[$activity->category_id])) {
      $node_title = $categories[$activity->category_id];
    } else {
      $node_title = t('Other');
    }
  }
  $node->title = $node_title;
}

/**
 * Process a node insert by creating the activity database rows.
 * 
 * @param $node
 *   The node being inserted.
 */
function openfit_activity_insert($node) {  
  // Code copied from pathauto module. We don't allow pathauto to process alias
  // because it doesn't work with language-specific nodes. Instead create an
  // alias explicitly with the undefined language.
  // @todo Remove the next line when http://drupal.org/node/1025870 is fixed.
  unset($node->uri);
  pathauto_node_update_alias($node, 'insert', array('language' => LANGUAGE_NONE));
  $node->path['pathauto'] = 0; // Prevent further pathauto processing.
  
  OpenFitActivity::insertActivityRecords($node);
}

/**
 * Process a node update by updating the activity database rows.
 * 
 * @param $node
 *   The node being updated.
 */
function openfit_activity_update($node) {
  if ($node->type != OpenFitActivity::NODE_TYPE_ACTIVITY) return;
  $activity = $node->openfit_info['activity'];
  if (!isset($activity->activity_id)) return;
  
  db_update('openfit_activity')
    ->fields(array(
      'activity_start' => $activity->activity_start,
      'activity_timezone' => $activity->activity_timezone,
      'activity_duration' => $activity->activity_duration,
      'activity_distance' => $activity->activity_distance,
      'activity_calories' =>  $activity->activity_calories,
      'activity_sufferbucks' =>  $activity->activity_sufferbucks,
	// 'activity_notes' => '',
      'activity_category_id' => $activity->category_id,
    ))
    ->condition('activity_id', $activity->activity_id)
    ->execute();
}


/**
 * Method to return the 4 most recent public workouts.
 */
function openfit_activity_ticker_form() {
  if (variable_get('install_task', '') != 'done') return TRUE; 
  
  $form = array();
  
  // TODO: Horribly, horribly inefficient. Too many queries and joins. Revisit this.
  $query = db_select('node', 'n');
  $query->innerJoin(OpenFitActivity::TABLE_NODE_ACTIVITY_ASSOC, 'na', 'n.nid = na.nid');
  $query->innerJoin(OpenFitActivity::TABLE_ACTIVITY, 'a', 'a.activity_id = na.activity_id');
  $query->fields('n', array('nid'));
  $query->orderBy('activity_start', 'DESC');
  $query->condition('n.status', 1);
  $query->range(0, 4);
  
  $results = $query->execute()->fetchAllAssoc('nid');  
  $nodes = node_load_multiple(array_keys($results));

  if (count($nodes) > 0) {  
    foreach ($nodes as $node) {
      $form[] = node_view($node, 'teaser');  
    }    
    print drupal_render($form);
  }
  return TRUE;
}