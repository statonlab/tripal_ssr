<?php

/**
 * SSR search form
 *
 * @param $form
 * @param $form_state
 *
 * @throws \Exception
 */
function tripal_ssr_marker_search_form($form, &$form_state, $organism = NULL, $url_query = []) {
  global $base_url;

  $constructed_url = '';
  $i = 0;
  foreach ($url_query as $key => $value) {
    if ($i > 0) {
      $constructed_url .= '&';
    }
    $constructed_url .= "{$key}={$value}";
    $i++;

    $form['hidden'][$key] = [
      '#type' => 'value',
      '#value' => $value,
    ];
  }
  $form['#attributes']['id'] = 'SSR-search-form';

  $form['organism'] = [
    '#type' => 'hidden',
    '#value' => $organism,
  ];
  $form['tripal_pane'] = [
    '#type' => 'hidden',
    '#value' => 'group_ssr_search',
  ];

  $form['options']['search_term'] = [
    '#type' => 'textfield',
    '#size' => '50',
    '#attributes' => [
      'placeholder' => t('marker name'),
      'id' => 'tripal-ssr-search-field',
    ],
    '#description' => '<br><small>Search for markers</small><br>',
    '#required' => TRUE,
    '#default_value' => isset($_GET['search_term']) ? $_GET['search_term'] : '',
  ];

  $form['options']['search_button'] = [
    '#type' => 'submit',
    '#value' => t('Search'),
    '#attributes' => [
      'id' => 'tripal-ssr-search-button',
    ],
  ];

  if (!empty($_GET['search_term']) || !empty($_GET['organism'])) {
    $form['results'] = [
      '#type' => 'markup',
      '#markup' => tripal_get_ssr_results(),
    ];
  }
  $form['#method'] = 'get';

  return $form;
}

/**
 * @return string
 */
function tripal_get_ssr_results() {
  $term = $_GET['search_term'];
  $results = tripal_ssr_search_by_name($term);

  $markup = tripal_ssr_results_formatter($results);

  return $markup;
}

/**
 * @param $search_term
 *
 * @return mixed
 */
function tripal_ssr_search_by_name($search_term) {
  $organism = $_GET['organism'];
  $ssrTerm = chado_get_cvterm(['id' => 'SO:0000289']);
  $query = db_select('chado.feature', 'f');
  $query->fields('f', ['feature_id', 'uniquename', 'name']);
  $query->join('chado.feature_relationship', 'fr', 'fr.object_id = f.feature_id');
  $query->join('chado.feature', 'fssr', 'fr.subject_id = fssr.feature_id');
  $query->fields('fssr', ["name"]);
  $query->condition('fssr.name', $search_term, 'LIKE');
  $query->condition('fssr.type_id', $ssrTerm->cvterm_id);
  $query->condition('fssr.organism_id', $organism);
  $results = $query->execute()->fetchAll();


  foreach ($results as $result) {
    $entity_id = chado_get_record_entity_by_table('feature', $result->feature_id);
   // $bundle_id = tripal_ssr_get_feature_bundle_id($entity_id);
    $url = 'bio_data/' . $entity_id;
    $result->url = $url;
  }
  return ($results);
}

/**
 * Format the results.
 *
 * @param array $search_results
 *
 * @return string
 */
function tripal_ssr_results_formatter($search_results) {

  if (count($search_results) === 0) {

    $markup = '<p>No results found.</p>
        <p>Please try a different search term.</p>';
    return $markup;
  }

  $header = ['Gene Identifier', 'Marker'];
  $rows = [];

  foreach ($search_results as $result) {
    $result = (array) $result;
    $url = l($result['uniquename'], $result['url']);
    $rows[] = [
      $url,
      $result['fssr_name'],
    ];
  }

  return theme('table', [
    'header' => $header,
    'rows' => $rows,
  ]);
}

/**
 * @param $entity_id
 *
 * @return mixed | the term id
 */
function tripal_ssr_get_feature_bundle_id($entity_id) {
  $query = db_select('public.tripal_entity', 'te');
  $query->fields('te', ['term_id']);
  $query->condition('id', $entity_id);
  return $query->execute()->fetchField();
}
