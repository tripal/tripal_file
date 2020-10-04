<?php

class schema__itemlocation_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Location';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['schema__itemlocation'];


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

    $name_term = chado_get_semweb_term('fileloc', 'name');
    $uri_term = chado_get_semweb_term('fileloc', 'uri');
    $rank_term = chado_get_semweb_term('fileloc', 'rank');
    $md5_term = chado_get_semweb_term('fileloc', 'md5checksum');

    $headers = ['Available Location'];
    $rows = [];

     foreach ($items as $delta => $item) {
       $fileloc = $item['value'];
       if (!$fileloc) {
         continue;
       }
       $uri = l($fileloc[$uri_term], $fileloc[$uri_term]);
       $rows[] = [$uri];
     }

    $content = '';
    if (count($items) > 0) {
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
    }

    $element[0] = [
      // We create a render array to produce the desired markup,
      '#type' => 'markup',
      '#markup' => $content,
    ];
  }
}
