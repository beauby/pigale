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
#include  <TAXI/Tmessage.h>
#include  <TAXI/color.h>

#include <TAXI/KantShel.h>
// vertex:Candidates.push(-v)
// face  :Candidates.push(v)
// vertices incident to last face should not be candidates

void KantShelling:: _visited(int v, int i)
// vertex
  {visited[v] += i;
  if(BelongToLastFace[v])return;
  
  if (visited[v]>=1 && sepf[v]==0)
      {if(!Candidates.InList(-v)) Candidates.push(-v);}
  else
      {if (Candidates.InList(-v)) Candidates.del(-v);}
  }

void KantShelling:: _sepf(int v, int i)
// vertex
  {sepf[v] += i;
  if(BelongToLastFace[v])return;
  if (visited[v]>=1 && sepf[v]==0)
      {if (!Candidates.InList(-v)) Candidates.push(-v);}
  else
      {if (Candidates.InList(-v)) Candidates.del(-v);}
  }

void KantShelling::_outv(int f, int i)
  {outv[f] += i;
  if (outv[f]==oute[f]+1 && oute[f]>=2)
      {if (!Candidates.InList(f)) Candidates.push(f);}
  else
      {if (Candidates.InList(f)) Candidates.del(f);}
  }

void KantShelling::_oute(int f, int i)
  {oute[f] += i;
  if (outv[f]==oute[f]+1 && oute[f]>=2)
      {if (!Candidates.InList(f)) Candidates.push(f);}
  else
      {if (Candidates.InList(f)) Candidates.del(f);}
  }

void KantShelling::UpdateSepf1(IntList &NewSepFaces)
// for all f \in NewxSepFaces, for all v incident
// to f, sepf[v]++.
  {tbrin b, b0;
  int f;
  NewSepFaces.ToHead();
  while ((f=~NewSepFaces)!=0)
      {b0=b=Face2Brin[f];
      do 
          {if(IsOuterV[G.vin[b]()]) _sepf(G.vin[b](),1);
          b=G.cir[-b];
          } 
      while (b!=b0);
      ++NewSepFaces;
      }
  }

void KantShelling::UpdateSepf2(IntList &NewOuterVertices)
// for all v \in NewOuterVertices, recalculate sepf[v].
  {tbrin b, b0;
  int f;
  NewOuterVertices.ToHead();
  int vi, count;
  while ((vi=~NewOuterVertices)!=0)
      {count=0;
      b0=b=G.FirstBrin(vi);
      do 
          {f=Brin2Face[b];
          if(!MarkedF[f] && (outv[f]>=3 || (outv[f]==2 && oute[f]==0))) 
              count++;
          b=G.cir[b];
          }
      while (b!=b0);
      _sepf(vi,count-sepf[vi]);
      ++NewOuterVertices;
      }
  } 

void KantShelling::SetAdjFaces()  
/* set Brin2Face, Face2Brin, IsOuterV, IsOuterE.
       b = Face2Brin[num]
       b and acir[b] defines an angle
       Moving along a face: cir[-b] or -acir[b]
    */
  {tbrin b, b0;
  // Mark the vertices incident to the last face
  Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);
  Prop<int> ewidth(G.Set(tedge()),PROP_WIDTH);
  b = b0 = FirstBrin();
  do
      {BelongToLastFace[G.vin[b]()] = 1;
      if(debug()) {ecolor[b.GetEdge()] = Green2;ewidth[b.GetEdge()] = 3;}
      }while((b = -G.cir[b]) != b0);

  IntList Brins;  // Brins: list of all brins 
  for(int i = -G.ne();i <= G.ne();i++)
      if(i)Brins.push(i);
  // the edge {v_1,v_2}. IsOuterE is set to 0.
  Brin2Face[FirstBrin()]=1;			  
  IsOuterV[v_1] = 1;
  Brins.del(FirstBrin());

  // the outer face is indexed 1
  b = G.cir[-FirstBrin];
  do 
      {Brins.del(b());
      Brin2Face[b()] = 1;
      IsOuterE[b.GetEdge()] = 1;
      IsOuterV[G.vin[b]()] = 1;
      }while ((b = G.cir[-b])  != FirstBrin);
  Face2Brin[1]=FirstBrin();

  // indexing other faces.
  int FaceIndex=2;
  while (!Brins.empty()) 
      {b0 = b =Brins.first();
       if(debug())cout << "face:" << FaceIndex << endl;
      do 
          {Brins.del(b());
          Brin2Face[b]=FaceIndex;
           if(debug())cout << G.vin[b]() << " " <<endl;
          } 
      while((b = G.cir[-b]) != b0);

      Face2Brin[FaceIndex]=b();
      FaceIndex++;
      }
  }



