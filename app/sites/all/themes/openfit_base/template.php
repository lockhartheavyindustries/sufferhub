<?php

function openfit_base_preprocess_html(&$vars) {

  // Load the media queries styles
  // If you change the names of these files they must match here - these files are
  // in the /css/ directory of your subtheme - the names must be identical!
  $media_queries_css = array(
    'openfit_base.responsive.style.css',
    'openfit_base.responsive.gpanels.css'
  );
  load_subtheme_media_queries($media_queries_css, 'openfit_base');

 /**
  * Load IE specific stylesheets
  * AT automates adding IE stylesheets, simply add to the array using
  * the conditional comment as the key and the stylesheet name as the value.
  *
  * See our online help: http://adaptivethemes.com/documentation/working-with-internet-explorer
  *
  * For example to add a stylesheet for IE8 only use:
  *
  *  'IE 8' => 'ie-8.css',
  *
  * Your IE CSS file must be in the /css/ directory in your subtheme.
  */
  
  $ie_files = array(
    'lte IE 7' => 'ie-lte-7.css',
  );
  load_subtheme_ie_styles($ie_files, 'openfit_base');
}

/**
 * Returns HTML for a sort icon.
 *
 * @param $vars
 *   An associative array containing:
 *   - style: Set to either 'asc' or 'desc', this determines which icon to show.
 */
function openfit_base_tablesort_indicator($vars) {
  // Use custom arrow images.
  if ($vars['style'] == 'asc') {
    return theme('image', array('path' => path_to_theme() . '/images/tablesort-ascending.png', 'alt' => t('sort ascending'), 'title' => t('sort ascending')));
  }
  else {
    return theme('image', array('path' => path_to_theme() . '/images/tablesort-descending.png', 'alt' => t('sort descending'), 'title' => t('sort descending')));
  }
}

/**
 * Add the view mode template to theme hook suggestions for our types
 */
function openfit_base_preprocess_node(&$variables) {
  $openfit_node_types = array(
    'activity' => TRUE,
  );
  if (isset($openfit_node_types[$variables['type']])) {
    $variables['theme_hook_suggestions'][] = 'node__' . $variables['type'] . '__' . $variables['view_mode'];
  }
}

/**
 * Show the athlete's full name instead of username
 */
function openfit_base_preprocess_username(&$variables) {
  $name = OpenFitUserSetting::get($variables['uid'], OpenFitUserSetting::TYPE_FULLNAME, $variables['name']);
  if (isset($name) && strlen($name) > 0) $variables['name'] = $name;
}

function openfit_base_preprocess_comment(&$variables) {
  $openfit_node_types = array(
    'activity' => TRUE,
  );
  if (!isset($variables['node']) || !isset($openfit_node_types[$variables['node']->type])) return;
  
  $comment = $variables['comment'];
  
  // Remove the standard comment links: reply, edit, delete
  unset($variables['content']['links']['comment']['#links']);
  
  // Add a delete menu if the user posted the comment or is admin
  global $user;
  if (user_access('administer comments') || $user->uid == $comment->uid) {
    $url = drupal_get_path_alias('node/' . $variables['node']->nid) . '/comments';
    $variables['content']['links']['comment']['#links'] = array(
        'comment-delete' => array(
          'title' => '&nbsp;',
          'href' => 'comment/' . $comment->cid . '/delete',
          'query' => array('destination' => $url),
          'html' => TRUE,
        ),
    );
  }
  
  // Display "XX ago" for posts less than 1 day, otherwise use locale to format datetime
  $ago =  time() - $comment->created;
  if ($ago < 86400) {
    $variables['created'] = t('!interval ago', array('!interval' => format_interval(time() - $comment->created)));
  } else {
    $locale = OpenFitUserSetting::getCurrentUserLocale();
    $fmt = new IntlDateFormatter($locale, IntlDateFormatter::SHORT, IntlDateFormatter::SHORT);
    $created = new DateTime('now');
    $created->setTimestamp($comment->created);
    $variables['created'] = $fmt->format($created);
  }
  
  $variables['submitted'] = $variables['author'] . '&nbsp;' . 
    '<time datetime="' . $variables['datetime'] . '" pubdate="pubdate">' . $variables['created'] . '</time>';
}