<?php

class schema__itemlocation_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Location';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['schema__itemlocation'];

  /**
   *
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);

    $record_id = '';
    $file_id = '';
    $uri = '';
    $md5 = '';
    $remote = '';
    $local = '';

    $uri_term = chado_get_semweb_term('fileloc', 'uri');
    $rank_term = chado_get_semweb_term('fileloc', 'rank');
    $md5_term = chado_get_semweb_term('fileloc', 'md5checksum');

    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (count($items) > 0 and array_key_exists($delta, $items)) {
      $file_id = tripal_get_field_item_keyval($items, $delta, 'chado-fileloc__file_id', $file_id);
      $uri = tripal_get_field_item_keyval($items, $delta, 'chado-fileloc__uri', $uri);
      $record_id = tripal_get_field_item_keyval($items, $delta, 'chado-fileloc__fileloc_id', $record_id);
      $md5 = tripal_get_field_item_keyval($items, $delta, 'chado-fileloc__md5checksum', $md5);
    }

    $widget['value'] = [
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    ];
    $widget['chado-fileloc__fileloc_id'] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    $widget['chado-fileloc__file_id'] = [
      '#type' => 'value',
      '#default_value' => $file_id,
    ];
    $widget['chado-fileloc__uri'] = [
      '#type' => 'value',
      '#default_value' => $uri,
    ];
    $widget['chado-fileloc__rank'] = [
      '#type' => 'value',
      '#default_value' => $delta,
    ];
    $widget['chado-fileloc__md5checksum'] = [
      '#type' => 'value',
      '#default_value' => $md5,
    ];

    // Set the remote or local value based on what the value of the
    // fieloc.uri field.
    if ($uri) {
      if (preg_match('/^(http|ftp)/', $uri)) {
        $remote = $uri;
      }
      else {
        $query = db_select('file_managed', 'FM');
        $query->fields('FM', ['fid']);
        $query->distinct();
        $query->condition('FM.uri', $uri);
        $local = $query->execute()->fetchField();
      }
    }

    $widget['remote'] = [
      '#type' => 'textfield',
      '#title' => t('Remote URL'),
      '#description' => t('If the file is not managed by Drupal or Tripal then enter the full URL to the file. Use this field if the file is shared publicly on this same web server but not managed by Drupal or Tripal.'),
      '#default_value' => $remote,
      '#maxlength' => 100000,
    ];

    global $user;
    $files = tripal_get_user_uploads($user->uid, ['.*']);
    $options = [];
    $options[0] = '-- Select a File --';
    foreach ($files as $fid => $file) {
      $options[$fid] = $file->filename;
    }
    $widget['local'] = [
      '#type' => 'select',
      '#title' => t('Tripal Managed File'),
      '#options' => $options,
      '#default_value' => $local,
      '#description' => t('Please select a file.'),
    ];

  }


  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {
    $field_name = $this->field['field_name'];

    $remote = $form_state['values'][$field_name]['und'][0]['remote'];
    $local = $form_state['values'][$field_name]['und'][0]['local'];

    if ($remote and $local) {
      form_set_error($field_name . '][und][0][remote', "For the file location, please provide only URL or Tripal managed file but not both.");
    }
    if (!$remote and !$local) {
      form_set_error($field_name . '][und][0][remote', "Please specify a location as a URL or a Tripal managed file.");
    }
    if ($remote) {
      $form_state['values'][$field_name]['und'][0]['chado-fileloc__uri'] = $remote;
    }
    if ($local) {
      $file = file_load($local);
      $form_state['values'][$field_name]['und'][0]['chado-fileloc__uri'] = $file->uri;
    }
  }
}
