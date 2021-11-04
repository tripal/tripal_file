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
    $display_name_term = chado_get_semweb_term('fileloc', 'display_name');
    $rank_term = chado_get_semweb_term('fileloc', 'rank');
    $md5_term = chado_get_semweb_term('fileloc', 'md5checksum');
    $size_term = chado_get_semweb_term('fileloc', 'size');
    $display_file_name = variable_get('tripal_file_display_file_name', FALSE);

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

       $row = [$uri, $size, $md5];
       // ability to display file name as left column can be enabled in admin interface
       if ($display_file_name) {
         $display_name = '';
         if (array_key_exists($display_name_term, $item['value'])) {
           $display_name = $item['value'][$display_name_term];
         }
         // if an explicit display_name is not defined, construct a file name from the
         // uri by removing the path portion e.g. "https://a/b/c.txt" becomes "c.txt"
         if (!$display_name) {
           $display_name = basename($fileloc[$uri_term]);
           // URLs can be a messy POST with multiple parameters, which is hard to
           // simplify, but we can at least remove anything before a question mark
           // e.g. downloadSupplement?doi=10.123%2F456&file=filename.txt
           $display_name = preg_replace('/^.*\?/', '', $display_name);
         }
         array_unshift($row, $display_name);
       }
       $rows[] = $row;
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
    if ($display_file_name) {
      array_unshift($table['header'], 'File Name');
    }
    $content = theme_table($table);

    $element[0] = [
      // We create a render array to produce the desired markup,
      '#type' => 'markup',
      '#markup' => $content,
    ];
  }
}
