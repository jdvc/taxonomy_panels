<?php

/**
 * @file
 *
 * Plugin to provide a term_edit_form context
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t("Term edit form"),
  'description' => t('A term edit form.'),
  'context' => 'taxonomy_panels_context_create_term_edit_form',
  'edit form' => 'taxonomy_panels_context_term_edit_form_settings_form',
  'defaults' => array(
  	'vid' => '',
  	'tid' => '',
  ),
  'keyword' => 'term_edit',
  'context name' => 'term_edit_form',
  'convert list' => 'taxonomy_panels_context_term_edit_convert_list',
  'convert' => 'taxonomy_panels_context_term_edit_convert',
  'placeholder form' => array(
    '#type' => 'textfield',
    '#description' => t('Enter the term ID for this argument:'),
  ),
);

function taxonomy_panels_context_create_term_edit_form($empty, $term = NULL, $conf = FALSE) {
  static $creating = FALSE;
  $context = new ctools_context(array('form', 'term_form', 'term_edit_form', 'term', 'taxonomy_term', 'entity:taxonomy_term'));
  $context->plugin = 'term_edit_form';

  if ($empty || ($creating)) {
    return $context;
  }
  $creating = TRUE;

  if ($conf) {
    // In this case, $node is actually our $conf array.
    $tid = is_array($term) && isset($term['tid']) ? $term['tid'] : (is_object($term) ? $term->tid : 0);

    if (is_array($term) || !empty($reload)) {
      $term = taxonomy_term_load($tid);
    }
  }

  if (!empty($term)) {
    $form_id = 'taxonomy_form_term';

    $form_state = array('want form' => TRUE, 'build_info' => array('args' => array($term)));

    $file = drupal_get_path('module', 'taxonomy') . '/taxonomy.admin.inc';
    require_once DRUPAL_ROOT . '/' . $file;
    // This piece of information can let other modules know that more files
    // need to be included if this form is loaded from cache:
    $form_state['build_info']['files'] = array($file);

    $form = drupal_build_form($form_id, $form_state);

    // Fill in the 'term' portion of the context
    $context->data     = $term;
    $context->title    = isset($term->name) ? $term->name : '';
    $context->argument = isset($term->tid) ? $term->tid : '';

    $context->form       = $form;
    $context->form_state = &$form_state;
    $context->form_id    = $form_id;
    $context->form_title = isset($term->name) ? $term->name : '';
    $context->vid  = $term->vid;
    //$context->restrictions['type'] = array($node->type);
    $context->restrictions['form'] = array('form');

    $creating = FALSE;
    return $context;
  }
  $creating = FALSE;
}

/**
 * Provide a list of ways that this context can be converted to a string.
 */
function taxonomy_panels_context_term_edit_convert_list() {
  // Use the "term" context convert list.
  return array(
    'tid' => t('Term ID'),
    'name' => t('Term name'),
    'name_dashed' => t('Term name, lowercased and spaces converted to dashes'),
    'description' => t('Term Description'),
    'vid' => t('Vocabulary ID'),
  );
}

/**
 * Convert a context into a string.
 */
function taxonomy_panels_context_term_edit_convert($context, $type) {
  // Pass through to the "term" context convert list.
  $plugin = ctools_get_context('term');
  return ctools_context_term_convert($context, $type);
}

function taxonomy_panels_context_term_edit_form_settings_form($form, &$form_state) {
  $conf = &$form_state['conf'];
  
  $form['term'] = array(
    '#title' => t('Enter the title or ID of a taxonomy_term entity'),
    '#type' => 'textfield',
    '#maxlength' => 512,
    '#autocomplete_path' => 'ctools/autocomplete/taxonomy_term',
    '#weight' => -10,
  );

  if (!empty($conf['tid'])) {
    $info = entity_load('taxonomy_term', array($conf['tid']));
    $info = $info[$conf['tid']];
    if ($info) {
      /*$entity = entity_get_info($plugin['keyword']);
      $uri = entity_uri($plugin['keyword'], $info);
      if (is_array($uri) && $entity['entity keys']['label']) {
        $link = l(t("'%title' [%type id %id]", array('%title' => $info->{$entity['entity keys']['label']}, '%type' => $plugin['keyword'], '%id' => $conf['entity_id'])), $uri['path'], array('attributes' => array('target' => '_blank', 'title' => t('Open in new window')), 'html' => TRUE));
      }
      elseif (is_array($uri)) {
        $link = l(t("[%type id %id]", array('%type' => $plugin['keyword'], '%id' => $conf['entity_id'])), $uri['path'], array('attributes' => array('target' => '_blank', 'title' => t('Open in new window')), 'html' => TRUE));
      }
      elseif ($entity['entity keys']['label']) {
        $link = l(t("'%title' [%type id %id]", array('%title' => $info->{$entity['entity keys']['label']}, '%type' => $plugin['keyword'], '%id' => $conf['entity_id'])), file_create_url($uri), array('attributes' => array('target' => '_blank', 'title' => t('Open in new window')), 'html' => TRUE));
      }
      else {
        $link = t("[%type id %id]", array('%type' => $plugin['keyword'], '%id' => $conf['entity_id']));
      }*/

      $link = l(t("'%name' [term id %tid]", array('%name' => $info->name, '%tid' => $info->tid)), "taxonomy/term/$info->tid", array('attributes' => array('target' => '_blank', 'title' => t('Open in new window')), 'html' => TRUE));
      $form['term']['#description'] = t('Currently set to !link', array('!link' => $link));
    }
  }

  $form['tid'] = array(
    '#type' => 'value',
    '#value' => $conf['tid'],
  );

  $form['set_identifier'] = array(
    '#type' => 'checkbox',
    '#default_value' => FALSE,
    '#title' => t('Reset identifier to term name'),
    '#description' => t('If checked, the identifier will be reset to the term name of the selected term.'),
  );

  return $form;
}

/**
 * Validate a term
 */
function taxonomy_panels_context_term_edit_form_settings_form_validate($form, &$form_state) {
  // Validate the autocomplete
  if (empty($form_state['values']['tid']) && empty($form_state['values']['term'])) {
    form_error($form['term'], t('You must select a term.'));
    return;
  }

  if (empty($form_state['values']['term'])) {
    return;
  }

  $tid          = $form_state['values']['term'];
  $preg_matches = array();
  $match        = preg_match('/\[id: (\d+)\]/', $tid, $preg_matches);
  if (!$match) {
    $match = preg_match('/^id: (\d+)/', $tid, $preg_matches);
  }

  if ($match) {
    $tid = $preg_matches[1];
  }

  if (is_numeric($tid)) {
    $term = db_query('SELECT tid FROM {taxonomy_term_data} WHERE tid = :tid', array(':tid' => $tid))->fetchObject();
  }
  else {
    $term = db_query('SELECT tid FROM {taxonomy_term_data} WHERE LOWER(name) = LOWER(:name)', array(':name' => $tid))->fetchObject();
  }
  
  if (!$term) {
    form_error($form['term'], t('Invalid term selected.'));
  }
  else {
    form_set_value($form['tid'], $term->tid, $form_state);
  }
}

function taxonomy_panels_context_term_edit_form_settings_form_submit($form, &$form_state) {
  if ($form_state['values']['set_identifier']) {
    $term = taxonomy_term_load($form_state['values']['tid']);
    $form_state['values']['identifier'] = $term->name;
  }

  // This will either be the value set previously or a value set by the
  // validator.
  $form_state['conf']['tid'] = $form_state['values']['tid'];
}
