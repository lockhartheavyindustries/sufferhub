<?php

/**
 * Return a list of file importers.
 *
 * File import is performed in two steps:
 * 1) After a file is selected the valid_callback function is called with $filename.
 *   If valid_callback is not specified or the callback returns TRUE processing continues.
 *   If the importer is assigned to a unique file extension return TRUE, otherwise
 *   if a common extension (such as XML) is used, open the file and parse enough to 
 *   determine if the file is something your importer can handle (perhaps looking a the
 *   schema tags or document element name)
 * 2) The first importer which matches valid is called with the $filename again to 
 *   perform the full import and must return an array of import results for each
 *   type of data to be imported (activities, etc).
 *
 * @return
 * An array of file importers.
 * Each element of the array consists of an array with the following properties:
 *   - extension: The file extension (eg. 'gpx')
 *   - name: Optional short name used in import menus. Defaults to extension.
 *   - desc: Optional descriptive text of the file extension type.
 *   - valid_file: Optional file to find the valid_callback in. Defaults to modulename.module
 *   - valid_callback: Optional function name to validate whether the file is valid. Return boolean.
 *   - import_file: Optional file to find the import_callback in. Defaults to modulename.module
 *   - import_callback: Function name to import the file. Return import results array.
 *
 */
function hook_openfit_file_importer_info() {
  return array();
}