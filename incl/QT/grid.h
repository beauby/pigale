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

#ifndef GRID_H
#define GRID_H

#include <TAXI/Tpoint.h>
struct Tgrid {
  Tpoint delta;
  Tpoint orig;

  Tgrid(double dx, double dy, double x0, double y0): delta(dx,dy), orig(x0,y0) {}
  Tgrid() {}

  Tpoint toGrid(Tpoint &p)
      { double pos;
      int npos;
      Tpoint res;
      pos = p.x() - orig.x();  
      npos = pos>0 ? (int) (.5 + pos/delta.x()) :(int) (-.5 + pos/delta.x()) ;
      res.x() =  orig.x() + npos*delta.x();
      pos = p.y() - orig.y();  
      npos = pos>0 ? (int) (.5 + pos/delta.y()) :(int) (-.5 + pos/delta.y()) ;
      res.y() =  orig.y() + npos*delta.y();
      return res;
      }
  Tgrid & operator=(const Tgrid &g) {delta=g.delta; orig=g.orig; return *this;}
  void compute(const Tpoint &bbmin, const Tpoint &bbmax, int nx, int ny)
      {delta=(bbmax-bbmin);
      delta.x()/= nx;
      delta.y()/= ny;
      }
};


#endif
