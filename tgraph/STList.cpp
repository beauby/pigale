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

#include <TAXI/graphs.h>
#include <TAXI/Tmessage.h>

/*
ref[sommet] brin par lequel on a atteint un sommet
top[sommet] premier brin non utilisee ou 0 s'il ny en a plus
link[sommet] determine le st-Ordering
lowest sommet le plus bas a partir duquel on oriente une chaine
*/

#define next(b,iv)  ((tbrin)((cir[(b)] == ref[(iv)]) ? 0 : cir[(b)]()))

int TopologicalGraph::PseudoBipolarPlan(tbrin& st_brin,int &NumberOfSinks)
  {if(debug())DebugPrintf("Executing PseudoBipolar");
    Prop<bool> eoriented(Set(tedge()),PROP_ORIENTED,true); eoriented.SetName("PseudoB:eoriented");
  RemoveLoops();
  int m_origin = ne();
  MakeConnected();

  if(!CheckPlanar())return -1;
  eoriented.clear();
  if(!st_brin())st_brin = acir[extbrin()];

  tvertex source,lowest, iv, liv, piv;
  tbrin b;
  NumberOfSinks = 0;

  svector<tbrin> ref(0,nv()); ref.SetName("ref");
  svector<tbrin> top(-1,nv()); top.SetName("top");;
  svector<int> level(0,nv()); level.SetName("level");
  int clevel = 0;
  svector<tvertex> link(-1,nv()); link.clear(); link.SetName("link");

  b = cir[st_brin];
  lowest = source = vin[b];
  ref[source] = b;
  b = top[source] = top[-1] = b;
  link[source] = -1;
  link[-1] = -1;
  while((iv = lowest) != -1)   /* Path construction */
      {++clevel;
      top[lowest] = next(b,lowest);
      level[lowest] = clevel;
      liv = link[lowest];
      while(b!= 0 && !link[vin[-b]])
          {eoriented[b.GetEdge()]=true;
          piv = iv, b = -b; iv = vin[b];
          ref[iv] = b; b = cir[b];
          top[iv] = next(b,iv);
          link[piv] = iv;
          level[iv] = clevel;
          }
      link[iv] = liv;
      if (level[vin[-b]] == clevel)
          ++NumberOfSinks;
      else
          {eoriented[b.GetEdge()]=true;
          tbrin bb = -b;
          tvertex vv = vin[bb];
          if(top[vv] == bb)
              {do
                  bb = next(bb,vv);
               while (bb() && eoriented[bb.GetEdge()]);
              top[vv] = bb;
              }
          }

      /* Recherche du sommet le plus bas non sature */
      while((b = top[lowest]) == 0) lowest = link[lowest];
      // si lowest==vin[-st_brin], prendre autre chose nsources++ et remettre puits ds link.
      }
  svector<int> num(1,nv()); num.SetName("num");
  int i=1;
  tvertex v;
  for (v=vin[st_brin]; v!=-1; v=link[v])
      num[v]=i++;
  tedge e;
  for (e=1; e <=ne(); e++)
      if (num[vin[e.firsttbrin()]] > num[vin[e.secondtbrin()]])  
          ReverseEdge(e);
  
  for (e = ne();e > m_origin; e--)
      DeleteEdge(e);
  if(debug())DebugPrintf("END PseudoBipolar");	
  return 0;
  }
int TopologicalGraph::BipolarPlan(tbrin FirstBrin)
  {if(!CheckConnected() || !CheckPlanar())return -1;
  if(!FirstBrin)FirstBrin = extbrin();
  if(!CheckBiconnected())return -1;
  ComputeSTlist(FirstBrin);
  Prop<tvertex> stlink(Set(tvertex()),PROP_STLINK);
  svector<int> num(1,nv());
  int i = 0;  //avant i = 1! pas grave
  tvertex v;
  for (v = vin[FirstBrin]; v!=0; v=stlink[v])
      num[v]=i++;
  
  Prop<bool> eoriented(Set(tedge()),PROP_ORIENTED,true);
  for (tedge e = 1;e <= ne(); e++)
      {if(num[vin[e.firsttbrin()]] > num[vin[e.secondtbrin()]])
          ReverseEdge(e);
      eoriented[e] = true;
      }
  if(debug())
      {int ns,nt;
      CheckAcyclic(ns,nt);
      if(ns != 1 || nt != 1)setPigaleError(A_ERRORS_BIPOLAR_PLAN);
      }
  return 0;
  }

int TopologicalGraph::ComputeSTlist(tbrin st_brin)
  {tvertex source,sink, lowest, iv, liv, piv;
  tbrin b;
  svector<tbrin> ref(0,nv());
  svector<tbrin> top(0,nv());
  Prop<tvertex> link(Set(tvertex()),PROP_STLINK); link.clear();

  lowest = source = vin[st_brin]; sink = vin[-st_brin];
  ref[source] = st_brin;
  b = top[source] = top[sink] = cir[st_brin];
  link[source] = link[sink] = sink;

  while((iv = lowest) != sink)   /* Path construction */
      {top[lowest] = next(b,lowest);
      liv = link[lowest];
      while(b!=0 && !link[vin[-b]])
          {piv = iv, b = -b; iv = vin[b];
          ref[iv] = b; b = cir[b];
          top[iv] = next(b,iv);
          link[piv] = iv;
          }
      link[iv] = liv;
      if(top[vin[-b]] == -b) top[vin[-b]] = 0;

      /* Recherche du sommet le plus bas non sature */
      while((b = top[lowest]) == 0) lowest = link[lowest];
      }
  link[sink] = 0;
  return 0;
  }
