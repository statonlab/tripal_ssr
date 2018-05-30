<?php
namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;
require_once(__DIR__ . '/../includes/TripalImporter/SSRLoader.inc');


class SSRLoader extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  // use DBTransaction;
  use DBTransaction;

  public function testImporterRuns(){
    $file = ['file_local' => __DIR__ . '/../example/no_ws_example.bib'];
    $run_args = [];
    $importer = new \SSRLoader();
    $importer->create($run_args, $file);
    $importer->run();


    $query = db_select('chado.feature', 'f')
      ->fields('f', ['title', 'type_id'])
      ->condition('title', 'NO WHITE SPACE');//title from the example file
    $result = $query->execute()->FetchAll();
    $this->assertNotEmpty($result);

  }
}
