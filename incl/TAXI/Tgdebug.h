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

#ifndef __TGDEBUG_H__
#define __TGDEBUG_H__

#include  <TAXI/graphs.h>

class GraphDebug {
 public:
  virtual void DrawGraph(Graph &G) {}
  virtual void DrawGraph(void) {}
  virtual int Twait(const char *) {return 0;}
  virtual void printf(const char *,...) {}
  static GraphDebug *gd;
};

inline void DrawGraph(Graph &G) {GraphDebug::gd->DrawGraph(G);}
inline void DrawGraph() {GraphDebug::gd->DrawGraph();}
inline int Twait(const char *msg) {return GraphDebug::gd->Twait(msg);}
#define Tprintf GraphDebug::gd->printf
inline void SetGDebug(GraphDebug *gdbg) {GraphDebug::gd=gdbg;}

#endif
