<?php

/**
 * @file
 * Ajax functionality for ek_simulpub.
 */

/**
 * AJAX callback for Quick form.
 */
function ek_simulpub_serie_autofill_ajax_callback($form, &$form_state) {
  if (!form_get_errors()) {
    ek_simulpub_serie_autofill_default_values($form, $form_state);
  }

  return $form['entity'];
}

function ek_simulpub_serie_section($serie, $action, $mode = NULL, $section) {
  // Remove old messages;
  $commands[] = ajax_command_remove('.messages, .form-section-remove, .form-section-rename');

  switch ($action) {
    case 'add':
      $object = new stdClass();
      $object->serie_id = $serie->serie_id;
      $count = db_select('ek_simulpub_sections', 'ess')
        ->fields('ess', array('section_id'))
        ->condition('serie_id', $serie->serie_id)
        ->execute()
        ->rowCount();
      $object->title = 'Tome ' . ($count + 1);
      drupal_write_record('ek_simulpub_sections', $object);

      module_load_include('inc', 'ek_simulpub', 'includes/ek_simulpub.admin');

      $commands[] = ajax_command_after('form.drag-grid:last', drupal_render(drupal_get_form('ek_simulpub_chapitres_list_entities', $serie->serie_id, $object->section_id, $_GET['section_count'])));
      break;

    case 'remove':
      $commands[] = ajax_command_before('#ek-simulpub-chapitres-list-entities-' . $section, drupal_render(drupal_get_form('ek_simulpub_section_remove_confirm', $section)));
      break;

    case 'rename':
      $commands[] = ajax_command_before('#ek-simulpub-chapitres-list-entities-' . $section, drupal_render(drupal_get_form('ek_simulpub_section_rename', $serie->serie_id, $section)));
      break;
  }

  if ($mode != 'ajax') {
    drupal_goto(isset($_SERVER['HTTP_REFERER']) ? $_SERVER['HTTP_REFERER'] : '<front>');
  }
  return array('#type' => 'ajax', '#commands' => $commands);
}

function ek_simulpub_section_remove_confirm($form , &$form_state, $section) {
  $form['#id'] = 'ek-simulpub-section-remove-' . $section;
  $form['#attributes']['class'] = array('form-section-remove');

  $form['section'] = array(
    '#type' => 'value',
    '#value' => $section,
  );

  $form['desc'] = array(
    '#markup' => t('Are you sure you want to delete this section?'),
  );

  $form['actions'] = array(
    '#type' => 'actions',
  );

  $form['actions']['submit'] = array(
    '#type' => 'button',
    '#value' => t('Yes'),
    '#ajax' => array(
      'effect' => 'fade',
      'callback' => 'ek_simulpub_section_remove_confirm_submit',
    ),
  );

  $form['actions']['cancel'] = array(
    '#type' => 'button',
    '#value' => t('No'),
    '#ajax' => array(
      'effect' => 'fade',
      'callback' => 'ek_simulpub_section_remove_confirm_cancel',
    ),
  );

  return $form;
}

function ek_simulpub_section_remove_confirm_submit($form, &$form_state) {
  $section = $form_state['values']['section'];

  db_delete('ek_simulpub_sections')
    ->condition('section_id', $section)
    ->execute();

  $chapitres = db_update('ek_simulpub_chapitre')
    ->fields(
      array(
        'status' => 0,
        'section_id' => 0,
      )
    )
    ->condition('section_id', $section)
    ->execute();

  if ($chapitres) {
    drupal_set_message(
      t('All chapitres from this section are unpublished. You can find it on page !link.',
        array(
          '!link' => l(t('chapitres list'), SIMULPUB_MANAGE_CHAPITRE)
        )
      )
    );
    $commands[] = ajax_command_before('#ek-simulpub-chapitres-list-entities-' . $section, theme('status_messages'));
  }

  $commands[] = ajax_command_remove('#ek-simulpub-chapitres-list-entities-' . $section);
  $commands[] = ajax_command_remove('#ek-simulpub-section-remove-' . $section);

  return array('#type' => 'ajax', '#commands' => $commands);
}

function ek_simulpub_section_remove_confirm_cancel($form, &$form_state) {
  $section = $form_state['values']['section'];
  $commands[] = ajax_command_remove('#ek-simulpub-section-remove-' . $section);

  return array('#type' => 'ajax', '#commands' => $commands);
}

function ek_simulpub_section_rename($form , &$form_state, $serie_id, $section) {
  $form['#id'] = 'ek-simulpub-section-rename-' . $section;
  $form['#attributes']['class'] = array('form-section-rename');

  $form['serie_id'] = array(
    '#type' => 'value',
    '#value' => $serie_id,
  );

  $form['section'] = array(
    '#type' => 'value',
    '#value' => $section,
  );

  $title = db_select('ek_simulpub_sections', 'ess')
    ->fields('ess', array('title'))
    ->condition('section_id', $section)
    ->execute()
    ->fetchField();

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Rename tome'),
    '#required' => TRUE,
    '#default_value' => $title,
    '#size' => 60,
  );

  $form['actions'] = array(
    '#type' => 'actions',
  );

  $form['actions']['submit'] = array(
    '#type' => 'button',
    '#value' => t('Save'),
    '#ajax' => array(
      'effect' => 'fade',
      'callback' => 'ek_simulpub_section_rename_submit',
    ),
  );

  $form['actions']['cancel'] = array(
    '#type' => 'button',
    '#value' => t('No'),
    '#limit_validation_errors' => array(),
    '#ajax' => array(
      'effect' => 'fade',
      'callback' => 'ek_simulpub_section_rename_cancel',
    ),
  );

  return $form;
}

function ek_simulpub_section_rename_submit($form, &$form_state) {
  $serie_id = $form_state['values']['serie_id'];
  $section = $form_state['values']['section'];

  if (!form_get_errors()) {
    $object = new stdClass();
    $object->section_id = $section;
    $object->serie_id = $serie_id;
    $object->title = $form_state['values']['title'];
    drupal_write_record('ek_simulpub_sections', $object, array('section_id'));

    $rename = array(
      '#theme' => 'link',
      '#prefix' => '<div class="section-title">',
      '#suffix' => '</div>',
      '#text' => $form_state['values']['title'],
      '#path' => 'js/simulpub_serie_section/' . $serie_id . '/rename/nojs/' . $section,
      '#options' => array(
        'attributes' => array('class' => array('use-ajax')),
        'html' => FALSE
      ),
    );

    $commands[] = ajax_command_replace('#ek-simulpub-chapitres-list-entities-' . $section . ' .section-title', drupal_render($rename));
    $commands[] = ajax_command_remove('#ek-simulpub-section-rename-' . $section);
  }
  $commands[] = ajax_command_before('#ek-simulpub-chapitres-list-entities-' . $section, theme('status_messages'));

  return array('#type' => 'ajax', '#commands' => $commands);
}

function ek_simulpub_section_rename_cancel($form, &$form_state) {
  $section = $form_state['values']['section'];
  $commands[] = ajax_command_remove('#ek-simulpub-section-rename-' . $section);

  return array('#type' => 'ajax', '#commands' => $commands);
}
