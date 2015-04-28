/****************************************************************************
**
** Copyright (C) 2001 Hubert de Fraysseix, Patrice Ossona de Mendez.
** All rights reserved.
** This file is part of the PIGALE Toolkit.
**
** This file may be distributed under the terms of the GNU Public License
** appearing in the file LICENSE.HTML included in the packaging of this file.
**
*****************************************************************************/



const char * fileopen_txt = "Click this button to open a <em>new file</em>. <br>\
	A <em>.txt</em> file contains only <b>One</b> graph <br>\
	A <em>.tgf</em> file contains a <b>collection</b> of graphs";
const char * left_txt = "Opens <b>previous</b> graph from  the current <em>tgf</em> file";
const char * redo_txt = "Reload the <b>current</b> graph from the current  opened <em>tgf</em> file";
const char * right_txt = "Opens <b>next</b> graph of the current opened <em>tgf</em> file";

const char * fpp_txt = "The original Fraysseix Pach Pollack algorithm to produce a Fary drawing";
const char * ccd_txt = "Convex Compact drawing";
const char * cd_txt = "Convex drawing";
const char * jacquard_txt ="Jacquard spring embedder which preserves planarity\nTutte circle may give better results\nESC to stop";
const char *schnyder_txt ="<qt>\
The original Schnyder algorithm to produce a Fary drawing\
<ul>\
<li> <b>Schnyder</b> triangulates the graph by adding edges.\
<li> <b>Schnyder V</b> triangulates the graph by adding vertices.\
<li>If the option <b>longest face</b> is selected, the exterior face will be the longest face.\
<li>The shape of the exterior face is defined in the options menu <b>Schnyder: Rect</b>.\
<li>The edges will be colored <b>R G B</b> if the options menu <b>Schnyder: Color Edges</b> is set.\
</ul>";
const char * tutte_circle_txt="The resulting embedding is similar to traditional spring embedders";
const char * spring_txt ="General spring embedder\nESC to stop";
const char * springPM_txt ="Spring embedder which preserves the map and the crossings\nESC to stop";
const char * embed3dSchnyder_txt ="<qt>\
The Schnyder algorithm is used to compute an embedding in 3-dimensional space.<br>\
The axis <b>X</b>,<b>Y</b>,<b>Z</b> are colored\
<b>Red</b>,<b>Green</b>,<b>Blue</b>\
<H2><b>Mouse Action</b></H2>\
<ul>\
<li><b>Left button:</b>\
 <ul>\
  <li>Moving along X axis -> Rotate along Y\
  <li>Moving along Y axis -> Rotate along X\
  <li>Double Click -> Start/Stop Rotate (depends on vertical slider)\
 </ul>\
<li><b>Right button:</b> \
 Moving along X axis -> Rotate along Z\
 <li><b>Middle:</b>\
    Translate\
<li><b>Wheel:</b> \
   Zoom\
</ul>\
<H2>Buttons</H2>\
The spin boxes let you select the coordinates to display.\
The <b>facet</b> button triangulate the graph and draw the faces.\
</qt>"; 
const char * embed3d_txt ="<qt>\
The axis <b>X</b>,<b>Y</b>,<b>Z</b> are colored\
<b>Red</b>,<b>Green</b>,<b>Blue</b>\
<H2><b>Mouse Action</b></H2>\
<ul>\
<li><b>Left button:</b>\
 <ul>\
  <li>Moving along X axis -> Rotate along Y\
  <li>Moving along Y axis -> Rotate along X\
  <li>Double Click -> Start/Stop Rotate (depends on vertical slider)\
 </ul>\
<li><b>Right button:</b> \
 Moving along X axis -> Rotate along Z\
 <li><b>Middle:</b>\
    Translate\
<li><b>Wheel:</b> \
   Zoom\
</ul>\
<H2>Buttons</H2>\
The spin boxes let you select the coordinates to display.\
The <b>facet</b> button triangulate the graph, if it is planar, and draw the faces.\
</qt>"; 
const char * dual_g_txt ="The dual graph without the vertex corresponding to the outer face";
const char * angle_g_txt ="The angle graph without the vertex corresponding to the outer face";

