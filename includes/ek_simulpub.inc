<?php

/**
 * @file
 * Functions for ek_simulpub.
 */

function ek_simulpub_data_order($data, $order) {
  asort($order);
  $order_data = array();
  foreach ($order as $key => $weight) {
    if (isset($data[$key])) {
      $order_data[$key] = $data[$key];
      $order_data[$key]->weight = $weight['weight'];
    }
    unset($data[$key]);
  }

  if ($data) {
    $max = 0;
    if ($order) {
      $max = max(array_map('current', $order));
    }

    foreach ($data as $id => $entity) {
      $max ++;
      $order_data[$id] = $entity;
      $order_data[$id]->weight = $max;
    }
  }

  return $order_data;
}

function ek_simulpub_drupal_add_tablegriddrag($table_id, $action, $relationship, $group, $subgroup = NULL, $source = NULL, $hidden = TRUE, $limit = 0) {
  $js_css_added = &drupal_static(__FUNCTION__, FALSE);
  if (!$js_css_added) {
    $module_path = drupal_get_path('module', 'ek_simulpub');
    drupal_add_css($module_path . '/css/ek_simulpub_tablegriddrag.css', array('weight' => 0));
    drupal_add_js($module_path . '/js/ek_simulpub_tablegriddrag.js', array('weight' => 0));
    $js_css_added = TRUE;
  }
  drupal_add_tabledrag($table_id, $action, $relationship, $group, $subgroup, $source, $hidden, $limit);
}

function ek_simulpub_get_chapitre_statuses_list() {
  return array(
    '0' => t('Draft'),
    '3' => t('Program'),
    '2' => t('Offline'),
    '1' => t('Online'),
  );
}

function ek_simulpub_get_serie_statuses_list() {
  return array(
    '0' => t('Offline'),
    '1' => t('Online'),
  );
}

function ek_simulpub_serie_autofill_default_values(&$form, &$form_state) {
  $serie = $form_state['serie'];
  $wrapper = entity_metadata_wrapper('node', $serie);

  $map = ek_simulpub_serie_map();

  $instances = field_info_instances('simulpub_serie');
  foreach ($instances['ek_simulpub_serie'] as $key => $instance) {
    if (isset($map[$key]) && isset($form['entity'][$key])) {
      $new_value = $wrapper->{$map[$key]}->value();

      switch ($instance['widget']['type']) {
        case 'text_textfield':
        case 'text_textarea':
          $new_value = $new_value['value'];
          ek_simulpub_serie_warning($form['entity'][$key][LANGUAGE_NONE][0]['value']['#value'], $new_value, $instance['label']);
          $form['entity'][$key][LANGUAGE_NONE][0]['value']['#value'] = $new_value;
          break;

        case 'link_field':
          $new_value = $new_value['input'];
          ek_simulpub_serie_warning($form['entity'][$key][LANGUAGE_NONE][0]['#value']['url'], $new_value, $instance['label']);
          $form['entity'][$key][LANGUAGE_NONE][0]['url']['#value'] = $new_value;
          break;

        case 'entityreference_autocomplete':
          $new_value = $new_value->name . ' (' . $new_value->tid . ')';
          ek_simulpub_serie_warning($form['entity'][$key][LANGUAGE_NONE][0]['target_id']['#value'], $new_value, $instance['label']);
          $form['entity'][$key][LANGUAGE_NONE][0]['target_id']['#value'] = $new_value;
          break;

        case 'options_select':
          $options = $form['entity'][$key][LANGUAGE_NONE]['#options'];
          ek_simulpub_serie_warning($options[$form['entity'][$key][LANGUAGE_NONE]['#value']], $options[$new_value], $instance['label']);
          $form['entity'][$key][LANGUAGE_NONE]['#value'] = $new_value;
          break;
      }
    }
  }

  $new_value = $wrapper->field_authors->value();
  $base_author = $form['entity']['field_simulpub_serie_authors'][LANGUAGE_NONE][0];
  $child = element_children($form['entity']['field_simulpub_serie_authors'][LANGUAGE_NONE]);
  foreach ($child as $key) {
    if ($key != 'add_more') {
      unset($form['entity']['field_simulpub_serie_authors'][LANGUAGE_NONE][$key]);
    }
  }

  foreach ($new_value as $key => $new_author) {
    $wrapper = entity_metadata_wrapper('node', $new_author);
    $base_author['field_simulpub_serie_author'][LANGUAGE_NONE][0]['target_id']['#value'] = $new_author->title . ' (' . $new_author->nid . ')';
    $base_author['field_simulpub_serie_author_id'][LANGUAGE_NONE][0]['value']['#value'] = $wrapper->field_ean13->value();
    $form['entity']['field_simulpub_serie_authors'][LANGUAGE_NONE][$key] = $base_author;
  }

  ek_simulpub_serie_warning($form['entity']['title']['#value'], $serie->title, t('Title of the serie'));
  $form['entity']['title']['#value'] = $serie->title;
}

function ek_simulpub_serie_map() {
  return array(
    'field_simulpub_serie_resume' => 'body',
    'field_status' => 'field_status',
    'field_simulpub_serie_collection' => 'field_collection',
    'field_simulpub_serie_trailer' => 'field_youtube_video',
  );
}

function ek_simulpub_serie_warning($old, $new, $key) {
  $old = trim($old);
  $new = trim($new);
  if ($old && $old != $new) {
    drupal_set_message(
      t(
        'Field "!key" replaced from <pre>"!old"</pre> to <pre>"!new"</pre>',
        array(
          '!key' => $key,
          '!old' => $old,
          '!new' => $new,
        )
      ),
      'warning'
    );
  }
}

function ek_simulpub_get_current_series($node_serie = FALSE) {
  if (!$node_serie) {
    if (arg(0) == 'simulpub' && arg(1)) {
      $node_serie = node_load(arg(1));
    }
    else {
      $node_serie = menu_get_object();
    }
  }

  $series = FALSE;
  if ($node_serie) {
    if ($node_serie->type = 'serie') {
      $wrapper = entity_metadata_wrapper('node', $node_serie);
      $yonix_id =  $wrapper->field_ean13->value();
      if ($yonix_id) {
        $series = ek_simulpub_get_simulpub_series_by_yonix_id($yonix_id);
      }
    }
  }
  elseif (arg(0) == 'simulpub' && !arg(1)) {
    if (!$serie = menu_get_object('ek_simulpab_serie')) {
      // Get last simulpab for front.
      $arr = variable_get('ek_simulpub_order', array());

      $query = db_select('ek_simulpub_serie', 'ess')
        ->fields('ess', array('serie_id'))
        ->condition('ess.status', 1);
      $serie_ids = $query->execute()->fetchCol();

      $sort_ids = array_sort_by_column($arr, 'weight', 'SORT_ASC');
      foreach ($serie_ids as $k => $v) {
        $order_serie[$sort_ids[$v]] = $v;
      }
      ksort($order_serie);
      foreach ($order_serie as $k => $v) {
        $sids[] = $v;
      }
      if ($sids) {
        $series = simulpub_serie_load_multiple($sids);
      }
    }
  }

  return $series;
}


function array_sort_by_column(&$arr, $col, $dir = SORT_ASC) {
  $sort_col = array();
  foreach ($arr as $k => $v) {
    $sort_col[$k] = $v[$col];
  }
  asort($sort_col);
  return $sort_col;
}


function ek_simulpub_get_chapitres_query($serie_id) {
  $query = db_select('ek_simulpub_chapitre', 'esc');
  $query->fields('esc', array('chapitre_id'));
  $query->join('ek_simulpub_sections', 'ess', 'ess.section_id = esc.section_id');
  $query->condition('ess.serie_id', $serie_id);
  return $query;
}

function ek_simulpub_get_chapitres_count($serie_id) {
  $query = ek_simulpub_get_chapitres_query($serie_id);
  $query->condition('esc.status', 1);
  return $query->execute()->rowCount();
}

function ek_simulpub_get_serie_by_yonix_id($yonix_id) {
  $nid = db_select('field_data_field_ean13', 'fdfe')
    ->fields('fdfe', array('entity_id'))
    ->condition('field_ean13_value', $yonix_id)
    ->condition('bundle', 'serie')
    ->execute()
    ->fetchField();

  $serie = FALSE;
  if ($nid) {
    $serie = node_load($nid);
  }

  return $serie;
}

function ek_simulpub_get_simulpub_series_by_yonix_id($yonix_id) {
  $serie_ids = db_select('ek_simulpub_serie', 'ess')
    ->fields('ess', array('serie_id'))
    ->condition('id_serie_yonix', $yonix_id)
    ->condition('status', 1)
    ->execute()
    ->fetchCol();

  $series = FALSE;
  if ($serie_ids) {
    $series = simulpub_serie_load_multiple($serie_ids);
  }

  return $series;
}

function ek_simulpub_get_sections($serie_id, $all_fields = FALSE) {
  $query = db_select('ek_simulpub_sections', 'ess');
  if ($all_fields) {
    $query->fields('ess', array());
  }
  else {
    $query->fields('ess', array('section_id'));
  }
  return $query->condition('serie_id', $serie_id)
    ->execute();
}

function ek_simulpub_get_chapitre_viewed_state($chapitre) {
  if ($chapitre->status == 3) {
    $wrapper = entity_metadata_wrapper('simulpub_chapitre', $chapitre);
    if ($date =  $wrapper->field_chapitre_program->value()) {
      $start = strtotime($date['value']);
      $end = strtotime($date['value2']);
      if ($start && $start > REQUEST_TIME) {
        return 2;
      }
      elseif ($end && $end < REQUEST_TIME) {
        return 2;
      }
    }

    return 1;
  }
  else {
    return $chapitre->status;
  }
}
