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
  'title'       => t('Simulpub serie'),
  'single'      => TRUE,
  'description' => t('Return simulpub serie.'),
  'category'    => t('Editis Kurokawa'),
);

/**
 * Output function for the 'EK: site logo' content type.
 *
 * Outputs the logo for the current page.
 */
function ek_simulpub_simulpub_serie_content_type_render($subtype, $conf, $panel_args) {
  $output = '';
  if ($series = ek_simulpub_get_current_series()) {
    foreach ($series as $serie) {
      $params = array(
        'serie' => $serie,
      );
      $output .= theme('ek_simulpub_serie', $params);
    }
  }
  if ($output) {
    $output = '<div class="block-simulpub"><h2>' . t('Simulpub') . '</h2>' . $output . '</div>';
  }
  return (object) array('content' => $output);
}
