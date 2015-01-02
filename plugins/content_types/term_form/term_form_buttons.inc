<?php

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'single' => TRUE,
  'icon' => 'icon_term_form.png',
  'title' => t('Term form submit buttons'),
  'description' => t('Submit buttons for the term form.'),
  'required context' => new ctools_context_required(t('Form'), 'term_form'),
  'category' => t('Form'),
);

function taxonomy_panels_term_form_buttons_content_type_render($subtype, $conf, $panel_args, &$context) {
  $block = new stdClass();
  $block->module = t('term_form');

  $block->title = '';
  $block->delta = 'buttons';

  if (isset($context->form)) {
    $block->content = array();
    foreach (array('actions', 'form_token', 'form_build_id', 'form_id') as $element) {
      $block->content[$element] = isset($context->form[$element]) ? $context->form[$element] : NULL;
      unset($context->form[$element]);
    }
  }
  else {
    $block->content = t('Term form buttons.');
  }
  return $block;
}

function taxonomy_panels_term_form_buttons_content_type_admin_title($subtype, $conf, $context) {
  return t('"@s" term form submit buttons', array('@s' => $context->identifier));
}

function taxonomy_panels_term_form_buttons_content_type_edit_form($form, &$form_state) {
  // provide a blank form so we have a place to have context setting.
  return $form;
}
