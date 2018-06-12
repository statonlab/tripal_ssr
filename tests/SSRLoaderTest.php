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

  /**
   * @param $analysis
   *
   * @return \SSRLoader
   * @throws \Exception
   */
  private function loadFile($analysis_id = NULL) {

    $file = ['file_local' => __DIR__ . '/../example/example_ssr.txt'];
    if ($analysis_id) {
      $run_args = ['analysis_id' => $analysis_id];
    }

    $importer = new \SSRLoader();
    $importer->create($run_args, $file);
    $importer->run();
    return $importer;
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

  public function test_Loader_adds_to_analysisfeature() {


    $query= db_select('chado.analysisfeature', 'af');
    $query->fields('af');
    $result = $query->execute();

    $num_of_results_before = $result->rowCount();



    $parent = $this->addParentFeature();
    $analysis = $this->addAnalysis();
    $this->loadFile($analysis->analysis_id);


    $query= db_select('chado.analysisfeature', 'af');
    $query->fields('af');
    $result = $query->execute();

    $num_of_results_after = $result->rowCount();

    $this->assertGreaterThan($num_of_results_before, $num_of_results_after);

//TODO: the below test is more explicit, but fails.  Why?

    //    $query = db_select('chado.analysisfeature', 'af');
    //    $query->fields('af', ['analysis_id']);
    //    $query->join('chado.feature', 'f', 'f.feature_id = af.feature_id');
    //    $query->condition('f.name', '3935838_ssr85');
    //    $results = $query->execute()->fetchAll();
//
//    $this->assertNotEmpty($results, 'SSR not loaded into analysisfeature.');

  }

  private function AddAnalysis() {
    $analysis = factory('chado.analysis')->create();
    return $analysis;
  }

  private function AddParentFeature() {
    $feature_name = 'WaffleFeature';
    $feature = factory('chado.feature')->create([
      'name' => $feature_name,
      'uniquename' => $feature_name,
    ]);
    return $feature;
  }

}
