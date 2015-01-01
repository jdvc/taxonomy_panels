<?php

/**
 * @file
 * Plugin to provide an relationship handler for term from term.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t('Term edit form from term'),
  'keyword' => 'term_form',
  'description' => t('Adds term edit form from a term context.'),
  'required context' => new ctools_context_required(t('Term'), 'term'),
  'context' => 'ctools_term_edit_form_from_term_context',
);

/**
 * Return a new context based on an existing context.
 */
function ctools_term_edit_form_from_term_context($context, $conf) {
  if (empty($context->data)) {
    return ctools_context_create_empty('term_edit_form', NULL);
  }

  if (isset($context->data->tid)) {
    return ctools_context_create('term_edit_form', $context->data);
  }
}
