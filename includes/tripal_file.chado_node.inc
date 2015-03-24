<?php

/**
 * @file
 * This file should contain all Drupal hooks for interacting with nodes.
 *
 */

/**
 *  Implementation of hook_node_info().
 *
 *  This hook provides information to drupal about any node types that are being
 *  created by this module. If your module does not create any node types then
 *  this function is not required.
 *
 * @ingroup tripal_file
 */
function tripal_file_node_info() {
  $nodes = array();

  // EXPLANATION: this array describes all of the node types that are created
  // by this module. For many Tripal modules (e.g. tripal_file, tripal_stock,
  // tripal_library, tripal_pub, etc.) new node types are created. It is
  // customary to name all new node types that interact with data in Chado
  // with a 'chado_' prefix.

  $nodes['chado_file'] = array(
    'name'        => t('file'),
    'base'        => 'chado_file',
    'description' => t('A record from the fake chado file table'),
    'has_title'   => TRUE,
    'locked'      => TRUE,
    // EXPLANATION: This section of the node type array specifies how Tripal will sync the node
    // types with data in Chado. When Drupal creates a node it has no way of
    // coordinating which node belongs to which record in Chado. Therefore,
    // Tripal maintains tables in the Drupal schema that maps Drupal nodes
    // to recrords in Chado.  Syncing is the process of creating Drupal nodes
    // and linking them to the appropriate record.
    'chado_node_api' => array(
      'base_table' => 'file',       // the base table name (e.g. file, file, contact)
      'hook_prefix' => 'chado_file',// the node type hook prefix
      'record_type_title' => array(
        'singular' => t('file'),    // how to refer to the record
        'plural' => t('files')      // how to refer to the record in plurals
      ),
      'sync_filters' => array(
        'type_id' => TRUE,             // if the record has a type_id set to TRUE
        'organism_id' => TRUE          // if the record has an organism_id set to TRUE
      ),
    )
  );

  return $nodes;
}

/**
 * Implement hook_access().  This hook provides instructions to
 * drupal for which users can access the custom content types
 * created in the function above.  The available permissions
 * are set in the chado_file_permissions() hook in the
 * tripal_file.module file.  This hook is not needed
 * if no node types were defined in the hook_node_info() hook.
 *
 * @return
 * This function should return null if it does not specificially
 * deny access. This allows for other mechanisms to to deny
 * or reject access. If the return value is TRUE then access
 * is granted regardless of any other rules that might be implemented
 * by other modules.
 */
function chado_file_node_access($node, $op, $account) {
  $node_type = $node;
  if (is_object($node)) {
    $node_type = $node->type;
  }
  // EXPLANATION:  in the tripal_file_permissions() function we
  // created the permission types that are used here to check for
  // access permissions to the 'chado_exmaple' node type.
  if($node_type == 'chado_file') {
    if ($op == 'create') {
      if (!user_access('create chado_file content', $account)) {
        return NODE_ACCESS_DENY;
      }
      return NODE_ACCESS_ALLOW;
    }
    if ($op == 'update') {
      if (!user_access('edit chado_file content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'delete') {
      if (!user_access('delete chado_file content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'view') {
      if (!user_access('access chado_file content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
  }
  return NODE_ACCESS_IGNORE;
}

/**
 * Implementation of hook_form()
 *
 * Creates the form for editing or inserting a record
 *
 * @ingroup tripal_file
 */
function chado_file_form($node, &$form_state) {

  // EXPLANATION: This function should construct a form array that is used
  // by Drupal to contruct a form for inserting or editing our new node type.
  // See this page for information about the Form API:
  // https://api.drupal.org/api/drupal/includes!form.inc/group/form_api/7
  //
  // The code below is laid out in the following order
  // 1) Set default values
  // 2) Add form elements used by this node type
  // 3) Use the Tripal API to add form elemetns for properties,
  //    dbxref's and relationships
  //
  // For the file code below we assume that the fake 'file' table
  // only has a uniquename, organism_id, type_id and file_id.

  $form = array();

  // Default values can come in the following ways:
  //
  // 1) as elements of the $node object.  This occurs when editing an existing file
  // 2) in the $form_state['values'] array which occurs on a failed validation or
  //    ajax callbacks from non submit form elements
  // 3) in the $form_state['input'[ array which occurs on ajax callbacks from submit
  //    form elements and the form is being rebuilt
  //
  // set form field defaults


  // SET FORM DEFAULTS
  //---------------------------------------------
  $file      = null;   // holds the file object record
  $file_id   = null;   // when editing an file record we'll have an file_id

  // initialize the defaults for the form fields
  $uniquename   = '';
  $file_type = '';
  $organism_id  = '';
  $description  = '';

  // if we are editing an existing node then the 'file' record from Chado
  // is already part of the node, so we set the defaults from that object
  if (property_exists($node, 'file')) {
    $file = $node->file;
    $file_id   = $file->file_id;
    $uniquename   = $file->uniquename;
    $description  = $file->description;
    $organism_id  = $file->organism_id;

    // keep track of the file id
    $form['file_id'] = array(
      '#type' => 'value',
      '#value' => $file_id,
    );
  }
  // if we are re constructing the form from a failed validation or ajax callback
  // then use the $form_state['values'] values
  if (array_key_exists('values', $form_state)) {
     $uniquename   = $form_state['values']['uniquename'];
     $file_type = $form_state['values']['file_type'];
     $description  = $form_state['values']['description'];
     $organism_id  = $form_state['values']['organism_id'];
  }
  // if we are re building the form from after submission (from ajax call) then
  // the values are in the $form_state['input'] array
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
     $uniquename   = $form_state['input']['uniquename'];
     $file_type = $form_state['input']['file_type'];
     $organism_id  = $form_state['input']['organism_id'];
  }


  // FORM ELEMENTS
  //---------------------------------------------
  $form['uniquename'] = array(
    '#type' => 'textfield',
    '#title' => t('Unique Name'),
    '#required' => TRUE,
    '#default_value' => $uniquename,
    '#description' => t('Enter a unique name for this file.  This name must be unique.'),
    '#maxlength' => 255
  );

  // for the type_id we want to use the default vocabulary so that this
  // field can have autocomplete functionality
  $type_cv = tripal_get_default_cv('file', 'type_id');
  $cv_id = $type_cv->cv_id;
  $form['file_type'] = array(
    '#title'       => t('file Type'),
    '#type'        => 'textfield',
    '#description' => t("Choose the file type (e.g. Test Type)."),
    '#required'    => TRUE,
    '#default_value' => $file_type,
    '#autocomplete_path' => "admin/tripal/chado/tripal_cv/cvterm/auto_name/$cv_id",
  );

  // add a select box of organisms
  $organisms = tripal_get_organism_select_options();
  $form['organism_id'] = array(
    '#title'       => t('Organism'),
    '#type'        => t('select'),
    '#description' => t("Choose the organism with which this file is associated"),
    '#required'    => TRUE,
    '#default_value' => $organism_id,
    '#options'     => $organisms,
  );

  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#required' => TRUE,
    '#default_value' => $description,
    '#description' => t('Enter a description for this file.'),
  );

  // PROPERTIES FORM
  //---------------------------------------------
  // If there is a fileprop table and you want to allow users to add/remove entries
  // from it through your node form then add this section to your own node form
  $prop_cv = tripal_get_default_cv('fileprop', 'type_id');
  $cv_id = $prop_cv ? $prop_cv->cv_id : NULL;
  $details = array(
    'property_table' => 'fileprop',      // the name of the prop table
    'chado_id' => $file_id,              // the value of file_id for this record
    'cv_id' => $cv_id                       // the cv.cv_id of the cv governing fileprop.type_id
  );
  // Adds the form elements to your current form
  chado_add_node_form_properties($form, $form_state, $details);

  // ADDITIONAL DBXREFS FORM
  //---------------------------------------------
  // If there is a file_dbxref table and you want to allow users to add/remove entries
  // from it through your node form then add this section to your own node form
  $details = array(
    'linking_table' => 'file_dbxref',  // the name of the _dbxref table
    'base_foreign_key' => 'file_id',   // the name of the key in your base chado table
    'base_key_value' => $file_id       // the value of file_id for this record
  );
  // Adds the form elements to your current form
  chado_add_node_form_dbxrefs($form, $form_state, $details);

  // RELATIONSHIPS FORM
  //---------------------------------------------
  // If there is a file_relationship table and you want to allow users to add/remove entries
  // from it through your node form then add this section to your own node form
  $rels_cv = tripal_get_default_cv('file_relationship', 'type_id');
  $cv_id = $rels_cv ? $rels_cv->cv_id : NULL;
  $details = array(
    'relationship_table' => 'file_relationship', // the name of the _relationship table
    'base_table' => 'file',                      // the name of your chado base table
    'base_foreign_key' => 'file_id',             // the name of the key in your base chado table
    'base_key_value' => $file_id,                // the value of file_id for this record
    'nodetype' => 'file',                        // the human-readable name of your node type
    'cv_id' => $cv_id                               // the cv.cv_id of the cv governing file_relationship.type_id
  );
  // Adds the form elements to your current form
  chado_add_node_form_relationships($form, $form_state, $details);

  // return the form
  return $form;
}

/**
 * Implementation of hook_validate
 *
 * This function validates a form prior to insert or update. If an error
 * is detected, it sets the error using form_set_error() which takes the
 * user back to the form to make corrections.
 *
 * This validation is being used for three activities:
 *   CASE A: Update a node that exists in both drupal and chado
 *   CASE B: Synchronizing a node from chado to drupal
 *   CASE C: Inserting a new node that exists in niether drupal nor chado
 *
 * @param $node
 *
 *
 * @ingroup tripal_file
 */
function chado_file_validate($node, $form, &$form_state) {

  // if this is a delete then don't validate
  if($node->op == 'Delete') {
    return;
  }

  // we are syncing if we do not have a node ID but we do have a file_id. We don't
  // need to validate during syncing so just skip it.
  if (is_null($node->nid) and property_exists($node, 'file_id') and $node->file_id != 0) {
    return;
  }

  // be sure to always trim text fields
  $node->uniquename   = trim($node->uniquename);
  $node->description  = trim($node->description);

  // Validating for an update. If the 'nid' property is present in the node then
  // this is an update and validation can be different for updates
  if (property_exists($node, 'nid')) {

    // make sure the feature type is an allowed term
    $type_cv = tripal_get_default_cv('file', 'type_id');
    $type = tripal_get_cvterm(array(
      'name' => $node->file_type,
      'cv_id' => $type_cv->cv_id,
    ));
    if (!$type) {
      form_set_error('feature_type', t("The feature type is not a valid name from the Sequence Ontology."));
    }

    // TODO: also we should check that the unique constraint is not invalidated by
    // changing either the type_id, organism_id or uniquename.
  }
  // Validating for an insert
  else {
    // make sure the feature type is an allowed term
    $type_cv = tripal_get_default_cv('file', 'type_id');
    $type = tripal_get_cvterm(array(
      'name' => $node->file_type,
      'cv_id' => $type_cv->cv_id,
    ));
    if (!$type) {
      form_set_error('feature_type', t("The feature type is not a valid name from the Sequence Ontology."));
    }

    // TODO: also we should check that the unique constraint doesn't already exist
  }
}

/**
 *  Implementation of hook_insert(). This function is called after the
 *  node is inserted into the database. We need it so that we can insert
 *  appropriate fields as provided by the user into the database. And so that
 *  we can link the new Drupal node to the data in Chado via the chado_file
 *  linking table. We can get to this function also during "syncing".
 *  With syncing, however, the data already exists in Chado and we do not want
 *  to try to re-add it. But we do need to add an entry to the chado_file table
 *  to link the Drupal node with the data in the 'file' table of Chado.
 *
 *  This function is not required if the hook_node_info() does not define
 *  any custom node types.
 *
 * @ingroup tripal_file
 */
function chado_file_insert($node) {
  // be sure to always trim text fields
  $node->uniquename   = trim($node->uniquename);
  $node->description  = trim($node->description);

  // get the file type record
  $type_cv = tripal_get_default_cv('file', 'type_id');
  $type = tripal_get_cvterm(array(
    'name' => $node->file_type,
    'cv_id' => $type_cv->cv_id,
  ));

  // if there is an file_id in the $node object then this must be a sync so
  // we can skip adding the file as it is already there, although
  // we do need to proceed with the rest of the insert
  if (!property_exists($node, 'file_id')) {

    // perform the insert using the chado_insert_record function();
    $values = array(
      'uniquename' => $node->uniquename,
      'description' => $node->description,
      'type_id' => $type->cvterm_id,
      'organism_id' => $node->organism_id,
    );
    $file = chado_insert_record('file', $values);
    if (!$file) {
      drupal_set_message(t('Unable to add file.'), 'warning');
      tripal_report_error('tripal_file', TRIPAL_WARNING, 'Insert file: Unable to create file where values: %values',
        array('%values' => print_r($values, TRUE)));
      return;
    }

    // get the file_id for linking Drupal node with Chado data
    $file_id = $file['file_id'];

    // Only add to other chado tables if the base record was inserted properly
    if ($file_id > 0) {

      // If you implemented the properties form in chado_file_form then you need to
      // handle inserting these properties into your chado prop table.
      $details = array(
        'property_table'   => 'fileprop',   // the name of the prop table
        'base_table'       => 'file',       // the name of your chado base table
        'foreignkey_name'  => 'file_id',    // the name of the key in your base table
        'foreignkey_value' => $file_id      // the value of the file_id key
      );
      chado_update_node_form_properties($node, $details);

      // If you implemented the dbxrefs form in chado_file_form then you need to
      // handle inserting these database references into your chado _dbxref table.
      $details = array(
        'linking_table'    => 'file_dbxref', // the name of your _dbxref table
        'foreignkey_name'  => 'file_id',     // the name of the key in your base table
        'foreignkey_value' => $file_id       // the value of the file_id key
      );
      chado_update_node_form_dbxrefs($node, $details);

      // If you implemented the relationships form in chado_file_form then you need to
      // handle inserting these relationships into your chado _relationship table.
      $details = array(
        'relationship_table' => 'file_relationship',  // name of the _relationship table
        'foreignkey_value' => $file_id                // value of the file_id key
      );
      chado_update_node_form_relationships($node, $details);
    }
  }
  else {
    // the node has an file_id so get it for linking Drupal node with Chado data
    $file_id = $node->file_id;
  }

  // Make sure the entry for this file doesn't already exist in the
  // chado_file table if it doesn't exist then we want to add it.
   $check_org_id = chado_get_id_from_nid('file', $node->nid);
  if (!$check_org_id) {
    $record = new stdClass();
    $record->nid = $node->nid;
    $record->vid = $node->vid;
    $record->file_id = $file_id;
    drupal_write_record('chado_file', $record);
  }
}

/**
 * Implementation of hook_update().  This function runs after the
 * node has been inserted into the Drupal schema and allows us to
 * update the record in Chado.
 *
 *  This function is not required if the hook_node_info() does not define
 *  any custom node types.
 *
 * @ingroup tripal_file
 */
function chado_file_update($node) {
  // be sure to always trim text fields
  $node->uniquename   = trim($node->uniquename);
  $node->description  = trim($node->description);

  // use the chado_update_record() function to update the record
  $match = array(
    'file_id' => $file_id,
  );
  $values = array(
    'uniquename' => $node->uniquename,
  );
  $options = array('return_record' => TRUE);
  $status = chado_update_record('file', $match, $values, $options);

  if (!$status) {
    drupal_set_message(t('Unable to update file.'), 'warning');
    tripal_report_error('tripal_file', TRIPAL_WARNING, 'Update file: Unable to update file where values: %values',
      array('%values' => print_r($values, TRUE)));
  }

  // If you implemented the properties form in chado_file_form then you need to
  // handle updating these properties into your chado prop table.
  $details = array(
    'property_table' => 'fileprop',   // the name of the prop table
    'base_table' => 'file',           // the name of your chado base table
    'foreignkey_name' => 'file_id',   // the name of the key in your base table
    'foreignkey_value' => $file_id    // the value of the file_id key
  );
  chado_update_node_form_properties($node, $details);

  // If you implemented the dbxrefs form in chado_file_form then you need to
  // handle updating these database references into your chado _dbxref table.
  $details = array(
    'linking_table' => 'file_dbxref',   // the name of your _dbxref table
    'foreignkey_name' => 'file_id',     // the name of the key in your base table
    'foreignkey_value' => $file_id      // the value of the file_id key
  );
  chado_update_node_form_dbxrefs($node, $details);

  // If you implemented the relationships form in chado_file_form then you need to
  // handle updating these relationships into your chado _relationship table.
  $details = array(
    'relationship_table' => 'file_relationship',  // name of the _relationship table
    'foreignkey_value' => $file_id                // value of the file_id key
  );
  chado_update_node_form_relationships($node, $details);

}
/**
 * Implementation of hook_delete().  This function runs after the
 * node has been deleted from the Drupal schema and allows us to
 * delete the corresponding recrod in Chado.
 *
 * This function is not required if the hook_node_info() does not define
 * any custom node types.
 *
 * @ingroup tripal_file
 */
function chado_file_delete($node) {

  // get the file id from the node
  $file_id  = chado_get_id_from_nid('file', $node->nid);

  // if we don't have a file id for this node then this isn't a node of
  // type chado_file or the entry in the chado_file table was lost.
  if (!$file_id) {
    return;
  }

  // remove the entry in the chado_exapmle table linking the deleted
  // Drupal node with the data in chado
  $sql_del = "DELETE FROM {chado_file} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));

  // Remove data from file tables of chado database.  This will
  // cause a cascade delete and remove all data in referencing tables
  // for this file
  chado_query("DELETE FROM {file} WHERE file_id = :file_id", array(':file_id' => $file_id));

  // inform the user that the data was deleted
  drupal_set_message(t("The file and all associated data were removed from Chado"));

}

/**
 * Implementation of hook_load().  This function is necessary to load
 * into the $node object the fields of the table form Chado. For file
 * for the file table, the chado_file_load() function adds in
 * a file object which contains all of the fields and sub objects
 * for data in tables with foreign key relationships.
 *
 * This function is not required if the hook_node_info() does not define
 * any custom node types.
 *
 * @ingroup tripal_file
 */
function chado_file_load($nodes) {

  // EXPLANATION: when displaying or node or accessing the node in a template
  // we need the data from Chado.  This fucntion finds the record in Chado that
  // this node belongs to and adds the record.

  // there may be multiple nodes that get passed in so we have to iterate through
  // them all
  foreach ($nodes as $nid => $node) {
    // find the file and add in the details
    $file_id = chado_get_id_from_nid('file', $nid);

    // if the nid does not have a matching record then skip this node.
    // this can happen with orphaned nodes.
    if (!$file_id) {
      continue;
    }

    // build the file variable by using the chado_generate_var() function
    $values = array('file_id' => $file_id);
    $file = chado_generate_var('file', $values);

    // for fields in the table that are of type 'text' you may want to include those
    // by default, the chado_generate_var does not include text fields as
    // they may be very large and including a large text field can slow the page load.
    // If you know a text field will never be large and it is important for the
    // other functions that will see the node to have access to a field you can
    // include it here using the chado_expand_var() function.  In most
    // cases it is probably best to let the end-user decide if text fields should
    // be included by using this function in the templates.
    $file = chado_expand_var($file, 'field', 'file.description');


    // If your module is using the Chado Node: Title & Path API to allow custom titles
    // for your node type. Every time you want the title of the node, you need to use the
    // following API function:
    $node->title = chado_get_node_title($node);

    // add the new file object to this node.
    $nodes[$nid]->file = $file;
  }
}

/**
 * Implementation of hook_node_presave().
 *
 * Performs actions on a node object prior to it being saved
 *
 * @ingroup tripal_file
 */
function tripal_file_node_presave($node) {

  // EXPLANATION: This node is useful for
  // making changes to the node prior to it being saved to the database.
  // One useful case for this is to set the title of a node using values 
  // supplied by the user.
  //
  // This function is not required. You probably won't need it if you
  //  don't define a custom node type in the hook_node_info() function. But
  // it is node type agnostic, so you can use this function to change the
  // contents of any node regardless of it's type.

  // set the node title
  switch ($node->type) {
    // This step is for setting the title for the Drupal node.  This title
    // is permanent and thus is created to be unique.  Title changes provided
    // by tokens are generated on the fly dynamically, but the node title
    // seen in the content listing needs to be set here. Do not call
    // the chado_get_node_title() function here to set the title as the node
    // object isn't properly filled out and the function will fail.
    case 'chado_file':
      // for a form submission the 'uniquename' field will be set,
      // for a sync, we must pull from the file object
      if (property_exists($node, 'uniquename')) {
        // set the title
        $node->title = $node->uniquename;
      }
      else if (property_exists($node, 'file')) {
        $node->title = $node->file->uniquename;
      }
      break;
  }
}

/**
 * Implementation of hook node_insert().
 *
 * Performs actions after any node has been inserted.
 *
 * @ingroup tripal_file
 */
function tripal_file_node_insert($node) {

  // EXPLANATION: This function is used
  // after any a node is inserted into the database.  It is different
  // from the hook_insert() function above in that it is called after
  // any node is saved, regardlesss of it's type. This function is useful
  // for making changes to the database after a node is inserted.
  // An file comes from the tripal_feature module where the URL alias
  // of a node cannot be set in the hook_insert() function. Therefore
  // the tripal_feature module uses this function to set the url path
  // of a newly inserted file node.
  //
  // This function is not required. You probably won't need it if you
  // don't define a custom node type in the hook_node_info() function. But
  // it is node type agnostic, so you can use this function to do any
  // activity after insert of any node.

  // the file code below will set the URL path after inserting.  We do it
  // here because we do not know the file_id in the presave and cannot do
  // it in the hook_insert()
  switch ($node->type) {
    case 'chado_file':

      // find the file and add in the details
      $file_id = chado_get_id_from_nid('file', $nid);

      // build the file variable by using the chado_generate_var() function
      $values = array('file_id' => $file_id);
      $file = chado_generate_var('file', $values);
      $node->file = $file;

      // If your module is using the 'Chado Node: Title & Path API' to allow custom titles
      // for your node type. Every time you want the title of the node, you need to use the
      // following API function:
      $file->title = chado_get_node_title($node);

      // set the URL for this file page
      // see the code in the tripal_feature/includes/tripal_feature.chado_node.inc file
      // in the function tripal_feature_node_insert for an file of how that
      // module sets the URL.  It uses a configuration file to allow the user
      // to dynmically build a URL schema and then uses that schema to generate
      // a URL string.
      break;
  }
}

/**
 * Implementation of hook node_update().
 *
 * Performs actions after any node has been updated.
 *
 */
function tripal_file_node_update($node) {

  // EXPLANATION: This function is used
  // after any a node is updated in the database.  It is different
  // from the hook_update() function above in that it is called after
  // any node is updated, regardlesss of it's type.
  // An file comes from the tripal_feature module where the URL alias
  // of a node cannot be set in the hook_update() function. Therefore
  // the tripal_feature module uses this function to reset the url path
  // of an updated feature node.
  //
  // This function is not required. You probably won't need it if you
  // don't define a custom node type in the hook_node_info() function. But
  // it is node type agnostic, so you can use this function to do any
  // activity after insert of a node.

  // add items to other nodes, build index and search results
  switch ($node->type) {
    case 'chado_file':

      // If your module is using the Chado Node: Title & Path API to allow custom titles
      // for your node type. Every time you want the title of the node, you need to use the
      // following API function:
      $file->title = chado_get_node_title($node);

      // set the URL for this file page
      // see the code in the tripal_feature/includes/tripal_feature.chado_node.inc file
      // in the function tripal_feature_node_insert for an file of how that
      // module sets the URL.  It uses a configuration file to allow the user
      // to dynmically build a URL schema and then uses that schema to generate
      // a URL string.
      break;
  }
}
/**
 * Implementation of hook_node_view().
 *
 * @ingroup tripal_file
 */
function tripal_file_node_view($node, $view_mode, $langcode) {

  // EXPLANATION: This function defines the content "blocks" that appear
  // when thhe node is displayed. It is node type agnostic so we can add
  // content to any node type.  So, we use this function to add the content
  // from all of our theme templates onto our new node type. We will also
  // use this function to add content to other node types.

  switch ($node->type) {
    case 'chado_file':
      // there are different ways a node can be viewed. Primarily Tripal
      // supports full page view and teaser view.
      if ($view_mode == 'full') {

        // If you want to use the default Tripal node template then you need to tell
        // tripal to generate the Table of Contents. This is done by setting the following
        // to TRUE. If your content type follows the chado_<base table> convention
        // then this is the default. In this case if you don't want to use the default
        // template then you need to set the following to FALSE.
        $node->content['#tripal_generic_node_template'] = TRUE;

        // There is always a base template.  This is the template that
        // is first shown when the file node type is first displayed.
        // if you are using the default Tripal node template, then you should
        // also set two additional items in each array:  tripal_toc_id and
        // tripal_toc_title.  The tripal_tock_id should be a single unqiue
        // world that is used to reference the template. This ID is used for
        // constructing URLs for the content.  The tripal_toc_title contains
        // the title that should appear in the table of contents for this
        // content.  You should only set the '#weight' element for the
        // base template (or Overview) to ensure that it appears at the top of
        // the list. Otherwise items are sorted alphabetically.
        $node->content['tripal_file_base'] = array(
          '#markup' => theme('tripal_file_base', array('node' => $node)),
          '#tripal_toc_id'    => 'base',
          '#tripal_toc_title' => 'Overview',
          '#weight' => -100,
        );
        // we can add other templates as well for properties, publications,
        // dbxrefs, etc...
        $node->content['tripal_file_properties'] = array(
          '#markup' => theme('tripal_file_properties', array('node' => $node)),
          '#tripal_toc_id'    => 'properties',
          '#tripal_toc_title' => 'Properties',
        );
        $node->content['tripal_file_references'] = array(
          '#markup' => theme('tripal_file_references', array('node' => $node)),
          '#tripal_toc_id'    => 'references',
          '#tripal_toc_title' => 'Cross References',
        );
        $node->content['tripal_file_relationships'] = array(
          '#markup' => theme('tripal_file_relationships', array('node' => $node)),
          '#tripal_toc_id'    => 'relationships',
          '#tripal_toc_title' => 'Relationships',
        );

        // Note: if you create a template that you do not want a user to
        // know where it is (discourage editing of it), you can add the following
        // key:  '#tripal_template_show' => FALSE. If this key/value is set
        // the the administrator message that Tripal provides indicating
        // where the template is housed will not be shown.
      }
      // set the content for the teaser view
      if ($view_mode == 'teaser') {
        // The teaser is also a required template
        $node->content['tripal_file_teaser'] = array(
          '#value' => theme('tripal_file_teaser', array('node' => $node)),
        );
      }
      break;
    // you can add custom content to any node type by adding
    // content to the node in the same way as above.
    case 'chado_organism':
      if ($view_mode == 'full') {
        $node->content['tripal_file_generic'] = array(
          '#markup' => theme('tripal_file_generic', array('node' => $node)), 		
          '#tripal_toc_id'    => 'files',
          '#tripal_toc_title' => 'files',
        );
      }
      break;
    // ... etc
  }
}
