<?php

/**
 * Method to construct the activities form.
 */
function openfit_activity_activities_form($form, &$form_state) {
  drupal_page_is_cacheable(FALSE);
  global $user;
  
  // TODO: Read from user preference
  $rows_per_page = 25;
  
  // TODO: Read from user preference
  $columns = array('name', 'start', 'sport', 'distance', 'time', 'avg-speed', 'avg-pace', 'calories');
  $sort_column = 'start';
  $sort_direction = 'desc';
  
  $field_info = OpenFitActivity::getTableFieldInfo();
  $field_info['name']['link'] = TRUE;
  $field_info['name']['link_class'] = 'view-activity';

  $header = OpenFitDataTable::getTableHeader($field_info, $columns, $sort_column, $sort_direction);
  $activities = OpenFitActivity::getActivities($user->uid, null, $header, $rows_per_page);
  $table_rows = OpenFitDataTable::getTableRows($field_info, $columns, $activities);
  $table_class = array('activity-table');
  $table_class[] = $table_rows > 0 ? 'data-table' : 'data-table-empty';

  drupal_add_library('system', 'ui');
  drupal_add_library('system', 'ui.dialog');
  $form['#attached']['css'][] = drupal_get_path('module', 'openfit_activity') . '/openfit_activity.css';
  $form['#attached']['js'][] = drupal_get_path('module', 'openfit_api') . '/js/jquery.table.js';
  $form['#attached']['js'][] = drupal_get_path('module', 'openfit_activity') . '/js/view.activities.js';
  
  $form['actions'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('clearfix')),
    
    'hidden-action-type' => array(
      '#type' => 'container',
      '#attributes' => array('style' => 'display:none'),
      'action-type' => array(
        '#type' => 'select',
        '#default_value' => 'edit',
        '#options' => array('edit' => t('Edit'), 'delete' => t('Delete'), 'view' => t('View')),
      ),
    )
  );
  
  $items = array(
    'add' => array(
      '#attributes' => array('id' => 'add-button'),
      '#left_image' => TRUE,
      '#disabled' => !user_access('create activity content'),
      '#text' => t('Add'),
      '#url' => drupal_get_path_alias('node/add/activity'),    
    ),
  );
  if (count($table_rows) > 0) {
    $items['edit'] = array(
      '#attributes' => array('id' => 'edit-button'),
      '#left_image' => TRUE,
      //'#disabled' => !can_edit_activity($node->nid),
      '#text' => t('Edit'),
      '#url' => '#',
    );
    // TODO: Open a menu allowing user to select privacy settings
    /*
    $items['sharing'] = array(
      '#attributes' => array('id' => 'sharing-button'),
      '#left_image' => TRUE,
      '#right_image' => TRUE,
      //'#disabled' => !can_edit_activity($node->nid),
      '#text' => t('Sharing'),
      '#url' => '#',
    );
    */
    $items['delete'] = array(
      '#attributes' => array('id' => 'delete-button'),
      '#left_image' => TRUE,
      //'#disabled' => !can_edit_activity($node->nid),
      '#text' => t('Delete'),
      '#url' => 'activity/delete',
    );
  }

  $form['actions']['left-action-container'] = array(
    '#type' => 'container',
    'items' => theme('commandbar', array(
      'items' => $items,
    )),
  );
  
  $form['actions']['right-action-container'] = array(
    '#type' => 'container',
    
    'pager' => array(
      '#type' => 'container',
      'small-pager' => array(
        '#type' => 'small_pager',
        // TODO: For testing pager, remove in production
        '#options' => array('hide_empty' => FALSE),
      ),
    ),    

    'right-actions' => array(
      '#type' => 'container',
      // TODO: Open a popup to set options for this list
      /*
      'settings' => theme('commandbar', array(
        'items' => array(
          'settings' => array(
            '#attributes' => array('id' => 'settings-button'),
            '#left_image' => TRUE,
            '#url' => '#',
            '#url_options' => array('attributes' => array('title' => t('Settings'))),
          ),        
        ),
      )),
      */
    ),    
  );
  
  $form['activity-table'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $table_rows,
    '#empty' => t('No activities'),
    '#attributes' => array('class' => $table_class, 'id' => 'activity-table'),
  );
  
  $delete_confirm_markup = '<span class="ui-icon ui-icon-alert" style="float:left;"></span>';
  $delete_confirm_markup .= '<span>' . t('Are you sure you want to delete the selected items?') . '</span>';
  $form['delete-confirm'] = array(
    '#type' => 'container',
    '#attributes' => array('id' => 'delete-confirm', 'title' => t('Confirm delete'), 'style' => 'display:none'),
    'text' => array(
      '#markup' => $delete_confirm_markup,
    ),
  );
  
  // We need a hidden submit button for the actions 
  $form['submit'] = array('#type' => 'submit', '#attributes' => array('style' => 'display: none;'));

  return $form;
}

/**
 * Form processing function.
 */
function openfit_activity_activities_form_submit($form, &$form_state) {
  switch ($form_state['values']['action-type']) {
    case 'delete':
      $nids = array();
      foreach ($form_state['values']['activity-table'] as $key => $value) {
        if ($value) $nids[] = $key;
      }
      OpenFitActivity::deleteActivities($nids);
      break;
  }   
}

