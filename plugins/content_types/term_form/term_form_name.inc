<?php

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'single' => TRUE,
  'icon' => 'icon_term_form.png',
  'title' => t('Term form name field'),
  'description' => t('The term name form.'),
  'required context' => new ctools_context_required(t('Form'), 'term_form'),
  'category' => t('Form'),
);

function taxonomy_panels_term_form_name_content_type_render($subtype, $conf, $panel_args, &$context) {
  $block = new stdClass();
  $block->module = t('term_form');

  $block->delta = 'name-options';

  if (isset($context->form)) {
    if (!empty($context->form['name'])) {
      $block->content['name'] = $context->form['name'];
      unset($context->form['name']);
    }
  }
  else {
    $block->content = t('Term name form.');
  }
  return $block;
}

function taxonomy_panels_term_form_name_content_type_admin_title($subtype, $conf, $context) {
  return t('"@s" term form name field', array('@s' => $context->identifier));
}

function taxonomy_panels_term_form_name_content_type_edit_form($form, &$form_state) {
  // provide a blank form so we have a place to have context setting.
  return $form;
}
