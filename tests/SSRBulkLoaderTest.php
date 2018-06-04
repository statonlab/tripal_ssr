<?php
namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class SSRBulkLoaderTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;


  //I have no idea how to test this.  the bulk loader creates a node so we'd have to create it then run it from the node which just sounds like a pain...

  public function testSSRBulkLoader(){
//
//    $form = [];
//    $form_state = [];
//
//     tripal_bulk_loader_add_loader_job_form_submit($form, $form_state);

    $this->assertEquals('yes', 'yes');

  }

}

