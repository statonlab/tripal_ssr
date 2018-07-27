<?php
namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;
use test_module\tests\Feature\DatabaseSeederTest;

class SeederTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
   use DBTransaction;


  /**
   * @group test
   */
   public function testBasicsImporter(){
     $seed = DatabaseSeeders\DevSeedSeeder::seed();

     $query = db_select('chado.organism', 'o');
     $query->fields('o', ['genus']);
     $query->condition('o.common_name', 'F. excelsior miniature');
     $result = $query->execute()->fetchField();
     $this->assertNotEmpty($result);


     $query = db_select('chado.analysis', 'a');
     $query->fields('a', ['name']);
     $query->condition('a.name', 'Fraxinus exclesior miniature dataset');
     $result = $query->execute()->fetchField();
     $this->assertNotEmpty($result);
   }
  /**
   * Basic test example.
   * Tests must begin with the word "test".
   * See https://phpunit.readthedocs.io/en/latest/ for more information.
   * @group test
   */
  public function testGFFImporter() {

    $seed = DatabaseSeeders\DevSeedSeeder::seed();

    $query = db_select('chado.feature', 'f');
    $query->fields('f', ['name']);
    $query->join('chado.organism', 'o', 'o.organism_id = f.organism_id');
    $query->condition('o.common_name', 'F. excelsior miniature');
    $result = $query->execute()->fetchAll();
    $this->assertNotEmpty($result);

  }
}
