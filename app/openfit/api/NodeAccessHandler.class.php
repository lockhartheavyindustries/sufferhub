<?php 
class NodeAccessHandler {
  
  public static function setNodeValue() {
    // Gather URL parameters.
    $nid = isset($_GET['nid']) ? intval($_GET['nid']) : null;
    $field = isset($_GET['f']) ? $_GET['f'] : null;
    $value = isset($_GET['v']) ? $_GET['v'] : null;
    
    // Validate parameters and coerce types as needed.
    switch ($field) {
      case 'sharing':
        switch ($value) {
          case 'public': $value = 1; break;
          case 'private': $value = 0; break;
          default: $value = null; break;
        }
        break;
      default: $field = null; break;
    }
    if (!isset($nid) || !isset($field) || !isset($value)) throw new Exception(t('Invalid method parameters.'));
    
    // Load the node and perform update access check.
    require_once DRUPAL_ROOT . '/includes/common.inc';
    module_load_include('module', 'node');
    module_load_include('inc', 'field', 'field.attach');
    module_load_include('module', 'user');
    
    $node = node_load($nid);
    if (!node_access('update', $node)) throw new Exception('User does not have access to nid ' . $nid, 403);
  
    // Set the appropriate node fields.
    switch ($field) {
      case 'sharing':
        $node->status = $value;
        break;
    }
    // Save the node.
    node_save($node);
    
    die('ok');
  }
}
?>
