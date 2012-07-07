<?php

/**
 * Returns HTML for a small query pager.
 *
 * Menu callbacks that display paged query results should call theme('small_pager') to
 * retrieve a pager control so that users can view other results. Format a list
 * of nearby pages with additional query results.
 *
 * @param $variables
 *   An associative array containing:
 *   - element: An optional integer to distinguish between multiple pagers on one page.
 *   - options: Associative array of display options
 *     - hide_empty: If TRUE, hide the entire pager when not needed (default to TRUE)
 *   - attributes: Class attributes for the entire pager.
 *
 * @ingroup themeable
 */
function theme_small_pager($variables = array()) {
  global $pager_page_array, $pager_total, $pager_total_items, $pager_limits;
  $variables += array('attributes' => array(), 'element' => 0,  'options' => array('hide_empty' => TRUE), );
  
  $element = $variables['element'];
  $options = $variables['options'];
  $attributes = $variables['attributes'];
  
  $total_items = $pager_total_items[$element];
  $items_per_page = $pager_limits[$element];
  $current_page = $pager_page_array[$element];
  $last_page = max($pager_total[$element] - 1,0);
  
  $from = min($current_page * $items_per_page + 1, $total_items);
  $to = min($from + $items_per_page - 1, $total_items);
  
  $range_text = '<span class="pager-item-from">' . $from . '</span>';
  if ($from != $to) {
    $range_text .= '-';
    $range_text .= '<span class="pager-item-to">' . $to . '</span>';
  }
  $total_text = '<span class="pager-item-total">' . $total_items . '</span>';
  
  $id_suffix = ($element != 0) ? '-' . $element : '';
  $query = drupal_get_query_parameters($_GET, array('q','page'));
  
  $page_range_of = array(
    'id' => 'page-of' . $id_suffix,
    'data' => t('!range of !total', array('!range' => $range_text, '!total' => $total_text)),
  );
  $pager_prev = array(
    'id' => 'pager-prev' . $id_suffix,
    'class' => array('pager-prev'),
  );
  $pager_next = array(
    'id' => 'pager-next' . $id_suffix,
    'class' => array('pager-next'),
  );
  $pager_prev['data'] = '<div class="img">&nbsp;</div>';
  if ($current_page != 0) {
    $prev_query = ($current_page == 1) ? $query : array_merge($query, array('page' => $current_page - 1));
    $pager_prev['data'] = l($pager_prev['data'], $_GET['q'], array(
      'attributes' => array('title' => t('Previous page')), 'query' => $prev_query, 'html' => TRUE)
    );
  } else {
    $pager_prev['class'] = array_merge($pager_prev['class'], array('disabled'));
  }
  $pager_next['data'] = '<div class="img">&nbsp;</div>';
  if ($current_page != $last_page) {
    $next_query = array_merge($query, array('page' => $current_page + 1));
    $pager_next['data'] = l($pager_next['data'], $_GET['q'], array(
      'attributes' => array('title' => t('Next page')), 'query' => $next_query, 'html' => TRUE)
    );
  } else {
    $pager_next['class'] = array_merge($pager_next['class'], array('disabled'));
  }
  
  $attributes += array('class' => array('buttonbar'));
  if ($options['hide_empty'] == TRUE && $total_items <= $items_per_page) {
    $attributes += array('style' => array('display:none'));
  }
  $pager_next['data'] = '<div class="btn">' . $pager_next['data'] . '</div>';
  $pager_prev['data'] = '<div class="btn">' . $pager_prev['data'] . '</div>';

  $items = array();
  if ($total_items > 0) {
    $items['page-range-of'] = $page_range_of;
  }  
  $items['pager-prev'] = $pager_prev;
  $items['pager-next'] = $pager_next;
  return theme('item_list', array(
      'attributes' => $attributes,
      'items' => $items,
    ));
}

function theme_commandbar($variables = array()) {
  $items = $variables['items'];
  $bar_items = array();
  foreach ($items as $id => $item) {
    if (isset($item['#markup'])) $item += array('#type' => 'markup');
    $item += array('#type' => 'button', '#disabled' => FALSE);
    $bar_item = array('class' => array());
    if ($item['#disabled']) $bar_item['class'][] = 'disabled';
    switch ($item['#type']) {
      case 'button':
        $item += array('#text' => '', '#left_image' => FALSE, '#right_image' => FALSE, '#url' => '');
        $link_text = htmlentities($item['#text'], ENT_COMPAT, 'UTF-8');
        $text_classes = array();
        if (strlen($link_text) > 0) {
          if ($item['#left_image']) $text_classes[]= 'btnleft';
          if ($item['#right_image']) $text_classes[] = 'btnright';
        }
        $link_text = '<span class="' . implode($text_classes, ' ') . '">' . $link_text . '</span>';
        if ($item['#left_image']) $link_text = '<div class="img">&nbsp;</div>' . $link_text;
        if ($item['#right_image']) $link_text .= '<div class="img right">&nbsp;</div>';
        if ($item['#disabled']) {
          $bar_item['data'] = $link_text;
        } else {
          $options = array('html' => TRUE);
          if (isset($item['#url_options'])) $options = array_merge($options, $item['#url_options']);
          $bar_item['data'] = l($link_text, $item['#url'], $options);
        }
        $bar_item['data'] = '<div class="btn">' . $bar_item['data'] . '</div>';
        if (isset($item['#menu'])) {
          $menu_id = $id . '-menu';
          $bar_item['data'] .= '<div id="' . $menu_id . '" class="menu" style="display:none">' . $item['#menu']['#markup'] . '</div>';
        }
        break;
      case 'markup':
        $item += array('#markup' => '');
        $bar_item['data'] = $item['#markup'];
        break;
    }
    if (isset($item['#attributes'])) {
      if (isset($item['#attributes']['id'])) $bar_item['id'] = $item['#attributes']['id'];
      if (isset($item['#attributes']['class'])) $bar_item['class'] = array_merge($bar_item['class'], $item['#attributes']['class']);
    }
    $bar_items[$id] = $bar_item;
  }
  return array(
    '#theme' => 'item_list',
    '#attributes' => array('class' => array('buttonbar'),),
    '#items' => $bar_items,
  );
}

function theme_openfit_timeofday($variables = array('element' => array())) {
  $element = $variables['element'];
  $element += array('#autocomplete_path' => FALSE, '#input' => TRUE,  '#maxlength' => 20, '#size' => 60,);
  return theme('textfield', array('element' => $element));
}

function theme_openfit_duration($variables = array('element' => array())) {
  $element = $variables['element'];
  $element += array('#autocomplete_path' => FALSE, '#input' => TRUE,  '#maxlength' => 20, '#size' => 60,);
  return theme('textfield', array('element' => $element));
}

function theme_openfit_measurement($variables = array('element' => array())) {
  $element = $variables['element'];
  $element += array('#autocomplete_path' => FALSE, '#input' => TRUE,  '#maxlength' => 20, '#size' => 60,);
  return theme('textfield', array('element' => $element));
}
