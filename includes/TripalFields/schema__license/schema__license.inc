<?php

class schema__license extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'License';

  // The default description for this field.
  public static $description = 'A license describing access and usage permissions.';

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
    'term_name' => 'license',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'license',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  ];

  // The default widget for this field.
  public static $default_widget = 'chado_schema__license_widget';

  // The default formatter for this field.
  public static $default_formatter = 'chado_schema__license_formatter';

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
    $name_term = chado_get_semweb_term('license', 'name');
    $summary_term = chado_get_semweb_term('license', 'summary');
    $uri_term = chado_get_semweb_term('license', 'uri');

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
            'help' => 'The Universal Resource Identifier for the license.',
            'operations' => [],
            'sortable' => FALSE,
            'type' => 'xs:string',
            'readonly' => FALSE,
            'required' => TRUE,
          ],
          $name_term => [
            'searchable' => TRUE,
            'label' => 'Name',
            'help' => 'The name of the license.',
            'operations' => [],
            'sortable' => TRUE,
            'type' => 'xs:integer',
            'readonly' => FALSE,
            'required' => TRUE,
          ],
          $summary_term => [
            'searchable' => FALSE,
            'label' => 'Summary',
            'help' => 'A brief summary describing the user privileges under the license.',
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

    $name_term = chado_get_semweb_term('license', 'name');
    $summary_term = chado_get_semweb_term('license', 'summary');
    $uri_term = chado_get_semweb_term('license', 'uri');

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = [
      'value' => [],
      'chado-file_license__file_license_id' => '',
      'chado-file_license__file_id' => '',
      'chado-file_license__license_id' => '',
    ];

    if ($record) {
      $options = ['return_array' => 1];
      $record = chado_expand_var($record, 'table', 'file_license', $options);
      $file_licenses = $record->file_license;
      if ($file_licenses) {
        foreach ($file_licenses as $i => $license) {
          $license = chado_expand_var($license, 'field', 'license.summary');
          $entity->{$field_name}['und'][$i] = [
            'value' => [
              $name_term => $license->license_id->name,
              $summary_term => $license->license_id->summary,
              $uri_term => $license->license_id->uri,
            ],
            'chado-file_license__file_license_id' => $license->file_license_id,
            'chado-file_license__file_id' => $license->file_id->file_id,
            'chado-file_license__license_id' => $license->license_id->license_id,
          ];
          // Is there a published entity for this license?
          if (property_exists($license->license_id, 'entity_id')) {
            $entity->{$field_name}['und'][0]['value']['entity'] = 'TripalEntity:' . $license->license_id->entity_id;
          }
        }
      }
    }
  }
}
