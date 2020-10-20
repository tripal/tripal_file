<?php

class sio__file extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'File';

  // The default description for this field.
  public static $description = 'A file.';

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
    'term_name' => 'file',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '000396',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  ];

  // The default widget for this field.
  public static $default_widget = 'sio__file_widget';

  // The default formatter for this field.
  public static $default_formatter = 'sio__file_formatter';

  // --------------------------------------------------------------------------
  //              PROTECTED CLASS MEMBERS -- DO NOT OVERRIDE
  // --------------------------------------------------------------------------
  // An array containing details about the field. The format of this array
  // is the same as that returned by field_info_fields()
  protected $field;

  // An array containing details about an instance of the field. A field does
  // not have to have an instance.  But if dealing with an instance (such as
  // when using the widgetForm, formatterSettingsForm, etc.) it should be set.
  protected $instance;


  /**
   * @see TripalField::elements()
   */
  public function elementInfo() {

    $field_term = $this->getFieldTermID();

    // All of the file linker tables have the same fields so we'll
    // use the feature_file table to get the cvterms.
    $name_term = chado_get_semweb_term('feature_file', 'name');
    $type_term = chado_get_semweb_term('feature_file', 'type_id');
    $rank_term = chado_get_semweb_term('feature_file', 'rank');

    return [
      $field_term => [
        'operations' => ['eq', 'contains', 'starts'],
        'sortable' => TRUE,
        'searchable' => TRUE,
        'type' => 'xs:complexType',
        'readonly' => FALSE,
        'elements' => [
          $name_term => [
            'searchable' => TRUE,
            'label' => 'Name',
            'help' => 'The name of the file.',
            'operations' => [],
            'sortable' => TRUE,
            'type' => 'xs:integer',
            'readonly' => TRUE,
            'required' => FALSE,
          ],
          $type_term => [
            'searchable' => FALSE,
            'label' => 'Type',
            'help' => 'The type of file.',
            'operations' => [],
            'sortable' => FALSE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => FALSE,
          ],
          $rank_term => [
            'searchable' => FALSE,
            'label' => 'Rank',
            'help' => 'The order that this file should appear in the list when multiple files are available.',
            'operations' => [],
            'sortable' => TRUE,
            'type' => 'xs:integer',
            'readonly' => FALSE,
            'required' => FALSE,
          ],
          'entity' => [
            'searchable' => FALSE,
          ],
        ],
      ],
    ];
  }

  /**
   *
   * @see TripalField::load()
   */
  public function load($entity) {
    $record = $entity->chado_record;

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];

    $name_term = chado_get_semweb_term('feature', 'name');
    $type_term = chado_get_semweb_term('feature', 'type_id');
    $rank_term = chado_get_semweb_term('feature_file', 'rank');

    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];
    $linker_table = $base_table . '_file';

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = [
      'value' => [],
      'chado-' . $field_table . '__' . $pkey => '',
      'chado-' . $field_table . '__' . $fkey_lcolumn => '',
      'chado-' . $field_table . '__' . 'file_id' => '',
      'chado-' . $field_table . '__' . 'rank' => '',
    ];

    // Get the list of publications
    $files = [];
    if ($linker_table) {
      $options = [
        'return_array' => TRUE,
        'include_fk' => [
          'file_id' => [
            'type_id' => TRUE,
          ],
        ],
        'order_by' => [
          'rank' => 'ASC'
        ]
      ];
      $record = chado_expand_var($record, 'table', $linker_table, $options);
      $linked_files = $record->$linker_table;
      if ($linked_files) {
        $i = 0;
        foreach ($linked_files as $linked_file) {
          $entity->{$field_name}['und'][$i] = [
            'value' => [
              $name_term => $linked_file->file_id->name,
              $type_term => $linked_file->file_id->type_id->name,
              $rank_term => $linked_file->rank,
            ],
            'chado-' . $field_table . '__' . $pkey => $linked_file->$pkey,
            'chado-' . $field_table . '__' . $fkey_lcolumn => $linked_file->$fkey_lcolumn,
            'chado-' . $field_table . '__' . 'file_id' => $linked_file->file_id->file_id,
            'chado-' . $field_table . '__' . 'rank' => $linked_file->rank,
          ];
          // Is there a published entity for this license?
          if (property_exists($linked_file->file_id, 'entity_id')) {
            $entity->{$field_name}['und'][$i]['value']['entity'] = 'TripalEntity:' . $linked_file->file_id->entity_id;
          }
          $i++;
        }
      }
    }
  }
}
