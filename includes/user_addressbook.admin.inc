<?php

/**
 * @file
 * User page callback file for the user_addressbook module.
 */

 /**
 * Form builder; edit a user account or one of their profile categories.
 *
 * @ingroup forms
 * @see user_account_form()
 * @see user_account_form_validate()
 * @see user_profile_form_validate()
 * @see user_profile_form_submit()
 * @see user_cancel_confirm_form_submit()
 */
function user_addressbook_address_form($form, &$form_state, $user_address = NULL) {
  global $user;

  // During initial form build, add the entity to the form state for use during
  // form building and processing. During a rebuild, use what is in the form
  // state.
  if (empty($form_state['user_address'])) {
    if (!$user_address) {
      $user_address = user_addressbook_new($user->uid);
    }
    $form_state['user_address'] = $user_address;
  }
  // Only show name field on registration form or user can change own username.
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Address name'),
    '#description' => t('Name for this address, must be unique'),
    '#required' => TRUE,
    '#default_value' => $user_address->name,
    '#weight' => -20,
  );

  field_attach_form('user_address', $user_address, $form, $form_state);

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  /*
  $form['actions']['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel account'),
    '#submit' => array('user_edit_cancel_submit'),
    '#access' => $account->uid > 1 && (($account->uid == $user->uid && user_access('cancel account')) || user_access('administer users')),
  );
  }
  */

  $form['#validate'][] = 'user_addressbook_address_form_validate';
  // Add the final user addressbook form submit handler.
  $form['#submit'][] = 'user_addressbook_address_form_submit';

  return $form;
}

/**
 * Validation function for the user account and profile editing form.
 */
function user_addressbook_address_form_validate($form, &$form_state) {
  entity_form_field_validate('user_address', $form, $form_state);
}

/**
 * Submit function for the user account and profile editing form.
 */
function user_addressbook_address_form_submit($form, &$form_state) {
  $user_address = &$form_state['user_address'];
  foreach ($form_state['values'] as $key => $value) {
    if (isset($user_address->{$key})) {
      $user_address->{$key} = $value;
    }
  }

  // Notify field widgets.
  field_attach_submit('user_address', $user_address, $form, $form_state);

  // Save the profile.
  entity_save('user_address', $user_address);
  $form_state['redirect'] = 'user/' . $user_address->uid . '/addresses';

  drupal_set_message(t('The changes have been saved.'));
}
