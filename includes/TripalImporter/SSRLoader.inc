<?php

class SSRLoader extends TripalImporter {

  /**
   * The name of this loader.  This name will be presented to the site
   * user.
   */
  public static $name = 'Chado SSR Loader';

  /**
   * The machine name for this loader. This name will be used to construct
   * the URL for the loader.
   */
  public static $machine_name = 'chado_ssr_loader';

  /**
   * A brief description for this loader.  This description will be
   * presented to the site user.
   */
  public static $description = 'Load SSR and primer info as output by a statonlab perl script.  The extension must be .txt or .tsv';

  /**
   * An array containing the extensions of allowed file types.
   */
  public static $file_types = [
    'txt',
    'tsv',
  ];

  /**
   * Allow, and require, an analysis.
   */
  public static $use_analysis = TRUE;

  public static $require_analysis = TRUE;

  /**
   * Provides information to the user about the file upload.  Typically this
   * may include a description of the file types allowed.
   */
  public static $upload_description = 'Please provide the file. The file must have a .txt or .tsv extension.';

  /**
   * The title that should appear above the file upload section.
   */
  public static $upload_title = 'SSR File Upload';

  /**
   * Text that should appear on the button at the bottom of the importer
   * form.
   */
  public static $button_text = 'Import SSR file';


  /**
   * Indicates the methods that the file uploader will support.
   */
  public static $methods = [
    // Allow the user to upload a file to the server.
    'file_upload' => TRUE,
    // Allow the user to provide the path on the Tripal server for the file.
    'file_local' => TRUE,
    // Allow the user to provide a remote URL for the file.
    'file_remote' => TRUE,
  ];

  /**
   * @see TripalImporter::form()
   */
  public function form($form, &$form_state) {


    $form['instructions'] = [
      '#markup' => '
<h2>An SSR importer.</h2>
<p>More information coming soon.</p>
',
    ];


    $form['regexp'] = [
      '#type' => 'textfield',
      '#title' => 'Regular expression for parent',
      '#description' => 'A regular expression that uniquely defines the parent.  Use this field with a 10 column file, the first column being an SSR name that also contains the parent.',
    ];

    return $form;
  }

  /**
   * @see TripalImporter::formValidate()
   */
  public function formValidate($form, &$form_state) {

    $regexp = trim($form_state['input']['regexp']);
    $form_state['values']['regexp']  = $regexp;
    // check the regular expression to make sure it is valid
//    set_error_handler(function() {}, E_WARNING);
//    $result = preg_match("/" . $form_state['values']['regexp'] . "/", null);
//    restore_error_handler();
//    if ($result === FALSE) {
//      form_set_error('regexp', 'Invalid regular expression.');
//    }

  }

  /**
   * @see TripalImporter::run()
   */
  public function run() {


    $arguments = $this->arguments['run_args'];


    $analysis_id = $arguments['analysis_id'];
    $parent_regex = isset($arguments['regexp']) ? $arguments['regexp'] : NULL;

    $file_path = $this->arguments['files'][0]['file_path'];
    $this->tripal_ssr_import_ssr($file_path, $analysis_id, $parent_regex);

  }

  //Read in the file, split the columns

  /**
   * @param $file_path
   * @param $analysis_id
   * @param $regexp
   */

  private function tripal_ssr_import_ssr($file_path, $analysis_id, $regexp = NULL) {
    //Read in the file, split the columns
    $file = new SplFileObject($file_path);

    // Loop until we reach the end of the file.
    $parent_id = NULL;

    while (!$file->eof()) {
      // Echo one line from the file.
      $line = $file->fgets();
      $split = $this->parse_line($line, $regexp);
      if (!$split) {
        continue;
      }

      if ($regexp) {
        $features = $split[0];

        preg_match($regexp, $features, $matches);

        if (!$matches[0]) {
          print "Error: Could not parse parent feature given regular expression " . $regexp . " for entry " . $features . "!\n";
          return;
        }

        $parent_feature_uniquename = $matches[0];

        $ssr_uniquename = $split[0];
        $motif = $split[1];
        $num_repeats = $split[2];
        $start_pos = $split[3]; //start position of SSR relative to parent
        $end_pos = $split[4];
        $f_primer = $split[5];//forward primer
        $r_primer = $split[6];
        $f_primer_tm = $split[7];
        $r_primer_tm = $split[8];
        $product_size = $split[9];
      }
      else {
        $parent_feature_uniquename = $split[0];
        $ssr_uniquename = $split[1];
        $motif = $split[2];
        $num_repeats = $split[3];
        $start_pos = $split[4]; //start position of SSR relative to parent
        $end_pos = $split[5];
        $f_primer = $split[6];//forward primer
        $r_primer = $split[7];
        $f_primer_tm = $split[8];
        $r_primer_tm = $split[9];
        $product_size = $split[10];
      }

      $parent_feature = $this->find_parent_feature($parent_feature_uniquename);
      if (!$parent_feature) {
        print "Error: Could not uniquely identify parent feature.";
        return;
      }

      $parent_feature_id = $parent_feature->feature_id;
      $organism_id = $parent_feature->organism_id;

      $ssr_feature = $this->get_ssr_feature($ssr_uniquename, $organism_id, $parent_feature_id);

      $this->associate_feature_analysisfeature($ssr_feature, $analysis_id);
      //Add props to ssr_feature
      $this->add_ssr_props($ssr_feature, $motif, $num_repeats, $f_primer, $r_primer, $f_primer_tm, $r_primer_tm, $product_size);

      $this->add_parent_props($parent_feature_id, $ssr_uniquename);
      //Add location information
      $this->insert_featureloc($ssr_feature, $parent_feature_id, $start_pos, $end_pos);
    }
  }

  function parse_line($line, $regexp) {
    $split = explode("\t", $line);

    if ($regexp && count($split) != 10) {
      print "Error: Input file should be an 10 column tab delimited file if supplying a regular expression.";
      return;
    }

    if (!$regexp && count($split) != 11) {
      print "Error: Input file should be an 11 column tab delimited file";
      return;
    }

    //skip headers
    if ($split[0] == 'Feature_ID' || $split[0] == 'ID') {
      return NULL;
    }
    return $split;
  }

  /**
   * @param $ssr_uniquename
   * @param $organism_id
   * @param $parent_feature_id
   *
   * @return ssr feature id, or false.
   */
  function get_ssr_feature($ssr_uniquename, $organism_id, $parent_feature_id) {

    $ssrTerm = chado_get_cvterm(['id' => 'SO:0000289']);

    $query = db_select('chado.feature', 'f');
    $query->fields('f', ['feature_id']);
    $query->condition('uniquename', $ssr_uniquename);
    $query->condition('type_id', $ssrTerm->cvterm_id);
    $result = $query->execute()->fetchField();

    if (!$result) {
      // Doesnt exist so insert
      $values = [
        'organism_id' => $organism_id,
        'name' => $ssr_uniquename,
        'uniquename' => $ssr_uniquename,
        'type_id' => $ssrTerm->cvterm_id,
      ];
      $result = chado_insert_record('feature', $values);

      if (!$result) {
        $this->logMessage("Failed to insert feature '!name'", [
          '!name' => $ssr_uniquename,
        ], TRIPAL_ERROR);
        return FALSE;
      }
    }

    $ssr_feature_id = $result['feature_id'];
    $term = chado_get_cvterm(['id' => 'SO:associated_with']);

    $query = db_insert('chado.feature_relationship')
      ->fields([
        'subject_id' => $ssr_feature_id,
        'object_id' => $parent_feature_id,
        'type_id' => $term->cvterm_id,
      ])->execute();
    //TODO: ERROR HANDLING HERE.

    return $ssr_feature_id;
  }

  /**
   * We have the uniquename and possibly a type_id.  retrieve said feature.
   *
   * @param $unique_name
   * @param $type_id feature type_id
   *
   * @return bool
   */
  public function find_parent_feature($unique_name, $type_id = NULL) {
    $query = db_select('chado.feature', 'f');
    $query->fields('f', ['feature_id', 'organism_id']);
    $query->condition('uniquename', $unique_name);

    if ($type_id) {
      $query->condition('type_id', $type_id);
    }
    $result = $query->execute()->fetchAll();

    if (count($result) > 1) {

      $this->logMessage("Failed to uniquely identify feature '!name'", [
        '!name' => $unique_name,
      ], TRIPAL_ERROR);
      return FALSE;
    }
    if ($result) {
      return $result[0];
    }
    return FALSE;
  }

  /**
   * Associates the ssr feature with the analysis via analysisfeature
   *
   * @param $ssr_feature_id
   * @param $analysis_id
   *
   * @return mixed
   */
  function associate_feature_analysisfeature($ssr_feature_id, $analysis_id) {

    $result = db_select('chado.analysisfeature', 'af')
      ->fields('af', ['analysisfeature_id'])
      ->condition('feature_id', $ssr_feature_id)
      ->condition('analysis_id', $analysis_id)
      ->execute()
      ->fetchField();

    if (!$result) {
      $result = db_insert('chado.analysisfeature')
        ->fields([
          'feature_id' => $ssr_feature_id,
          'analysis_id' => $analysis_id,
        ])->execute();
    }
    return $result;
  }

  /**
   * Adds properties to the sSR feature
   *
   * @param $feature_id
   * @param $motif
   * @param $num_repeats
   * @param $f_primer
   * @param $r_primer
   * @param $f_primer_tm
   * @param $r_primer_tm
   * @param $product_size
   */
  function add_ssr_props($feature_id, $motif, $num_repeats, $f_primer, $r_primer, $f_primer_tm, $r_primer_tm, $product_size) {
    $props = [
      [
        'value' => $motif,
        'type_name' => 'tripal_ssr_motif',
        'cv_name' => 'feature_property',
      ],
      [
        'value' => $num_repeats,
        'type_name' => 'tripal_ssr_repeats',
        'cv_name' => 'feature_property',
      ],
      [
        'value' => $f_primer,
        'type_name' => 'tripal_ssr_forward_primer',
        'cv_name' => 'feature_property',
      ],
      [
        'value' => $r_primer,
        'type_name' => 'tripal_ssr_reverse_primer',
        'cv_name' => 'feature_property',
      ],
      [
        'value' => $f_primer_tm,
        'type_name' => 'tripal_ssr_forward_tm',
        'cv_name' => 'feature_property',
      ],
      [
        'value' => $r_primer_tm,
        'type_name' => 'tripal_ssr_reverse_tm',
        'cv_name' => 'feature_property',
      ],
      [
        'value' => $product_size,
        'type_name' => 'tripal_ssr_product_size',
        'cv_name' => 'feature_property',
      ],

    ];

    $record = ['id' => $feature_id, 'table' => 'feature'];
    $options = ['update_if_present' => TRUE];

    foreach ($props as $prop) {
      $property = [
        'type_name' => $prop['type_name'],
        'cv_name' => $prop['cv_name'],
        'value' => $prop['value'],
      ];
      if (!chado_insert_property($record, $property, $options)) {
        tripal_report_error('Tripal SSR', TRIPAL_ERROR, "Could not add property" . $prop['type_name']);
      }
    }
  }


  /**
   * Inserts location into featureloc.
   *
   * @param $ssr_feature - the feature ID being located
   * @param $parent_feature - the source feature ID that ssr is relative to.
   * @param $fmin - int.  The leftmost/minimal boundary in the linear range
   *   represented by the featureloc.
   * @param $fmax - int.  The rightmost/max boundary in the linear range
   *   represented by the featureloc.
   */
  private function insert_featureloc($ssr_feature, $parent_feature, $fmin, $fmax) {

    $sql = 'SET search_path to chado, public;
      INSERT INTO {featureloc} (feature_id, srcfeature_id, fmin, fmax)
      VALUES (:feature_id, :srcfeature_id, :fmin, :fmax)';

    $args = [
      ':feature_id' => $ssr_feature,
      ':srcfeature_id' => $parent_feature,
      ':fmin' => (int) $fmin,
      ':fmax' => (int) $fmax,
    ];
    $results = chado_query($sql, $args);

    return $results;
  }

  /**
   * retrieves the analysisfeature record or, adds one.
   *
   * @param $ssr_feature_id
   * @param $analysis_id
   */
  private function associate_ssr_analysis($ssr_feature_id, $analysis_id) {

    $query = db_select('chado.analysisfeature', 'af')
      ->fields('af', ['analysisfeature_id'])
      ->condition('analysis_id', $analysis_id)
      ->condition('feature_id', $ssr_feature_id);
    $result = $query->execute()->fetchField();

    if ($result) {
      return $result;
    }
    return db_insert('chado.analysisfeature')
      ->fields(['feature_id' => $ssr_feature_id, 'analysis_id' => $analysis_id])
      ->execute();
  }

  private function add_parent_props($parent_id, $ssr_uniquename) {

    $record = ['id' => $parent_id, 'table' => 'feature'];
    $options = ['update_if_present' => TRUE];

    $property = [
      'type_name' => 'tripal_associated_ssr',
      'cv_name' => 'feature_property',
      'value' => $ssr_uniquename,
    ];
    if (!chado_insert_property($record, $property, $options)) {
      tripal_report_error('Tripal SSR', TRIPAL_ERROR, "Could not add property Associated SSR to parent feature");
    }
  }
}
