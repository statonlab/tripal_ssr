<?php
namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class SeederTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
   use DBTransaction;

  /**
   * Basic test example.
   * Tests must begin with the word "test".
   * See https://phpunit.readthedocs.io/en/latest/ for more information.
   * @group test
   */
  public function testBasicExample() {

    $seed = DatabaseSeeders\DevSeedSeeder::seed();


    $this->assertTrue(true);
  }
}
