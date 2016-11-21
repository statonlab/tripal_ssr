<?php

echo "<div style='position: fixed; width: 500px; height: 500px; top: 0; left: 0; z-index: 999999; background: #fff'>In feature</div>";

if(property_exists($variables['node'], 'feature')) {
  $feature = $variables['node']->feature;
  $feature = chado_expand_var($feature, 'table', 'featureprop', array('return_array' => 1));
  $properties = $feature->featureprop;

  $display = 0;
  if($properties) {
    foreach($properties as $property) {
      if (preg_match("/tripal_ssr_motif/", $property->type_id->name)) {
        $display = 1;
        break; 
      }    
    } 
  }

  $headers = array();
  $rows = array();

  if ($display) {

    foreach($properties as $property) {
      switch($property->type_id->name) {
        case 'tripal_ssr_motif':
          $motif = array(
            array(
              'data' => 'Motif', 
              'header' => TRUE,
              'width' => '20%',
            ),
            $property->value
          );
        break; 

        case 'tripal_ssr_repeats':
          $repeats = array(
            array(
              'data' => 'Number of repeats', 
              'header' => TRUE,
              'width' => '20%',
            ),
            $property->value
          );
        break;

        case 'tripal_ssr_start':
          $start = array(
            array(
              'data' => 'SSR start', 
              'header' => TRUE,
              'width' => '20%',
            ),
            $property->value
          );
        break;

        case 'tripal_ssr_end':
          $end = array(
            array(
              'data' => 'SSR end', 
              'header' => TRUE,
              'width' => '20%',
            ),
            $property->value
          );
        break;

        case 'tripal_ssr_forward_primer':
          $fprimer = array(
            array(
              'data' => 'Forward primer', 
              'header' => TRUE,
              'width' => '20%',
            ),
            $property->value
          );
        break;

        case 'tripal_ssr_reverse_primer':
          $rprimer = array(
            array(
              'data' => 'Reverse primer', 
              'header' => TRUE,
              'width' => '20%',
            ),
            $property->value
          );
        break;
        
        case 'tripal_ssr_forward_tm':
          $ftm = array(
            array(
              'data' => 'Forward Tm', 
              'header' => TRUE,
              'width' => '20%',
            ),
            $property->value
          );
        break;

        case 'tripal_ssr_reverse_tm':
          $rtm = array(
            array(
              'data' => 'Reverse Tm', 
              'header' => TRUE,
              'width' => '20%',
            ),
            $property->value
          );
        break;

        case 'tripal_ssr_product_size':
          $size = array(
            array(
              'data' => 'Product size', 
              'header' => TRUE,
              'width' => '20%',
            ),
            $property->value
          );
        break; 

      } 
    }

    $rows = array(
      $motif,
      $repeats,
      $start,
      $end,
      $fprimer,
      $rprimer,
      $ftm,
      $rtm,
      $size,
    );

     // Generate the table of data provided above. 
    $table = array(
      'header' => $headers,
      'rows' => $rows,
      'attributes' => array(
        'id' => 'tripal_feature-table-ssr',
        'class' => 'tripal-feature-data-table tripal-data-table',
      ),
      'sticky' => FALSE,
      'caption' => '',
      'colgroups' => array(),
      'empty' => '',
    ); 

  print theme_table($table);

  }
}
