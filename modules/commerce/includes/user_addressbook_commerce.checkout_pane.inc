<?php

/**
 * @file
 * Based on commerce/modules/customer/commerce_customer.checkout_pane.inc
 */

/**
 * Checkout pane callback: returns the customer profile pane's settings form.
 */
function user_addressbook_commerce_checkout_pane_settings_form($checkout_pane) {
  $form = array();

  return $form;
}

/**
 * Checkout pane callback: returns a customer profile edit form.
 */
function user_addressbook_commerce_checkout_pane_checkout_form($form, &$form_state, &$checkout_pane, $order) {
  global $user;
  $pane_form = array();

  $pane_id = $checkout_pane['pane_id'];
  foreach (commerce_customer_profile_types() as $type => $profile) {
    if ($profile['addressfield'] && $pane = reset(commerce_checkout_panes(array('pane_id' => 'customer_profile_' . $type)))) {
      //
    }
  }
  $mode = 'select_address';

  // If an AJAX rebuild happened, we might have our data in form state
  if (!empty($form_state['pane_' . $pane_id])) {
    $profile = $form_state['pane_' . $pane_id]['profile'];
    $mode = $form_state['pane_' . $pane_id]['mode'];
  }

  // No profile set yet. First see if one exists already.
  if (empty($profile)) {
    $profile = commerce_single_address_active_profile_load($user->uid, $type);

    if ($profile) {
      // Incomplete profiles go into edit mode. Others into view mode.
      //$mode = empty($profile->data['incomplete']) ? 'view' : 'edit';
    }
    else {
      // No profiles found. Create a new one.
      $profile = commerce_customer_profile_new($type, $order->uid);
      //$mode = 'new';
    }
  }
  $wrapper_id = strtr($pane_id, '_', '-') . '-ajax-wrapper';

  // Remember the current profile and mode in form state.
  $form_state['pane_' . $checkout_pane['pane_id']] = array(
    'profile' => $profile,
    'mode' => $mode,
  );

  $pane_form = array(
    '#parents' => array($pane_id),
    '#prefix' => '<div id="' . $wrapper_id . '">',
    '#suffix' => '</div>',
  );
  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['pane'] = drupal_get_path('module', 'user_addressbook_commerce') . '/includes/user_addressbook_commerce.checkout_pane.inc';


  // Get the view form in the pane.
  $user_addresses_checkout = _user_addressbook_retrieve_view_form($pane_form, $form_state, 'user_addressbook_checkout', 'default', array($user->uid, $wrapper_id, $type));
  $user_addresses_checkout['#profile_type'] = $type;

  unset($user_addresses_checkout['actions']);
  $pane_form['user_addresses'] = $user_addresses_checkout;
  $pane_form['user_addresses']['#weight'] = -10;

  if ($mode == 'select_address') {
    $pane_form['customer_profile'] = array(
      '#type' => 'submit',
      '#value' => t('Add new'),
      '#limit_validation_errors' => array(),
      '#ajax' => array(
        'callback' => 'user_addressbook_ajax_form_refresh',
        'wrapper' => strtr($pane_id, '_', '-') . '-ajax-wrapper',
      ),
      '#submit' => array('user_addressbook_commerce_checkout_pane_add_new_customer_profile'),
    );
  }
  elseif ($mode == 'new_customer_profile') {
    $profile = commerce_customer_profile_new($type, $order->uid);
    // Add the field widgets for the profile.
    field_attach_form('commerce_customer_profile', $profile, $pane_form, $form_state);

    // Tweak the form to remove the fieldset from the address field if there
    // is only one on this profile.
    $addressfields = array();
    foreach (commerce_info_fields('addressfield', 'commerce_customer_profile') as $field_name => $field) {
      if (!empty($pane_form[$field_name]['#language'])) {
        $langcode = $pane_form[$field_name]['#language'];

        // Only consider this addressfield if it's represented on the form.
        if (!empty($pane_form[$field_name][$langcode])) {
          $addressfields[] = array($field_name, $langcode);
        }
      }
    }

    // Check to ensure only one addressfield was found on the form.
    if (count($addressfields) == 1) {
      list($field_name, $langcode) = array_shift($addressfields);

      foreach (element_children($pane_form[$field_name][$langcode]) as $delta) {
        // Don't mess with the "Add another item" button that could be present.
        if ($pane_form[$field_name][$langcode][$delta]['#type'] != 'submit') {
          $pane_form[$field_name][$langcode][$delta]['#type'] = 'container';
        }
      }
    }
    // Cancel add new customer profile.
    $pane_form['save'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#limit_validation_errors' => array(),
      '#ajax' => array(
        'callback' => 'user_addressbook_ajax_form_refresh',
        'wrapper' => strtr($pane_id, '_', '-') . '-ajax-wrapper',
      ),
      '#submit' => array('user_addressbook_commerce_checkout_pane_add_new_customer_profile_save'),
    );
    // Cancel add new customer profile.
    $pane_form['cancel'] = array(
      '#type' => 'submit',
      '#value' => t('Cancel'),
      '#limit_validation_errors' => array(),
      '#ajax' => array(
        'callback' => 'user_addressbook_ajax_form_refresh',
        'wrapper' => strtr($pane_id, '_', '-') . '-ajax-wrapper',
      ),
      '#submit' => array('user_addressbook_commerce_checkout_pane_add_new_customer_profile_cancel'),
    );
  }

  return $pane_form;
}

/**
 * Submit callback for the "add new" button.
 */
function user_addressbook_commerce_checkout_pane_add_new_customer_profile($form, &$form_state) {
  $pane_id = reset($form_state['triggering_element']['#parents']);
  $form_state['pane_' . $pane_id]['mode'] = 'new_customer_profile';
  $form_state['rebuild'] = TRUE;
}

/**
 * Submit callback for the "cancel" button.
 */
function user_addressbook_commerce_checkout_pane_add_new_customer_profile_cancel($form, &$form_state) {
  $pane_id = reset($form_state['triggering_element']['#parents']);
  $form_state['pane_' . $pane_id]['mode'] = 'select_address';
  $form_state['rebuild'] = TRUE;
}

/**
 * Checkout pane callback: validates a customer profile edit form.
 */
function user_addressbook_commerce_checkout_pane_checkout_form_validate($form, &$form_state, $checkout_pane, $order) {
  return TRUE;
  $pane_id = $checkout_pane['pane_id'];
  $field_name = variable_get('commerce_' . $pane_id . '_field', '');
  // The specified profile reference field doesn't exist on this order type.
  // A consequence of the checkout settings applying for all order types.
  if ($field_name && !field_info_instance('commerce_order', $field_name, $order->type)) {
    return TRUE;
  }

  // Validate "edit" and "new" modes.
  if ($form_state['pane_' . $pane_id]['mode'] != 'view') {
    $profile = $form_state['pane_' . $pane_id]['profile'];

    // Notify field widgets to validate their data.
    field_attach_form_validate('commerce_customer_profile', $profile, $form[$checkout_pane['pane_id']], $form_state);

    // If there were any errors in the field attach validate process for fields on
    // this checkout pane's customer profile, fail the checkout pane validation.
    if ($errors = form_get_errors()) {
      foreach ($errors as $field => $error) {
        if (substr($field, 0, strlen($pane_id) + 2) == $pane_id . '][') {
          return FALSE;
        }
      }
    }
  }

  return TRUE;
}

/**
 * Checkout pane callback: submits a customer profile edit form.
 */
function user_addressbook_commerce_checkout_pane_checkout_form_submit($form, &$form_state, $checkout_pane, $order) {
  $pane_id = $checkout_pane['pane_id'];
  $field_name = variable_get('commerce_' . $pane_id . '_field', '');
  // The specified profile reference field doesn't exist on this order type.
  // A consequence of the checkout settings applying for all order types.
  if ($field_name && !field_info_instance('commerce_order', $field_name, $order->type)) {
    return;
  }

  $profile = $form_state['pane_' . $pane_id]['profile'];
  // Save the customer profile in the "new" and "edit" modes.
  if ($form_state['pane_' . $pane_id]['mode'] != 'view') {
    // Ensure the profile is active.
    $profile->status = TRUE;

    // Set the profile's uid if it's being created at this time.
    if (empty($profile->profile_id)) {
      $profile->uid = $order->uid;
    }

    // Notify field widgets.
    field_attach_submit('commerce_customer_profile', $profile, $form[$checkout_pane['pane_id']], $form_state);

    // Make sure that the incomplete flag is removed, if it was previously set.
    if (!empty($profile->data['incomplete'])) {
      unset($profile->data['incomplete']);
    }

    // Save the profile.
    commerce_customer_profile_save($profile);
  }

  // Store the profile ID for the related field as specified on the settings form.
  $wrapper = entity_metadata_wrapper('commerce_order', $order);

  if ($field_name) {
    $wrapper->{$field_name} = $profile;
  }
  else {
    // Or make the association in the order's data array if no field was found.
    $order->data['profiles'][$checkout_pane['pane_id']] = $profile->profile_id;
  }
}

/**
 * Checkout pane callback: returns the cart contents review data for the
 *   Review checkout pane.
 *
 * Copied verbatim from commerce_customer_profile_pane_review.
 */
function user_addressbook_commerce_checkout_pane_review($form, $form_state, $checkout_pane, $order) {
  // Load the profile based on the related customer profile reference field...
  if ($field_name = variable_get('commerce_' . $checkout_pane['pane_id'] . '_field', '')) {
    $profile = entity_metadata_wrapper('commerce_order', $order)->{$field_name}->value();
  }
  else {
    // Or use the association stored in the order's data array if no field is set.
    $profile = commerce_customer_profile_load($order->data['profiles'][$checkout_pane['pane_id']]);
  }

  if ($profile) {
    $content = entity_view('commerce_customer_profile', array($profile->profile_id => $profile), 'customer');

    return drupal_render($content);
  }
  else {
    return t('No information');
  }
}
