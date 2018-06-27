<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

//module_load_include('install', 'tripal_ssr', 'tripal_ssr');
//require_once(__DIR__ . '/../tripal_ssr.install');
//cant get this to work! moving function to .module.

class MigrateExistingInstallTest extends TripalTestCase {

  use DBTransaction;

  public function testMigrateExisting() {

    $mrna_term = chado_get_cvterm(['id' => 'SO:0000234']);
    $feature = factory('chado.feature')->create(['type_id' => $mrna_term->cvterm_id]);
    $this->addOldStyleProps($feature);
   \tripal_ssr_migrate_existing_ssrs();

    //verify old props are gone
    $query = db_select('chado.feature', 'f');
    $query->join('chado.featureprop', 'fp', 'fp.feature_id = f.feature_id');
    $query->fields('fp', ['value']);
    $query->condition('f.feature_id', $feature->feature_id);

    $results = $query->execute()->fetchAll();

    foreach ($results as $result){
      ///values specified in addOldStyleProps
      $vals = ['800', '20', 'atgcatgcatgc', 'aaaa'];
      foreach ($vals as $val){
      }
      $this->assertNotEquals($val, $result->value);
    }

    //verify new relationship of type ssr to this feature.

    $query = db_select('chado.feature', 'f');
    $query->join('chado.feature_relationship', 'fr', 'fr.object_id = f.feature_id');
    $query->fields('f');
    $query->join('chado.feature', 'ftwo', 'fr.subject_id = ftwo.feature_id');
    $query->condition();
    $query->execute();
    $count = $query->rowCount();
    $this->assertNotEquals(0,$count, "SSR not re-added as a feature");

  }


  private function addOldStyleProps($feature) {


    //attach the props

    $props = [
      [
        'value' => 'aaaa',
        'type_name' => 'tripal_ssr_motif',
        'cv_name' => 'feature_property',
      ],
      [
        'value' => '10',
        'type_name' => 'tripal_ssr_repeats',
        'cv_name' => 'feature_property',
      ],
      [
        'value' => 'atgcatgcatgc',
        'type_name' => 'tripal_ssr_forward_primer',
        'cv_name' => 'feature_property',
      ],
      [
        'value' => 'tcagtcagtcag',
        'type_name' => 'tripal_ssr_reverse_primer',
        'cv_name' => 'feature_property',
      ],
      [
        'value' => '20',
        'type_name' => 'tripal_ssr_forward_tm',
        'cv_name' => 'feature_property',
      ],
      [
        'value' => '20',
        'type_name' => 'tripal_ssr_reverse_tm',
        'cv_name' => 'feature_property',
      ],
      [
        'value' => '800',
        'type_name' => 'tripal_ssr_product_size',
        'cv_name' => 'feature_property',
      ],

    ];

    $record = ['id' => $feature->feature_id, 'table' => 'feature'];
    $options = ['update_if_present' => TRUE];

    foreach ($props as $prop) {
      $property = [
        'type_name' => $prop['type_name'],
        'cv_name' => $prop['cv_name'],
        'value' => $prop['value'],
      ];
      chado_insert_property($record, $property, $options);
    }
  }
}
