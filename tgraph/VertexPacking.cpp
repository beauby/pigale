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



#include  <TAXI/SchPack.h>

//SchnyderMode == 3 // manual packing
//SchnyderMmode==1  // rightmost packing
//SchnyderMmode==2  // leftmost packing
void SchnyderPacking::MarkEdge(tbrin b)
  {tvertex v = G.vin[b];
  MarkedE[b.GetEdge()] = 1;
  if (!MarkedV[v])
      {MarkedV[v] = 1; n_cones[v] = 1;}
  else
      {int lm, rm;
      // update #cones of v
      lm = MarkedE[G.acir[b].GetEdge()];
      rm = MarkedE[G.cir[b].GetEdge()];
      if (lm == rm)
          {if (lm == 1) n_cones[v]--;
          else n_cones[v]++;
          }
      // update packable
      if (n_cones[v] == 1 && !packable.InList(v()))
          {if (v != v_n) packable.push(v());}
      else if (n_cones[v] != 1 && packable.InList(v()))
          packable.del(v());
      }
  }

void SchnyderPacking::pack(tvertex v, tbrin & left_brin, tbrin & right_brin)
  {tbrin b = G.pbrin[v];
  if(mode==1)  // rightmost packing
      {while ( MarkedE[b.GetEdge()]) b = G.acir[b];
      while (!MarkedE[b.GetEdge()]) b = G.cir[b];
      // now, b is the left_most marked brin of v.
      left_brin = b;
      b = G.acir[b];
      while (!MarkedE[b.GetEdge()])
          {MarkEdge(-b);b = G.acir[b];}
      right_brin = b;
      }
  else   // leftmost packing
      {while ( MarkedE[b.GetEdge()]) b = G.cir[b];
      while (!MarkedE[b.GetEdge()]) b = G.acir[b];
      // now, b is the right_most marked brin of v.
      right_brin = b;
      b = G.cir[b];
      while (!MarkedE[b.GetEdge()])
          {MarkEdge(-b);b = G.cir[b];}
      left_brin = b;
      }
  }

tvertex SchnyderPacking::FindVertex(tbrin &left, tbrin &right)
  {if(packable.empty())
      {if(count==G.nv()-1)
          {count++; left=FirstBrin; right=G.acir[FirstBrin]; return v_n;}
      //else if(count<G.nv());
      return 0;
      }
  count++;
  tvertex v=packable.pop();
  pack(v, left, right);
  return v;
  }

tvertex SchnyderPacking::FindVertex()
  {tbrin left, right;
  return FindVertex(left, right);
  }

