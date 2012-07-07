<?php
require_once('openfit_api.ActivityDataTrack.inc');

class GpxFileImporter {
  public function _construct() {
  }

  public function importFile($filename) {
    $this->locationDataFormat = ActivityDataTrackAccess::getDataFormat(ActivityDataTrackAccess::LOCATION);
    $this->elevationDataFormat = ActivityDataTrackAccess::getDataFormat(ActivityDataTrackAccess::ELEVATION);
    
    $import_results = array();
    // Set the import metadata.
    $import_results['metadata'] = array(
      'type' => 'file',
      'file' => array('name' => $filename),
    );

    $this->readFile($filename);
    
    $import_results['activities'] = $this->activities;
    
    return $import_results;
  }
  
  private function readFile($filename) {
    $this->reader = new XMLReader();  
    if (!$this->reader->open($filename)) return;
    
    while($this->reader->read()) { 
      if ($this->reader->nodeType == XmlReader::WHITESPACE || $this->reader->nodeType == XmlReader::SIGNIFICANT_WHITESPACE) continue;
      
      switch ($this->reader->nodeType) {
        case XmlReader::ELEMENT:
          $this->startElement();
          if ($this->reader->isEmptyElement) $this->endElement();  
          break;
          
        case XmlReader::END_ELEMENT:
          if ($this->elements[count($this->elements)-1] != $this->reader->localName) throw new Exception('Unexpected element: ' . $this->reader->localName);
          $this->endElement();
          break;
          
        case XmlReader::TEXT:
          if ($this->reader->hasValue) $this->characters();
          break;
      }
    }
    $this->reader->close(); 
    $this->reader = null;
  }
  
  private function startElement() {
    $this->parent = $this->element;
    $this->element = $this->reader->localName;
    $this->elements[] = $this->element;
    
    switch ($this->element) {
      case 'trk':
        $this->createActivity();
        break;
      case 'trkpt':
        if ($this->parent == 'trkseg') {
          $this->trackPoint = array();
          $this->trackPoint['lat'] = floatval($this->reader->getAttribute('lat'));
          $this->trackPoint['lon'] = floatval($this->reader->getAttribute('lon'));
        }
        break;
    }
  }
  
  private function endElement() {
    switch ($this->element) {
      case 'trk':
        if (isset($this->activity['start_time'])) {
          $this->activity['start_time'] = $this->activity['start_time']->format('Y-m-d H:i:s');
          if (isset($this->locationTrack)) {
            $this->locationTrack['start_time'] = $this->locationTrack['start_time']->format('Y-m-d H:i:s');
            $this->activity['data_tracks'][ActivityDataTrackAccess::LOCATION] = $this->locationTrack;
          }
          if (isset($this->elevationTrack)) {
            $this->elevationTrack['start_time'] = $this->elevationTrack['start_time']->format('Y-m-d H:i:s');
            $this->activity['data_tracks'][ActivityDataTrackAccess::ELEVATION] = $this->elevationTrack;
          }
          $this->activities[] = $this->activity;
        }
        $this->activity = null;
        break;
      case 'trkseg':
        $this->segmentEndTime = $this->lastPointTime;
        break;
      case 'trkpt':
        if ($this->parent == 'trkseg') {
          if (isset($this->trackPoint['time'])) {
            if (isset($this->segmentEndTime)) {
              $from = $this->segmentEndTime->diff($this->activity['start_time'], true);
              $from = (($from->days * 24 + $from->h) * 60 + $from->i) * 60 + $from->s;
              $to = $this->trackPoint['time']->diff($this->activity['start_time'], true);
              $to = (($to->days * 24 + $to->h) * 60 + $to->i) * 60 + $to->s;
              $this->activity['timer_stops'][] = array('from' => $from, 'to' => $to);
            }
            $this->segmentEndTime = null;
            if (!isset($this->activity['start_time'])) $this->activity['start_time'] = $this->trackPoint['time'];
            
            if (!isset($this->locationTrack)) {
              $this->locationTrack = array('start_time' => $this->trackPoint['time'],'data' => '');
            }
            $offset = $this->trackPoint['time']->diff($this->locationTrack['start_time'], true);
            $offset = (($offset->days * 24 + $offset->h) * 60 + $offset->i) * 60 + $offset->s;
            $this->locationTrack['data'] .= pack('L',$offset) . pack($this->locationDataFormat,$this->trackPoint['lat'],$this->trackPoint['lon']);
            
            if (isset($this->trackPoint['ele'])) {
              if (!isset($this->elevationTrack)) {
                $this->elevationTrack = array('start_time' => $this->trackPoint['time'],'data' => '');
              }
              $offset = $this->trackPoint['time']->diff($this->elevationTrack['start_time'], true);
              $offset = (($offset->days * 24 + $offset->h) * 60 + $offset->i) * 60 + $offset->s;
              $this->elevationTrack['data'] .= pack('L',$offset) . pack($this->elevationDataFormat,$this->trackPoint['ele']);
            }
            $this->lastPointTime = $this->trackPoint['time'];
          }
          $this->trackPoint = array();
        }
        break;
    }
    
    array_pop($this->elements);
    $this->element = $this->parent;
    $this->parent = count($this->elements) > 1 ? $this->elements[count($this->elements)-2] : null;    
  }
  
  private function characters() {
    switch ($this->element) {
      case 'name':
        $this->activity['name'] = $this->reader->value;
        break;
      case 'cmt':
        $this->activity['notes'] = $this->reader->value;
        break;
      case 'ele':
        if ($this->parent == 'trkpt') $this->trackPoint['ele'] = floatval($this->reader->value);
        break;
      case 'time':
        if ($this->parent == 'trkpt') $this->trackPoint['time'] = new DateTime($this->reader->value);
        break;
    }
  }
  
  private function createActivity() {
    $this->activity = array();
    $this->activity['data_tracks'] = array();
    $this->activity['timer_stops'] = array();
    $this->trackPoint = array();
    $this->lastPointTime = null;
    $this->segmentEndTime = null;
    $this->locationTrack = null;
    $this->elevationTrack = null;
  }

  private $locationDataFormat;
  private $elevationDataFormat;
  private $reader = null;
  private $elements = array();
  private $element = null;
  private $parent = null;
  
  private $activity = null;
  private $trackPoint = array();
  private $lastPointTime = null;
  private $segmentEndTime = null;
  private $locationTrack = null;
  private $elevationTrack = null;
  
  private $activities = array();
}
