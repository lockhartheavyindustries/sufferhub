<?php 

class OpenFitDataTable {
  
  /**
   * Construct a header array that can be passed to the theme_table() method.
   *
   * Translates column data information and visible columns into header info. Specifies the sort column and direction.
   *
   * @param $field_info
   *   An array of information about the fields in the table.
   * @param $columns
   *   An array of column ids that appear in the $field_info param.
   * @param $sort_column
   *   An optional parameter indicating the column to sort on. If specified, $sort_direction must also be specified.
   * @param $sort_direction
   *   An optional parameter of ASC or DESC indicating the sort direction.
   */
  public static function getTableHeader($field_info, $columns, $sort_column = null, $sort_direction = null) {
    $is_sortable = isset($sort_column) && isset($field_info[$sort_column]);
    $header = array();
    foreach ($columns as $field_id) {
      if (isset($field_info[$field_id])) {
        $header[$field_id] = array('data' => $field_info[$field_id]['title']);
        if ($is_sortable) $header[$field_id]['field'] = $field_id;
        if (isset($field_info[$field_id]['data_field'])) $header[$field_id]['query_field'] = $field_info[$field_id]['data_field'];
        if (isset($field_info[$field_id]['sort_reverse'])) $header[$field_id]['query_reverse'] = $field_info[$field_id]['sort_reverse'];
        if (isset($field_info[$field_id]['data_type']) && isset(self::$dataTypeCellClasses[$field_info[$field_id]['data_type']])) {
          $header[$field_id]['class'] = self::$dataTypeCellClasses[$field_info[$field_id]['data_type']];
        }
      }
    }
    if ($is_sortable) $header[$sort_column]['sort'] = $sort_direction;
    return $header;
  }

  /**
   * Construct an array of table rows that can be passed to the theme_table() method.
   *
   * Iterates over $query_results using fetchAssoc() and outputs each column using
   * the properties from $field_info such as data_field, localize and format_function.
   *
   * @param $field_info
   *   An array of information about the fields in the table.
   * @param $columns
   *   An array of column ids that appear in the $field_info param.
   * @param $query_results
   *   A DatabaseStatementInterface (returned from db_select()->execute() for example) or an array of stdClass rows.
   */
  public static function getTableRows($field_info, $columns, $query_results) {  
    $table_rows = array();
    if (!$query_results) return $table_rows;

    $primary_key_field = null;    
    foreach ($field_info as $fid => $info) {
      if (isset($info['primary_key']) && $info['primary_key']) {
        $primary_key_field = $fid;
        break;
      }
    }
    
    $rows = array();
    if (is_array($query_results)) {
      $rows = $query_results;
    } else {
      while ($row = $query_results->fetchAssoc()) $rows[] = (object)$row;
    }
    
    foreach ($rows as $row) {
      $row_id = null;
      if (isset($primary_key_field) && isset($row->$primary_key_field)) $row_id = $row->$primary_key_field;
      
      $table_row = array();
      foreach ($columns as $field_id) {
        if (isset($field_info[$field_id])) {
          $value = null;
          $info = $field_info[$field_id];
          $data_field = $field_id;
          if (isset($info['data_field'])) $data_field = $info['data_field'];
          if (isset($row->$data_field)) $value = $row->$data_field;
          if (isset($value) && isset($info['localize']) && $info['localize']) $value = t($value);
          if (isset($info['format_function'])) {
            $function = $info['format_function'];
            if (function_exists($function)) {
              $value = call_user_func($function, $row, $field_id, $info, $value);
            } else if (isset($info['format_class']) && method_exists($info['format_class'], $info['format_function'])) {
              $value = call_user_func(array($info['format_class'], $info['format_function']), $row, $field_id, $info, $value);
            }
          }
          if (!isset($value)) $value = '';
          
          $classes = null;
          if (isset($field_info[$field_id]['data_type']) && isset(self::$dataTypeCellClasses[$field_info[$field_id]['data_type']])) {
            $classes = self::$dataTypeCellClasses[$field_info[$field_id]['data_type']];
          }
          
          $table_row[$field_id] = array('data' => $value);
          if (isset($classes)) $table_row[$field_id]['class'] = $classes;
        }
      }
      if (isset($row_id)) {
        $table_rows[$row_id] = $table_row;
      } else {
        $table_rows[] = $table_row;
      }
    }
    return $table_rows;
  }
  
  private static $dataTypeCellClasses = array(
    OpenFitDataField::DATA_TYPE_NUMERIC => array('field-type-number'),
    OpenFitDataField::DATA_TYPE_DISTANCE => array('field-type-number', 'field-type-distance'),
    OpenFitDataField::DATA_TYPE_TIME => array('field-type-number', 'field-type-time'),
    OpenFitDataField::DATA_TYPE_SPEED => array('field-type-number', 'field-type-speed'),
    OpenFitDataField::DATA_TYPE_PACE => array('field-type-number', 'field-type-pace'),
    OpenFitDataField::DATA_TYPE_DATE => array('field-type-date'),
  );
  
}
