<?php

/**
 * @file
 * Contains page callbacks and theme functions for the frontend UI.
 */

function ek_simulpub_serie_list_entities($form, &$form_state) {
  $form = array();
  $rows = array();
  $row_elements = array();

  // Put it into a fieldset for no reason.
  $form['data_table'] = array(
    '#type' => 'fieldset',
    '#title' => t('Series'),
  );

  $data = simulpub_serie_load_multiple();
  $order = variable_get('ek_simulpub_order', array());
  $data = ek_simulpub_data_order($data, $order);

  $statuses = ek_simulpub_get_serie_statuses_list();

  // Build the rows.
  foreach ($data as $id => $entity) {
    // Build the table rows.
    $rows[$id] = array(
      'data' => array(
        // Cell for the cross drag&drop element.
        array('class' => array('entry-cross')),

        // Weight item for the tabledrag.
        'weight' => array(
          'data' => array(
            '#type' => 'weight',
            '#title' => t('Weight'),
            '#title_display' => 'invisible',
            '#default_value' => $entity->weight,
            '#parents' => array('data_table', $id, 'weight'),
            '#attributes' => array(
              'class' => array('entry-order-weight'),
            ),
          )
        ),

        // Title.
        'title' => array(
          'data' => array(
            '#markup' => '<div>' . l($entity->title, SIMULPUB_MANAGE_SERIE . '/' . $entity->serie_id . '/chapitres') . '</div>',
          ),
          'class' => array('td-title'),
        ),

        // State.
        'state' => array(
          'data' => array(
            '#markup' => $statuses[$entity->status],
          ),
          'class' => array(
            'td-state',
            $entity->status ? 'online' : 'offline',
          ),
        ),

        // Operations.
        'operation_1' => array(
          'data' => array(
            '#theme' => 'link',
            '#text' => t('Edit'),
            '#path' => SIMULPUB_MANAGE_SERIE . '/' . $entity->serie_id . '/edit',
            '#options' => array('attributes' => array(), 'html' => FALSE),
          ),
          'class' => array('td-operation', 'first'),
        ),
        'delimeter' => array(
          'data' => array(
            '#markup' => '|',
          ),
          'class' => array('td-delimeter'),
        ),
        'operation_2' => array(
          'data' => array(
            '#theme' => 'link',
            '#text' => t('Supp.'),
            '#path' => SIMULPUB_MANAGE_SERIE . '/' . $entity->serie_id . '/delete',
            '#options' => array('attributes' => array(), 'html' => FALSE),
          ),
          'class' => array('td-operation', 'second'),
        ),
      ),
      'class' => array('draggable'),
    );
    // Build rows of the form elements in the table.
    $row_elements[$id] = array(
      'weight' => &$rows[$id]['data']['weight']['data'],
      'title' => &$rows[$id]['data']['title']['data'],
    );
  }

  $rows[] = array(
    'data' => array(
      // Cell for the cross drag&drop element.
      array('class' => array('entry-cross')),
      // Weight item for the tabledrag.
      'weight' => array(),
      // Plus.
      'title' => array(
        'data' => array(
          '#theme' => 'link',
          '#text' => '+',
          '#path' => SIMULPUB_MANAGE_SERIE . '/manage/add',
          '#options' => array(
            'attributes' => array(
              'class' => array('plus'),
            ),
            'html' => FALSE
          ),
          '#prefix' => '<div>',
          '#suffix' => '</div>',
        ),
        'class' => array('td-title'),
      ),
    ),
  );

  // Add the table to the form.
  $form['data_table']['table'] = array(
    '#theme' => 'table',
    // The row form elements need to be processed and build,
    // therefore pass them as element children.
    'elements' => $row_elements,
    '#header' => array(),
    '#rows' => $rows,
    '#empty' => t('There are no entries available.'),
    '#attributes' => array('id' => 'entry-order'),
  );
  ek_simulpub_drupal_add_tablegriddrag('entry-order', 'order', 'sibling', 'entry-order-weight');

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

function ek_simulpub_serie_list_entities_submit(&$form, &$form_state) {
  variable_set('ek_simulpub_order', $form_state['values']['data_table']);
}

/**
 * Returns a render array with all entity_example_basic entities.
 *
 * In this basic example we know that there won't be many entities,
 * so we'll just load them all for display. See pager_example.module
 * to implement a pager. Most implementations would probably do this
 * with the contrib Entity API module, or a view using views module,
 * but we avoid using non-core features in the Examples project.
 *
 * @see pager_example.module
 */
function ek_simulpub_list_entities($type) {
  $content = array();
  // Load all of our entities.
  switch ($type) {
    case 'simulpub_chapitre':
      $entities = simulpub_chapitre_load_multiple();
      $id       = 'chapitre_id';
      break;

    case 'simulpub_serie':
      $entities = simulpub_serie_load_multiple();
      $id       = 'serie_id';
      break;
  }

  if (!empty($entities)) {
    foreach ($entities as $entity) {
      $title = $entity->title;
      if ($type == 'simulpub_chapitre') {
        if (!$entity->section_id) {
          $title .= ' (<span class="marker">' . t('without section') . '</span>)';
        }
      }
      // Create tabular rows for our entities.
      $rows[] = array(
        'data' => array(
          'id' => $entity->{$id},
          'title' => $title,
          'actions' => l(t('Edit'), 'admin/structure/simulpub/' . $type . '/' . $entity->{$id} . '/edit'),
        ),
      );
    }

    $header = array(t('ID'), t('Title'), t('Actions'));
    // Put our entities into a themed table. See theme_table() for details.
    $content['entity_table'] = array(
      '#theme' => 'table',
      '#rows' => $rows,
      '#header' => $header,
    );
  }
  else {
    // There were no entities. Tell the user.
    $content[] = array(
      '#type' => 'item',
      '#markup' => t('No entities currently exist.'),
    );
  }
  return $content;
}

/**
 * Provides a wrapper on the edit form to add a new entity.
 */
function ek_simulpub_serie_add() {
  // Create a basic entity structure to be used and passed to the validation
  // and submission functions.
  $entity = entity_get_controller('simulpub_serie')->create();
  return drupal_get_form('ek_simulpub_serie_form', $entity);
}

/**
 * Form function to create an entity_example_basic entity.
 *
 * The pattern is:
 * - Set up the form for the data that is specific to your
 *   entity: the columns of your base table.
 * - Call on the Field API to pull in the form elements
 *   for fields attached to the entity.
 */
function ek_simulpub_serie_form($form, &$form_state, $entity) {
  $form['#ajax-id'] = drupal_html_class('ek-simulpub-serie-wrapper');

  $form['entity'] = array(
    '#type' => 'container',
    '#prefix' => '<div id="' . $form['#ajax-id'] . '">',
    '#suffix' => '</div>',
  );

  $form['entity']['serie_wrapper'] = array(
    '#type' => 'container',
    '#weight' => -10,
  );

  $form['entity']['serie_wrapper']['id_serie_yonix'] = array(
    '#type' => 'textfield',
    '#title' => t('Id Serie Yonix'),
    '#required' => TRUE,
    '#size' => 20,
    '#default_value' => $entity->id_serie_yonix,
    '#element_validate' => array('ek_simulpub_serie_yonix_id_validate'),
  );

  // AJAX form submitting.
  $form['entity']['serie_wrapper']['autofill'] = array(
    '#type' => 'button',
    '#value' => t('Automatic filling'),
    // Skip form validation for button.
    '#validate' => array('ek_simulpub_autofill_validate'),
    '#limit_validation_errors' => array(
      array('id_serie_yonix'),
    ),
    '#ajax' => array(
      'effect' => 'fade',
      'callback' => 'ek_simulpub_serie_autofill_ajax_callback',
      'method' => 'replace',
      'wrapper' => $form['#ajax-id'],
    ),
  );

  $form['entity']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title of the serie'),
    '#required' => TRUE,
    '#default_value' => $entity->title,
  );

  $form['entity']['simulpub_serie_entity'] = array(
    '#type' => 'value',
    '#value' => $entity,
  );

  field_attach_form('simulpub_serie', $entity, $form['entity'], $form_state);

  $form['entity']['status_wrapper'] = array(
    '#type' => 'container',
    '#weight' => 99,
  );

  $form['entity']['status_wrapper']['status'] = array(
    '#type' => 'radios',
    '#title' => t('Status'),
    '#required' => TRUE,
    '#options' => ek_simulpub_get_serie_statuses_list(),
    '#default_value' => $entity->status,
  );

  $form['actions'] = array(
    '#type' => 'actions',
    '#weight' => 100,
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

function ek_simulpub_serie_yonix_id_validate(&$element, &$form_state) {
  if (!empty($form_state['values']['id_serie_yonix'])) {
    if ($serie = ek_simulpub_get_serie_by_yonix_id($form_state['values']['id_serie_yonix'])) {
      $form_state['serie'] = $serie;
    }
    else {
      form_set_error('id_serie_yonix', t('Serie node with this id do not found'));
    }
  }
}

function ek_simulpub_autofill_validate(&$form, &$form_state) {
  module_load_include('inc', 'ek_simulpub', 'includes/ek_simulpub.ajax');
  if (!form_get_errors()) {
    $wrapper = entity_metadata_wrapper('node', $form_state['serie']);
    $new_value = $wrapper->{'field_image'}->value();
    if ($new_value) {
      $form_state['simulpub']['field_simulpub_serie_visual_left'] = (object)$new_value;
    }
  }
}

/**
 * Validation handler for entity_example_basic_add_form form.
 *
 * We pass things straight through to the Field API to handle validation
 * of the attached fields.
 */
function ek_simulpub_serie_form_validate($form, &$form_state) {
  field_attach_form_validate('simulpub_serie', $form_state['values']['simulpub_serie_entity'], $form, $form_state);
}

/**
 * Form submit handler: Submits basic_add_form information.
 */
function ek_simulpub_serie_form_submit($form, &$form_state) {
  $entity = $form_state['values']['simulpub_serie_entity'];
  $entity->id_serie_yonix = $form_state['values']['id_serie_yonix'];
  $entity->title = $form_state['values']['title'];
  $entity->status = $form_state['values']['status'];
  field_attach_submit('simulpub_serie', $entity, $form, $form_state);
  $entity = simulpub_serie_save($entity);
  $form_state['redirect'] = SIMULPUB_MANAGE;
}

function ek_simulpub_serie_delete_form($form, &$form_state, $serie) {
  $form['simulpub_serie_entity'] = array(
    '#type' => 'value',
    '#value' => $serie,
  );

  return confirm_form($form,
    t('Can you delete entity %title (Yonix serie ID: %id_serie_yonix)?',
      array('%title' => $serie->title, '%id_serie_yonix' => $serie->id_serie_yonix)),
    SIMULPUB_MANAGE,
    '',
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Form deletion handler.
 */
function ek_simulpub_serie_delete_form_submit($form, &$form_state) {
  $entity = $form_state['values']['simulpub_serie_entity'];
  simulpub_serie_delete($entity);
  drupal_set_message(t('The entity %title (Yonix serie ID: %id_serie_yonix) has been deleted',
      array('%title' => $entity->title, '%id_serie_yonix' => $entity->id_serie_yonix))
  );
  $form_state['redirect'] = SIMULPUB_MANAGE;
}

function ek_simulpub_serie_chapitres_form($entity) {
  $output = array();

  $output[] = array(
    '#markup' => '<h2>' . t('Serie') . ' ' . $entity->title . '</h2>'
  );

  $sections = ek_simulpub_get_sections($entity->serie_id);
  if (!$sections->rowCount()) {
    $object = new stdClass();
    $object->serie_id = $entity->serie_id;
    $object->title = 'Tome 1';
    drupal_write_record('ek_simulpub_sections', $object);
    $sections = array($object);
  }


  foreach ($sections as $section_count => $section) {
    $output['section_' . $section->section_id] = drupal_get_form('ek_simulpub_chapitres_list_entities', $entity->serie_id, $section->section_id, $section_count + 1);
  }

  return $output;
}

function ek_simulpub_chapitres_list_entities($form, &$form_state, $serie_id, $section_id, $section_count) {
  $form = array();

  $form['#action'] = base_path() . SIMULPUB_MANAGE_SERIE . '/' . $serie_id . '/chapitres';

  $form['#id'] = 'ek-simulpub-chapitres-list-entities-' . $section_id;
  $form['#attributes']['class'] = array('drag-grid');

  $form['section_id'] = array(
    '#type' => 'hidden',
    '#value' => $section_id,
  );

  $chapitres = db_select('ek_simulpub_chapitre', 'esc')
    ->fields('esc', array('chapitre_id'))
    ->condition('section_id', $section_id)
    ->orderBy('esc.num', 'ASC')
    ->execute()
    ->fetchCol();

  $rows = array();
  $row_elements = array();

  $section = db_select('ek_simulpub_sections', 'ess')
    ->fields('ess', array('data', 'title'))
    ->condition('section_id', $section_id)
    ->execute()
    ->fetchAssoc();

  $form['rename'] = array(
    '#theme' => 'link',
    '#prefix' => '<div class="section-title">',
    '#suffix' => '</div>',
    '#text' => $section['title'],
    '#path' => 'js/simulpub_serie_section/' . $serie_id . '/rename/nojs/' . $section_id,
    '#options' => array('attributes' => array('class' => array('use-ajax')), 'html' => FALSE),
  );

  $form['remove'] = array(
    '#theme' => 'link',
    '#prefix' => '<div class="section-action section-remove">',
    '#suffix' => '</div>',
    '#text' => '-',
    '#path' => 'js/simulpub_serie_section/' . $serie_id . '/remove/nojs/' . $section_id,
    '#options' => array('attributes' => array('class' => array('use-ajax')), 'html' => FALSE),
  );

  // Put it into a fieldset for no reason.
  $form['data_table'] = array(
    '#type' => 'fieldset',
    '#title' => '',
  );

  $data = simulpub_chapitre_load_multiple($chapitres);

  $section_options = FALSE;
  if ($section && $section['data']) {
    $section_options = unserialize($section['data']);
  }
  $order = isset($section_options['order']) ? $section_options['order'] : array();
  $data = ek_simulpub_data_order($data, $order);

  $statuses = ek_simulpub_get_chapitre_statuses_list();

  // Build the rows.
  foreach ($data as $id => $entity) {
    // Build the table rows.
    $rows[$id] = array(
      'data' => array(
        // Cell for the cross drag&drop element.
        array('class' => array('entry-cross')),

        // Weight item for the tabledrag.
        'weight' => array(
          'data' => array(
            '#type' => 'weight',
            '#title' => t('Weight'),
            '#title_display' => 'invisible',
            '#default_value' => $entity->weight,
            '#parents' => array('data_table', $id, 'weight'),
            '#attributes' => array(
              'class' => array('entry-order-weight'),
            ),
          )
        ),

        // Title.
        'title' => array(
          'data' => array(
            '#markup' => '<div>' . $entity->title . '</div>',
          ),
          'class' => array('td-title'),
        ),

        // State.
        'state' => array(
          'data' => array(
            '#markup' => $statuses[$entity->status],
          ),
          'class' => array(
            'td-state',
            $entity->status ? 'online' : 'offline',
          ),
        ),

        // Operations.
        'operation_1' => array(
          'data' => array(
            '#theme' => 'link',
            '#text' => t('Edit'),
            '#path' => SIMULPUB_MANAGE_CHAPITRE . '/' . $entity->chapitre_id . '/edit',
            '#options' => array('attributes' => array(), 'html' => FALSE),
          ),
          'class' => array('td-operation', 'first'),
        ),
        'delimeter' => array(
          'data' => array(
            '#markup' => '|',
          ),
          'class' => array('td-delimeter'),
        ),
        'operation_2' => array(
          'data' => array(
            '#theme' => 'link',
            '#text' => t('Supp.'),
            '#path' => SIMULPUB_MANAGE_CHAPITRE . '/' . $entity->chapitre_id . '/delete',
            '#options' => array('attributes' => array(), 'html' => FALSE),
          ),
          'class' => array('td-operation', 'second'),
        ),
      ),
      'class' => array('draggable'),
    );
    // Build rows of the form elements in the table.
    $row_elements[$id] = array(
      'weight' => &$rows[$id]['data']['weight']['data'],
      'title' => &$rows[$id]['data']['title']['data'],
    );
  }

  $rows[] = array(
    'data' => array(
      // Cell for the cross drag&drop element.
      array('class' => array('entry-cross')),
      // Weight item for the tabledrag.
      'weight' => array(),
      // Plus.
      'title' => array(
        'data' => array(
          '#theme' => 'link',
          '#text' => '+',
          '#path' => SIMULPUB_MANAGE_CHAPITRE . '/manage/add/' . $serie_id . '/' . $section_id,
          '#options' => array(
            'attributes' => array(
              'class' => array('plus'),
            ),
            'html' => FALSE
          ),
          '#prefix' => '<div>',
          '#suffix' => '</div>',
        ),
        'class' => array('td-title'),
      ),
    ),
  );

  $table_id =  'entry-order-' . $section_id;
  // Add the table to the form.
  $form['data_table']['table'] = array(
    '#theme' => 'table',
    // The row form elements need to be processed and build,
    // therefore pass them as element children.
    'elements' => $row_elements,
    '#header' => array(),
    '#rows' => $rows,
    '#empty' => t('There are no entries available.'),
    '#attributes' => array('id' => $table_id),
  );

  ek_simulpub_drupal_add_tablegriddrag($table_id, 'order', 'sibling', 'entry-order-weight');

  $form['add'] = array(
    '#theme' => 'link',
    '#prefix' => '<div class="section-action section-add">',
    '#suffix' => '</div>',
    '#text' => '+',
    '#path' => 'js/simulpub_serie_section/' . $serie_id . '/add/nojs',
    '#options' => array(
      'attributes' => array(
        'class' => array('use-ajax')
      ),
      'html' => FALSE,
      'query' => array(
        'section_count' => $section_count + 1,
      ),
    ),
  );

  drupal_add_library('system', 'drupal.ajax');
  drupal_add_library('system', 'jquery.form');

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

function ek_simulpub_chapitres_list_entities_submit(&$form, &$form_state) {
  if (isset($form_state['values']['data_table'])) {
    $object = new stdClass();
    $object->section_id = $form_state['values']['section_id'];
    $object->data = array('order' => $form_state['values']['data_table']);
    drupal_write_record('ek_simulpub_sections', $object, array('section_id'));
  }
}

/**
 * Provides a wrapper on the edit form to add a new entity.
 */
function ek_simulpub_chapitre_add($serie, $section) {
  // Create a basic entity structure to be used and passed to the validation
  // and submission functions.
  $entity = entity_get_controller('simulpub_chapitre')->create($serie->serie_id);
  $entity->field_chapitre_serie[LANGUAGE_NONE][0]['target_id'] = $serie->serie_id;
  $entity->section_id = $section;

  return drupal_get_form('ek_simulpub_chapitre_form', $entity, $serie->serie_id);
}

/**
 * Form function to create an entity_example_basic entity.
 *
 * The pattern is:
 * - Set up the form for the data that is specific to your
 *   entity: the columns of your base table.
 * - Call on the Field API to pull in the form elements
 *   for fields attached to the entity.
 */
function ek_simulpub_chapitre_form($form, &$form_state, $entity, $serie_id = FALSE) {
  $form['#suffix'] = '<div class="preview"/>';

  $form['simulpub_chapitre_entity'] = array(
    '#type' => 'value',
    '#value' => $entity,
  );

  field_attach_form('simulpub_chapitre', $entity, $form, $form_state);

  if ($entity->field_chapitre_serie[LANGUAGE_NONE][0]['target_id']) {
    $form['field_chapitre_serie']['#access'] = FALSE;

    if (!$entity->section_id) {
      $sections = ek_simulpub_get_sections($entity->field_chapitre_serie[LANGUAGE_NONE][0]['target_id'], TRUE);
      $options = array();
      foreach ($sections as $key => $section) {
        $options[$section->section_id] = $section->title;
      }
      $form['section_id'] = array(
        '#type' => 'select',
        '#title' => t('Section'),
        '#options' => $options,
        '#required' => TRUE,
      );
    }
  }

  $form['title_wrapper'] = array(
    '#type' => 'container',
    '#weight' => -9,
  );

  $form['title_wrapper']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title of the chapitre'),
    '#required' => TRUE,
    '#default_value' => $entity->title,
  );

  $form['num_wrapper'] = array(
    '#type' => 'container',
    '#weight' => -8,
  );

  $form['num_wrapper']['num'] = array(
    '#type' => 'textfield',
    '#title' => t('Chapter Number'),
    '#required' => TRUE,
    '#default_value' => $entity->num,
  );

  $form['status_wrapper'] = array(
    '#type' => 'container',
    '#weight' => 11,
  );

  $form['status_wrapper']['status'] = array(
    '#type' => 'radios',
    '#title' => t('Status'),
    '#required' => TRUE,
    '#options' => ek_simulpub_get_chapitre_statuses_list(),
    '#default_value' => $entity->status,
  );

  $form['field_chapitre_designation']['#weight'] = -10;
  $form['field_chapitre_program']['#weight'] = 12;
  $form['field_chapitre_permalink']['#weight'] = 13;

  $form['actions'] = array(
    '#type' => 'actions'
  );

  $form['actions']['preview'] = array(
    '#type' => 'button',
    '#value' => t('Preview'),
    '#ajax' => array(
      'effect' => 'fade',
      'callback' => 'ek_simulpub_chapitre_form_preview',
    ),
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

function ek_simulpub_chapitre_form_preview(&$form, &$form_state) {
  if (form_get_errors()) {
    $commands[] = ajax_command_replace('.preview', '<div class="preview">' . theme('status_messages') . '</div>');
  }
  else {
    $entity = $form_state['values']['simulpub_chapitre_entity'];

    $entity->title = $form_state['values']['title'];
    $entity->status = $form_state['values']['status'];
    if (isset($form_state['values']['section_id'])) {
      $entity->section_id = $form_state['values']['section_id'];
    }

    $_SESSION['entity_simulpub_preview'] = $entity;

    $url = url('js/simulpub/' . $form_state['values']['field_chapitre_serie'][LANGUAGE_NONE][0]['target_id']);
    $commands[] = ajax_command_replace('.preview', '<div class="preview"><iframe src="' . $url . '" width="100%" height="600px" frameborder="0"></iframe></div>');
  }

  return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 * Validation handler for entity_example_basic_add_form form.
 *
 * We pass things straight through to the Field API to handle validation
 * of the attached fields.
 */
function ek_simulpub_chapitre_form_validate($form, &$form_state) {
  field_attach_form_validate('simulpub_chapitre', $form_state['values']['simulpub_chapitre_entity'], $form, $form_state);
  if (!is_numeric($form_state['values']['num'])) {
    form_set_error('num', t('Chapter Number must be numeric'));
  }
  if ($form_state['values']['status'] == 3) {
    if (!$form_state['values']['field_chapitre_program'][LANGUAGE_NONE][0]['value']
      || !$form_state['values']['field_chapitre_program'][LANGUAGE_NONE][0]['value2']) {
      form_set_error('field_chapitre_program', t('Field program required for status programe'));
    }
  }
}

/**
 * Form submit handler: Submits basic_add_form information.
 */
function ek_simulpub_chapitre_form_submit($form, &$form_state) {
  $entity = $form_state['values']['simulpub_chapitre_entity'];

  $entity->title = $form_state['values']['title'];
  $entity->status = $form_state['values']['status'];
  if (isset($form_state['values']['section_id'])) {
    $entity->section_id = $form_state['values']['section_id'];
  }
  $entity->num = (int) $form_state['values']['num'];

  field_attach_submit('simulpub_chapitre', $entity, $form, $form_state);

  $entity = simulpub_chapitre_save($entity);


  // Save chapter weight.
  $section = db_select('ek_simulpub_sections', 'ess')
    ->fields('ess', array('data', 'title'))
    ->condition('section_id', $entity->section_id)
    ->execute()
    ->fetchAssoc();
  $data = unserialize($section['data']);
  $data['order'][$entity->chapitre_id]['weight'] = $form_state['values']['num'];
  $data_s = serialize($data);

  db_update('ek_simulpub_sections')
    ->fields(array('data' => $data_s))
    ->condition('section_id', $entity->section_id)
    ->execute();



  $form_state['redirect'] = SIMULPUB_MANAGE_SERIE . '/' . $entity->field_chapitre_serie[LANGUAGE_NONE][0]['target_id'] . '/chapitres';
}

function ek_simulpub_chapitre_delete_form($form, &$form_state, $chapitre) {
  $form['simulpub_chapitre_entity'] = array(
    '#type' => 'value',
    '#value' => $chapitre,
  );

  return confirm_form($form,
    t('Can you delete entity %title?',
      array('%title' => $chapitre->title)),
    SIMULPUB_MANAGE_SERIE . '/' . $chapitre->field_chapitre_serie[LANGUAGE_NONE][0]['target_id'] . '/chapitres',
    '',
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Form deletion handler.
 */
function ek_simulpub_chapitre_delete_form_submit($form, &$form_state) {
  $entity = $form_state['values']['simulpub_chapitre_entity'];
  simulpub_chapitre_delete($entity);
  drupal_set_message(t('The entity %title has been deleted',
      array('%title' => $entity->title))
  );
  $form_state['redirect'] = SIMULPUB_MANAGE_SERIE . '/' . $entity->field_chapitre_serie[LANGUAGE_NONE][0]['target_id'] . '/chapitres';
}
