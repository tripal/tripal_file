<?php

class sio__file_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Files';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['sio__file'];

  /**
   *
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);

    // All of the file linker tables have the same fields so we'll
    // use the feature_file table to get the cvterms.
    $field_table = $this->instance['settings']['chado_table'];
    $base_table = $this->instance['settings']['base_table'];
    $record = $widget['#entity']->chado_record;

    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];
    $linker_table = $base_table . '_file';

    // Set defaults.
    $record_id = '';
    $file_id = '';
    $fk_id = $record->$fkey_lcolumn;
    $rank = $delta;


    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (count($items) > 0 and array_key_exists($delta, $items)) {
      $record_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__' . $pkey, $record_id);
      $fk_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__' . $fkey_lcolumn, $fk_id);
      $file_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__' . 'file_id', $file_id);
      $rank = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__' . 'rank', $rank);
    }

    $widget['value'] = [
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    ];
    $widget['chado-' . $field_table . '__' . $pkey] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    $widget['chado-' . $field_table . '__' . $fkey_lcolumn] = [
      '#type' => 'value',
      '#default_value' => $fk_id,
    ];
    $widget['chado-' . $field_table . '__' . 'rank'] = [
      '#type' => 'value',
      '#default_value' => $rank,
    ];

    $options = [];
    $options[0] = '-- Select a File --';
    $sel_options = [
      'return_array' => TRUE,
      'include_fk' => [
        'type_id' => TRUE
      ],
      'order_by' => ['name' => 'DESC']
    ];
    $files = chado_generate_var('file', [], $sel_options);
    foreach ($files as $file) {
      $options[$file->file_id] = $file->name . ' (' . $file->type_id->name . ')';
    }
    $widget['chado-' . $field_table . '__' . 'file_id'] = [
      '#type' => 'select',
      '#title' => t('Available Files'),
      '#options' => $options,
      '#default_value' => $file_id,
      '#description' => t('Please select a file.'),
    ];
  }


  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {
    $field_name = $this->field['field_name'];
    $field_table = $this->instance['settings']['chado_table'];
    $base_table = $this->instance['settings']['base_table'];

    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);

    // If the user re-ordered the items then set the rank to match.
    $rank = $form_state['values'][$field_name]['und'][$delta]['_weight'];
    $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . 'rank'] = $rank;

    // If no file id is provided then remove all elements.
    $file_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . 'file_id'];
    if (!$file_id) {
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $pkey] = '';
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $fkey_lcolumn] = '';
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . 'rank'] = '';
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . 'file_id'] = '';
    }
  }
}
