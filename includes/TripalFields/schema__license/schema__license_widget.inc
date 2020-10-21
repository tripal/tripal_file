<?php

class schema__license_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'License';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['schema__license'];

  /**
   *
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);

    $record_id = '';
    $file_id = '';
    $license_id = 0;

    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (count($items) > 0 and array_key_exists($delta, $items)) {
      $file_id = tripal_get_field_item_keyval($items, $delta, 'chado-file_license__file_id', $file_id);
      $license_id = tripal_get_field_item_keyval($items, $delta, 'chado-file_license__license_id', $license_id);
      $record_id = tripal_get_field_item_keyval($items, $delta, 'chado-file_license__file_license_id', $record_id);
    }

    $widget['value'] = [
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    ];
    $widget['chado-file_license__file_license_id'] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    $widget['chado-file_license__file_id'] = [
      '#type' => 'value',
      '#default_value' => $file_id,
    ];

    $licenses = chado_select_record('license', ['*'], [], ['return_array' => TRUE, 'order_by' => ['name' => 'ASC']]);
    $options = [];
    foreach ($licenses as $license) {
      $options[$license->license_id] = $license->name;
    }

    $widget['chado-file_license__license_id'] = [
      '#type' => 'select',
      '#title' => t('File License'),
      '#options' => $options,
      '#default_value' => $license_id,
      '#description' => t('Please select a license that properly describes to the user their rights, priviledge and or usage permissions for this file.'),
      '#required' => $element['#required'],
    ];
  }


  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {
    $field_name = $this->field['field_name'];

  }
}
