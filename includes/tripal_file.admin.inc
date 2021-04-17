<?php

/**
 * @file
 * Administrative interface for tripal file
 */

/**
 *
 */
function tripal_file_admin_form($form, $form_state) {

  $form['general'] = [
    '#type' => 'fieldset',
    '#title' => 'Display Options'
  ];

  $form['general']['display_file_name']= [
    '#type' => 'checkbox',
    '#title' => t('Display the file name'),
    '#description' => t('If this option is checked, the file name, without path,'
       . ' will be displayed in an additional column on file pages'),
    '#default_value' => variable_get('tripal_file_display_file_name', FALSE),
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => 'Save Configuration'
  ];

  return $form;
}

/**
 * Validate the Admin/Settings form.
 */
function tripal_file_admin_form_validate($form, &$form_state) {
}

/**
 * Submit the Admin/settings form.
 */
function tripal_file_admin_form_submit($form, $form_state) {

  variable_set('tripal_file_display_file_name', $form_state['values']['display_file_name']);

}
