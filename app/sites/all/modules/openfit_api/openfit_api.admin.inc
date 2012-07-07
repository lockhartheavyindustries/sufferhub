<?php

function openfit_api_general_admin_form($form, $form_state) {
  include_once DRUPAL_ROOT . '/includes/locale.inc';
  
  $countries = country_get_list();
  $zones = system_time_zones();
  $measurement_systems = OpenFitMeasurement::getMeasurementSystems();
  
  $default_country = variable_get('site_default_country', '');
  $default_timezone = variable_get('date_default_timezone', date_default_timezone_get());
  $default_week_start = variable_get('date_first_day', 0);
  
  $default_units_system = variable_get('openfit_measurement_system', 'metric');
  $gmaps_api_key = variable_get('openfit_google_maps_key', '');
  
  $units_options = array();
  foreach ($measurement_systems as $id => $system) {
    $units_options[$id] = t($system->title);
  }
  
  $form['general'] = array(
    '#type' => 'fieldset',
    '#title' => t('General'),
  );

  if (module_exists('locale')) {
    $languages =  locale_language_list('name', TRUE);
    $language_value = language_from_default();
    if (count($languages) > 1) {
      $native_languages = locale_language_list('native', TRUE);
      foreach ($native_languages as $id => $text) {
        if ($id != $language_value) {
          $language = $languages[$id];
          if (isset($language)) $languages[$id] = $language . ' (' . $text . ')';
        }
      }
      $language_options = array();
      if ($language_value == '') $language_options[''] = t('- None -');
      $language_options = array_merge($language_options, $languages);
      $form['general'][OpenFitUserSetting::TYPE_LANGUAGE] = array(
        '#type' => 'select',
        '#title' => t('Default Language'),
        '#default_value' => $language_value,
        '#options' => $language_options,
      );
    }
  }
  
  $form['general'][OpenFitUserSetting::TYPE_COUNTRY] = array(
    '#type' => 'select',
    '#title' => t('Default country'),
    '#empty_value' => '',
    '#default_value' => $default_country,
    '#options' => $countries,
    '#attributes' => array('class' => array('country-detect')),
  );

  $form['general'][OpenFitUserSetting::TYPE_TIMEZONE] = array(
    '#type' => 'select',
    '#title' => t('Default time zone'),
    '#default_value' => $default_timezone,
    '#options' => $zones,
  );
  
  $form['general'][OpenFitUserSetting::TYPE_WEEK_START] = array(
    '#type' => 'select',
    '#title' => t('Default first day of week'),
    '#default_value' => $default_week_start,
    '#options' => array(0 => t('Sunday'), 1 => t('Monday'), 2 => t('Tuesday'), 3 => t('Wednesday'), 4 => t('Thursday'), 5 => t('Friday'), 6 => t('Saturday')),
  );
  
  $form['general']['openfit_measurement_system'] = array(
    '#type' => 'select',
    '#title' => t('Default unit system'),
    '#default_value' => $default_units_system,
    '#options' => $units_options,
  );
  
  $form['general']['openfit_google_maps_key'] = array(
    '#type' => 'textfield',
    '#title' => 'Google Maps API key',
    '#maxlength' => 100,
    '#default_value' => $gmaps_api_key,
  );
  
  $form['general']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

function openfit_api_general_admin_form_submit($form, $form_state) {
  foreach($form_state['values'] as $key => $value) {
    $var_name = null;
    switch ($key) {
      case 'openfit_measurement_system':
      case 'openfit_google_maps_key':
        if (empty($value)) continue;
        $var_name = $key;
      break;
      case OpenFitUserSetting::TYPE_LANGUAGE:
        $var_name = 'language_default';
        $languages = language_list();
        $value = $languages[$value];

        // Changing the language settings impacts the interface.
        cache_clear_all('*', 'cache_page', TRUE);
        module_invoke_all('multilingual_settings_changed');
      break;
      case OpenFitUserSetting::TYPE_COUNTRY:
        $var_name = 'site_default_country';
      break;
      case OpenFitUserSetting::TYPE_TIMEZONE:
        $var_name = 'date_default_timezone';
      break;
      case OpenFitUserSetting::TYPE_WEEK_START:
         $var_name = 'date_first_day';
      break;
      default:
        continue;
    }
    if (isset($var_name)) variable_set($var_name, $value);
  }
}
