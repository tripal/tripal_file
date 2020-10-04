<?php

class schema__itemlocation extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Location';

  // The default description for this field.
  public static $description = 'The location of the file.';

  // Provide a list of instance specific settings. These can be accessed within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal will automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'schema',
    // The name of the term.
    'term_name' => 'itemLocation',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'itemLocation',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  ];

  // The default widget for this field.
  public static $default_widget = 'chado_schema__itemlocation_widget';

  // The default formatter for this field.
  public static $default_formatter = 'chado_schema__itemlocation_formatter';

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
    $uri_term = chado_get_semweb_term('fileloc', 'uri');
    $rank_term = chado_get_semweb_term('fileloc', 'rank');
    $md5_term = chado_get_semweb_term('fileloc', 'md5checksum');

    return [
      $field_term => [
        'operations' => ['eq', 'contains', 'starts'],
        'sortable' => TRUE,
        'searchable' => TRUE,
        'type' => 'xs:complexType',
        'readonly' => FALSE,
        'elements' => [
          $uri_term => [
            'searchable' => FALSE,
            'label' => 'URI',
            'help' => 'The Universal Resource Identifier for the file',
            'operations' => [],
            'sortable' => FALSE,
            'type' => 'xs:string',
            'readonly' => FALSE,
            'required' => TRUE,
          ],
          $rank_term => [
            'searchable' => FALSE,
            'label' => 'Rank',
            'help' => 'The order that this location should appear in the list when multiple locations are available.',
            'operations' => [],
            'sortable' => FALSE,
            'type' => 'xs:integer',
            'readonly' => FALSE,
            'required' => TRUE,
          ],
          $md5_term => [
            'searchable' => FALSE,
            'label' => 'MD5 Checksum',
            'help' => 'The MD5 checksum of the file.',
            'operations' => [],
            'sortable' => FALSE,
            'type' => 'xs:string',
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

    $uri_term = chado_get_semweb_term('fileloc', 'uri');
    $rank_term = chado_get_semweb_term('fileloc', 'rank');
    $md5_term = chado_get_semweb_term('fileloc', 'md5checksum');

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = [
      'value' => [],
      'chado-fileloc__fileloc_id' => '',
      'chado-fileloc__file_id' => '',
      'chado-fileloc__uri' => '',
      'chado-fileloc__rank' => '',
      'chado-fileloc__md5checksum' => '',
    ];

    if ($record) {
      $options = ['return_array' => 1];
      $record = chado_expand_var($record, 'table', 'fileloc', $options);
      $fileloc_linkers = $record->fileloc;
      if ($fileloc_linkers) {
        foreach ($fileloc_linkers as $i => $fileloc) {
          $entity->{$field_name}['und'][$i] = [
            'value' => [
              $uri_term => file_create_url($fileloc->uri),
              $rank_term => $fileloc->rank,
              $md5_term => $fileloc->md5checksum,
            ],
            'chado-fileloc__fileloc_id' => $fileloc->fileloc_id,
            'chado-fileloc__file_id' => $fileloc->file_id->file_id,
            'chado-fileloc__uri' => $fileloc->uri,
            'chado-fileloc__rank' => $fileloc->rank,
            'chado-fileloc__md5checksum' => $fileloc->md5checksum,
          ];
        }
      }
    }
  }
}