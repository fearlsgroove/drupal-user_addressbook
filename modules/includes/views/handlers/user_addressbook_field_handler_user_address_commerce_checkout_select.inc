<?php

/**
 * @file
 *
 * A Views' field handler for editing a node title.
 *
 */
class user_addressbook_field_handler_user_address_commerce_checkout_select extends views_handler_field {

  function construct() {
    parent::construct();
    $this->additional_fields['address_id'] = 'address_id';
    $this->additional_fields['uid'] = 'uid';
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  /**
   * Render the field contents.
   *
   * @param $values
   * @return string
   */
  function render($values) {
    // Render a Views form item placeholder.
    return '<!--form-item-' . $this->options['id'] . '--' . $this->view->row_index . '-->';
  }

  /**
   * Add to and alter the form.
   */
  function views_form(&$form, &$form_state) {
    // Create a container for our replacements
    $form[$this->options['id']] = array(
      '#type' => 'container',
      '#tree' => TRUE,
    );
    // Iterate over the result and add our replacement fields to the form.
    foreach($this->view->result as $row_index => $row) {
      $uid = $row->{$this->aliases['uid']};
      $user_default_address = user_addressbook_get_default_addresses($uid, $this->view->args[1]);
      // Add a text field to the form.  This array convention
      // corresponds to the placeholder HTML comment syntax.
      $form[$this->options['id']][$row_index] = array(
        '#type' => 'checkbox',
        '#return_value' => $row->{$this->aliases['address_id']},
        '#disabled' => ($user_default_address == $row->{$this->aliases['address_id']}),
        '#title' => $this->options['label'] ? $this->options['label'] : $this->definition['title'],
        '#default_value' => ($user_default_address == $row->{$this->aliases['address_id']}) ? TRUE : FALSE,
        '#element_validate' => array('user_addressbook_field_handler_commerce_checkout_default_address_validate'),
        '#ajax' => array(
          'callback' => 'user_addressbook_commerce_checkout_pane_refresh',
          'wrapper' => 'user-addressbook-ajax-wrapper',
        ),
      );
    }
    $this->options['label'] = '';

    // Submit to the current page if not a page display.
    if ($this->view->display_handler->plugin_name != 'page') {
      $form['#action'] = current_path();
    }
  }

}
