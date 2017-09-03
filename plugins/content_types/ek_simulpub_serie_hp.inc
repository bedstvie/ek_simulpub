<?php

/**
 * @file
 * Plugin to handle the 'page_logo' content type which allows the
 * logo of the site to be embedded into a panel.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title'       => t('EK: Simulpub serie in HP'),
  'single'      => TRUE,
  'description' => t('Return simulpub serie preview in HP.'),
  'category'    => t('Editis Kurokawa'),
);

/**
 * Output function for the 'EK: site logo' content type.
 *
 * Outputs the logo for the current page.
 */
function ek_simulpub_ek_simulpub_serie_hp_content_type_render($subtype, $conf, $panel_args) {
  $output = '';
  $query = db_select('ek_simulpub_serie', 'ss');
  $query->innerJoin('field_data_field_simulpub_serie_home', 'ssh', 'ssh.entity_id = ss.serie_id');
  $query->condition('ssh.field_simulpub_serie_home_value', '1')
    ->fields('ss', array('serie_id'))
    ->condition('ss.status', 1)
    ->orderBy('ss.changed', 'DESC')
    ->range(0, 1);
  $result = $query->execute()->fetchField();
  if (!empty($result)) {
    $serie = simulpub_serie_load($result);
    $params = array(
      'serie' => $serie,
    );
    $output .= theme('ek_simulpub_serie', $params);
  }
  if ($output) {
    $output = '<div class="block-simulpub"><h2>' . t('Simulpub') . '</h2>' . $output . '</div>';
  }
  return (object) array('content' => $output);
}
