<?php

/**
 * @file
 * Administrative page callbacks for the redirect module.
 */

/**
 * Form constructor for the module's settings form.
 *
 * @see search_exclude_nid_search_exclusion_form_submit()
 *
 * @ingroup forms
 */
function search_exclude_nid_search_exclusion_form() {
  $form = array();

  $form['search_exclude_nid_search_exclusion_nids'] = array(
    '#title' => t('Node ids to be excluded from search result'),
    '#type' => 'textfield',
    '#default_value' => implode(',', variable_get('search_exclude_nid_search_exclusion_nids', array())),
    '#description' => t('Please enter the node ids separated by a "," (for example: 1,23,45)'),
  );
  $form['search_exclude_nid_search_exclusion_auto'] = array(
    '#title' => t('Node title lookup'),
    '#type' => 'textfield',
    '#autocomplete_path' => 'search_exclusion/autocomplete',
    '#description' => t('Lookup a node title and copy & paste the node ID into the list field above or press just "Save configuration". The node ID will then be automatically added to the list.'),
  );

  $form['#attached']['js'] = array(
    drupal_get_path('module', 'search_exclude_nid') . '/search_exclude_nid.js',
  );

  if (count(variable_get('search_exclude_nid_search_exclusion_nids', array()))) {
    $items = array();
    $result = db_select('node', 'n')
      ->fields('n', array('nid', 'title'))
      ->condition('nid', variable_get('search_exclude_nid_search_exclusion_nids', array()), 'IN')
      ->execute();

    // List of excluded nodes.
    $items = array();
    foreach ($result as $row) {
      $items[] = array(
        'data' => l($row->title . ' (nid:' . $row->nid . ')',
        'node/' . $row->nid),
        'id' => 'node-' . $row->nid,
      );
    }

    $form['exclusion_list'] = array(
      '#type' => 'markup',
      '#markup' => theme('item_list' , array(
        'items' => $items,
        'title' => 'List of excluded nodes',
        'type' => 'ul',
        'attributes' => array(),
        )
      ),
    );
  }
  $form['#submit'][] = 'search_exclude_nid_search_exclusion_form_submit';
  return system_settings_form($form);
}

/**
 * Form submission handler for search_exclude_nid_search_exclusion_form().
 */
function search_exclude_nid_search_exclusion_form_submit($form, &$form_state) {
  if (isset($form_state['values']['search_exclude_nid_search_exclusion_nids'])) {
    $filtered_nid_list = array();
    $nid_list = $form_state['values']['search_exclude_nid_search_exclusion_nids'];
    if ($nid_list) {
      $nid_list = explode(',', $nid_list);
      foreach ($nid_list as $nid) {
        // Add only add valid nodes.
        $node = node_load(intval(trim($nid)));
        if (isset($node->type)) {
          $filtered_nid_list[] = intval($nid);
        }
      }
    }
    $form_state['values']['search_exclude_nid_search_exclusion_nids'] = $filtered_nid_list;
  }
}

/**
 * Autocomplete helper, $string = string to search for.
 */
function search_exclude_nid_nodes_autocomplete($string) {
  $matches = array();
  $result = db_select('node', 'n')
    ->fields('n', array('nid', 'title'))
    ->condition('title', '%' . db_like($string) . '%', 'LIKE')
    ->execute();

  // Save the query to matches.
  foreach ($result as $row) {
    $matches[$row->nid] = check_plain($row->title);
  }

  // Return the result to the form in json.
  drupal_json_output($matches);
}
