<?php
// $Id$

/**
 * @file
 * Provides Rules integration for entities provided via the CRUD API.
 *
 * Rules automatically provides us with actions for CRUD and a suiting entity
 * data type. For events the controller automatically invokes Rules events once
 * Rules is active, so we just have to provide the appropriate info.
 */

/**
 * Default controller for generating Rules integration.
 */
class EntityDefaultRulesController {

  protected $type, $info;

  public function __construct($type) {
    $this->type = $type;
    $this->info = entity_get_info($type);
  }

  public function eventInfo() {
    $info = $this->info;
    $type = $this->type;

    $label = $info['label'];
    $defaults = array(
      'module' => isset($info['module']) ? $info['module'] : 'entity',
      'group' => $label,
      'access callback' => 'entity_rules_integration_access',
    );

    $items[$type . '_insert'] = $defaults + array(
      'label' => t('After saving a new @entity', array('@entity' => drupal_strtolower($label))),
      'variables' => array(
        $type => array('type' => $type, 'label' => t('created @entity', array('@entity' => drupal_strtolower($label)))),
      ),
    );
    $items[$type . '_update'] = $defaults + array(
      'label' => t('After updating an existing @entity', array('@entity' => drupal_strtolower($label))),
      'variables' => array(
        $type => array('type' => $type, 'label' => t('updated @entity', array('@entity' => drupal_strtolower($label)))),
      ),
    );
    $items[$type . '_presave'] = $defaults + array(
      'label' => t('Before saving a @entity', array('@entity' => drupal_strtolower($label))),
      'variables' => array(
        $type => array('type' => $type, 'label' => t('saved @entity', array('@entity' => drupal_strtolower($label)))),
      ),
    );
    $items[$type . '_delete'] = $defaults + array(
      'label' => t('After deleting a @entity', array('@entity' => drupal_strtolower($label))),
      'variables' => array(
        $type => array('type' => $type, 'label' => t('deleted @entity', array('@entity' => drupal_strtolower($label)))),
      ),
    );
    // Specify that on presave the entity is saved anyway.
    $items[$type . '_presave']['variables'][$type]['skip save'] = TRUE;
    return $items;
  }

}

/**
 * Implements hook_rules_event_info().
 */
function entity_rules_event_info() {
  $items = array();
  foreach (entity_get_info() as $type => $info) {
    if (isset($info['controller class']) && in_array('EntityAPIControllerInterface', class_implements($info['controller class']))) {
      $info += array('rules controller class' => 'EntityDefaultRulesController');
      $controller = new $info['rules controller class']($type);
      $items += $controller->eventInfo();
    }
  }
  return $items;
}

/**
 * Rules integration access callback.
 */
function entity_rules_integration_access($type, $name) {
  if ($type == 'event') {
    return TRUE;
    // @todo: Fix once entity_access() has been implemented.
    //return entity_access('view', $type);
  }
}