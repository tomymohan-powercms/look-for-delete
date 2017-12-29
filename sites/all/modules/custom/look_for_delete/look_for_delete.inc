<?php

/**
 *implement confirm form().
 *
 * @param $form
 *  Form object
 *
 * @param $form_state
 *  Form state values
 *
 * @return $form
 */
function look_for_delete_confirm_form($form, &$form_state) {
  if ($form_state['build_info']['form_id'] == 'look_for_delete_confirm_form') {
    $path = 'delete-contents';
    $form = confirm_form($form,
      t('Are you sure you want to update'),
      $path,
      t('This action cannot be undone.<br><br>'),
      t('Confirm'),
      t('Cancel')
    );
    $form['csv_link'] = array(
      '#type' => 'submit',
      '#value' => 'download list',
      '#weight' => 1,
      '#submit' => array('look_for_delete_csv'),
    );
    $form_state['rebuild'] = TRUE;
  }
  if (!empty($form_state['input']['select_content_type'])) {
    $default_content = $form_state['input']['select_content_type'];
    $currentfields = $form_state['input']['nodes'];
    if (isset($form_state['input']['nodes']['update'])) {
      $current_option =array_filter($form_state['input']['nodes']['update']);
      $serialised_update_options = serialize($current_option);
      $form['update_options'] = array(
        '#type' => 'hidden',
        '#value' => $serialised_update_options
      );
    }
    if (isset($form_state['input']['userdelete'])) {
      $delete_user = $form_state['input']['userdelete'];
      $form['user_delete'] = array(
        '#type' => 'hidden',
        '#value' => $delete_user
      );
    }
    if (isset($form_state['input']['range'])) {
      $range = $form_state['input']['range'];
    }
    if (isset($currentfields)) {
      foreach ($currentfields as $key1 => $value1) {
        $searchname = '';
        $searchname = isset($value1['search_name']) ? $value1['search_name'] : '' ;
        if (!empty($searchname)) {
          $current_searchname = array_filter(array_unique(array($searchname)));
        }
      }
    }
    $query = db_select('node','n')->distinct();
    $query->fields('n',array('nid','uid'));
    if (!empty($range)) {
      $query->range(0,$range);
    }
    $query->condition('n.type', $default_content);
    //$query->leftjoin('field_data_body', 'body', 'n.nid = body.entity_id');
    //$query->condition('body.body_value', 'apple', '=');
    foreach ($currentfields as $field_name => $field_data) {
      //search part
      if ((isset($field_data['search_name']) && !empty($field_data['search_name'])) && (isset($field_data['operator']) && !empty($field_data['operator']))) {
        $keyword = explode(',', $field_data['search_name']);
        if ($field_name == 'title') {
          if (($field_data['operator'] == '=') || ($field_data['operator'] == '!=') || ($field_data['operator'] == 'LIKE') || ($field_data['operator'] == 'NOT LIKE')) {
            if ($field_data['operator'] == '=') {
              if (is_array($keyword)) {
                $query_or = db_or();
                foreach ($keyword as $keyword_value) {
                  $query_or->condition('n.title', $keyword_value, $field_data['operator']);
                }
                $query->condition($query_or);
              }
              else {
                $query->condition('n.title', $keyword_value, $field_data['operator']);
              }
            }
            elseif ($field_data['operator'] == '!=') {
              if (is_array($keyword)) {
                $query_and = db_and();
                foreach ($keyword as $keyword_value) {
                  $query_and->condition('n.title', $keyword_value, $field_data['operator']);
                }
                $query->condition($query_and);
              }
              else {
                $query->condition('n.title', $keyword_value, $field_data['operator']);
              }
            }
            elseif ($field_data['operator'] == 'LIKE') {
              if (is_array($keyword)) {
                $query_or = db_or();
                foreach ($keyword as $keyword_value) {
                  $query_or->condition('n.title', '%' . db_like($keyword_value) . '%', $field_data['operator']);
                }
                $query->condition($query_or);
              }
              else {
                $query->condition('n.title', '%' . db_like($keyword_value) . '%', $field_data['operator']);
              }
            }
            else {
              if (is_array($keyword)) {
                $query_and = db_and();
                foreach ($keyword as $keyword_value) {
                  $query_and->condition('n.title', '%' . db_like($keyword_value) . '%', $field_data['operator']);
                }
                $query->condition($query_and);
              }
              else {
                $query->condition('n.title', '%' . db_like($keyword_value) . '%', $field_data['operator']);
              }
            }
          }
        }
        else {
          $field_types = field_info_field($field_name);
          $current_fieldtype = $field_types['type'];
          //for field_type == 'text','text_with_summary','telephone','link_field'.
          if (($current_fieldtype == 'text') || ($current_fieldtype == 'text_with_summary') || ($current_fieldtype == 'telephone') || ($current_fieldtype == 'link_field') || ($current_fieldtype == 'number_integer') || ($current_fieldtype == 'email')) {
            $query->leftjoin('field_data_' . $field_name, $field_name, 'n.nid = ' . $field_name . '.entity_id');
            if ($current_fieldtype == 'link_field') {
              if (($field_data['operator'] == '=') || ($field_data['operator'] == '!=')) {
                if ($field_data['operator'] == '=') {
                  if (is_array($keyword)) {
                    $query_or = db_or();
                    foreach ($keyword as $keyword_value) {
                      $query_or->condition($field_name . '.' . $field_name . '_url', $keyword_value, $field_data['operator']);
                    }
                    $query->condition($query_or);
                  }
                  else {
                    $query->$query->condition($field_name . '.' . $field_name . '_url', $keyword_value, $field_data['operator']);
                  }
                }
                else {
                  if (is_array($keyword)) {
                    $query_and = db_and();
                    foreach ($keyword as $keyword_value) {
                      $query_and->condition($field_name . '.' . $field_name . '_url', $keyword_value, $field_data['operator']);
                    }
                    $query->condition($query_and);
                  }
                  else {
                    $query->$query->condition($field_name . '.' . $field_name . '_url', $keyword_value, $field_data['operator']);
                  }
                }
              }
              elseif ($field_data['operator'] == 'LIKE') {
                if (is_array($keyword)) {
                  $query_or = db_or();
                  foreach ($keyword as $keyword_value) {
                    $query_or->condition($field_name . '.' . $field_name . '_url', '%' . db_like($keyword_value) . '%', $field_data['operator']);
                  }
                  $query->condition($query_or);
                }
                else {
                  $query->condition($field_name . '.' . $field_name . '_url','%' . db_like($keyword_value) . '%', $field_data['operator']);
                }
              }
              else {
                if (is_array($keyword)) {
                  $query_and = db_and();
                  foreach ($keyword as $keyword_value) {
                    $query_and->condition($field_name . '.' . $field_name . '_url', '%' . db_like($keyword_value) . '%', $field_data['operator']);
                  }
                  $query->condition($query_and);
                }
                else {
                  $query->condition($field_name . '.' . $field_name . '_url','%' . db_like($keyword_value) . '%', $field_data['operator']);
                }
              }
            }
            elseif ($current_fieldtype == 'number_integer') {
              if (($field_data['operator'] == '=') || ($field_data['operator'] == '!=')) {
                if ($field_data['operator'] == '=') {
                  if (is_array($keyword)) {
                    $query_or = db_or();
                    foreach ($keyword as $keyword_value) {
                      $query_or->condition($field_name . '.' . $field_name . '_value', $keyword_value, $field_data['operator']);
                    }
                    $query->condition($query_or);
                  }
                  else {
                    $query->condition($field_name . '.' . $field_name . '_value', $keyword_value, $field_data['operator']);
                  }
                }
                else {
                  if (is_array($keyword)) {
                    $query_and = db_and();
                    foreach ($keyword as $keyword_value) {
                      $query_and->condition($field_name . '.' . $field_name . '_value', $keyword_value, $field_data['operator']);
                    }
                    $query->condition($query_and);
                  }
                  else {
                    $query->condition($field_name . '.' . $field_name . '_value',$keyword_value, $field_data['operator']);
                  }
                }
              }
              elseif (($field_data['operator'] == '>') || ($field_data['operator'] == '>=') || ($field_data['operator'] == '<') || ($field_data['operator'] == '<=')) {
                if (is_array($keyword)) {
                  $query_and = db_and();
                  foreach ($keyword as $keyword_value) {
                    $query_and->condition($field_name . '.' . $field_name . '_value', $keyword_value, $field_data['operator']);
                  }
                  $query->condition($query_and);
                }
                else {
                  $query->condition($field_name . '.' . $field_name . '_value', $keyword_value, $field_data['operator']);
                }
              }
              elseif ($field_data['operator'] == 'LIKE') {
                if (is_array($keyword)) {
                  $query_or = db_or();
                  foreach ($keyword as $keyword_value) {
                    $query_or->condition($field_name . '.' . $field_name . '_value', '%' . db_like($keyword_value ) . '%', $field_data['operator']);
                  }
                  $query->condition($query_or);
                }
                else {
                  $query->condition($field_name . '.' . $field_name . '_value', '%' .db_like($keyword_value) . '%', $field_data['operator']);
                }
              }
              else {
                if (is_array($keyword)) {
                  $query_and = db_and();
                  foreach ($keyword as $keyword_value) {
                    $query_and->condition($field_name . '.' . $field_name . '_value', '%' . db_like($keyword_value ). '%', $field_data['operator']);
                  }
                  $query->condition($query_and);
                }
                else {
                  $query->condition($field_name . '.' . $field_name . '_value','%' .db_like($keyword_value) . '%', $field_data['operator']);
                }
              }
            }
            elseif ($current_fieldtype == 'email') {
              if (($field_data['operator'] == '=') || ($field_data['operator'] == '!=')) {
                if ($field_data['operator'] == '=') {
                  if (is_array($keyword)) {
                    $query_or = db_or();
                    foreach ($keyword as $keyword_value) {
                      $query_or->condition($field_name . '.' . $field_name . '_email', $keyword_value, $field_data['operator']);
                    }
                    $query->condition($query_or);
                  }
                  else {
                    $query->condition($field_name . '.' . $field_name . '_email', $keyword_value, $field_data['operator']);
                  }
                }
                else {
                  if (is_array($keyword)) {
                    $query_and = db_and();
                    foreach ($keyword as $keyword_value) {
                      $query_and->condition($field_name . '.'. $field_name . '_email', $keyword_value, $field_data['operator']);
                    }
                    $query->condition($query_and);
                  }
                  else {
                  $query->condition($field_name . '.' . $field_name . '_email', $keyword_value, $field_data['operator']);
                  }
                }
              }
              elseif ($field_data['operator'] == 'NOT LIKE') {
                if (is_array($keyword)) {
                  $query_and = db_and();
                  foreach ($keyword as $keyword_value) {
                    $query_and->condition($field_name . '.' . $field_name . '_email', '%' . $keyword_value . '%', $field_data['operator']);
                  }
                  $query->condition($query_and);
                }
                else {
                  $query->condition($field_name . '.' . $field_name . '_email', '%' . $keyword_value . '%', $field_data['operator']);
                }
              }
              else {
                if (is_array($keyword)) {
                  $query_or = db_or();
                  foreach ($keyword as $keyword_value) {
                    $query_or->condition($field_name . '.'. $field_name . '_email','%' . $keyword_value . '%', $field_data['operator']);
                  }
                  $query->condition($query_or);
                }
                else {
                  $query->condition($field_name . '.'. $field_name . '_email','%' . $keyword_value . '%', $field_data['operator']);
                }
              }
            }
            else {
              if (($field_data['operator'] == '=') || ($field_data['operator'] == '!=')) {
                if ($field_data['operator'] == '=') {
                  if (is_array($keyword)) {
                    $query_or = db_or();
                    foreach ($keyword as $keyword_value) {
                      $query_or->condition($field_name . '.' . $field_name . '_value', $keyword_value, $field_data['operator']);
                    }
                    $query->condition($query_or);
                  }
                  else {
                    $query->condition($field_name . '.' . $field_name . '_value', $keyword_value, $field_data['operator']);
                  }
                }
                else {
                  if (is_array($keyword)) {
                    $query_and = db_and();
                    foreach ($keyword as $keyword_value) {
                      $query_and->condition($field_name . '.' . $field_name . '_value', $keyword_value, $field_data['operator']);
                    }
                    $query->condition($query_and);
                  }
                  else {
                    $query->condition($field_name . '.' . $field_name . '_value', $keyword_value, $field_data['operator']);
                  }
                }
              }
              elseif ($field_data['operator'] == 'LIKE') {
                if (is_array($keyword)) {
                  $query_or = db_or();
                  foreach ($keyword as $keyword_value) {
                    $query_or->condition($field_name . '.' . $field_name . '_value', '%' . db_like($keyword_value) . '%', $field_data['operator']);
                  }
                  $query->condition($query_or);
                }
                else {
                  $query->condition($field_name . '.' . $field_name . '_value', '%' . db_like($keyword_value) . '%', $field_data['operator']);
                }
              }
              else {
                if (is_array($keyword)) {
                  $query_and = db_and();
                  foreach ($keyword as $keyword_value) {
                    $query_and->condition($field_name . '.' . $field_name . '_value', '%' . db_like($keyword_value) . '%', $field_data['operator']);
                  }
                  $query->condition($query_and);
                }
                else {
                  $query->condition($field_name . '.' . $field_name . '_value','%' . db_like($keyword_value) . '%', $field_data['operator']);
                }
              }
            }
          }
          //for field_type == 'image'.
          elseif ($current_fieldtype == 'image') {
            $query->leftjoin('field_data_' . $field_name, $field_name, 'n.nid = ' . $field_name . '.entity_id');
            $query->leftjoin( 'file_managed', 'fm', $field_name . '.' . $field_name. '_fid = fm.fid');
            if (($field_data['operator'] == '=') || ($field_data['operator'] == '!=')) {
              if ($field_data['operator'] == '=') {
                if (is_array($keyword)) {
                  $query_or = db_or();
                  foreach ($keyword as $keyword_value) {
                    $query_or->condition('fm.filename', $keyword_value, $field_data['operator']);
                  }
                  $query->condition($query_or);
                }
                else {
                  $query->condition('fm.filename', $keyword_value, $field_data['operator']);
                }
              }
              else {
                if (is_array($keyword)) {
                  $query_and = db_and();
                  foreach ($keyword as $keyword_value) {
                    $query_and->condition('fm.filename', $keyword_value, $field_data['operator']);
                  }
                  $query->condition($query_and);
                }
                else {
                  $query->condition('fm.filename', $keyword_value, $field_data['operator']);
                }
              }
            }
            elseif ($field_data['operator'] == 'LIKE') {
              if (is_array($keyword)) {
                $query_or = db_or();
                foreach ($keyword as $keyword_value) {
                  $query_or->condition('fm.filename', '%' . db_like($keyword_value) . '%', $field_data['operator']);
                }
                $query->condition($query_or);
              }
              else {
                $query->condition('fm.filename', '%' . db_like($keyword_value) . '%', $field_data['operator']);
              }
            }
            else {
              if (is_array($keyword)) {
                $query_and = db_and();
                foreach ($keyword as $keyword_value) {
                  $query_and->condition('fm.filename', '%' . db_like($keyword_value) . '%', $field_data['operator']);
                }
                $query->condition($query_and);
              }
              else {
                $query->condition('fm.filename', '%' . db_like($keyword_value) . '%', $field_data['operator']);
              }
            }
          }
          //for field_type == 'taxonomy_term_reference'.
          elseif ($current_fieldtype == 'taxonomy_term_reference') {
            $query->leftjoin('field_data_' . $field_name, $field_name, 'n.nid = ' . $field_name . '.entity_id');
            $query->leftjoin( 'taxonomy_term_data', 'ttd', $field_name . '.' . $field_name. '_tid = ttd.tid');
            if (($field_data['operator'] == '=') || ($field_data['operator'] == '!=')) {
              if ($field_data['operator'] == '=') {
                if (is_array($keyword)) {
                  $query_or = db_or();
                  foreach ($keyword as $keyword_value) {
                    $query_or->condition('ttd.name', $keyword_value, $field_data['operator']);
                  }
                  $query->condition($query_or);
                }
                else {
                  $query->condition('ttd.name', $keyword_value, $field_data['operator']);
                }
              }
              else {
                if (is_array($keyword)) {
                  $query_and = db_and();
                  foreach ($keyword as $keyword_value) {
                    $query_and->condition('ttd.name', $keyword_value, $field_data['operator']);
                  }
                  $query->condition($query_and);
                }
                else {
                  $query->condition('ttd.name', $keyword_value, $field_data['operator']);
                }
              }
            }
            elseif ($field_data['operator'] == 'LIKE') {
              if (is_array($keyword)) {
                $query_or = db_or();
                foreach ($keyword as $keyword_value) {
                  $query_or->condition('ttd.name', '%' . db_like($keyword_value) . '%', $field_data['operator']);
                }
                $query->condition($query_or);
              }
              else {
                $query->condition('ttd.name','%' . db_like($keyword_value) . '%', $field_data['operator']);
              }
            }
            else {
              if (is_array($keyword)) {
                $query_and = db_and();
                foreach ($keyword as $keyword_value) {
                  $query_and->condition('ttd.name', '%' . db_like($keyword_value) . '%', $field_data['operator']);
                }
                $query->condition($query_and);
              }
              else {
                $query->condition('ttd.name', '%' . db_like($keyword_value) . '%', $field_data['operator']);
              }
            }
          }
          //for field_type == 'paragraphs'.
          elseif ($current_fieldtype == 'paragraphs') {
            $query->leftjoin('field_data_' . $field_name, $field_name, 'n.nid = ' . $field_name . '.entity_id');
            $query->leftjoin( 'paragraphs_item', 'pi', $field_name . '.' . $field_name. '_value = pi.item_id');
            if (($field_data['operator'] == '=') || ($field_data['operator'] == '!=')) {
              if ($field_data['operator'] == '=') {
                if (is_array($keyword)) {
                  $query_or = db_or();
                  foreach ($keyword as $keyword_value) {
                    $query_or->condition('pi . bundle', $keyword_value, $field_data['operator']);
                  }
                  $query->condition($query_or);
                }
                else {
                  $query->condition('pi . bundle', $keyword_value, $field_data['operator']);
                }
              }
              else {
                if (is_array($keyword)) {
                  $query_and = db_and();
                  foreach ($keyword as $keyword_value) {
                    $query_and->condition('pi . bundle', $keyword_value, $field_data['operator']);
                  }
                  $query->condition($query_and);
                }
                else {
                  $query->condition('pi . bundle', $keyword_value, $field_data['operator']);
                }
              }
            }
            elseif ($field_data['operator'] == 'LIKE') {
              if (is_array($keyword)) {
                $query_or = db_or();
                foreach ($keyword as $keyword_value) {
                  $query_or->condition('pi . bundle', '%' . db_like($keyword_value) . '%', $field_data['operator']);
                }
                $query->condition($query_or);
              }
              else {
                $query->condition('pi . bundle', '%' . db_like($keyword_value) . '%', $field_data['operator']);
              }
            }
            else {
              if (is_array($keyword)) {
                $query_and = db_and();
                foreach ($keyword as $keyword_value) {
                  $query_and->condition('pi . bundle', '%' . db_like($keyword_value) . '%', $field_data['operator']);
                }
                $query->condition($query_and);
              }
              else {
                $query->condition('pi . bundle', '%' . db_like($keyword_value) . '%', $field_data['operator']);
              }
            }
          }
          //for field_type == 'field_collection'.
          elseif ($current_fieldtype == 'field_collection') {
            foreach ($keyword as $value) {
              $query->leftjoin('field_data_' . $field_name, $field_name, 'n.nid = ' . $field_name . '.entity_id');
              $query->leftjoin('field_data_field_name_of_the_student', 'ns', $field_name . '.' . $field_name . '_value = ns.entity_id');
              if (($field_data['operator'] == '=') || ($field_data['operator'] == '!=')) {
                if ($field_data['operator'] == '=') {
                  if (is_array($keyword)) {
                    $query_or = db_or();
                    foreach ($keyword as $keyword_value) {
                      $query_or->condition('ns.field_name_of_the_student_value', $keyword_value, $field_data['operator']);
                    }
                    $query->condition($query_or);
                  }
                  else {
                    $query->condition('ns.field_name_of_the_student_value', $keyword_value, $field_data['operator']);
                  }
                }
                else {
                  if (is_array($keyword)) {
                    $query_and = db_and();
                    foreach ($keyword as $keyword_value) {
                      $query_and->condition('ns.field_name_of_the_student_value', $keyword_value, $field_data['operator']);
                    }
                    $query->condition($query_and);
                  }
                  else {
                    $query->condition('ns.field_name_of_the_student_value', $keyword_value, $field_data['operator']);
                  }
                }
              }
              elseif ($field_data['operator'] == 'LIKE') {
                if (is_array($keyword)) {
                  $query_or = db_or();
                  foreach ($keyword as $keyword_value) {
                    $query_or->condition('ns.field_name_of_the_student_value', '%' . db_like($keyword_value) . '%', $field_data['operator']);
                  }
                  $query->condition($query_or);
                }
                else {
                  $query->condition('ns.field_name_of_the_student_value', '%' . db_like($keyword_value) . '%', $field_data['operator']);
                }
              }
              else {
                if (is_array($keyword)) {
                  $query_and = db_and();
                  foreach ($keyword as $keyword_value) {
                    $query_and->condition('ns.field_name_of_the_student_value', '%' . db_like($keyword_value) . '%', $field_data['operator']);
                  }
                  $query->condition($query_and);
                }
                else {
                  $query->condition('ns.field_name_of_the_student_value', '%' . db_like($keyword_value) . '%', $field_data['operator']);
                }
              }
            }
          }
        }
      }
    }
    $nids = $query->execute()->fetchAll();
    if (!empty($nids)) {
      $serialised_nids = serialize($nids);
      $form['nid_values'] = array(
        '#type' => 'hidden',
        '#value' => $serialised_nids
      );
    }
  }
  return $form;
}

/**
 *implement confirm_form_submit().
 *
 * @param $form
 *  Form object
 *
 * @param $form_state
 * Form state values
 *
 */
function look_for_delete_confirm_form_submit($form , &$form_state) {
  if (isset($form_state['input']['nid_values'])) {
    $needed_nids = $form_state['input']['nid_values'];
    $current_nids = unserialize($needed_nids);
    if (isset($form_state['input']['update_options'])) {
      $current_options = unserialize($form_state['input']['update_options']);
      foreach ($current_options as $current_options_value) {
        $current_option = $current_options_value;
      }
      $operations[] = array('look_for_delete_confirm_batch_process_1',array($current_options, $current_nids));
      $batch = array(
        'title' => t('Processing'),
        'operations' => $operations,
        'finished' => 'look_for_delete_batch_finished',
        'file' => drupal_get_path('module', 'look_for_delete') . '/delete_form/sites/all/modules/custom/look_for_delete.inc',
      );
      batch_set($batch);
      batch_process();
    }
    elseif (isset($form_state['input']['user_delete'])) {
      $user_delete = t("delete_user_permenently");
      $operations[] = array('look_for_delete_confirm_batch_process_2',array($current_nids, $user_delete));
      $uids = array();
      $batch = array(
        'title' => t('Processing'),
        'operations' => $operations,
        'finished' => 'look_for_delete_batch_finished',
      );
      batch_set($batch);
    }
    else {
      drupal_set_message('Error');
      drupal_goto('delete-contents');
    }
  }
}

/**
 *implement batch process for updation.
 *
 * @param $current_options
 *  Selected update option
 *
 * @param $current_nids
 * Node get as query output
 *
 */
function look_for_delete_confirm_batch_process_1($current_options, $current_nids) {
  foreach ($current_nids as $current_nid_value) {
    $current_nid = $current_nid_value->nid;
    foreach ($current_options as $current_option) {
      if (($current_option == 'unpublish') || ($current_option == 'publish')) {
        $nodes = node_load($current_nid);
        foreach ($nodes as $node) {
          if ($current_option == 'unpublish') {
            $nodes->status = 0;
            node_save($nodes);
          }
          else {
            $nodes->status = 1;
            node_save($nodes);
          }
        }
      }
      else {
        node_delete($current_nid);
      }
    }
  }
}

/**
 *implement batch process for user delete.
 *
 * @param $current_nids
 *  Node get as query output
 *
 * @param $user_delete
 * Status of checkbox for delete the user
 *
 */
function look_for_delete_confirm_batch_process_2($current_nids, $user_delete) {
  if (!empty($user_delete)) {
    foreach ($current_nids as $uids) {
      $current_uid = $uids->uid;
      user_delete($current_uid);
    }
  }
}

/**
 *implement confirm_form_submit().
 *
 * @param $success
 *  Batch output
 *
 * @param $results
 * Status of checkbox for delete the user
 *
 *@param $operations
 *  Node get as query output
 *
 */
function look_for_delete_batch_finished($success) {
  if ($success) {
    drupal_set_message('All done');
  }
  else {
    drupal_set_message('Error');
  }
  drupal_goto('delete-contents');
}

/**
 *Function to get csv file.
 *
 * @param $form
 *  Form object
 *
 * @param $form_state
 * Form state values
 *
 */
function look_for_delete_csv($form, &$form_state) {
  $needed_nid_csv = $form_state['input']['nid_values'];
  $current_nid_csv = unserialize($needed_nid_csv);
  if(!empty($current_nid_csv)) {
    //add necessary headers for browsers
     drupal_add_http_header('Content-Disposition', 'attachment; filename = look_for_ delete_csv_file.csv');
    //instead of writing down to a file we write to the output stream
    $fh = fopen('php://output', 'w');
    //form header
    fputcsv($fh, array(t('NIDS'), t('Node Title'), t('User')));
    //write data in the CSV format
    foreach ($current_nid_csv as $current_nids_values) {
      $csv_nid = $current_nids_values->nid;
      $csv_nodes = node_load($csv_nid);
      $data = array($csv_nodes->nid, $csv_nodes->title, $csv_nodes->name);
      fputcsv($fh, array($csv_nodes->nid, $csv_nodes->title, $csv_nodes->name));
    }
    //close the stream
    fclose($fh);
    drupal_exit();
  }
}

