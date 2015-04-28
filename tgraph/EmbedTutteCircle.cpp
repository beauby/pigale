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

#include <TAXI/Tbase.h>
#include <TAXI/graphs.h>
#include <TAXI/Tmessage.h>
#include <TAXI/color.h>

void TutteCircle(GeometricGraph &G, tbrin FirstBrin);

int EmbedTutteCircle(TopologicalGraph &G,bool schnyderLongestFace)
  {if(!G.CheckSimple() || !G.CheckPlanar())return -1;
  int m = G.ne();
  int n = G.nv();
  PSet1  propSave(G.Set());
  G.MakeConnected();
  tbrin FirstBrin;
  int len;
  if(schnyderLongestFace)
      {G.LongestFaceWalk(FirstBrin,len);G.extbrin() = FirstBrin;}
  else
      {FirstBrin = G.extbrin();len = G.FaceWalkLength(FirstBrin);}

  len++;
  svector<tbrin> fb(1,len);
  svector<tvertex> newv(1,len);
  svector<tedge> newe(1,len);
  tbrin b = FirstBrin;
  tvertex v,extv;
  int i=1;
  do  fb[i++] = b; while((b = G.cir[-b]) != FirstBrin);
  len = i-1;
  // create the external cycle
  newv[1] = G.NewVertex();
  for (i = 2; i <= len; i++)
      {newv[i] = G.NewVertex();
      newe[i-1] = G.NewEdge(newv[i-1],newv[i]);
      }
  newe[len] = G.NewEdge(newv[len],newv[1]);
  // link the cycle to the face
  for (i = 1; i <= len; i++)
      G.NewEdge(newe[i].firsttbrin(),fb[i]);
    
  // Add exterior vertex
  extv = G.NewVertex();
  G.incsize(tedge());
  b = G.ne();
  G.vin[b] = extv;
  G.pbrin[extv]=b; G.cir[b] = G.acir[b] = b;
  G.vin[-b] = newv[len];
  G.cir[-b] = newe[len].firsttbrin();
  G.acir[-b] = G.acir[newe[len].firsttbrin()];
  G.acir[G.cir[-b]] = G.cir[G.acir[-b]] = -b;
  for (i = len-1; i >= 1; i--)
      b = G.NewEdge(b,newe[i].firsttbrin()).firsttbrin();
        
  // Check planarity
  if (G.ComputeGenus() != 0)
      {Tprintf("TutteCircle: bad genus");
      return -1;
      }

  bool triang = !G.CheckSubdivTriconnected();
  if (triang)
      {//marke.definit(1);
      G.ZigZagTriangulate();
      }
  GeometricGraph GG(G);
  G.DeleteVertex(extv);
  b = newe[1].firsttbrin();
  if (G.vin[b]!=newv[1]) {b=-b; Tprintf("Orientation error");}
    

  TutteCircle(GG, b); // newe[1].firsttbrin());
//   Prop<short> vcolor (G.Set(tvertex()),PROP_COLOR);
//   Prop<short> ecolor (G.Set(tedge()),PROP_COLOR);
    
//   for(v = G.nv();v > 0;v--)
//       if(markv[v] == 1) G.DeleteVertex(v);
//   if (triang)
//       for(tedge e = G.ne();e > 0;e--)
//           if(marke[e] == 1) G.DeleteEdge(e);
//  for(tedge e = G.ne();e > m;e--)
//      G.DeleteEdge(e);

 for(v = G.nv();v > n;v--)
      G.DeleteVertex(v);
 for(tedge e = G.ne();e > m;e--)
     G.DeleteEdge(e);
 G.Set() =  propSave;
 return 0;
  }

