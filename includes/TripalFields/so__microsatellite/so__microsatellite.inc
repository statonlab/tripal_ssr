<?php

/**
 * @class
 * Purpose:
 *
 * Data:
 * Assumptions:
 */
class so__microsatellite extends ChadoField {

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
    'term_vocabulary' => 'SO', // The database, not the cv
    // The name of the term.
    'term_name' => 'so__microsatellite',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '0000289',
    // Set to TRUE if the site admin is not allowed to change the term
    // type, otherwise the admin can change the term mapped to a field.
    'term_fixed' => FALSE,
    // Indicates if this field should be automatically attached to display
    // or web services or if this field should be loaded separately. This
    // is convenient for speed.  Fields that are slow should for loading
    // should have auto_attach set to FALSE so tha their values can be
    // attached asynchronously.
    'auto_attach' => FALSE,
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

    $entity_table = $entity->chado_table;

    if ($entity_table == 'organism') {
      $entity->{$field}['und'][0]['value'] = isset($entity->{$field}['und'][0]['chado-organism__organism_id']) ? $entity->{$field}['und'][0]['chado-organism__organism_id'] : $entity->{$field}['und'][0]['chado-feature__organism_id'];

      if (isset($entity->{$field}['und'][0]['chado-organism__organism_id'])) {
        $species = $entity->chado_record->species;
        $genus = $entity->chado_record->genus;
        $entity->{$field}['und'][0]['path'] = $this->create_ssr_url($genus, $species);
      }

      $entity->{$field}['und'][0]['records'] = $this->get_ssr_organism($entity->{$field}['und'][0]['value']);
      $entity->{$field}['und'][0]['count'] = $this->get_ssr_features_count($entity->{$field}['und'][0]['value']);
      if ($entity->{$field}['und'][0]['count'] === 0) {
        $entity->{$field}['und'][0]['value'] = NULL;
      }
      return;
    }

    if ($entity_table == 'feature') {

      $entity->{$field}['und'][0] = NULL;
      $i = 0;
      $record_id = $entity->chado_record->feature_id;
      $ssrs = $this->get_related_SSRs($record_id);
      if (!$ssrs) {
        return;
      }

      foreach ($ssrs as $ssr) {
        $id = $ssr->subject_id;
        $name = $ssr->name;
        $uname = $ssr->uniquename;

        $entity->{$field}['und'][$i]['value'] = [
          'name' => $name,
          'unique_name' => $uname,
        ];
        $entity->{$field}['und'][$i]['feature_id'] = $id;

        $props = $this->get_ssr_props($id);
        if ($props) {
          foreach ($props as $prop) {
            $entity->{$field}['und'][$i]['value'][$prop['name']] = $prop['value'];
          }
        }

        $i++;
      }
    }
  }

  /**
   * @param $feature_record
   */
  function get_related_SSRs($feature_record) {

    $ssr_cvterm = chado_get_cvterm(['id' => 'SO:0000289']);
    $ssr_type_id = $ssr_cvterm->cvterm_id;

    $query = db_select('chado.feature_relationship', 'fr');
    $query->fields('fr', ['subject_id']);
    $query->join('chado.feature', 'f', 'fr.subject_id = f.feature_id');
    $query->fields('f', ['name', 'uniquename']);
    $query->condition('fr.object_id', $feature_record);
    $query->condition('f.type_id', $ssr_type_id);
    $ssrs = $query->execute()->fetchAll();
    // $query->join('chado.feature', 'fs', 'fr.subject_id = f.feature_id');
    // $query->join('chado.feature', 'f', 'fr.object_id = f.feature_id');
    return $ssrs;
  }

  function get_ssr_props($ssr_id) {

    $query = db_select('chado.featureprop', 'fp');
    $query->join('chado.cvterm', 'cvt', 'fp.type_id = cvt.cvterm_id');
    $query->fields('cvt', ['name']);
    $query->fields('fp', ['value']);
    $query->condition('fp.feature_id', $ssr_id);

    //see loader for terms.

    $query->condition('cvt.name', [
      'tripal_ssr_motif',
      'tripal_ssr_repeats',
      'tripal_ssr_forward_primer',
      'tripal_ssr_reverse_primer',
      'tripal_ssr_forward_tm',
      'tripal_ssr_reverse_tm',
      'tripal_ssr_product_size',
    ], 'IN');

    $results = $query->execute()->fetchAll();
    $props = [];

    if (!$results) {
      return FALSE;
    }
    foreach ($results as $result) {
      $name = $result->name;
      $value = $result->value;

      $props[] = ['name' => $name, 'value' => $value];
    }
    return $props;
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