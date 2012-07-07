<?php

// TODO:
/*
Import process:

Form submit of import file
1) Validation
  a) Is the files session variable empty?
  b) Is there a file upload error?
  c) Is the file an invalid file extension?

2) Data import
  a) Find the appropriate handlers for this extension (module hook)
  b) If multiple prompt the user for the import type << Client-side??
  c) Call the selected importer for the file extension/type
  d) Importer returns the data import array which may contain entities (activities, athlete entries, categories, routes, equipment, metadata, etc)

3) Process imported data
  a) Fill any missing data tracks, for example distance.
  b) Fill any missing summary info, for example total distance, calories, elevation gain/loss, etc
  c) Hook the modules to prefill any data

4) Import the data
  a) For each entity type, create the appropriate: content nodes, DB records, files
  b) Determine a status message to display based on imported data
  c) Determine where to redirect to display based on imported data
  d) Hook the modules to import more data

5) Create any summarization data, totals, top-scores, notifications, etc
  a) Totals
  b) Push to other services
  c) Hook the modules
*/

class OpenFitDatafileImportHandler {

  function __construct($file_id) {
    $this->fileId = $file_id;
  }
  
  private function normalizeImportResults(&$import_results) {
    // Create metadata.
    if (!isset($import_results['metadata'])) $import_results['metadata'] = array();
    if (!isset($import_results['metadata']['file'])) $import_results['metadata']['file'] = array();
    // Set the filename to the original source name.
    $import_results['metadata']['file']['name'] = $this->fileName;    
    
    if (isset($import_results['activities']) && is_array($import_results['activities'])) {
    
      foreach ($import_results['activities'] as &$activity) {
        // Attempt to figure out category for activities which have not specified it.
        // TODO: Improve this logic by looking at categories in database including localized names,
        // with a fallback to average speed
        if (!isset($activity['category'])) {
          $find_name = isset($activity['name']) ? $activity['name'] : null;
          $find_name = isset($activity['sport_type']) ? $activity['sport_type'] : $find_name;
          if (isset($find_name)) {;
            $find_name = strtolower($find_name);
            
            foreach ($this->categoryInfo as $category => $names) {
              foreach ($names as $name_match) {
                if (!(strpos($find_name,$name_match) === false)) {
                  $activity['category'] = $category;
                  break;
                }
              }
              if (isset($activity['category'])) break;
            }
          }
        }
      }
    }
  }
  
  public function processFileUpload() {
    // Check for any file upload errors and determine which importer to use based on file extension.
    $this->readFileInfo();
    if (!$this->checkFileUploadError()) return FALSE;
    if (!$this->setFileImporter()) return FALSE;
    
    // Use the importer to read the actual file and return the import results in a common format.
    $import_results = null;
    if (!$this->invokeFileImporter($import_results)) return FALSE;

    $this->normalizeImportResults($import_results);
    
    global $user;
    
    // TODO: Handle other kinds of data (weight measurement, route, etc) and missing scenarios, etc.
    if (!isset($import_results['activities']) || !is_array($import_results['activities']) || count($import_results['activities']) == 0) {
      drupal_set_message(t('No activities found in import file'));
    } else {
      $created_nodes = OpenFitActivity::createActivityNodes($import_results['activities']);
      if (count($created_nodes) == 1) {
        drupal_set_message(t('Activity imported.'));
        drupal_goto(drupal_get_path_alias('node/' . $created_nodes[0]->nid));
      } else {
        drupal_set_message(t('%count% activities imported.', array('%count%' => count($created_nodes))));
        drupal_goto('<front>');
      }
    }
    return TRUE;
  }
  
  public function getErrorText() { return $this->errorText; }

  /**
   * Read the file info from the $_FILES variable.
   **/
  private function readFileInfo() {
    $files = $_FILES['files'];
    $this->fileName = isset($files['name'][$this->fileId]) ? $files['name'][$this->fileId] : '';
    $this->fileError = isset($files['error'][$this->fileId]) ? $files['error'][$this->fileId] : null;
    $this->fileLocation = isset($files['tmp_name'][$this->fileId]) ? $files['tmp_name'][$this->fileId] : '';
  }
  
  /**
   * Check that the file uploaded ok. Set the errorText if it fails.
   *
   * @return
   *   TRUE if the file uploaded. FALSE otherwise.
   **/
  private function checkFileUploadError() {
    // Check if a file was selected.
    if (empty($this->fileName)) {
      $this->errorText = t('Select a file to import.');
      return FALSE;
    }
    // General error checking. Pulled from file_save_upload (somewhat).
    switch ($this->fileError) {
      case UPLOAD_ERR_INI_SIZE:
      case UPLOAD_ERR_FORM_SIZE:
        $this->errorText = t('The file %file could not be imported, because it exceeds %maxsize, the maximum allowed size for uploads.',
          array('%file' => $this->fileName, '%maxsize' => format_size(file_upload_max_size())));
        return FALSE;

      case UPLOAD_ERR_PARTIAL:
      case UPLOAD_ERR_NO_FILE:
        $this->errorText = t('The file %file could not be imported, because the upload did not complete.', array('%file' => $this->fileName));
        return FALSE;

      case UPLOAD_ERR_OK:
        // Final check that this is a valid upload, if it isn't, use the
        // default error handler.
        if (is_uploaded_file($this->fileLocation)) {
          break;
        }

        // Unknown error
      default:
        $this->errorText = t('The file %file could not be imported. An unknown error has occurred.', array('%file' => $this->fileName));
        return FALSE;
    }
    return TRUE;
  }

  /**
   * Use the uploaded file extension to set the file importer. Set the errorText if it fails.
   *
   * @return
   *   TRUE if an importer is found for this extension. FALSE otherwise.
   **/
  private function setFileImporter() {
    // TODO: Handle the scenario of multiple file importers per extension here, or possibly on form validate. Not sure.
    // If there are more than on handler defined per extension we need to prompt the user which to use. For example, CSV files, XML files.
    // The list should first be culled to only valid import handlers by calling the valid_callback method for each importer if > 1
    $file_extension = strtolower(pathinfo($this->fileName, PATHINFO_EXTENSION));
    
    $importers = self::getAllFileImporterInfos();
    foreach ($importers as $importer) {
      if (strtolower($importer['extension']) == $file_extension) {
        $this->fileImporterInfo = $importer;
        return TRUE;
      }
    }
    
    $this->errorText = t('The file %file could not be imported because the file type was not recognized.', 
      array('%file' => $this->fileName));
    return FALSE;
  }
  
  /**
   * Return a list of file import routines.
   *
   * @return
   *   An array where element has the following values:
   *   - extension: The file extension
   *   - file: An optional PHP file to include
   *   - callback: The callback method to import the file
   **/
  public static function getAllFileImporterInfos() {
    // Standard "built-in" importers
    $file_importers = array(
      array(
        'extension' => 'fit', 
        'module' => 'openfit_api', 
        'file' => 'openfit_api.DatafileImportHandler.inc', 
        'callback' => 'openfit_api_import_file_fit'
      ),
      array(
        'extension' => 'gpx', 
        'module' => 'openfit_api', 
        'file' => 'openfit_api.DatafileImportHandler.inc', 
        'callback' => 'openfit_api_import_file_gpx'
      ),
    );
    // Module hook importers
    $hook = 'openfit_file_importer_info';
    $implementors = module_implements($hook);
    foreach ($implementors as $module) {
      $importers = module_invoke($module, $hook);
      foreach ($importers as &$importer) {
        $importer['module'] = $module;
        $file_importers[] = $importer;
      }
    }

    return $file_importers;
  }

  private function invokeFileImporter(&$import_results) { 
    $import_results = null;
    $importer_module = $this->fileImporterInfo['module'];
    $importer_file = isset($this->fileImporterInfo['file']) ? $this->fileImporterInfo['file'] : $importer_module . '.module';
    $importer_callback = $this->fileImporterInfo['callback'];
    
    module_load_include(pathinfo($importer_file, PATHINFO_EXTENSION), $importer_module, pathinfo($importer_file, PATHINFO_FILENAME));
    try {
      if (!function_exists($importer_callback)) throw new Exception(t('Could not load import function \'%function%\'.', array('%function%' => $importer_callback)));
      $import_results = call_user_func($importer_callback, $this->fileLocation);
    } catch (Exception $e) { 
      $this->errorText = t('Problem importing file: ') . $e->getMessage();
      return FALSE;
    }
    
    if (!isset($import_results)) {
      $this->errorText = t('Problem importing file: No import results');
      return FALSE;
    }
    return TRUE;
  }
  
  private $errorText = null; // Text if an error occurred.
  private $fileName = ''; // The original uploaded file name (excluding path, including extension).
  private $fileError = UPLOAD_ERR_OK; // The file upload error status.
  private $fileLocation = ''; // The full path location of the temporary uploaded file.
  private $fileImporterInfo = null; // Information about the file importer for the uploaded file.  
  
  private $categoryInfo = array(
    // TODO: I18N support for other categories.
    'Running' => array('treadmill','running','run','córrer','course à pied','corsa','corrida','běh','beh','bieg','lěh','laufen','løping','löpning','juoksu','ランニング','달리기','hardlopen','tek','跑','跑步',),
    'Cycling' => array('bike','cycling','biking','ciclisme','ciclismo','cyklistika','cykling','sykling','zyklus','pyöräily','vélo','自転車','사이클링','fietsen','kolarstwo','kolesarjenje','骑自行车','自行車',),
    'Swimming' => array('swim'),
    'Walking' => array('walk'),
    'Hiking' => array('hike','hiking'),
    'Skiing' => array('ski'),
    'Rowing' => array('row','kayak','canoe'),
    'Skating' => array('skate','skating'),
  );
}

function openfit_api_import_file_fit($filename) {
  module_load_include('inc', 'openfit_api', 'openfit_api.FileImporter.Fit');
  $importer = new FitFileImporter();
  return $importer->importFile($filename);
}

function openfit_api_import_file_gpx($filename) {
  module_load_include('inc', 'openfit_api', 'openfit_api.FileImporter.Gpx');
  $importer = new GpxFileImporter();
  return $importer->importFile($filename);
}
