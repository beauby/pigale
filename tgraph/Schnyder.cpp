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
#include <TAXI/SchPack.h>
#include <TAXI/Tmessage.h>
#include <TAXI/color.h>
#include <TAXI/embedrn.h>


static void CountParents(GeometricGraph &G, short TreeColor, tbrin RootBrin,
                         svector<tvertex> &Father,svector<int> &Descendants,
                         svector<short> &ecolor)
  {svector<int> marked(1,G.ne(),(int)0);
  marked.SetName("marked");
  tbrin b = RootBrin;
  int root_distance = 0;

  while (1)
      {do
          {b = G.cir[b];
          if(b == RootBrin)return;
          }while (ecolor[b.GetEdge()] != TreeColor && !marked[b.GetEdge()]);

      if (!marked[b.GetEdge()])     // Montee dans l'arbre
          {root_distance++;
          Father[G.vin[-b]] = G.vin[b];
          marked[b.GetEdge()] = 1;
          b = -b;
          }
      else               // Descente
          {root_distance--;
          Descendants[G.vin[-b]] += Descendants[G.vin[b]];
          b = -b;
          }
      }
  }

static void CalcCoord(svector<int> &x, svector<tvertex> &Father1,
                      svector<tvertex> &Father2,svector<int> &Descendants, int n)
  {tvertex tmp;
  for (int v = 1; v <= n; v++)
      {tmp = Father1[v];
      while (tmp!=0) { x[v] += Descendants[tmp]; tmp = Father1[tmp];}
      tmp = v;
      while (tmp!=0) { x[v] += (Descendants[tmp] - 1); tmp = Father2[tmp]; }
      }
  }
void CalcXYZ(TopologicalGraph &G, tbrin brin,svector<short> &ecolor,
svector<int> &x, svector<int>&y, svector<int> &z)
  {GeometricGraph G0(G);

  svector<tvertex> FatherB(1,G.nv(),(tvertex)0);
  svector<int> DescendantsB(1,G.nv(),(int)1);
  svector<tvertex> FatherG(1,G.nv(),(tvertex)0);
  svector<int> DescendantsG(1,G.nv(),(int)1);
  svector<tvertex> FatherR(1,G.nv(),(tvertex)0);
  svector<int> DescendantsR(1,G.nv(),(int)1);
  x.clear(); y.clear(); z.clear();
  FatherB.SetName("FatherB");
  FatherG.SetName("FatherG");
  FatherR.SetName("FatherR");
  DescendantsB.SetName("DescendantsB");
  DescendantsG.SetName("DescendantsG");
  DescendantsR.SetName("DescendantsR");

  CountParents(G0, Blue,           -brin, FatherB, DescendantsB,ecolor);
  CountParents(G0, Green, -G0.acir[brin], FatherG, DescendantsG,ecolor);
  CountParents(G0, Red,             brin, FatherR, DescendantsR,ecolor);
  CalcCoord(x, FatherR, FatherB, DescendantsG, G.nv());
  CalcCoord(y, FatherB, FatherG, DescendantsR, G.nv());
  CalcCoord(z, FatherG, FatherR, DescendantsB, G.nv());

  tvertex v_1 = G0.vin[-brin];
  tvertex v_2 = G0.vin[-G0.cir[-brin]];
  tvertex v_n = G0.vin[brin];

  x[v_1] = 0;
  y[v_1] = 0;
  z[v_1] = G.nv()-1;

  x[v_2] = G.nv()-1;
  y[v_2] = 0;
  z[v_2] = 0;

  x[v_n] = 0;
  y[v_n] = G.nv()-1;
  z[v_n] = 0;
  }

static void CalcXY(TopologicalGraph &G, tbrin brin,svector<short> &ecolor,bool shape)
  {GeometricGraph G0(G);

  svector<int> x(1,G.nv(),(int)0), y(1,G.nv(),(int)0), z(1,G.nv(),(int)0);
  x.SetName("x");
  y.SetName("y");
  z.SetName("z");
  CalcXYZ(G,brin,ecolor,x,y,z);

  double a11, a12, a21, a22;
  if(!shape)          // equilateral
      {a11 = 1.0;
      a12 = 0.5;
      a21 = 0.0;
      a22 = pow(3.0, 0.5) / 2.0;
      }
  else     // rectangle
      {a11 = 1.0;
      a12 = 0.0;
      a21 = 0.0;
      a22 = 1.0;
      }
//   else                           // right rectangle
//       {a11 = 1.0;
//       a12 = 1.0;
//       a21 = 0.0;
//       a22 = 1.0;
//       }

  Prop<Tpoint> vcoord(G.Set(tvertex()),PROP_COORD);
  for (tvertex v = 1; v <= G.nv(); v++)
      vcoord[v] = Tpoint(a11*x[v] + a12*y[v], a21*x[v] + a22*y[v]);
  /*
    // reorient the arcs.
    tedge e;
    ForAllEdges(e,G0)
    {if (FatherR[G0.vin[e]]==G0.vin(-e) ||
    FatherG[G0.vin[e]]==G0.vin(-e) ||
    FatherB[G0.vin[e]]==G0.vin(-e))
    G0.ReverseEdge(e);
    }
  */
  }


void SchnyderDecomp(TopologicalGraph &G, tbrin brin,svector<short> &ecolor)
// precondition:
//   G is simple, maximal planar.
//   "cir" is a planar embedding.
  {SchnyderPacking SP(G,brin);
  // color all the edges RED.
  tedge e;
  ForAllEdges(e, G) ecolor[e] = Red;

  // pack vertices.
  tbrin left_brin, right_brin;
  tvertex v;
  while(1)
      {v = SP.FindVertex(left_brin, right_brin);
      if(v==0) break;
      ecolor[left_brin.GetEdge()] = Blue;
      ecolor[right_brin.GetEdge()] = Green;
      }
  // color three exterior edges Grey1,Grey2,Black.
  GeometricGraph G0(G);
  ecolor[brin.GetEdge()] = Grey1;
  ecolor[G0.acir[brin].GetEdge()] = Grey2;
  ecolor[G0.cir[-brin].GetEdge()] = Black;
  }

static int SchnyderOrientMaxPlanar(TopologicalGraph &G, tbrin brin)
// inf-3-orient a simple maximal planar graph.
// precondition:
//   G is simple, maximal planar.
//   "cir" is a planar embedding.
  {SchnyderPacking SP(G,brin);
  GeometricGraph G0(G);
  IntList RevList;
  
  // pack vertices.
  tbrin left_brin, right_brin, b;
  tvertex v;
  // skip two vertices.
  SP.FindVertex();
  SP.FindVertex();
  while(1)
      {v = SP.FindVertex(left_brin, right_brin);
      if(v==0) break;
      b=left_brin;
      if (b > 0) RevList.push(b.GetEdge()());
      b=G0.cir[b];
      while (b!=right_brin)
          {if (b < 0) RevList.push(b.GetEdge()()); b=G0.cir[b]; }
      if (b > 0) RevList.push(b.GetEdge()());
      }
  while (!RevList.empty()) G0.ReverseEdge(RevList.pop());
  return 0;
  }

int TopologicalGraph::SchnyderOrient(tbrin FirstBrin)
// inf-3-orient a graph.
  {if(nv() < 3)return -1;
  if(!CheckSimple())return -1;
  int OldNumEdge = ne();
  MakeConnected();

  if(!CheckPlanar())return -1;
  if(FirstBrin == 0)
      {FirstBrin =  extbrin();FirstBrin = -acir[FirstBrin];}

  if(ZigZagTriangulate())return -2;
  SchnyderOrientMaxPlanar(*this,FirstBrin);

  // delete the edges added by Connexity and Triangulation
  tedge e;
  for(e = ne();e > OldNumEdge;e--) DeleteEdge(e);
  Prop<bool> eoriented(Set(tedge()),PROP_ORIENTED,true);
  ForAllEdgesOfG(e) eoriented[e] = true;
  return 0;
  }
int TopologicalGraph::Schnyder(bool schnyderRect,bool schnyderColor,bool schnyderLongestFace
                               ,tbrin FirstBrin)
  {if(nv() < 3)return -1;
  if(!CheckSimple())return -1;
  int OldNumEdge = ne();
  PSet1  propSave(Set());
  MakeConnected();
  if(!CheckPlanar())return -1;
  bool MaxPlanar = (ne() != 3 * nv() - 6) ? false : true;
  int len;
  if(!FirstBrin && schnyderLongestFace && !MaxPlanar)
      LongestFace(FirstBrin,len);
  else if(FirstBrin == 0)
      {FirstBrin = extbrin();FirstBrin = -acir[FirstBrin];}
 tbrin extOld = extbrin();
 //if(!MaxPlanar && TriconTriangulate())return -2; // only if triconnected
  if(!MaxPlanar && ZigZagTriangulate())return -2;

  if(schnyderColor)
      {Prop<short> ecolor(Set(tedge()),PROP_COLOR);
      SchnyderDecomp(*this,FirstBrin,ecolor);
      CalcXY(*this,FirstBrin,ecolor,schnyderRect);
      }
  else
      {svector<short> eecolor(1,ne());
      SchnyderDecomp(*this,FirstBrin,eecolor);
      CalcXY(*this,FirstBrin,eecolor,schnyderRect);
      }

  // delete the edges added by Connexity and Triangulation
  for(tedge e = ne();e > OldNumEdge;e--)DeleteEdge(e);
  //extbrin() = extOld;
  Set() =  propSave;
  return 0;
  }
int TopologicalGraph::SchnyderXYZ(bool schnyderColor,bool schnyderLongestFace,
                                  tbrin FirstBrin,svector<int> &x,svector<int>&y,svector<int>&z)
  {if(nv() < 3)return -1;
  if(!CheckSimple())return -1;
  int OldNumEdge = ne();
  MakeConnected();
  if(!CheckPlanar())return -1;
  bool MaxPlanar = (ne() != 3 * nv() - 6) ? false : true;
  int len;
  if(!FirstBrin && schnyderLongestFace && !MaxPlanar)
      LongestFace(FirstBrin,len);
  else if(FirstBrin == 0)
      {FirstBrin = extbrin();FirstBrin = -acir[FirstBrin];}

  //if(!MaxPlanar && TriconTriangulate() && ZigZagTriangulate())return -2;
  if(!MaxPlanar && ZigZagTriangulate())return -2;

  if(schnyderColor)
      {Prop<short> ecolor(Set(tedge()),PROP_COLOR);
      SchnyderDecomp(*this,FirstBrin,ecolor);
      CalcXYZ(*this,FirstBrin,ecolor,x,y,z);
      }
  else
      {svector<short> eecolor(1,ne());
      SchnyderDecomp(*this,FirstBrin,eecolor);
      CalcXYZ(*this,FirstBrin,eecolor,x,y,z);
      }

  // delete the edges added by Connexity and Triangulation
  for(tedge e = ne();e > OldNumEdge;e--)DeleteEdge(e);
  extbrin() = FirstBrin;
  return 0;
  }
int TopologicalGraph::SchnyderV(bool schnyderRect,bool schnyderColor,bool schnyderLongestFace
                                ,tbrin FirstBrin)
// FirstBrin will be on left top
  {if(nv() < 3)return -1;
  if(!CheckSimple())return -1;
  int OldNumEdge = ne();
  int OldNumVertex = nv();
  //  Opt6Biconnect();
  Biconnect();
  if(!CheckPlanar())return -1;
 

  bool MaxPlanar = (ne() != 3 * nv() - 6) ? false : true;
  int len;
  if(!FirstBrin && schnyderLongestFace && !MaxPlanar)
      LongestFace(FirstBrin,len);
  else if(FirstBrin == 0)
      {FirstBrin = extbrin();FirstBrin = -acir[FirstBrin];}

  if(!MaxPlanar && VertexTriangulate())return -2;


  if(schnyderColor)
      {Prop<short> ecolor(Set(tedge()),PROP_COLOR);
      SchnyderDecomp(*this,FirstBrin,ecolor);
      CalcXY(*this,FirstBrin,ecolor,schnyderRect);
      }
  else
      {svector<short> eecolor(1,ne());
      SchnyderDecomp(*this,FirstBrin,eecolor);
      CalcXY(*this,FirstBrin,eecolor,schnyderRect);
      }

  // delete the edges added by Connexity and Triangulation
  for(tvertex v = nv();v > OldNumVertex;v--)DeleteVertex(v);
  for(tedge e = ne();e > OldNumEdge;e--) DeleteEdge(e);
  return 0;
  }

int Embed3dSchnyder(TopologicalGraph &G0,bool schnyderLongestFace)
{
  if(G0.nv() < 3 || G0.ne() < 2)return -1;
  if (!G0.TestPlanar() || !G0.CheckSimple()) return -1;
  //
  int OldNumEdge = G0.ne();
  G0.MakeConnected();
  if(!G0.CheckPlanar())return -1;
  bool MaxPlanar = (G0.ne() != 3 * G0.nv() - 6) ? false : true;
  int len;
  tbrin FirstBrin = 1;
  if(!MaxPlanar && schnyderLongestFace)G0.LongestFace(FirstBrin,len);
  if(!MaxPlanar && G0.ZigZagTriangulate())return -2;

  // Creating a new vertex and an edge incident to it
  tvertex v = G0.NewVertex();
  tbrin nb,b,b0;
  b = b0 = FirstBrin;
  G0.incsize(tedge()); nb = G0.ne();  
  G0.acir[nb] = G0.acir[b0];   G0.acir[b0] = G0.cir[G0.acir[nb]] = nb; 
  G0.cir[nb] = b0;  G0.vin[nb] = G0.vin[b0];
  nb = -nb; 
  G0.vin[nb] = v;   G0.pbrin[v] = nb; 
  G0.cir[nb] = nb;  G0.acir[nb] = nb;
  // Triangulate the face
  b0 = -nb;   
  while((b = G0.cir[-b]) != b0) 
      {G0.incsize(tedge());
      tbrin bb = G0.ne();
      G0.acir[bb]  = G0.acir[nb]; G0.acir[nb] = G0.cir[G0.acir[bb]] = bb; G0.cir[bb] = nb;
      G0.acir[-bb] = G0.acir[b]; G0.acir[b] = G0.cir[G0.acir[-bb]] = -bb; G0.cir[-bb] = b;
      G0.vin[bb] = G0.vin[nb];    G0.vin[-bb] = G0.vin[b];
      nb = bb;    // for next insertion
      }

  //
  svector<int> x(1,G0.nv(),(int)0), y(1,G0.nv(),(int)0), z(1,G0.nv(),(int)0);
  svector<short> eecolor(1,G0.ne());
  SchnyderDecomp(G0,FirstBrin,eecolor);
  CalcXYZ(G0,FirstBrin,eecolor,x,y,z);

  //G0.SchnyderXYZ(0,x,y,z);
  RnEmbedding &em = *new RnEmbedding(G0.nv(),3,1);  
  Prop1<RnEmbeddingPtr> embedp(G0.Set(),PROP_RNEMBED);
  if (embedp().ptr!=0) delete embedp().ptr;
  embedp().ptr=&em;
  for (int i=1; i<= G0.nv();i++)
    {
    double xx=sqrt((double)x[i])/sqrt((double)G0.nv()-1.);
    double yy=sqrt((double)y[i])/sqrt((double)G0.nv()-1.);
    double zz=sqrt((double)z[i])/sqrt((double)G0.nv()-1.);
    double tmp1,tmp2;
    tmp1=xx; tmp2=yy; xx=tmp1*tmp1-tmp2*tmp2; yy=2*tmp1*tmp2;
    tmp1=yy; tmp2=zz; yy=tmp1*tmp1-tmp2*tmp2; zz=2*tmp1*tmp2;
    tmp1=zz; tmp2=xx; zz=tmp1*tmp1-tmp2*tmp2; xx=2*tmp1*tmp2;

    em.x(i)=xx;
    em.y(i)=yy;
    em.z(i)=zz;
    }
  // delete extra vertex
  G0.DeleteVertex(v);
  // delete the edges added by Connexity and Triangulation
  for(tedge e = G0.ne();e > OldNumEdge;e--)G0.DeleteEdge(e);
  G0.extbrin() = FirstBrin;

  return 0;
}

T_STD  ostream& operator <<(T_STD  ostream &os,const RnEmbeddingPtr &x)
{return os << x.ptr << T_STD  endl;}
T_STD  istream& operator >>(T_STD  istream &is, RnEmbeddingPtr &x)
{return is >> x.ptr;}
  


