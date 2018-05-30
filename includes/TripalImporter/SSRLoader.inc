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
    '.txt', '.tsv'
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
<p>
More information coming soon.</p>
',
    ];


    return $form;
  }

  /**
   * @see TripalImporter::formValidate()
   */
  public function formValidate($form, &$form_state) {

  }

  /**
   * @see TripalImporter::run()
   */
  public function run() {

    $arguments = $this->arguments['run_args'];
    $file_path = $this->arguments['files'][0]['file_path'];
    tripal_ssr_import_ssr($file_path);
  }
}
