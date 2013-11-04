<?php

/**
 * Field handler to present a link to delete a customer profile.
 */
class user_addressbook_handler_field_user_address_link_delete extends user_addressbook_handler_field_user_address_link {

  function render($values) {
    // Ensure the user has access to delete this profile.
    $user_address = user_addressbook_new();
    $user_address->address_id = $this->get_value($values, 'address_id');
    $user_address->uid = $this->get_value($values, 'uid');

    //if (!commerce_customer_profile_access('update', $profile)) {
    //  return;
    //}

    $text = !empty($this->options['text']) ? $this->options['text'] : t('delete');

    return l($text, 'user/' . $user_address->uid . '/addresses/address/' . $user->address_id . '/delete', array('query' => drupal_get_destination()));
  }
}
