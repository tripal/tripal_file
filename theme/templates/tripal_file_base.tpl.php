<?php
$file  = $variables['node']->file;  ?>

<div class="tripal_file-data-block-desc tripal-data-block-desc"></div> <?php
 
// the $headers array is an array of fields to use as the colum headers. 
// additional documentation can be found here 
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
// This table for the analysis has a vertical header (down the first column)
// so we do not provide headers here, but specify them in the $rows array below.
$headers = array();

// the $rows array contains an array of rows where each row is an array
// of values for each column of the table in that row.  Additional documentation
// can be found here:
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7 
$rows = array();

// Unique Name row
$rows[] = array(
  array(
    'data' => 'Unique Name',
    'header' => TRUE
  ),
  $file->uniquename
);
// Type row
$rows[] = array(
  array(
    'data' => 'Type',
    'header' => TRUE
  ),
  $file->type_id->name
);
// Organism row
$organism = $file->organism_id->genus ." " . $file->organism_id->species ." (" . $file->organism_id->common_name .")";
if (property_exists($file->organism_id, 'nid')) {
  $organism = l("<i>" . $file->organism_id->genus . " " . $file->organism_id->species . "</i> (" . $file->organism_id->common_name .")", "node/".$file->organism_id->nid, array('html' => TRUE));
} 
$rows[] = array(
  array(
    'data' => 'Organism',
    'header' => TRUE,
  ),
  $organism
);

// allow site admins to see the file ID
if (user_access('view ids')) { 
  // Feature ID
  $rows[] = array(
    array(
      'data' => 'file ID',
      'header' => TRUE,
      'class' => 'tripal-site-admin-only-table-row',
    ),
    array(
      'data' => $file->file_id,
      'class' => 'tripal-site-admin-only-table-row',
    ),
  );
}


// the $table array contains the headers and rows array as well as other
// options for controlling the display of the table.  Additional
// documentation can be found here:
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
$table = array(
  'header' => $headers,
  'rows' => $rows,
  'attributes' => array(
    'id' => 'tripal_file-table-base',
    'class' => 'tripal-data-table'
  ),
  'sticky' => FALSE,
  'caption' => '',
  'colgroups' => array(),
  'empty' => '',
);

// once we have our table array structure defined, we call Drupal's theme_table()
// function to generate the table.
print theme_table($table); ?>
<div style="text-align: justify"><?php print $file->description ?></div>