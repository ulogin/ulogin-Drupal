<?php
function ulogin_settings_form() {
  $providers = _ulogin_providers_list();
  $fields = _ulogin_fields_list();
  $form = array();
  
  $form['fset_ulogin_display'] = array(
    '#type' => 'fieldset',
    '#title' => t('Widget type'),
  );
  $form['fset_ulogin_display']['ulogin_display'] = array(
    '#type' => 'radios',
    '#title' => t('Widget type'),
    '#options' => array('small' => t('Small icons'), 'panel' => t('Big icons'), 'window' => t('Popup window')),
    '#default_value' => variable_get('ulogin_display', 'panel'),
    '#description' => t('Select uLogin widget type.'),
  );
  $form['fset_ulogin_providers'] = array(
    '#type' => 'fieldset',
    '#title' => t('Authentication providers'),
  );
  $form['fset_ulogin_providers']['ulogin_providers'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Main authentication providers'),
    '#options' => $providers,
    '#default_value' => variable_get('ulogin_providers', array('vkontakte', 'odnoklassniki', 'mailru', 'facebook')),
    '#description' => t('Select main providers shown by default.'),
  );
  $form['fset_ulogin_providers']['ulogin_hidden'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Additional authentication providers'),
    '#options' => $providers,
    '#default_value' => variable_get('ulogin_hidden', array('twitter', 'google', 'yandex', 'livejournal', 'openid')),
    '#description' => t('Select additional providers hidden by default.'),
  );
  $form['fset_ulogin_fields'] = array(
    '#type' => 'fieldset',
    '#title' => t('Fields to request'),
  );
  $form['fset_ulogin_fields']['ulogin_fields'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Fields to request'),
    '#options' => $fields,
    '#default_value' => variable_get('ulogin_fields', array('first_name', 'last_name', 'email', 'nickname', 'bdate', 'sex', 'photo', 'photo_big', 'country', 'city')),
    '#description' => t('Select fields you need to have.'),
  );
  $form['fset_ulogin_other'] = array(
    '#type' => 'fieldset',
    '#title' => t('Other settings'),
  );
  /*$form['fset_ulogin_other']['ulogin_redirect'] = array(
    '#type' => 'checkbox',
    '#title' => t('Do not reload/redirect'),
    '#default_value' => variable_get('ulogin_redirect', 0),
  );*/
  $form['fset_ulogin_other']['ulogin_destination'] = array(
    '#type' => 'textfield',
    '#title' => t('Redirect after login'),
    '#default_value' => variable_get('ulogin_destination', ''),
    '#description' => t('Drupal path to redirect to, like "node/1". Leave empty to return to the same page.'),
  );
  $form['fset_ulogin_other']['ulogin_forms'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Drupal forms'),
    '#options' => array(
      'user_login' => t('User login form'),
      'user_login_block' => t('User login block'),
      'user_register' => t('User registration form'),
      'comment_form' => t('Comment form'),
    ),
    '#default_value' => variable_get('ulogin_forms', array('user_login', 'user_login_block')),
    '#description' => t('Add default uLogin widget to these forms.'),
  );
  $form['fset_ulogin_other']['ulogin_duplicate_emails'] = array(
    '#type' => 'radios',
    '#title' => t('Duplicate emails'),
    '#options' => array(0 => t('Allow duplicate email addresses'), 1 => t("Don't allow duplicate email addresses, block registration and advise to log in using the existing account")),
    '#default_value' => variable_get('ulogin_duplicate_emails', 1),
    '#description' => t('Select how to handle duplicate email addresses. This situation occurs when the same user is trying to authenticate using different authentication providers, but with the same email address.'),
  );
  $form['fset_ulogin_other']['ulogin_pictures'] = array(
    '#type' => 'checkbox',
    '#title' => t('Save uLogin provided picture as user picture'),
    '#description' => t('Save pictures provided by uLogin as user pictures. Check the "Picture support" option at <a href="@link">User settings</a> to make this option available.', array('@link' => url('admin/user/settings'))),
    '#default_value' => variable_get('ulogin_pictures', 1),
    '#disabled' => !variable_get('user_pictures', 0),
  );
  
  return system_settings_form($form);
}

function ulogin_adv_settings_form() {
  $form = array();
  
  $desc = t('Enter list of providers, like "facebook,twitter,youtube". This setting, if not empty, will override checkboxes on the main tab. See <a href="!link" target="_blank">uLogin constructor</a> for a full list of supported keys.',
      array('!link' => 'http://ulogin.ru/constructor.html'));
  $form['ulogin_adv_providers'] = array(
    '#type' => 'textfield',
    '#title' => t('Main authentication providers'),
    '#default_value' => variable_get('ulogin_adv_providers', ''),
    '#description' => $desc,
    '#size' => 120,
  );
  $form['ulogin_adv_hidden'] = array(
    '#type' => 'textfield',
    '#title' => t('Additional authentication providers'),
    '#default_value' => variable_get('ulogin_adv_hidden', ''),
    '#description' => $desc,
    '#size' => 120,
  );
  
  return system_settings_form($form);
}
