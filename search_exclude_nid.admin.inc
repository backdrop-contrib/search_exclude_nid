<?php

/**
 * @file
 * Administrative page callbacks for the redirect module.
 */

/**
 * Implements hook_form().
 */
function search_exclude_nid_search_exclusion_form() {
  $form = array();
  
  $form['search_exclude_nid_search_exclusion_nids'] = array(
    '#title' => t('Node ids to be excluded from search result'),
    '#type' => 'textfield',
    '#default_value' => implode(',', variable_get('search_exclude_nid_search_exclusion_nids', array())),
    '#description' => 'Please enter the node ids separated by a "," (for example: 1,23,45)',
  );
  $form['search_exclude_nid_search_exclusion_auto'] = array(
    '#title' => t('Node title lookup'),
    '#type' => 'textfield',
    '#autocomplete_path' => 'search_exclusion/autocomplete',
    '#description' => 'Lookup a node title and add the node id to your exclusion list by clicking the link below.',
  );

  $nid_add_link = "<a href='#' id='add-node-id-link'>+ ADD NID TO EXCLUSION LIST</a>
                  <script type='text/javascript'>
                    jQuery(document).ready(function() {
                      // Handler for .ready() called.
                      var filtered_nids_list = Array();
                      jQuery('#add-node-id-link').click(function(){
                        var nid = parseInt(jQuery('#edit-search-exclude-nid-search-exclusion-auto').val());
                        if (!(isNaN(nid)) && (nid != 0)) {
                          filtered_nids_list = getFilteredNidList();
                          if(filtered_nids_list.containsNot(nid)){
                            filtered_nids_list.push(nid);
                          }
                          jQuery('#edit-search-exclude-nid-search-exclusion-nids').val(filtered_nids_list.join(','));
                          jQuery('#edit-search-exclude-nid-search-exclusion-auto').val('').focus();
                        }        
                      });
                      jQuery('#edit-submit').click(function(){
                        filtered_nids_list = getFilteredNidList();
                        jQuery('#edit-search-exclude-nid-search-exclusion-nids').val(filtered_nids_list.join(','));
                      });
                    });
                    Array.prototype.containsNot = function(obj) {
                        var i = this.length;
                        while (i--) {
                            if (this[i] == obj) {
                                return false;
                            }
                        }
                        return true;
                    }
                    function getFilteredNidList() {
                      var nidsList = jQuery('#edit-search-exclude-nid-search-exclusion-nids').val().split(',');
                      var filtered_nids_list = new Array();  
                      var intRegex = /^\d+$/;  
                      for (var i in nidsList) {
                        if (intRegex.test(nidsList[i]) && (nidsList[i] > 0) && (filtered_nids_list.containsNot(nidsList[i]))) {
                          filtered_nids_list.push(nidsList[i]);
                        }
                      }
                      return filtered_nids_list;
                    }
                  </script>";

  $form['add_node_id_link'] = array(
    '#type' => 'markup',
    '#markup' => $nid_add_link,
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
      '#markup' => theme_item_list(array(
        'items' => $items,
        'title' => 'List of excluded nodes',
        'type' => 'ul',
        'attributes' => array(),
        )
      ),
    );
  }
  return system_settings_form($form);
}

/*
 * Implements hook_form_validate().
 */
function search_exclude_nid_search_exclusion_form_validate($form, &$form_state) {
  if (isset($form_state['values']['search_exclude_nid_search_exclusion_nids'])) {
    $filtered_nid_list = array();
    $nid_list = $form_state['values']['search_exclude_nid_search_exclusion_nids'];
    if ($nid_list) {
      $nid_list = explode(',', $nid_list);
      foreach($nid_list as $nid) {
        // only add valid nodes
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
