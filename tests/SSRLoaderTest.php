<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

require_once(__DIR__ . '/../includes/TripalImporter/SSRLoader.inc');

class SSRLoaderTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  //I'm not sure we need the TripalImporter, if the bulk loader works fine.  we'll see.

  /**
   * Check that the thing does the thing.
   *
   * @throws \Exception
   * @ticket 322
   */
  public function testImporterRuns() {
    $parent = $this->addParentFeature();
    $this->loadFile();
    $query = db_select('chado.feature', 'f')
      ->fields('f', ['name', 'type_id'])
      ->condition('name', '3935838_ssr85');//did we load in our ssr feature?
    $result = $query->execute()->FetchAll();
    $this->assertNotEmpty($result);
  }

  public function testImporterAddsFeatureloc() {
    $parent = $this->addParentFeature();
    $this->loadFile();

    $sql = 'SET search_path to chado, public;
SELECT * FROM {featureloc} 
      WHERE srcfeature_id = :srcfeature_id';

    $args = [
      ':srcfeature_id' => $parent->feature_id,
    ];
    $results = chado_query($sql, $args);
    $this->assertGreaterThan(0, $results);

    foreach ($results as $result) {
      $this->assertObjectHasAttribute('fmin', $result);
      $this->assertObjectHasAttribute('fmax', $result);
    }

  }

  public function testImporterAddsProps() {
    $term = chado_get_cvterm(['id' => 'tripal:tripal_ssr_forward_tm']);
    $prop_term = $term->cvterm_id;

    //get a term used, count num featureprops with it

    $query = db_select('chado.featureprop', 'fp')
      ->fields('fp', ['feature_id'])
      ->condition('type_id', $prop_term);//did
    $count = $query->execute()->rowCount();

    $parent = $this->addParentFeature();

    $this->loadFile();

    //count again, did it go up?

    $query = db_select('chado.featureprop', 'fp')
      ->fields('fp', ['feature_id'])
      ->condition('type_id', $prop_term);
    $new_count = $query->execute()->rowCount();

    $this->assertGreaterThan($count, $new_count, 'No featureprops were added by the importer.');
  }


  public function testImporterAddsRelationship() {
    $parent = $this->addParentFeature();
    $this->loadFile();
    $query = db_select('chado.feature_relationship', 'fr')
      ->fields('fr', ['type_id', 'subject_id'])
      ->condition('object_id', $parent->feature_id);
    $result = $query->execute()->fetchField();
    $this->assertNotFalse($result);
  }


  public function testFind_parent_feature_FindsFeature() {
    $importer = new \SSRLoader();
    $feature = $this->addParentFeature();

    $result = $importer->find_parent_feature('WaffleFeature');

    $this->assertEquals($result->feature_id, $feature->feature_id);

  }


  public function testFind_parent_feature_ReturnsFalseIfNoFeature() {
    $importer = new \SSRLoader();
    $feature = 'gobbledocky not in my database';
    $result = $importer->find_parent_feature($feature);
    $this->assertFalse($result);

  }

  public function test_regexp_for_parent() {
    $parent = $this->addParentFeature();
    $this->loadFile('regex_example.txt', '/WaffleFeature/');
    $query = db_select('chado.feature', 'f')
      ->fields('f', ['name', 'type_id'])
      ->condition('name', 'WaffleFeature_3935838_ssr85');
    $result = $query->execute()->FetchAll();
    $this->assertNotEmpty($result, 'Could not find example feature that was loaded with regexpression');
  }

  public function test_importer_adds_to_analysisfeature() {

    $parent = $this->addParentFeature();

    $this->loadFile();
    $query = db_select('chado.analysis', 'a');
    $query->join('chado.analysisfeature', 'af', 'a.analysis_id = af.analysis_id');
    $query->fields('af', ['analysisfeature_id']);
    $query->condition('a.name', 'ssr_example_test');
    $result = $query->execute()->fetchAll();
    $this->assertNotEmpty($result);
  }

  public function testParentHasProp() {

    $parent = $this->addParentFeature();

    $prop_type = chado_get_cvterm(['id' => 'tripal:tripal_associated_ssr']);
    $this->loadFile();
    $query = db_select('chado.featureprop', 'f');
    $query->fields('f', ['value']);
    $query->condition('type_id', $prop_type->cvterm_id);
    $result = $query->execute()->fetchAll();
    $this->assertNotEmpty($result);

  }

  /**
   * @ticket 46
   * @group failing
   * Features can share names across types ie mRNA and protein.
   */
  public function testHandleParentType() {

    /**
     * sanity check: is our feature absent?
     */
    $query = db_select('chado.feature', 'f')
      ->fields('f', ['name', 'type_id'])
      ->condition('name', '3935838_ssr85');
    $result = $query->execute()->FetchAll();
    $this->assertEmpty($result);

    /**
     * Load improperly
     */
    $mrna_term = chado_get_cvterm(['id' => 'SO:0000234']);
    $parent = $this->addParentFeature();
    $polypeptide_term = chado_get_cvterm(['id' => 'SO:0000104']);
    $parent_two = $this->addParentFeature($polypeptide_term->cvterm_id);
    $this->loadFile();
    $query = db_select('chado.feature', 'f')
      ->fields('f', ['name', 'type_id'])
      ->condition('name', '3935838_ssr85');//we shouldn't load in feature if we can't identify which is the parent
    $result = $query->execute()->FetchAll();
    $this->assertEmpty($result);

    //this time, specify the cvterm
    $this->loadFile('example_ssr.txt', NULL, $mrna_term->cvterm_id);
    $query = db_select('chado.feature', 'f')
      ->fields('f', ['name', 'type_id'])
      ->condition('name', '3935838_ssr85');
    $result = $query->execute()->FetchAll();
    $this->assertNotEmpty($result, 'specifying the type_id did not uniquely identify parent loading the SSR.');

  }

  private function AddParentFeature($type_id = NULL) {

    if (!$type_id) {
      $type = chado_get_cvterm(['id' => 'SO:0000234']);
      $type_id = $type->cvterm_id;
    }
    $feature_name = 'WaffleFeature';
    $feature = factory('chado.feature')->create([
      'name' => $feature_name,
      'uniquename' => $feature_name,
      'type_id' => $type_id,
    ]);
    return $feature;
  }


  /**
   *
   * @return \SSRLoader
   * @throws \Exception
   */
  private function loadFile($file_name = 'example_ssr.txt', $regexp = NULL, $parent_type = NULL) {

    $file = ['file_local' => __DIR__ . '/../example/' . $file_name];

    $analysis = factory('chado.analysis')->create(['name' => 'ssr_example_test']);


    $run_args = ['analysis_id' => $analysis->analysis_id];
    if ($regexp) {
      $run_args['regexp'] = $regexp;
    }
    if ($parent_type) {
      $run_args['p_type_id'] = $parent_type;
    }

    $importer = new \SSRLoader();
    $importer->create($run_args, $file);
    $importer->run();
    return $importer;
  }

}
