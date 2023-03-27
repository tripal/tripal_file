<?php

/**
 * @file
 * Provides an application programming interface (API) for working with the tripal_file module
 */

/**
 * @defgroup tripal_file_api Example API
 * @ingroup tripal_api
 * @{
 * Provides an application programming interface (API) for working with files
 * @}
 */

/**
 * Used for autocomplete in forms for identifying files
 *
 * @param $string
 *   The string to search for.
 *
 * @return
 *   A json array of terms that begin with the provided string.
 *
 * @ingroup tripal_file_api
 */
function chado_autocomplete_file($string = '') {
  $items = [];
  $sql = "
    SELECT
      F.file_id as id, F.name
    FROM {file} F
    WHERE lower(F.name) like lower(:str)
    ORDER by F.name
    LIMIT 25 OFFSET 0
  ";
  $records = chado_query($sql, [':str' => $string . '%']);
  while ($r = $records->fetchObject()) {
    $key = "$r->name [id: $r->id]";
    $items[$key] = "$r->name";
  }

  drupal_json_output($items);
}
