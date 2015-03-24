<?php

/**
 * @file
 * 
 * This file should contain all Drupal hooks for theming content.  For templates 
 * that need specific the hook_preprocess functions should be included here
 *
 */

/**
 * implementation of hook_preprocess_HOOK()
 * 
 * Used to alter or add to theme variables.  The variables are passed into
 * templates when prossing.  This function organizes the relationships
 * into more simple structures for parsing in the template file.
 *
 * @ingroup tripal_file
 */
function tripal_file_preprocess_tripal_file_relationships(&$variables) {
  // EXPLANATION:  If you have implmented a new chado node type and the record
  // that belongs to the node has a corresponding xxxx_relationship table
  // this this function can be used to provide relationships to the template
  // in a format that is easier to parse.  This is one file where specific SQL
  // statements can improve performance over Tripal API calls.  SQL is not 
  // recommended inside of template files, but rather the Tripal API calls only.
  // Therefore, this function queries the relationships and then organizes them
  // into arrays that are easier and faster to parse.  You should be able to
  // copy the content of this function 
  // and adjust as necessary to change table names if your record has relationships.

  $file = $variables['node']->file;
  
   // expand the file object to include the file relationships.
  $options = array(
    'return_array' => 1,
    // we don't want to fully recurse we only need information about the
    // relationship type and the object and subject files (including file type)
    'include_fk' => array(
      'type_id' => 1,
      'object_id' => array(
        'type_id' => 1,
      ),
      'subject_id'  => array(
        'type_id' => 1,
      ),
    ),
  );
  $file = chado_expand_var($file, 'table', 'file_relationship', $options);

  // get the subject relationships
  $srelationships = $file->file_relationship->subject_id;
  $orelationships = $file->file_relationship->object_id;

  // combine both object and subject relationshisp into a single array
  $relationships = array();
  $relationships['object'] = array();
  $relationships['subject'] = array();

  // iterate through the object relationships
  if ($orelationships) {
    foreach ($orelationships as $relationship) {
      $rel = new stdClass();
      $rel->record = $relationship;
       
      // get the relationship and child types
      $rel_type = t(preg_replace('/_/', " ", $relationship->type_id->name));
      $child_type = $relationship->subject_id->type_id->name;
       
      // get the node id of the subject
      $sql = "SELECT nid FROM {chado_file} WHERE file_id = :file_id";
      $n = db_query($sql, array(':file_id' => $relationship->subject_id->file_id))->fetchObject();
      if ($n) {
        $rel->record->nid = $n->nid;
      }

      if (!array_key_exists($rel_type, $relationships['object'])) {
        $relationships['object'][$rel_type] = array();
      }
      if (!array_key_exists($child_type, $relationships['object'][$rel_type])) {
        $relationships['object'][$rel_type][$child_type] = array();
      }
      $relationships['object'][$rel_type][$child_type][] = $rel;
    }
  }

  // now add in the subject relationships
  if ($srelationships) {
    foreach ($srelationships as $relationship) {
      $rel = new stdClass();
      $rel->record = $relationship;
      $rel_type = t(preg_replace('/_/', " ", $relationship->type_id->name));
      $parent_type = $relationship->object_id->type_id->name;
       
      // get the node id of the subject
      $sql = "SELECT nid FROM {chado_file} WHERE file_id = :file_id";
      $n = db_query($sql, array(':file_id' => $relationship->object_id->file_id))->fetchObject();
      if ($n) {
        $rel->record->nid = $n->nid;
      }
       
      if (!array_key_exists($rel_type, $relationships['subject'])) {
        $relationships['subject'][$rel_type] = array();
      }
      if (!array_key_exists($parent_type, $relationships['subject'][$rel_type])) {
        $relationships['subject'][$rel_type][$parent_type] = array();
      }
      $relationships['subject'][$rel_type][$parent_type][] = $rel;
    }
  }
  $file->all_relationships = $relationships;
}