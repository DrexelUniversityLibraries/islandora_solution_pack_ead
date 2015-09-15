<?php

/**
 * @file
 * Handles the display/submission for uploading associated objects for an EAD.
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
 * Defines a file upload form for uploading an associated file.
 *
 * @param array $form
 *   The drupal form.
 * @param array $form_state
 *   The drupal form state.
 * @param AbstractObject $ead
 *   The object representing the EAD.
 * @param string $refid
 *   ID of the level in the EAD content inventory being modified.
 *
 * @return array
 *   The drupal form definition.
 */
function islandora_ead_association_upload_form(array $form, array &$form_state, AbstractObject $ead, $refid) {
  $options = array();
  $model_info = array(
    'islandora:sp_pdf' => array('pdf'),
    'islandora:sp_large_image_cmodel' => array('tiff tif'),
    'islandora:sp_basic_image' => array('gif png jpg jpeg'),
  );
  $current_model = !empty($form_state['values']['models']) ? $form_state['values']['models'] : key($model_info);
  $extensions = $model_info[$current_model];

  foreach (array_keys($model_info) as $model) {
    $object = islandora_object_load($model);
    $options[$model] = $object->label;
  }

  $upload_size = min((int) ini_get('post_max_size'), (int) ini_get('upload_max_filesize'));

  $form['eadid'] = array(
    '#type' => 'hidden',
    '#value' => $ead->id,
  );

  $form['refid'] = array(
    '#type' => 'hidden',
    '#value' => $refid,
  );

  $form['models'] = array(
    '#type' => 'select',
    '#title' => t('Select a Content Model to Ingest (EAD)'),
    '#options' => $options,
    '#ajax' => array(
      'callback' => 'islandora_ead_association_upload_form_ajax',
      'wrapper' => 'upload-div',
      'method' => 'replaceWith',
      'effect' => 'fade',
    ),
  );

  $file_upload = array(
    '#type' => 'managed_file',
    '#title' => t('File'),
    '#required' => TRUE,
    '#default_value' => isset($form_state['values']['file_upload']) ? $form_state['values']['file_upload'] : NULL,
    '#description' => filter_xss(t('Select file to upload.<br/>Files must be less than <b>!size MB.</b><br/>Allowed file types: <b>!ext.</b>',
                                   array('!size' => $upload_size, '!ext' => $extensions[0])), array('br', 'b')),
    '#upload_location' => 'temporary://',
    '#upload_validators' => array(
      'file_validate_extensions' => $extensions,
      'file_validate_size' => array($upload_size * 1024 * 1024),
    ),
  );

  $form['upload_wrapper'] = array(
    '#type' => 'container',
    '#attributes' => array('id' => 'upload-div'),
    'file_upload' => $file_upload,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#default_value' => 'Ingest',
  );

  return $form;
}

/**
 * AJAX callback for EAD association form.
 */
function islandora_ead_association_upload_form_ajax(array $form, array &$form_state) {
  return $form['upload_wrapper'];
}

/**
 * Submit handler, adds uploaded file to ingest object.
 */
function islandora_ead_association_upload_form_submit(array $form, array &$form_state) {

  $tuque = islandora_get_tuque_connection();

  $model = $form_state['values']['models'];
  $eadid = $form_state['values']['eadid'];
  $refid = $form_state['values']['refid'];

  // Create new object to add to repo; Assign content model and relationships.
  $object = $tuque->repository->constructObject(islandora_get_namespace($eadid));
  $object->models = $model;
  $object->relationships->add(FEDORA_RELS_EXT_URI, 'isPartOf', $eadid);
  $object->relationships->add(FEDORA_RELS_EXT_URI, 'eadLevelId', $refid);

  // Retrieve uploaded file contents and create OBJ datastream.
  $ds = $object->constructDatastream('OBJ', 'M');
  $file = file_load($form_state['values']['file_upload']);
  $path = drupal_realpath($file->uri);
  $ds->setContentFromFile($path, FALSE);
  $ds->label = $file->filename;
  $ds->mimetype = $file->filemime;
  $object->ingestDatastream($ds);

  // Create DC datastream and populate with metadata from parent EAD.
  $ds = $object->constructDatastream('DC', 'X');
  $ds->label = "Dublin Core Record for this object";
  $ds->setContentFromString(islandora_ead_generate_dc($object->id, $eadid, $refid));
  $object->ingestDatastream($ds);

  // Actually add the object to the repository.
  islandora_add_object($object);

  // Redirect browser to the newly added object.
  $form_state['redirect'] = "islandora/object/{$object->id}";
}

/**
 * Creates DC XML based on a specific subset of EAD metadata.
 *
 * @param string $objid
 *   The PID of the object the metadata applies to.
 * @param string $eadid
 *   The PID of the parent EAD object.
 * @param string $refid
 *   The id of the section in the EAD to retrieve metadata from.
 */
function islandora_ead_generate_dc($objid, $eadid, $refid) {
  $ead = islandora_object_load($eadid);
  $eadxml = new SimpleXMLElement($ead['EAD']->content);
  $level = $eadxml->archdesc->dsc->xpath("//*[@id='$refid']");

  // refid's are unique, should only be one result.
  $level = $level[0];
  $title = (string) $level->did->unittitle;

  $dcxml = <<<XML
<oai_dc:dc xmlns:dc="http://purl.org/dc/elements/1.1/" xmlns:oai_dc="http://www.openarchives.org/OAI/2.0/oai_dc/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.openarchives.org/OAI/2.0/oai_dc/ http://www.openarchives.org/OAI/2.0/oai_dc.xsd">
  <dc:title>$title</dc:title>
  <dc:identifier>$objid</dc:identifier>
</oai_dc:dc>
XML;

  return $dcxml;
}
