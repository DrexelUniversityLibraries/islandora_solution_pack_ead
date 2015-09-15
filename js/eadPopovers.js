/**
 * @file
 * Triggers popup containing thumbnails of child documents on hover.
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

(function() {
  "use strict";

  function islandoraEadAddPopovers() {
    jQuery('.hasPopover').each(function() {
      var url = jQuery(this).attr('href') + "/datastream/TN/view";
      var img = '<img src="' + url + '"/>';
      jQuery(this).popover({ trigger: 'hover', content: img, html: true });
    });
  }

  jQuery(document).ready(function() {
    islandoraEadAddPopovers();
  });
})();
