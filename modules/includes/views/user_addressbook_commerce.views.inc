<?php

/**
 * Export User addressbook commerce to Views.
 */

/**
 * Implements hook_views_data_alter()
 */
function user_addressbook_commerce_views_data_alter(&$data) {

  $data['user_address']['user_address_commerce_checkout_select'] = array(
    'title' => t('Select user address'),
    'help' => t('Set the current address as default.'),
    'field' => array(
      'handler' => 'user_addressbook_field_handler_user_address_commerce_checkout_select',
    ),
  );
}
