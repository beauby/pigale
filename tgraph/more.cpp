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
#include <TAXI/Tsvector.h>
#include <TAXI/Tprop.h>
#include <TAXI/graph.h>
#include <TAXI/Tpoint.h>
#include <TAXI/Tstring.h>

// Entries for inlined functions...
void Destroy(_svector &x) 
  {delete (char *)x.buff;}
void Resize(_svector &x, int a, int b) 
  {x.resize(a,b);}
_svector & Get(PSet &X, int num,int type) 
  {switch(type) 
      {case 0: return Prop<int>::get(X,num);
      case 1: return Prop<tvertex>::get(X,num);
      case 2: return Prop<tedge>::get(X,num);
      case 3: return Prop<tbrin>::get(X,num);
      case 4: return Prop<Tpoint>::get(X,num);
      case 5: return Prop<double>::get(X,num);
      case 6: return Prop<bool>::get(X,num);
      }
  return *(_svector *)0;
  }
void *Get(PSet1 &X, int num,int type) 
  {switch(type) 
      {case 0: return &Prop1<int>::get(X,num);
      case 1: return &Prop1<tvertex>::get(X,num);
      case 2: return &Prop1<tedge>::get(X,num);
      case 3: return &Prop1<tbrin>::get(X,num);
      case 4: return &Prop1<Tpoint>::get(X,num);
      case 5: return &Prop1<double>::get(X,num);
      case 6: return &Prop1<bool>::get(X,num);
      case 7: return &Prop1<tstring>::get(X,num);
      }
  return (void *)0;
  }

void Clear(GraphContainer &x) 
  {x.clear();}
void Setsize(GraphContainer &gc, int n, int m) 
  {gc.setsize(n,m);}
int SaveGraphTgf(GraphAccess& G,tstring filename,int tag);
void SaveIt(GraphContainer &x) 
  {GraphAccess ga(x); SaveGraphTgf(ga,tstring("cast.tgf"),1);}





