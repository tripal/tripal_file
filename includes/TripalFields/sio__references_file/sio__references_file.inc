<?php

class sio__references_file extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'References';

  // The default description for this field.
  public static $description = 'Records referencing this file.';

  // Provide a list of instance specific settings. These can be accessed within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal will automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'SIO',
    // The name of the term.
    'term_name' => 'references',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '000631',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
    // The table in Chado that the instance maps to.
    'chado_table' => 'file',
    // The primary key column of hte table in Dhado.
    'chado_column' => 'file_id',
    // The base table.
    'base_table' => 'file',
  ];

  // The default widget for this field.
  public static $default_widget = 'sio__references_file_widget';

  // The default formatter for this field.
  public static $default_formatter = 'sio__references_file_formatter';

  // A boolean specifying that users should not be allowed to create
  // fields and instances of this field type through the UI. Such
  // fields can only be created programmatically with field_create_field()
  // and field_create_instance().
  public static $no_ui = FALSE;


  /**
   * @see TripalField::elementInfo()
   */
  public function elementInfo() {
    $field_term = $this->getFieldTermID();
    return [
      $field_term => [
        'operations' => [],
        'sortable' => FALSE,
        'searchable' => FALSE,
        'type' => 'xs:complexType',
        'readonly' => TRUE,
        'elements' => [
          'entity' => [
            'searchable' => FALSE,
          ],
        ]
      ],
    ];
  }

  /**
   *
   * @see TripalField::load()
   */
  public function load($entity) {

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];

    // Set some defaults for the empty record.
    $chado_record = $entity->chado_record;
    $entity->{$field_name}['und'][0] = [
      'value' => '',
    ];

    // Iterate through all of the _file tables and look for any that have
    // linked to this record. If so then add them.
    $chado_tables = chado_get_table_names(TRUE);
    $delta = 0;
    foreach ($chado_tables as $chado_table) {
      $matches = [];
      if (preg_match('/^(.+?)_file$/', $chado_table, $matches)) {

        $reference_table = $matches[1];

        // Get the schema for the file linker table table.
        $schema = chado_get_schema($chado_table);

        // Skip tables that don't have a foreign key definition.
        if (!array_key_exists('foreign keys', $schema)) {
          continue;
        }

        // Get information about the linking table.
        $fkeys = $schema['foreign keys'];
        $fkleft = NULL;
        $fkright = NULL;
        $islinked = FALSE;
        foreach ($fkeys as $linked_table => $fk_details) {
          if ($linked_table == 'file') {
            $islinked = TRUE;
          }
          if ($linked_table == $reference_table) {
            $fkleft = array_keys($fk_details['columns'])[0];
            $fkright = $fk_details['columns'][$fkleft];
          }
        }
        // If this table doesn't have an FK to a reference table
        // then it's just a table with a _file in the name.
        if (!$fkleft) {
          continue;
        }
        // If this table does not have a FK to the file table then
        // we don't want to search it.
        if (!$islinked) {
          continue;
        }


        // Build the SQL to find records associated with this file.
        $ref_schema = chado_get_schema($reference_table);
        $ref_pkey = $ref_schema['primary key'][0];
        $select = "SELECT REF.* ";
        $from = "FROM {" . $chado_table . "} LINK
            INNER JOIN {" . $reference_table . "} REF on LINK.$fkleft = REF.$fkright
        ";
        if (array_key_exists('type_id', $ref_schema['fields'])) {
          $select .= ", CVT.name as type_name";
          $from .= "INNER JOIN {cvterm} CVT on REF.type_id = CVT.cvterm_id ";
        }

        // Are the records in this table associated with a content type?
        // if so, we want to get those types so we can find the entity ID.
        $bundles = db_select('chado_bundle', 'CB')
          ->fields('CB', ['bundle_id'])
          ->condition('CB.data_table', $reference_table)
          ->execute();
        $entity_sql = '';
        while ($bundle_id = $bundles->fetchField()) {
          $entity_sql .= "SELECT CONCAT(entity_id,'|',label) as entity ".
            "FROM [chado_bio_data_" . $bundle_id . "] CBD" . $bundle_id . " " .
            "  INNER JOIN [tripal_bundle] TB ON TB.id = " . $bundle_id . " " .
            "WHERE record_id = LINK.$ref_pkey UNION ";
        }
        if (!empty($entity_sql)) {
          $entity_sql = rtrim($entity_sql, " UNION ");
          $entity_sql = ", (" . $entity_sql . " LIMIT 1) as entity_id";
        }

        // Iterate through all of the records in the linker table that
        // match the given file ID.
        $sql = "$select $entity_sql $from WHERE LINK.file_id = :file_id";
        $args = [':file_id' => $chado_record->file_id];
        $records = chado_query($sql, $args);
        while($record = $records->fetchObject()) {

          // We want to add a 'type' and 'name' element to the values (at a
          // minimum) for each of the records.  Unfortunately, every base table
          // is different and there may not be an easy to identify name,
          // so... we'll do the best we can.
          $entity->{$field_name}['und'][$delta]['value'] = [];

          // If this record is also a published entity then include that.
          $ref_type = NULL;
          if (property_exists($record, 'entity_id') and !empty($record->entity_id)) {
            [$entity_id, $ref_type] = explode('|', $record->entity_id);
            $entity->{$field_name}['und'][$delta]['value']['entity'] = 'TripalEntity:' . $entity_id;
          }

          // First get the type of record.
          if (property_exists($record, 'type_name')) {
            $entity->{$field_name}['und'][$delta]['value']['rdfs:type'] = $record->type_name;
          }
          else if ($ref_type) {
            $entity->{$field_name}['und'][$delta]['value']['rdfs:type'] = $ref_type;
          }

          // Add in the name and uniquename (identifier) if those fields exist.
          // If name does not exist, e.g. for publication, use title
          if (property_exists($record, 'uniquename')) {
            $entity->{$field_name}['und'][$delta]['value']['data:0842'] = $record->uniquename;
          }
          if (property_exists($record, 'name')) {
            $entity->{$field_name}['und'][$delta]['value']['schema:name'] = $record->name;
          }
          else if (property_exists($record, 'title')) {
            $entity->{$field_name}['und'][$delta]['value']['schema:name'] = $record->title;
          }
          else {
            // if we reach here, a new handler needs to be added, but for now add a placeholder
            $entity->{$field_name}['und'][$delta]['value']['schema:name'] = 'Link';
          }

          // If this is the organism table then we will create the name
          // specially.
          if (property_exists($record, 'genus')) {
            $name = $record->genus . ' ' . $record->species;
            if (function_exists('chado_get_organism_scientific_name')) {
              $name = chado_get_organism_scientific_name(chado_get_organism(['organism_id' => $record->organism_id], []));
            }
            $entity->{$field_name}['und'][$delta]['value']['schema:name'] = $name;
            // revert back to $ref_type of 'Organism' because the type_id will be
            // for the infraspecific type
            $entity->{$field_name}['und'][$delta]['value']['rdfs:type'] = $ref_type;
          }
          $delta++;
        }
      }
    }
  }

}
