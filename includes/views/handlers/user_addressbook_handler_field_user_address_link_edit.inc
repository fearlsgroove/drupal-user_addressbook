<?php

/**
 * Field handler to present a customer profile edit link.
 */
class user_addressbook_handler_field_user_address_link_edit extends user_addressbook_handler_field_user_address_link {

  function render($values) {
    // Ensure the user has access to edit this profile.
    $user_address = user_addressbook_new();
    $user_address->address_id = $this->get_value($values, 'address_id');
    $user_address->uid = $this->get_value($values, 'uid');



    $text = !empty($this->options['text']) ? $this->options['text'] : t('edit');

    return l($text, 'user/' . $user_address->uid . '/addresses/address/' . $user->address_id . '/edit', array('query' => drupal_get_destination()));
  }
}
