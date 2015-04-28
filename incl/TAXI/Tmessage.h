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

#ifndef _TMESSAGE_H_INCLUDED_
#define _TMESSAGE_H_INCLUDED_
#include  <TAXI/graphs.h>

class QString;

class GraphDebug {
 public:
  virtual void DrawGraph(Graph &) {}
  virtual void DrawGraph(void) {}
  virtual int wait(const char *) {return 0;}
  virtual void clear(void) {}
  virtual void printf(const char *,...) {}
  virtual void printf(QString &) {}
  static GraphDebug *gd;
  GraphDebug() {if (gd==(GraphDebug *)0) gd=this;}
  virtual ~GraphDebug() {}
};

inline void DrawGraph(Graph &G) {GraphDebug::gd->DrawGraph(G);}
inline void DrawGraph() {GraphDebug::gd->DrawGraph();}
inline int Twait(const char *msg) {return GraphDebug::gd->wait(msg);}
inline void Tclear() {GraphDebug::gd->clear();}
#define Tprintf GraphDebug::gd->printf
inline void SetGDebug(GraphDebug *gdbg) {GraphDebug::gd=gdbg;}

#endif
