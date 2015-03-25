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
//I Think I may have broke this..
function tripal_file_node_info() {

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

}

/**
 * Implementation of hook_form()
 *
 * Creates the form for editing or inserting a record
 *
 * @ingroup tripal_file
 */
function chado_file_form($node, &$form_state) {


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


  
}

/**
 * Implementation of hook_node_presave().
 *
 * Performs actions on a node object prior to it being saved
 *
 * @ingroup tripal_file
 */
function tripal_file_node_presave($node) {


}

/**
 * Implementation of hook node_insert().
 *
 * Performs actions after any node has been inserted.
 *
 * @ingroup tripal_file
 */
function tripal_file_node_insert($node) {

}

/**
 * Implementation of hook node_update().
 *
 * Performs actions after any node has been updated.
 *
 */
function tripal_file_node_update($node) {


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
