<?php

class sio__file_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Files';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['sio__file'];


  /**
   *
   * @see TripalFieldFormatter::settingsForm()
   */
  public function settingsForm($view_mode, $form, &$form_state) {

  }

  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    $name_term = chado_get_semweb_term('feature', 'name');
    $type_term = chado_get_semweb_term('feature', 'type_id');
    $rank_term = chado_get_semweb_term('feature_file', 'rank');

    $rows = [];

    foreach ($items as $item) {
      if (empty($item['value'])) {
        continue;
      }
      [$content_type, $entity_id] = array_key_exists('entity', $item['value']) ? explode(':', $item['value']['entity']) : [NULL, NULL];
      $rows[] = [
        $entity_id ? l($item['value'][$name_term], 'bio_data/' . $entity_id) : $item['value'][$name_term],
        $item['value'][$type_term],
      ];
    }

    $content = '';
    $headers = ['File', 'Type'];
    if (count($rows) > 0) {
      $table = [
        'header' => $headers,
        'rows' => $rows,
        'attributes' => [
          'id' => 'tripal_linker-table-file-object',
          'class' => 'tripal-data-table',
        ],
        'sticky' => FALSE,
        'caption' => "",
        'colgroups' => [],
        'empty' => 'There are no locations for this file.',
      ];
      $content = theme_table($table);

      $element[0] = [
        // We create a render array to produce the desired markup,
        '#type' => 'markup',
        '#markup' => $content,
      ];
    }
  }
}
