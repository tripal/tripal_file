<?php

class schema__license_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'License';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['schema__license'];


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

    $name_term = chado_get_semweb_term('license', 'name');
    $summary_term = chado_get_semweb_term('license', 'summary');
    $uri_term = chado_get_semweb_term('license', 'uri');

    [$content_type, $entity_id] = array_key_exists('entity', $items[0]['value']) ? explode(':', $items[0]['value']['entity']) : [NULL, NULL];
    $rows = [];
    $rows[] = [
      [
        'data' => 'Name',
        'header' => TRUE,
      ],
      $entity_id ? l($items[0]['value'][$name_term], 'bio_data/' . $entity_id) : $items[0]['value'][$name_term],
    ];
    $rows[] = [
      [
        'data' => 'License Summary',
        'header' => TRUE,
      ],
      $items[0]['value'][$summary_term],
    ];
    $rows[] = [
      [
        'data' => 'Full Legal Text',
        'header' => TRUE,
        'width' => '20%'
      ],
      $items[0]['value'][$uri_term] ? l($items[0]['value'][$uri_term], $items[0]['value'][$uri_term]) : '<i>Not Available</i>',
    ];

    $table = [
      'header' => [],
      'rows' => $rows,
      'attributes' => [
        'class' => 'tripal-data-table',
      ],
      'sticky' => FALSE,
      'caption' => "",
      'colgroups' => [],
      'empty' => 'There is no license for this file.',
    ];
    $content = theme_table($table);

    $element[0] = [
      // We create a render array to produce the desired markup,
      '#type' => 'markup',
      '#markup' => $content,
    ];
  }
}
