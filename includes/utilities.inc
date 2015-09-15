<?php

/**
 * @file
 * Utility functions.
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
 * Return mapping of internal inventory level IDs to PIDs of associated objects.
 *
 * @param string $eadid
 *   The PID of the EAD object to retrieve the associations of.
 */
function islandora_ead_retrieve_associations($eadid) {

  $tuque = islandora_get_tuque_connection();
  $sparql_query = <<<EOQ
SELECT ?partid ?levelid
FROM <#ri>
WHERE {
  ?partid <fedora-rels-ext:isPartOf> <info:fedora/$eadid> ;
          <fedora-rels-ext:eadLevelId> ?levelid .
}
EOQ;

  $sparql_results = $tuque->repository->ri->sparqlQuery($sparql_query);
  $associations = array();
  foreach ($sparql_results as $objects) {
    $associations[$objects['levelid']['value']] = $objects['partid']['value'];
  }

  return $associations;
}
