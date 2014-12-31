<?php
/**
 * Specialized implementation of hook_page_manager_task_tasks(). See api-task.html for
 * more information.
 */
function taxonomy_panels_term_edit_page_manager_tasks() {
  dpm('first_functin');
  return array(
    // This is a 'page' task and will fall under the page admin UI
    'task type' => 'page',

    'title' => t('Term add/edit form'),
    'admin title' => t('Term add/edit form'),
    'admin description' => t('When enabled, this overrides the default Drupal behavior for adding or edit nodes at <em>node/%node/edit</em> and <em>node/add/%node_type</em>. If you add variants, you may use selection criteria such as node type or language or user access to provide different edit forms for nodes. If no variant is selected, the default Drupal node edit will be used.'),
    'admin path' => 'taxonomy/term/%taxonomy_term/edit',

    // Menu hooks so that we can alter the term/%taxonomy_term/edit menu entry to point to us.
    'hook menu' => 'taxonomy_panels_term_edit_menu',
    'hook menu alter' => 'taxonomy_panels_term_edit_menu_alter',

    // This is task uses 'context' handlers and must implement these to give the
    // handler data it needs.
    'handler type' => 'context',
    'get arguments' => 'taxonomy_panels_term_edit_get_arguments',
    'get context placeholders' => 'taxonomy_panels_term_edit_get_contexts',

    // Allow this to be enabled or disabled:
    'disabled' => variable_get('taxonomy_panels_term_edit_disabled', TRUE),
    'enable callback' => 'taxonomy_panels_term_edit_enable',
    'access callback' => 'taxonomy_panels_term_edit_access_check',
  );
}

/**
 * Callback defined by term_edit_term_edit_page_manager_tasks().
 *
 * Alter the term edit input so that term edit comes to us rather than the
 * normal term edit process.
 */
function taxonomy_panels_term_edit_menu_alter(&$items, $task) {
  dpm('TEST');
  dpm('taxonomy_panels_term_edit_menu_alter');
  if (variable_get('taxonomy_panels_term_edit_disabled', TRUE)) {
    return;
  }

  $callback = $items['taxonomy/term/%taxonomy_term/edit']['page callback'];
  // Override the term edit handler for our purpose.
  if ($callback == 'drupal_get_form' || variable_get('taxonomy_panels_override_anyway', FALSE)) {
    // this is overriding the menu system
    $items['taxonomy/term/%taxonomy_term/edit']['page callback'] = 'taxonomy_panels_term_edit';
    $items['taxonomy/term/%taxonomy_term/edit']['file path'] = $task['path'];
    $items['taxonomy/term/%taxonomy_term/edit']['file'] = $task['file'];
  }
  else {
    variable_set('taxonomy_panels_term_edit_disabled', TRUE);
    if (!empty($GLOBALS['taxonomy_panels_enabling_term_edit'])) {
      drupal_set_message(t('Taxonomy panels module is unable to enable taxonomy/term/%taxonomy_term because some other module already has overridden with %callback.', array('%callback' => $callback)), 'warning');
    }
    return;
  }

  //Also catch node/add handling:
  foreach (taxonomy_get_vocabularies() as $type) {
    $path = 'taxonomy/' . str_replace('_', '-', $type->type) . '/add';
    if ($items[$path]['page callback'] != 'drupal_get_form') {
      if (!empty($GLOBALS['taxonomy_panels_enabling_term_edit'])) {
        drupal_set_message(t('Taxonomy panels module is unable to override @path because some other module already has overridden with %callback. Node edit will be enabled but that edit path will not be overridden.', array('@path' => $path, '%callback' => $items[$path]['page callback'])), 'warning');
      }
      continue;
    }

    //$items[$path]['page callback'] = 'page_manager_node_add';
    $items[$path]['page callback'] = 'drupal_get_form';
    // maybe we have to add extra arguments here
    $items[$path]['file path'] = $task['path'];
    $items[$path]['file'] = $task['file'];
    // Why str_replace things back?
    $items[$path]['page arguments'] = array($type->type);
  }
}

/**
 * Entry point for our overridden term edit.
 *
 * This function asks its assigned handlers who, if anyone, would like
 * to run with it. If no one does, it passes through to Drupal core's
 * term edit, which is drupal_get_form() with extra arguments...
 */
function taxonomy_panels_term_edit($term) {

  $task = page_manager_get_task('term_edit');
  //$task = taxonomy_panels_get_task('term_edit');

  // Load the node into a context.
  ctools_include('context');
  ctools_include('context-task-handler');
  $contexts = ctools_context_handler_get_task_contexts($task, '', array($term));

  $arg = array(isset($term->tid) ? $term->tid : $term->vid);
  $output = ctools_context_handler_render($task, '', $contexts, $arg);
  if ($output === FALSE) {
    // Fall back!
    // We've already built the form with the context, so we can't build it again, or
    // form_clean_id will mess up our ids. But we don't really need to, either:
    $context = reset($contexts);
    $output = $context->form;
  }

  return $output;
}

/**
 * Callback to handle the process of adding a term.
 *
 * This creates a basic $term and passes that off to page_manager_term_edit().
 * It is modeled after Drupal's node_add() function.
 *
 * Unlike node_add() we do not need to check node_access because that was
 * already checked by the menu system.
 */
function taxonomy_panels_term_add($type) {
  global $user;

  $types = taxonomy_get_vocabularies();

  // Initialize settings:
  $term = (object) array(
    'uid' => $user->uid,
    'name' => (isset($user->name) ? $user->name : ''),
    'type' => $type,
    'language' => LANGUAGE_NONE,
  );

  drupal_set_title(t('Create @name', array('@name' => $types[$type]->name)));
  return taxonomy_panels_term_edit($term);
}

/**
 * Callback to get arguments provided by this task handler.
 *
 * Since this is the node edit and there is no UI on the arguments, we
 * create dummy arguments that contain the needed data.
 */
function taxonomy_panels_term_edit_get_arguments($task, $subtask_id) {
  $task_array = array(
    array(
      'keyword' => 'term',
      'identifier' => t('Term being edited'),
      'id' => 1,
      'name' => 'term_edit',
      'settings' => array(),
    ),
  ); 
  // dpm($task_array);
  //

}

/**
 * Callback to get context placeholders provided by this handler.
 */
function taxonomy_panels_term_edit_get_contexts($task, $subtask_id) {
  return ctools_context_get_placeholders_from_argument(taxonomy_panels_term_edit_get_arguments($task, $subtask_id));
}

/**
 * Callback to enable/disable the page from the UI.
 */
function taxonomy_panels_term_edit_enable($cache, $status) {
  variable_set('taxonomy_panels_term_edit_disabled', $status);
  // Set a global flag so that the menu routine knows it needs
  // to set a message if enabling cannot be done.
  if (!$status) {
    $GLOBALS['taxonomy_panels_enabling_term_edit'] = TRUE;
  }
}

/**
 * Callback to determine if a page is accessible.
 *
 * @param $task
 *   The task plugin.
 * @param $subtask_id
 *   The subtask id
 * @param $contexts
 *   The contexts loaded for the task.
 * @return
 *   TRUE if the current user can access the page.
 */
function taxonomy_panels_term_edit_access_check($task, $subtask_id, $contexts) {
  $context = reset($contexts);
  return taxonomy_term_edit_access('update', $context->data);
}

