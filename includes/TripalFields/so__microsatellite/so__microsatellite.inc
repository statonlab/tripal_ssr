<?php

/**
 * @class
 * Purpose:
 *
 * Data:
 * Assumptions:
 */
class so__microsatellite extends ChadoField
{
    // --------------------------------------------------------------------------
    //                     EDITABLE STATIC CONSTANTS
    //
    // The following constants SHOULD be set for each descendant class.  They are
    // used by the static functions to provide information to Drupal about
    // the field and it's default widget and formatter.
    // --------------------------------------------------------------------------

    // The default label for this field.
    public static $default_label = 'so__microsatellite';

    // The default description for this field.
    public static $default_description = 'Counts and links the SSR (microsatellite) containing features for an organism.';

    // The default widget for this field.
    public static $default_widget = 'so__microsatellite_widget';

    // The default formatter for this field.
    public static $default_formatter = 'so__microsatellite_formatter';

    // The module that manages this field.
    public static $module = 'tripal_ssr';

    // A list of global settings. These can be accessed within the
    // globalSettingsForm.  When the globalSettingsForm is submitted then
    // Drupal will automatically change these settings for all fields.
    // Once instances exist for a field type then these settings cannot be
    // changed.
    public static $default_settings = [
        'storage' => 'field_chado_storage',
        // It is expected that all fields set a 'value' in the load() function.
        // In many cases, the value may be an associative array of key/value pairs.
        // In order for Tripal to provide context for all data, the keys should
        // be a controlled vocabulary term (e.g. rdfs:type). Keys in the load()
        // function that are supported by the query() function should be
        // listed here.
        'searchable_keys' => [],
    ];

    // Provide a list of instance specific settings. These can be access within
    // the instanceSettingsForm.  When the instanceSettingsForm is submitted
    // then Drupal with automatically change these settings for the instance.
    // It is recommended to put settings at the instance level whenever possible.
    // If you override this variable in a child class be sure to replicate the
    // term_name, term_vocab, term_accession and term_fixed keys as these are
    // required for all TripalFields.
    public static $default_instance_settings = [
        // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
        'term_vocabulary' => 'SO',
        // The name of the term.
        'term_name' => 'Microsatellite',
        // The unique ID (i.e. accession) of the term.
        'term_accession' => '0000289',
        // Set to TRUE if the site admin is not allowed to change the term
        // type, otherwise the admin can change the term mapped to a field.
        'term_fixed' => false,
        // Indicates if this field should be automatically attached to display
        // or web services or if this field should be loaded separately. This
        // is convenient for speed.  Fields that are slow should for loading
        // should have auto_attach set to FALSE so tha their values can be
        // attached asynchronously.
        'auto_attach' => false,
        // The table in Chado that the instance maps to.
        'chado_table' => 'organism',
        // The column of the table in Chado where the value of the field comes from.
        'chado_column' => 'organism_id',
        // The base table.
        'base_table' => '',
    ];

    // A boolean specifying that users should not be allowed to create
    // fields and instances of this field type through the UI. Such
    // fields can only be created programmatically with field_create_field()
    // and field_create_instance().
    public static $no_ui = false;

    // A boolean specifying that the field will not contain any data. This
    // should exclude the field from web services or downloads.  An example
    // could be a quick search field that appears on the page that redirects
    // the user but otherwise provides no data.
    public static $no_data = false;

    /**
     * Loads the field values from the underlying data store.
     *
     * @param $entity
     *
     * @return
     *   An array of the following format:
     *     $entity->{$field_name}['und'][0]['value'] = $value;
     *   where:
     *     - $entity is the entity object to which this field is attached.
     *     - $field_name is the name of this field
     *     - 'und' is the language code (in this case 'und' == undefined)
     *     - 0 is the cardinality.  Increment by 1 when more than one item is
     *       available.
     *     - 'value' is the key indicating the value of this field. It should
     *       always be set.  The value of the 'value' key will be the contents
     *       used for web services and for downloadable content.  The value
     *       should be of the follow format types: 1) A single value (text,
     *       numeric, etc.) 2) An array of key value pair. 3) If multiple entries
     *       then cardinality should incremented and format types 1 and 2 should
     *       be used for each item.
     *   The array may contain as many other keys at the same level as 'value'
     *   but those keys are for internal field use and are not considered the
     *   value of the field.
     */
    public function load($entity)
    {
        parent::load($entity);
        $field = 'so__microsatellite';
        $entity->{$field}['und'][0]['value'] = isset($entity->{$field}['und'][0]['chado-organism__organism_id']) ? $entity->{$field}['und'][0]['chado-organism__organism_id'] : $entity->{$field}['und'][0]['chado-feature__organism_id'];

        if (isset($entity->{$field}['und'][0]['chado-organism__organism_id'])) {
            $species = $entity->chado_record->species;
            $genus = $entity->chado_record->genus;
            $entity->{$field}['und'][0]['path'] = $this->create_ssr_url($genus, $species);
        }
        $entity->{$field}['und'][0]['records'] = $this->get_ssr_organism($entity->{$field}['und'][0]['value']);
        $entity->{$field}['und'][0]['count'] = is_array($entity->{$field}['und'][0]['records']) ? count($entity->{$field}['und'][0]['records']) : 0;
    }

    /**
     * Get the SSR for a given organism.
     *
     * @param $organism_id
     *
     * @return mixed
     */
    protected function get_ssr_organism($organism_id)
    {
        if (! $organism_id) {
            return false;
        }

        $sql = "SELECT feature_id FROM {feature_ssr_mview} WHERE organism_id = :organism_id";
        $result = chado_query($sql, [':organism_id' => $organism_id])->fetchAssoc();

        return $result;
    }


    protected function create_ssr_url($genus, $species)
    {
        $path = "ssr-features/{$genus}/{$species}";
        return($path);
=======
class so__microsatellite extends ChadoField{

  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendant class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'so__microsatellite';

  // The default description for this field.
  public static $default_description = 'Counts and links the SSR (microsatellite) containing features for an organism.';

  // The default widget for this field.
  public static $default_widget = 'so__microsatellite_widget';

  // The default formatter for this field.
  public static $default_formatter = 'so__microsatellite_formatter';

  // The module that manages this field.
  public static $module = 'tripal_ssr';

  // A list of global settings. These can be accessed within the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  // Once instances exist for a field type then these settings cannot be
  // changed.
  public static $default_settings = [
    'storage' => 'field_chado_storage',
    // It is expected that all fields set a 'value' in the load() function.
    // In many cases, the value may be an associative array of key/value pairs.
    // In order for Tripal to provide context for all data, the keys should
    // be a controlled vocabulary term (e.g. rdfs:type). Keys in the load()
    // function that are supported by the query() function should be
    // listed here.
    'searchable_keys' => [],
  ];

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'so',
    // The name of the term.
    'term_name' => 'so__microsatellite',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'microsatellite',
    // Set to TRUE if the site admin is not allowed to change the term
    // type, otherwise the admin can change the term mapped to a field.
    'term_fixed' => FALSE,
    // Indicates if this field should be automatically attached to display
    // or web services or if this field should be loaded separately. This
    // is convenient for speed.  Fields that are slow should for loading
    // should have auto_attach set to FALSE so tha their values can be
    // attached asynchronously.
    'auto_attach' => FALSE,
    // The table in Chado that the instance maps to.
    'chado_table' => 'organism',
    // The column of the table in Chado where the value of the field comes from.
    'chado_column' => 'organism_id',
    // The base table.
    'base_table' => '',
  ];

  // A boolean specifying that users should not be allowed to create
  // fields and instances of this field type through the UI. Such
  // fields can only be created programmatically with field_create_field()
  // and field_create_instance().
  public static $no_ui = FALSE;

  // A boolean specifying that the field will not contain any data. This
  // should exclude the field from web services or downloads.  An example
  // could be a quick search field that appears on the page that redirects
  // the user but otherwise provides no data.
  public static $no_data = FALSE;

  /**
   * Loads the field values from the underlying data store.
   *
   * @param $entity
   *
   * @return
   *   An array of the following format:
   *     $entity->{$field_name}['und'][0]['value'] = $value;
   *   where:
   *     - $entity is the entity object to which this field is attached.
   *     - $field_name is the name of this field
   *     - 'und' is the language code (in this case 'und' == undefined)
   *     - 0 is the cardinality.  Increment by 1 when more than one item is
   *       available.
   *     - 'value' is the key indicating the value of this field. It should
   *       always be set.  The value of the 'value' key will be the contents
   *       used for web services and for downloadable content.  The value
   *       should be of the follow format types: 1) A single value (text,
   *       numeric, etc.) 2) An array of key value pair. 3) If multiple entries
   *       then cardinality should incremented and format types 1 and 2 should
   *       be used for each item.
   *   The array may contain as many other keys at the same level as 'value'
   *   but those keys are for internal field use and are not considered the
   *   value of the field.
   */
  public function load($entity) {
    parent::load($entity);
    $field = 'so__microsatellite';
    $entity->{$field}['und'][0]['value'] = isset($entity->{$field}['und'][0]['chado-organism__organism_id']) ? $entity->{$field}['und'][0]['chado-organism__organism_id'] : $entity->{$field}['und'][0]['chado-feature__organism_id'];

    if (isset($entity->{$field}['und'][0]['chado-organism__organism_id'])) {
      $species = $entity->chado_record->species;
      $genus = $entity->chado_record->genus;
      $entity->{$field}['und'][0]['path'] = $this->create_ssr_url($genus, $species);
    }
    $entity->{$field}['und'][0]['records'] = $this->get_ssr_organism($entity->{$field}['und'][0]['chado-organism__organism_id']);
    $entity->{$field}['und'][0]['count'] = $this->get_ssr_features_count($entity->{$field}['und'][0]['chado-organism__organism_id']);
  }

  /**
   * Get the SSR for a given organism.
   *
   * @param $organism_id
   *
   * @return mixed
   */
  protected function get_ssr_organism($organism_id) {
    if (!$organism_id) {
      return FALSE;
    }

    $sql = 'SELECT feature_id FROM {feature_ssr_mview} WHERE organism_id = :organism_id';
    $result = chado_query($sql, [':organism_id' => $organism_id])->fetchAssoc();

    return $result;
  }

  /**
   * Get the SSR features count for a given organism.
   *
   * @param $organism_id
   *
   * @return mixed
   */
  protected function get_ssr_features_count($organism_id) {
    if (!$organism_id) {
      return FALSE;
    }

    $sql = 'SELECT COUNT(feature_id) as features_count FROM {feature_ssr_mview} WHERE organism_id = :organism_id';
    $result = chado_query($sql, [':organism_id' => $organism_id])->fetchObject()->features_count;

    return $result;
  }

  protected function create_ssr_url($genus, $species) {
    $path = "ssr-features/{$genus}/{$species}";
    return ($path);
  }


  //below function create a link to the file download.  For now, let's link to the view instead.

  //protected function create_ssr_url($genus, $species)
  //{
  //    $path = variable_get('file_public_path', conf_path().'/files')."/ssr_downloads/{$genus}_{$species}.ssr.fasta";
  //    if (file_exists($path)) {
  //        return $path;
  //    }
  //
  //    //return null;
  //}
}