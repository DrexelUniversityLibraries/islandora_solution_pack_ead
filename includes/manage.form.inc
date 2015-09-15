<?php

/**
 * @file
 * Handles displaying/managing level information for EAD content inventories.
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
 * Defines a form for viewing EAD content inventory information.
 *
 * @param array $form
 *   The drupal form.
 * @param array $form_state
 *   The drupal form state.
 * @param AbstractObject $object
 *   The object representing the EAD.
 */
function islandora_ead_manage_form(array $form, array &$form_state, AbstractObject $object) {

  $header = array(
    'level' => t('Level'),
    'info' => t('Info'),
    'association' => t('Association'),
  );

  $xsl = new DOMDocument();
  $xsl->load(drupal_get_path('module', 'islandora_ead') . '/xml/getEadLevels.xsl');

  $xslt = new XSLTProcessor();
  $xslt->importStyleSheet($xsl);

  module_load_include('inc', 'islandora_ead', 'includes/utilities');
  $associations = islandora_ead_retrieve_associations($object->id);

  $ead_dom = new DOMDocument();
  $ead_dom->loadXML($object['EAD']->content);
  $ead_dom = $xslt->transformToDoc($ead_dom);

  $partsxml = new SimpleXMLElement($ead_dom->saveXML());

  $parts_idx = 1;
  $parts = array();
  foreach ($partsxml->entity as $entity) {

    $info = $entity->title;
    if (trim($entity->extent) !== '') {
      $info .= "<br/>$entity->extent";
    }
    if (trim($entity->desc) !== '') {
      $info .= "<br/>$entity->desc";
    }

    $containers = array();
    foreach ($entity->container as $container) {
      if (trim($container->type) !== '') {
        $containers[] = "$container->type: $container->value";
      }
    }

    if ($containers) {
      $info .= "<br/>" . implode(", ", $containers);
    }

    $entity_id = (string) $entity->id;

    if (array_key_exists($entity_id, $associations)) {
      $association = $associations[$entity_id];
      $assoc_link = "<a href='/islandora/object/$association'>$association</a>";
    }
    else {
      $assoc_link = "<a href='/islandora/object/$object->id/manage/ead/$entity->id'>Add Association</a>";
    }

    $parts[$parts_idx++] = array(
      'level' => $entity->level,
      'info' => $info,
      'association' => $assoc_link,
    );

  }

  $form['table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $parts,
    '#empty' => 'Empty Finding Aid',
  );

  return $form;
}
