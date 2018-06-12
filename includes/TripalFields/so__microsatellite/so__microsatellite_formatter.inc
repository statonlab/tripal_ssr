<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class so__microsatellite_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'so__microsatellite';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['so__microsatellite'];

  // The list of default settings for this formatter.
  public static $default_settings = [
    'key' => 'default_value',
  ];

  /**
   * Provides the field's setting form.
   *
   * This function corresponds to the hook_field_formatter_settings_form()
   * function of the Drupal Field API.
   *
   * The settings form appears on the 'Manage Display' page of the content
   * type administration page. This function provides the form that will
   * appear on that page.
   *
   * To add a validate function, please create a static function in the
   * implementing class, and indicate that this function should be used
   * in the form array that is returned by this function.
   *
   * This form will not be displayed if the formatter_settings_summary()
   * function does not return anything.
   *
   * param $field
   *   The field structure being configured.
   * param $instance
   *   The instance structure being configured.
   * param $view_mode
   *   The view mode being configured.
   * param $form
   *   The (entire) configuration form array, which will usually have no use
   *   here.  Typically for reference only.
   * param $form_state
   *   The form state of the (entire) configuration form.
   *
   * @return
   *   A Drupal Form array containing the settings form for this field.
   */
  public function settingsForm($view_mode, $form, &$form_state) {
  }

  /**
   *  Provides the display for a field
   *
   * This function corresponds to the hook_field_formatter_view()
   * function of the Drupal Field API.
   *
   *  This function provides the display for a field when it is viewed on
   *  the web page.  The content returned by the formatter should only include
   *  what is present in the $items[$delta]['values] array. This way, the
   *  contents that are displayed on the page, via webservices and downloaded
   *  into a CSV file will always be identical.  The view need not show all
   *  of the data in the 'values' array.
   *
   * @param $element
   * @param $entity_type
   * @param $entity
   * @param $langcode
   * @param $items
   * @param $display
   *
   * @return
   *    An element array compatible with that returned by the
   *    hook_field_formatter_view() function.
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    //deal with organism case
    if ($entity->chado_table == 'organism') {
      $count = $entity->so__microsatellite['und'][0]['count'];

      $message = NULL;

      if ($count > 0) {
        $path = $entity->so__microsatellite['und'][0]['path'];
        $message = 'There are <a href="/' . $path . '">' . $count . ' features with SSR annotations in this organism.</a>';
      }
      $organism = $entity->chado_record;

      $form = drupal_get_form('tripal_ssr_marker_search_form', TRUE, $organism->organism_id, [
        'tripal_pane' => 'group_ssr_search',
        'organism' => $organism->organism_id,
      ]);

      $content = '<p>You can search this organism for SSR markers.  The search will return features associated with the matching markers.</p>';
      $content .= drupal_render($form);
      $element[] = [
        '#type' => 'markup',
        '#markup' => $content,
      ];

      return $element;
    }

//deal with feature case
    if ($entity->chado_table == 'feature') {

      $i = 0;

      if (!isset($items[0]['value']['name'])) {
        return;
      }

      $header = [
        'name' => 'Name',
        'unique_name' =>'Unique Name',
        'tripal_ssr_motif' =>'Motif',
        'tripal_ssr_repeats' =>'Number of Repeats',
        'tripal_ssr_forward_primer' =>'Forward Primer',
       'tripal_ssr_forward_tm'  => 'Forward Primer Tm',
        'tripal_ssr_reverse_primer'=> 'Reverse Primer',
        'tripal_ssr_reverse_tm'=> 'Reverse Primer Tm',
        'tripal_ssr_product_size' => 'Product Size',
      ];
      $rows = [];
      foreach ($items as $item) {
        $info = $item['value'];
        $row = [];
        foreach ($info as $li => $value) {
          $row[$li] = $value;
        }
        $rows[] = $row;
        $i++;
      }
      $element[0] = [
        '#theme' => 'table',
        '#header' => $header,
        "#rows" => $rows,
      ];
      return $element;
    }
  }

  public function esView(&$element, $entity_type, $entity, $langcode, $items, $display) {


    return;
  }

  /**
   * Provides a summary of the formatter settings.
   *
   * This function corresponds to the hook_field_formatter_settings_summary()
   * function of the Drupal Field API.
   *
   * On the 'Manage Display' page of the content type administration page,
   * fields are allowed to provide a settings form.  This settings form can
   * be used to allow the site admin to define how the field should be
   * formatted.  The settings are then available for the formatter()
   * function of this class.  This function provides a text-based description
   * of the settings for the site developer to see.  It appears on the manage
   * display page inline with the field.  A field must always return a
   * value in this function if the settings form gear button is to appear.
   *
   * See the hook_field_formatter_settings_summary() function for more
   * information.
   *
   * @param $field
   * @param $instance
   * @param $view_mode
   *
   * @return string
   *   A string that provides a very brief summary of the field settings
   *   to the user.
   *
   */
  public function settingsSummary($view_mode) {
  }

  /**
   * Searches for markers for the given organism.
   * Will return the features associated with it.
   *
   * @param $search_term
   */
  public function query_ssr($search_term){

  }
}
