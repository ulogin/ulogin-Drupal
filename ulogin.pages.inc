<?php
function ulogin_callback() {
  if (!empty($_POST['token'])) {
    $data = drupal_http_request('http://ulogin.ru/token.php?token=' . $_POST['token'] . '&host=' . $_SERVER['HTTP_HOST']);
    $data = json_decode($data->data, TRUE);
    if (!empty($data['error'])) {
      drupal_set_message($data['error'], 'error');
      return MENU_ACCESS_DENIED;
    }
  }
  else {
    drupal_set_message('no token given', 'error');
    return MENU_ACCESS_DENIED;
  }
  
  global $user;
  //user is already logged in, tries to add new identity
  if (!empty($user->uid)) {
    //identity is already registered
    if ($identity = _ulogin_identity_load($data)) {
      //registered to this user
      if ($user->uid == $identity['uid']) {
        drupal_set_message(t('You have already registered this identity.'));
        $destination = drupal_get_destination();
        drupal_goto($destination['destination']);
      }
      //registered to another user
      else {
        drupal_set_message(t('This identity is registered to another user.'), 'error');
        $destination = drupal_get_destination();
        drupal_goto($destination['destination']);
      }
    }
    //identity is not registered - register it to the logged in user
    else {
      _ulogin_identity_save($data);
      drupal_set_message(t('New identity added.'));
      $destination = drupal_get_destination();
      drupal_goto($destination['destination']);
    }
  }
  
  if ($identity = _ulogin_identity_load($data)) {
    user_external_login(user_load($identity['uid']));
  }
  //handle duplicate email addresses
  elseif (variable_get('ulogin_duplicate_emails', 1) && !empty($data['email']) && $account = ulogin_user_load_by_mail($data['email'])) {
    if (!empty($account->ulogin) && $account->ulogin['network'] == $data['network'] && $account->ulogin['uid'] == $data['uid']) {
      global $ulogin_data;
      $ulogin_data = $data;
      user_external_login_register(_ulogin_make_username($data), 'ulogin');
      _ulogin_identity_save($data);
    }
    else {
      drupal_set_message(t('You are trying to login with email address of another user.'), 'error');
      if (!empty($account->ulogin)) {
        $providers = _ulogin_providers_list();
        drupal_set_message(t('If you are completely sure it is your email address, try to login through %network.', array('%network' => $providers[$account->ulogin['network']])), 'status');
      }
      else {
        drupal_set_message(t('If you are completely sure it is your email address, try to login using your username and password on this site. If you don\'t remember your password - <a href="@password">request new password</a>.', array('@password' => url('user/password'))));
      }
    }
  }
  else {
    global $ulogin_data;
    $ulogin_data = $data;
    user_external_login_register(_ulogin_make_username($data), 'ulogin');
    _ulogin_identity_save($data);
    //save user picture
    if (variable_get('user_pictures', 0) && variable_get('ulogin_pictures', 1)) {
      $photo_url = '';
      if (!empty($data['photo_big']) && $data['photo_big'] != 'http://ulogin.ru/img/photo_big.png') {
        $photo_url = $data['photo_big'];
      }
      elseif (!empty($data['photo']) && $data['photo'] != 'http://ulogin.ru/img/photo.png') {
        $photo_url = $data['photo'];
      }
      if ($photo_url) {
        $photo = drupal_http_request($photo_url);
			  $file = file_save_data($photo->data, file_directory_temp() . '/' . md5($photo_url), FILE_EXISTS_REPLACE);
			  
        $info = image_get_info($file);
        $destination = variable_get('user_picture_path', 'pictures') . '/picture-' . $user->uid . '.' . $info['extension'];
        if (file_copy($file, $destination, FILE_EXISTS_REPLACE)) {
          $edit['picture'] = $file;
          user_save($user, $edit);
        }
      }
    }
  }
  
  $destination = drupal_get_destination();
  drupal_goto($destination['destination']);
}

function ulogin_user_identity($form_state, $account) {
  //drupal_set_title(format_username($account));
  //drupal_set_title($account->name);
  
  $identities = _ulogin_identity_load_by_uid($account->uid);
  $providers = _ulogin_providers_list();
  
  $header = array(t('Authentication provider'), t('Identity'), t('Delete'));
  $rows = array();
  $data_array = array();
  foreach ($identities as $identity) {
    $data = unserialize($identity['data']);
    $data_array[] = $data;
    $rows[] = array(
      $providers[$data['network']],
      l($data['identity'], $data['identity'], array('attributes' => array('target' => '_blank'), 'external' => TRUE)),
      l(t('Delete'), 'user/' . $account->uid . '/ulogin/delete/' . $identity['id']),
    );
  }
  
  $form = array();
  
  $form['identity'] = array(
    '#value' => theme('table', $header, $rows),
  );
  
  //add more identities
  if (user_access('use ulogin')) {
    $form['ulogin_widget'] = array(
      '#type' => 'ulogin_widget',
      '#title' => t('Add more identities'),
      '#ulogin_destination' => '',
    );
  }
  
  //tokens browser for admins
  if (user_access('administer site configuration') || user_access('administer users')) {
    $header = array(t('Token'), t('Value'));
    //user tokens
    if (!empty($account->ulogin)) {
      $form['fset_user_tokens'] = array(
        '#type' => 'fieldset',
        '#title' => t('User tokens'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
      
      $rows = array();
      foreach ($account->ulogin as $key => $value) {
        if ($key != 'manual') {
          $rows[] = array('[user:ulogin:' . $key . ']', $value);
        }
      }
      $form['fset_user_tokens']['tokens'] = array(
        '#value' => theme('table', $header, $rows),
      );
    }
    
    //data from auth providers
    foreach ($data_array as $data) {
      $form['fset_' . $data['network'] . '_' . $data['uid']] = array(
        '#type' => 'fieldset',
        '#title' => $providers[$data['network']],
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
      
      $rows = array();
      foreach ($data as $key => $value) {
        if ($key != 'manual') {
          $rows[] = array($key, $value);
        }
      }
      $form['fset_' . $data['network'] . '_' . $data['uid']]['tokens'] = array(
        '#value' => theme('table', $header, $rows),
      );
    }
  }
  
  return $form;
}

function ulogin_user_identity_delete($form_state, $account, $id) {
  $del_identity = _ulogin_identity_load_by_id($id);
  if (!$del_identity || $account->uid != $del_identity['uid']) {
    drupal_set_message(t('You are trying to delete non-existing identity.'), 'error');
    drupal_access_denied();
  }
  $del_identity_data = unserialize($del_identity['data']);
  //$username = format_username($account);
  $username = $account->name;
  $quetion = t('Are you sure you want to detach the uLogin identity !identity from %user?',
    array(
      '!identity' => l($del_identity_data['identity'], $del_identity_data['identity'], array('attributes' => array('target' => '_blank'), 'external' => TRUE)),
      '%user' => $username));
  
  $form = array();
  $form['#account'] = $account;
  $form['#identity_id'] = $id;
  $form['question'] = array(
    '#value' => $quetion,
    '#prefix' => '<div>',
    '#suffix' => '</div>',
  );
  
  if (!empty($account->ulogin) && $account->ulogin['network'] == $del_identity_data['network'] &&
    $account->ulogin['uid'] == $del_identity_data['uid']) {
    $identities = _ulogin_identity_load_by_uid($account->uid);
    $providers = _ulogin_providers_list();
    $options = array();
    foreach ($identities as $key => $identity) {
      $data = unserialize($identity['data']);
      if ($key != $id) {
        $options[$key] = $providers[$identity['network']] . ' - ' . l($data['identity'], $data['identity'], array('attributes' => array('target' => '_blank'), 'external' => TRUE));
      }
    }
    
    if (!empty($options)) {
      $form['explanation'] = array(
        '#value' => t('This identity was used to create your account. Please choose another identity to replace it.'),
        '#prefix' => '<div>',
        '#suffix' => '</div>',
      );
      $form['identity_choice'] = array(
        '#type' => 'radios',
        //'#title' => t('Identities'),
        '#options' => $options,
        '#default_value' => count($options) == 1 ? $key : NULL,
        //'#required' => TRUE, //required has bugs with radios http://drupal.org/node/811542
      );
    }
    else {
      $form['explanation'] = array(
        '#value' => t('This identity was used to create your account. To delete it you should first add another identity to your account.'),
        '#prefix' => '<div>',
        '#suffix' => '</div>',
      );
      return $form;
    }
  }
  
  $form = confirm_form($form, '', 'user/' . $account->uid . '/ulogin');
  drupal_set_title($username);
  
  return $form;
}

function ulogin_user_identity_delete_validate($form, &$form_state) {
  if (!empty($form['identity_choice']) && empty($form_state['values']['identity_choice'])) {
    form_set_error('identity_choice', t('Please choose identity for replacement.'));
  }
}

function ulogin_user_identity_delete_submit($form, &$form_state) {
  if (!empty($form_state['values']['identity_choice'])) {
    $identity = _ulogin_identity_load_by_id($form_state['values']['identity_choice']);
    $data = unserialize($identity['data']);
    $name = _ulogin_make_username($data);
    //change name
    $edit['name'] = $name;
    //change ulogin data used for tokens
    $edit['data']['ulogin'] = $data;
    user_save($form_state['build_info']['args'][0], $edit);
    //change authname in authmap DB table
    user_set_authmaps($form['#account'], array('authname_ulogin' => $name));
  }
  
  $deleted = _ulogin_identity_delete_by_id($form['#identity_id']);
  if ($deleted) {
    drupal_set_message(t('Identity deleted.'));
  }
  
  $form_state['redirect'] = 'user/' . $form['#account']->uid . '/ulogin';
}

function ulogin_user_load_by_mail($mail) {
  $uid = db_result(db_query("SELECT uid FROM {users} WHERE mail = '%s'", $mail));
  return user_load($uid);
}
