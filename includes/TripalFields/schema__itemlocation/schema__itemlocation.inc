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
  public static $default_label = 'File Location';

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
    $filename_term = chado_get_semweb_term('fileloc', 'filename');
    $rank_term = chado_get_semweb_term('fileloc', 'rank');
    $md5_term = chado_get_semweb_term('fileloc', 'md5checksum');
    $size_term = chado_get_semweb_term('fileloc', 'size');

    return [
      $field_term => [
        'operations' => ['eq', 'contains', 'starts'],
        'sortable' => FALSE,
        'searchable' => FALSE,
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
          $filename_term => [
            'searchable' => FALSE,
            'label' => 'Download File Name',
            'help' => 'The file name displayed to the user',
            'operations' => [],
            'sortable' => FALSE,
            'type' => 'xs:string',
            'readonly' => FALSE,
            'required' => FALSE,
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
          $size_term => [
            'searchable' => FALSE,
            'label' => 'File Size',
            'help' => 'The size of the file.',
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
    $filename_term = chado_get_semweb_term('fileloc', 'filename');
    $rank_term = chado_get_semweb_term('fileloc', 'rank');
    $md5_term = chado_get_semweb_term('fileloc', 'md5checksum');
    $size_term = chado_get_semweb_term('fileloc', 'size');

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = [
      'value' => [],
      'chado-fileloc__fileloc_id' => '',
      'chado-fileloc__file_id' => '',
      'chado-fileloc__uri' => '',
      'chado-fileloc__filename' => '',
      'chado-fileloc__rank' => '',
      'chado-fileloc__md5checksum' => '',
      'chado-fileloc__size' => '',
    ];

    if ($record) {
      $options = [
        'return_array' => 1,
        'order_by' => ['rank' => 'ASC'],
      ];
      $record = chado_expand_var($record, 'table', 'fileloc', $options);
      $fileloc_linkers = $record->fileloc;
      if ($fileloc_linkers) {
        foreach ($fileloc_linkers as $i => $fileloc) {
          $filesize = $fileloc->size;  // integer or empty string
          if (preg_match('/^\d+$/', $filesize)) {
            $filesize = tripal_format_bytes($filesize);
          }
          $uri = file_create_url($fileloc->uri);
          // ftp:// won't pass through file_create_url(), so use value directly.
          if (!$uri) {
            $uri = $fileloc->uri;
          }
          $filename = $this->extractFileName($uri, $fileloc->filename);
          $entity->{$field_name}['und'][$i] = [
            'value' => [
              $uri_term => $uri,
              $filename_term => $filename,
              $rank_term => $fileloc->rank,
              $md5_term => $fileloc->md5checksum,
              $size_term => $filesize,
            ],
            'chado-fileloc__fileloc_id' => $fileloc->fileloc_id,
            'chado-fileloc__file_id' => $fileloc->file_id->file_id,
            'chado-fileloc__uri' => $fileloc->uri,
            'chado-fileloc__filename' => $filename,
            'chado-fileloc__rank' => $fileloc->rank,
            'chado-fileloc__md5checksum' => $fileloc->md5checksum,
            'chado-fileloc__size' => $fileloc->size,
          ];
        }
      }
    }
  }

  /**
   * Unless a file name is given, extract one from the uri.
   */
  private function extractFileName($uri, $filename) {
    if (!$filename) {
      // An explicit file name is not provided, so construct one from the uri
      // by removing the path portion e.g. "https://a/b/c.txt" becomes "c.txt"
      $filename = basename($uri);
      // URLs can be a messy POST with multiple parameters, which is hard to
      // simplify, but we can at least remove anything before a question mark,
      // e.g. downloadSupplement?doi=10.123%2F456&file=filename.txt
      $filename = preg_replace('/^.*\?/', '', $filename);
    }
    return($filename);
  }
}
