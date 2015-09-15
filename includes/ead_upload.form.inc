<?php

/**
 * @file
 * Handles the display/submission of the EAD upload form for this module.
 *
 * This file is part of the Islandora EAD Solution Pack.
 * Copyright (C) 2015  Drexel University.
 *
 * The Islandora EAD Solution Pack is free software; you can redistribute
 * it and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * The Islandora EAD Solution Pack is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with The Islandora EAD Solution Pack; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/**
 * Defines a file upload form for uploading the EAD.
 */
function islandora_ead_upload_form(array $form, array &$form_state) {
  $upload_size = min((int) ini_get('post_max_size'), (int) ini_get('upload_max_filesize'));
  $extensions = array('xml');
  return array(
    'file' => array(
      '#title' => t('EAD'),
      '#type' => 'managed_file',
      '#required' => TRUE,
      '#description' => t('Select file to upload.<br/>Files must be less than <b>@size MB.</b><br/>Allowed file types: <b>@ext.</b>', array('@size' => $upload_size, '@ext' => $extensions[0])),
      '#default_value' => isset($form_state['values']['files']) ? $form_state['values']['files'] : NULL,
      '#upload_location' => 'temporary://',
      '#upload_validators' => array(
        'file_validate_extensions' => $extensions,
        'file_validate_size' => array($upload_size * 1024 * 1024),
      ),
    ),
  );
}

/**
 * Submit handler, adds uploaded file to ingest object.
 */
function islandora_ead_upload_form_submit(array $form, array &$form_state) {
  $object = $form_state['islandora']['objects'][0];
  if (empty($object['EAD'])) {
    $ds = $object->constructDatastream('EAD', 'M');
    $object->ingestDatastream($ds);
  }
  else {
    $ds = $object['EAD'];
  }

  $file = file_load($form_state['values']['file']);
  $path = drupal_realpath($file->uri);
  $ds->setContentFromFile($path, FALSE);
  $ds->label = $file->filename;
  $ds->mimetype = $file->filemime;

  $eadxml = simplexml_load_file($path);
  if (isset($eadxml->archdesc->did->unittitle)) {
    $unittitle = $eadxml->archdesc->did->unittitle;
    $unitid = isset($eadxml->archdesc->did->unitid) ? $eadxml->archdesc->did->unitid . " " : "";
    $unitdate = isset($eadxml->archdesc->did->unitdate) ? ", " . $eadxml->archdesc->did->unitdate : "";

    $object->label = "$unitid$unittitle$unitdate";
  }
}
