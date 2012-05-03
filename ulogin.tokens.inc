<?php
/**
 * Implements hook_token_list().
 */
function ulogin_token_list($type = 'all') {
  $tokens = array();
  
  if ($type == 'user' || $type == 'all') {
    $tokens['user']['ulogin_network'] = t('Network providing authentication.');
    $tokens['user']['ulogin_identity'] = t('Identity provided by uLogin.');
    $tokens['user']['ulogin_uid'] = t('UID provided by uLogin.');
    $tokens['user']['ulogin_email'] = t('Email address provided by uLogin.');
    $tokens['user']['ulogin_nickname'] = t('Nickname provided by uLogin.');
    $tokens['user']['ulogin_first_name'] = t('First name provided by uLogin.');
    $tokens['user']['ulogin_last_name'] = t('Last name provided by uLogin.');
    $tokens['user']['ulogin_bdate'] = t('Birthday provided by uLogin.');
    $tokens['user']['ulogin_sex'] = t('Gender provided by uLogin.');
    $tokens['user']['ulogin_photo'] = t('Photo provided by uLogin.');
    $tokens['user']['ulogin_photo_big'] = t('Big photo provided by uLogin.');
    $tokens['user']['ulogin_country'] = t('Country provided by uLogin.');
    $tokens['user']['ulogin_city'] = t('City provided by uLogin.');
  }
  
  return $tokens;
}

/**
 * Implements hook_token_values().
 */
function ulogin_token_values($type, $object = NULL, $options = array()) {
  $values = array();
  
  if ($type == 'user' && !empty($object)) {
    $user = $object;
    $data = unserialize($user->data);
    if (!empty($data['ulogin'])) {
      $values['ulogin_network'] = $data['ulogin']['network'];
      $values['ulogin_identity'] = $data['ulogin']['identity'];
      $values['ulogin_uid'] = $data['ulogin']['uid'];
      $values['ulogin_email'] = $data['ulogin']['email'];
      $values['ulogin_nickname'] = $data['ulogin']['nickname'];
      $values['ulogin_first_name'] = $data['ulogin']['first_name'];
      $values['ulogin_last_name'] = $data['ulogin']['last_name'];
      $values['ulogin_bdate'] = $data['ulogin']['bdate'];
      $values['ulogin_sex'] = $data['ulogin']['sex'];
      $values['ulogin_photo'] = $data['ulogin']['photo'];
      $values['ulogin_photo_big'] = $data['ulogin']['photo_big'];
      $values['ulogin_country'] = $data['ulogin']['country'];
      $values['ulogin_city'] = $data['ulogin']['city'];
    }
  }
  
  return $values;
}
