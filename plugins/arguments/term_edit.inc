<?php

/**
 * @file
 *
 * Plugin to provide an argument handler for a term edit form
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t("Term edit form: term ID"),
  // keyword to use for %substitution
  'keyword' => 'term',
  'description' => t('Creates a term edit form context from a term ID argument.'),
  'context' => 'taxonomy_panels_term_edit_context',
  'placeholder form' => array(
    '#type' => 'textfield',
    '#description' => t('Enter the term ID of a term for this argument'),
  ),
);

/**
 * Discover if this argument gives us the term we crave.
 */
function taxonomy_panels_term_edit_context($arg = NULL, $conf = NULL, $empty = FALSE) {
  // If unset it wants a generic, unfilled context.
  if ($empty) {
    return ctools_context_create_empty('term_edit_form');
  }

  // We can accept either a term object or a pure tid.
  if (is_object($arg)) {
    return ctools_context_create('term_edit_form', $arg);
  }

  if (!is_numeric($arg)) {
    return FALSE;
  }

  $term = taxonomy_term_load($arg);
  if (!$term) {
    return NULL;
  }

  // This will perform a node_access check, so we don't have to.
  return ctools_context_create('term_edit_form', $term);
}

