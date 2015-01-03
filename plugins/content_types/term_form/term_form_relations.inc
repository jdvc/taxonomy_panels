<?php

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'single' => TRUE,
  'icon' => 'icon_term_form.png',
  'title' => t('Term form relations field'),
  'description' => t('The term relations form.'),
  'required context' => new ctools_context_required(t('Form'), 'term_form'),
  'category' => t('Form'),
);

function taxonomy_panels_term_form_relations_content_type_render($subtype, $conf, $panel_args, &$context) {
  $block = new stdClass();
  $block->module = t('term_form');

  $block->delta = 'relations-options';

  if (isset($context->form)) {
    if (!empty($context->form['relations'])) {
      $block->content['relations'] = $context->form['relations'];
      unset($context->form['relations']);
    }
  }
  else {
    $block->content = t('Term relations form.');
  }
  return $block;
}

function taxonomy_panels_term_form_relations_content_type_admin_title($subtype, $conf, $context) {
  return t('"@s" term form relations field', array('@s' => $context->identifier));
}

function taxonomy_panels_term_form_relations_content_type_edit_form($form, &$form_state) {
  // provide a blank form so we have a place to have context setting.
  return $form;
}