<?php

/**
 * @file
 * Handles the display of EAD objects.
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
 * Implements template_preprocess_HOOK().
 */
function islandora_ead_preprocess_islandora_ead(&$variables) {
  $object = $variables['object'];

  module_load_include('inc', 'islandora_ead', 'includes/utilities');
  $associated_objs = islandora_ead_retrieve_associations($object->id);

  $xsl = new DOMDocument();
  $xsl->load(drupal_get_path('module', 'islandora_ead') . '/xml/islandora_eadToHTML.xsl');

  $xslt = new XSLTProcessor();
  $xslt->importStyleSheet($xsl);

  $object_actual_dom = new DOMDocument();
  $object_actual_dom->preserveWhiteSpace = FALSE;
  $object_actual_dom->loadXML($object['EAD']->content);
  $object_actual_dom = $xslt->transformToDoc($object_actual_dom);

  $transformed_xml = simplexml_import_dom($object_actual_dom);

  $xpath = new DOMXpath($object_actual_dom);
  foreach ($associated_objs as $levelid => $pid) {
    $elements = $xpath->query("//*[@id='$levelid']");

    foreach ($elements as $element) {
      if ($element->childNodes->length > 0) {
        $href = $object_actual_dom->createElement('a');
        $href->setAttribute('href', "/islandora/object/$pid");
        $href->setAttribute('class', "hasPopover");

        $child_node = $element->childNodes->item(0);
        $sibling_node = $child_node->nextSibling;

        $element->removeChild($child_node);
        $href->appendChild($child_node);
        if ($sibling_node === NULL) {
          $element->appendChild($href);
        }
        else {
          $element->insertBefore($href, $sibling_node);
        }
      }
    }
  }

  $content = array();
  $content['narrative'] = $transformed_xml->asXML();

  $variables['content'] = $content;
}
