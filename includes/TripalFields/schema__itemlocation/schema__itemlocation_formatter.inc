<?php

class schema__itemlocation_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'File Location';

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
    $size_term = chado_get_semweb_term('fileloc', 'size');

    $rows = [];
    foreach ($items as $i => $item) {
       $fileloc = $item['value'];
       if (!$fileloc) {
         continue;
       }
       $url_parts = parse_url($fileloc[$uri_term]);

       if (array_key_exists('host', $url_parts)) {
         $source = $url_parts['host'];

         // Check if there is a db record for this URL.
         $db = new ChadoRecord('db');
         $db->setValues(['url' => 'https://' . $url_parts['host']]);
         if ($db->find()) {
           $source = $db->getValue($column_name);
         }
         $db->setValues(['url' => 'http://' . $url_parts['host']]);
         if ($db->find()) {
           $source = $db->getValue('name');
         }

         // Check if the base URL of the file is from this site.
         global $base_url;
         if ( ($base_url == 'http://' . $url_parts['host']) or
              ($base_url == 'https://' . $url_parts['host']) ) {
           $source = variable_get('site_name');
         }
       }
       $uri = l($source, $fileloc[$uri_term]);
       $size = $item['value'][$size_term] !== '0B' ? $item['value'][$size_term] : 'N/A';
       $md5 = $item['value'][$md5_term] !== '' ? $item['value'][$md5_term] : 'N/A';

       $rows[] = [$uri, $size, $md5];
     }

    $table = [
      'header' => ['Available at', 'Size', 'MD5'],
      'rows' => $rows,
      'attributes' => [
        'class' => 'tripal-data-table',
      ],
      'sticky' => FALSE,
      'caption' => "",
      'colgroups' => [],
      'empty' => 'This file is currently not available for download.'
    ];
    $content = theme_table($table);

    $element[0] = [
      // We create a render array to produce the desired markup,
      '#type' => 'markup',
      '#markup' => $content,
    ];
  }
}
