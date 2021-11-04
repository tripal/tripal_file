<?php

class schema__itemlocation_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'File Location';

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
    $display_name = '';
    $md5 = NULL;
    $size = NULL;
    $unmanaged = '';
    $managed = '';
    $rank = $delta;
    $display_file_name = variable_get('tripal_file_display_file_name', FALSE);

    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (count($items) > 0 and array_key_exists($delta, $items)) {
      $file_id = tripal_get_field_item_keyval($items, $delta, 'chado-fileloc__file_id', $file_id);
      $uri = tripal_get_field_item_keyval($items, $delta, 'chado-fileloc__uri', $uri);
      $display_name = tripal_get_field_item_keyval($items, $delta, 'chado-fileloc__display_name', $display_name);
      $rank = tripal_get_field_item_keyval($items, $delta, 'chado-fileloc__rank', $rank);
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
      '#default_value' => $rank,
    ];
    $widget['chado-fileloc__size'] = [
      '#type' => 'value',
      '#default_value' => $size,
    ];
    $widget['chado-fileloc__md5checksum'] = [
      '#type' => 'value',
      '#default_value' => $md5,
    ];

    // Set the remote or local value based on what the value of the
    // fileloc.uri field.
    if ($uri) {
      // test if this is a file managed by Tripal e.g. public://tripal/users/1/filename
      $query = db_select('file_managed', 'FM');
      $query->fields('FM', ['fid']);
      $query->distinct();
      $query->condition('FM.uri', $uri);
      $managed = $query->execute()->fetchField();
      
      // if not a managed file, it can be a remote url with http:// https:// or ftp://
      // prefix or a local unmanaged file with public:// prefix
      if (!$managed) {
        $unmanaged = $uri;
      }
    }

    $widget['unmanaged'] = [
      '#type' => 'textfield',
      '#title' => t('URL'),
      '#description' => t('If the file is not managed by Drupal or Tripal then enter the full URL to the file. Use this field with public:// prefix if the file is shared publicly on this same web server but not managed by Drupal or Tripal.'),
      '#default_value' => $unmanaged,
      '#maxlength' => 100000,
    ];

    global $user;
    $files = tripal_get_user_uploads($user->uid, ['.*']);
    $options = [];
    $options[0] = '-- Select a File --';
    foreach ($files as $fid => $file) {
      $options[$fid] = $file->filename;
    }
    $widget['managed'] = [
      '#type' => 'select',
      '#title' => t('Tripal Managed File'),
      '#options' => $options,
      '#default_value' => $managed,
      '#description' => t('Please select a file.'),
    ];

    // The display name option can be enabled in the admin settings.
    if ($display_file_name) {
      $widget['chado-fileloc__display_name'] = [
        '#type' => 'textfield',
        '#title' => t('Display Name'),
        '#description' => t('Enter an optional alternative name to display if there is no file name'
                           .' included in the URL. This applies only when file name display is'
                           .' enabled in the admin settings.'),
        '#default_value' => $display_name,
        '#maxlength' => 500,
      ];
    }
  }


  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {
    $field_name = $this->field['field_name'];

    // Perform validation of form elements.
    $unmanaged = $form_state['values'][$field_name]['und'][$delta]['unmanaged'];
    $managed = $form_state['values'][$field_name]['und'][$delta]['managed'];
    if ($unmanaged and $managed) {
      form_set_error($field_name . '][und]['. $delta . '][unmanaged', "For the file location, please provide only a URL or Tripal managed file but not both.");
    }
    if (!$unmanaged and !$managed and $delta == 0) {
      form_set_error($field_name . '][und]['. $delta . '][unmanaged', "Please specify a location as a URL or a Tripal managed file.");
    }

    // If the file is local then get the size, and get the md5 checksum if one exists.
    $local_file_path = '';
    if ($managed) {
      $which_element = 'managed';
      $local_file = file_load($managed);
      $local_file_path = drupal_realpath($local_file->uri);
    }
    // A URL here with a public:// prefix is a local file, but not managed by tripal.
    // We can look up file size and checksum for all local files whether managed or not.
    elseif ($unmanaged) {
      $which_element = 'unmanaged';
      if (preg_match('#^public://#', $unmanaged)) {
        $local_file_path = drupal_realpath($unmanaged);
        if (!$local_file_path) {
          form_set_error($field_name . '][und]['. $delta . '][' . $which_element, "The file \"$unmanaged\" does not exist on the local filesystem.");
        }
      }
      // remote URLs get no validation beyond having a recognized scheme
      elseif (!preg_match('#^https?://|ftp://#i', $unmanaged)) {
        form_set_error($field_name . '][und]['. $delta . '][' . $which_element, "Invalid URL \"$unmanaged\": must have http:// https:// ftp:// or public:// prefix.");
      }
    }
    if ($local_file_path) {
      if (file_exists($local_file_path)) {
        if (file_exists($local_file_path .'.md5')) {
          // substr here because non-managed md5 files might also contain a file name after
          // the checksum if generated using the md5sum program, and do extra verification.
          $md5 = substr(file_get_contents($local_file_path .'.md5'), 0, 32);
          if (preg_match('/^[A-Za-z0-9]{32}$/', $md5)) {
            $form_state['values'][$field_name]['und'][$delta]['chado-fileloc__md5checksum'] = $md5;
          }
          else {
            form_set_error($field_name . '][und]['. $delta . '][' . $which_element, "The file \"${local_file_path}.md5\" contains an invalid md5 checksum.");
          }
        }
        $size = filesize($local_file_path);
        $form_state['values'][$field_name]['und'][$delta]['chado-fileloc__size'] = $size;
      }
      else {
        form_set_error($field_name . '][und]['. $delta . '][' . $which_element, "The file \"$local_file_path\" does not exist on the local filesystem.");
      }
    }

    // If the user re-ordered the items then set the rank to match.
    $rank = $form_state['values'][$field_name]['und'][$delta]['_weight'];
    $form_state['values'][$field_name]['und'][$delta]['chado-fileloc__rank'] = $rank;


    // Set the File URI value and display name according to the user's selection.
    if ($unmanaged) {
      $form_state['values'][$field_name]['und'][$delta]['chado-fileloc__uri'] = $unmanaged;
    }
    elseif ($managed) {
      $file = file_load($managed);
      $form_state['values'][$field_name]['und'][$delta]['chado-fileloc__uri'] = $file->uri;
    }
    // If no file is selected then clear out all values but the Pkey so the
    // location record can be removed.
    else {
      $form_state['values'][$field_name]['und'][$delta]['chado-fileloc__file_id'] = '';
      $form_state['values'][$field_name]['und'][$delta]['chado-fileloc__uri'] = '';
      $form_state['values'][$field_name]['und'][$delta]['chado-fileloc__display_name'] = '';
      $form_state['values'][$field_name]['und'][$delta]['chado-fileloc__rank'] = '';
      $form_state['values'][$field_name]['und'][$delta]['chado-fileloc__md5checksum'] = '';
    }
  }
}
