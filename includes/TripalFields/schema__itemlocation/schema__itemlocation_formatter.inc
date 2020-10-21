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

    $content = [];
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
         if ($base_url == 'http://' . $url_parts['host']) {
           $source = variable_get('site_name');
         }
       }
       $uri = l('Download', $fileloc[$uri_term]);
       $size = !empty($item['value'][$size_term]) ? tripal_format_bytes($item['value'][$size_term]) : 'N/A';
       $md5 = !empty($item['value'][$md5_term]) ? $item['value'][$md5_term] : 'N/A';

       $element[0][] = [
         '#type' => 'item',
         '#title' => 'Source: ' . $source,
         '#markup' => '<p>' . $uri . '<br>Size: ' . $size . '<br> MD5: ' . $md5 . '</p>',
       ];
     }

    $headers = ['Available at', 'Size', 'MD5'];
    if (count($items) == 0) {
      $element[0] = [
        // We create a render array to produce the desired markup,
        '#type' => 'markup',
        '#markup' => 'This file is currently not available for retrieval',
      ];
    }
  }
}
