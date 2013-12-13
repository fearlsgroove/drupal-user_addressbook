<?php

/**
 * Field handler to present a link to delete an user address.
 */
class user_addressbook_handler_field_user_address_link_delete extends user_addressbook_handler_field_user_address_link {

  function render($values) {
    // Ensure the user has access to delete this user address.
    $user_address = user_addressbook_new();
    $user_address->address_id = $this->get_value($values, 'address_id');
    $user_address->uid = $this->get_value($values, 'uid');

    if (!user_address_access('edit', $user_address)) {
      return;
    }

    $text = !empty($this->options['text']) ? $this->options['text'] : t('delete');

    return l($text, 'user/' . $user_address->uid . '/addresses/delete/' . $user_address->address_id, array('query' => drupal_get_destination()));
  }
}
