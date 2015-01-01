<?php

/**
 * Load the contexts for a task, using arguments.
 *
 * This creates the base array of contexts, loaded from arguments, suitable
 * for use in rendering.
 */
function taxonomy_panels_context_handler_get_task_contexts($task, $subtask, $args) {
  
  $contexts = ctools_context_handler_get_base_contexts($task, $subtask);

  $arguments = ctools_context_handler_get_task_arguments($task, $subtask);

  taxonomy_panels_context_get_context_from_arguments($arguments, $contexts, $args);

  return $contexts;
}


/**
 * Load the contexts for a given list of arguments.
 *
 * @param $arguments
 *   The array of argument definitions.
 * @param &$contexts
 *   The array of existing contexts. New contexts will be added to this array.
 * @param $args
 *   The arguments to load.
 *
 * @return
 *   FALSE if an argument wants to 404.
 */
function taxonomy_panels_context_get_context_from_arguments($arguments, &$contexts, $args) {
  
  foreach ($arguments as $argument) {
    // pull the argument off the list.
    $arg = array_shift($args);
    $id = ctools_context_id($argument, 'argument');
    
    // For % arguments embedded in the URL, our context is already loaded.
    // There is no need to go and load it again.
    if (empty($contexts[$id])) {
      if ($context = taxonomy_panels_context_get_context_from_argument($argument, $arg)) {
        $contexts[$id] = $context;
      }
    }
    else {
      $context = $contexts[$id];
    }

    if ((empty($context) || empty($context->data)) && !empty($argument['default']) && $argument['default'] == '404') {
      return FALSE;
    }
  }
  return TRUE;
}

/**
 * Get a context from an argument.
 *
 * @param $argument
 *   The configuration of an argument. It must contain the following data:
 *   - name: The name of the argument plugin being used.
 *   - argument_settings: The configuration based upon the plugin forms.
 *   - identifier: The human readable identifier for this argument, usually
 *     defined by the UI.
 *   - keyword: The keyword used for this argument for substitutions.
 *
 * @param $arg
 *   The actual argument received. This is expected to be a string from a URL but
 *   this does not have to be the only source of arguments.
 * @param $empty
 *   If true, the $arg will not be used to load the context. Instead, an empty
 *   placeholder context will be loaded.
 *
 * @return
 *   A context object if one can be loaded.
 */
function taxonomy_panels_context_get_context_from_argument($argument, $arg, $empty = FALSE) {
  ctools_include('plugins');
  ctools_include('arguments/term_edit', 'taxonomy_panels', 'plugins');
  if (empty($argument['name']) || $argument['name'] != 'term_edit') {
    return;
  }

  $function = 'taxonomy_panels_term_edit_context';
    
  // Backward compatibility: Merge old style settings into new style:
  if (!empty($argument['settings'])) {
    $argument += $argument['settings'];
    unset($argument['settings']);
  }

  $context = $function($arg, $argument, $empty);
  
  if (is_object($context)) {
    $context->identifier = $argument['identifier'];
    $context->page_title = isset($argument['title']) ? $argument['title'] : '';
    $context->keyword    = $argument['keyword'];
    $context->id         = ctools_context_id($argument, 'argument');
    $context->original_argument = $arg;

    if (!empty($context->empty)) {
      $context->placeholder = array(
        'type' => 'argument',
        'conf' => $argument,
      );
    }
  }
  return $context;
  
}
