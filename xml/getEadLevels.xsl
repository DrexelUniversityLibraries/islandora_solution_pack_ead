<?xml version="1.0" encoding="UTF-8"?>

<!--
 This file is part of the Islandora EAD Solution Pack.
 Copyright (C) 2015  Drexel University

 The Islandora EAD Solution Pack is free software; you can redistribute
 it and/or modify it under the terms of the GNU General Public License as
 published by the Free Software Foundation; either version 2 of the License,
 or (at your option) any later version.

 The Islandora EAD Solution Pack is distributed in the hope that it will be
 useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 Public License for more details.

 You should have received a copy of the GNU General Public License along
 with The Islandora EAD Solution Pack; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
-->

<xsl:stylesheet version="1.0"
                xmlns:ead="urn:isbn:1-931666-22-9"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:ns2="http://www.w3.org/1999/xlink"
>

  <xsl:output method="xml" indent="yes" />

  <xsl:template match="/">
    <entities>
      <xsl:apply-templates/>
    </entities>
  </xsl:template>

  <xsl:template match="ead:dsc//*[substring(name(), 1, 2) = 'c0' or substring(name(), 1, 2) = 'c1' or name() = 'c']">
    <entity>
      <id>
        <xsl:value-of select="@id"/>
      </id>
      <level>
        <xsl:value-of select="@level"/>
      </level>
      <title>
        <xsl:value-of select="./ead:did/ead:unittitle"/>
      </title>
      <desc>
        <xsl:value-of select="./ead:dao/ead:daodesc/ead:p"/>
      </desc>
      <extent>
        <xsl:value-of select="./ead:did/ead:physdesc/ead:extent"/>
      </extent>
      <xsl:apply-templates select="./ead:did/ead:container"/>
    </entity>
    <xsl:apply-templates select="./*[substring(name(), 1, 2) = 'c0' or substring(name(), 1, 2) = 'c1' or name() = 'c']"/>
  </xsl:template>

  <xsl:template match="ead:container">
    <container>
      <type>
        <xsl:value-of select="@type"/>
      </type>
      <value>
        <xsl:value-of select="."/>
      </value>
    </container>
  </xsl:template>

  <xsl:template match="text() | @* | ead:did | ead:dao ">
    <xsl:apply-templates/>
   </xsl:template>

</xsl:stylesheet>
