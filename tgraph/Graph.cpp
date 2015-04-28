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
#include <TAXI/Tpoint.h>
#include <TAXI/graphs.h>
#include <TAXI/bicon.h>
#include <TAXI/Tmessage.h>
#include <TAXI/color.h>

//GRAPH
bool  Graph::CheckBiconnected()
  {if(Set().exist(PROP_BICONNECTED))return true;
  int n = nv();
  int m = ne();
  if (!m) return false;
  svector<tvertex> nvin(-m,m);
  svector<tvertex> &low = *new svector<tvertex>(0,n);
  svector<tbrin> &cir = *new svector<tbrin>(-m,m);
  int isconnected;
  PrepDFS(cir);
  isconnected = GDFS(cir,nvin);
  delete &cir;
  delete &low;
  if (!isconnected)return false;
  _Bicon Bicon(n);
  int ret=bicon(n,m,nvin,Bicon,low);
  if(ret)
      {Prop1<int> isbicon(Set(),PROP_BICONNECTED);return true;}
  return false;
  }

//TOPOLOGICALGRAPH
void  TopologicalGraph::init()
  {cir.SetName("cir"); acir.SetName("acir"); vin.SetName("vin");
  pbrin.SetName("pbrin"); 	
  pbrin.clear();
  tvertex v;
  vin[0]=0;
  for (tbrin b = -ne(); b <= ne(); b++)
      {v = vin[b];
      if(pbrin[v] == 0)
          {pbrin[v] = b;
          cir[b] = acir[b] = b;
          }
      else
          {acir[b]=acir[pbrin[v]];
          cir[acir[b]]=b;
          acir[pbrin[v]]=b;
          cir[b]=pbrin[v];
          }
      }
  extbrin()=1;
  Prop1<int> maptype(Set(),PROP_MAPTYPE);
  maptype() = PROP_MAPTYPE_ARBITRARY;
  }
tedge TopologicalGraph::NewEdge(const tbrin &ref1,const  tbrin &ref2)
// insertion apres ref1 et avant ref2
  {tbrin b2 = ref2,b1 = ref1;
  incsize(tedge());
  tbrin b = ne();
  cir[b]   = cir[b1];    cir[b1]   = acir[cir[b]]  = b;   acir[b]  = b1;
  acir[-b] = acir[b2];   acir [b2] = cir[acir[-b]] = -b;  cir[-b]  = b2;
  vin[b] = vin[b1];    vin[-b] = vin[b2];
  Set().erase(PROP_SIMPLE);
  if(planarMap() > 0) planarMap() = 0;
  Set().erase(PROP_PLANAR);
  Set().erase(PROP_SYM);
  //Prop1<int> maptype(Set(),PROP_MAPTYPE);maptype() =  PROP_MAPTYPE_UNKNOWN ; 
  if (Set().exist(PROP_HYPERGRAPH))
      { Prop<bool> ise(Set(tvertex()),PROP_HYPEREDGE);
      if ((ise[vin[b]]^ise[vin[-b]])==false)
          { Set().erase(PROP_HYPERGRAPH);
          Set().erase(PROP_BIPARTITE);
          }
      }
  else
      Set().erase(PROP_BIPARTITE);
  return ne();
  }
tvertex TopologicalGraph::ContractEdge(const tedge &e)
  {int n=nv();
  tbrin b=e.firsttbrin();
  tvertex x=vin[b];
  tvertex y=vin[-b];
  if (x==y) return x;
  int connected = Set().exist(PROP_CONNECTED);
  tbrin px = acir[b];
  if (px==b) //terminal edge
      {DeleteVertex(x);
      if(connected)Prop1<int> c(Set(),PROP_CONNECTED);
      Set().erase(PROP_SIMPLE);
      Set().erase(PROP_BIPARTITE);
      Set().erase(PROP_HYPERGRAPH);
      return (y==n) ? x : y ;
      }
  tbrin nx=cir[b];
  tbrin py = acir[-b];
  if(py==-b) //terminal edge
      {DeleteVertex(y);
      if(connected) Prop1<int> c(Set(),PROP_CONNECTED);
      Set().erase(PROP_SIMPLE);
      Set().erase(PROP_BIPARTITE);
      Set().erase(PROP_HYPERGRAPH);
      return (x==n) ? y : x;
      }
  tbrin ny=cir[-b];
  for (tbrin bb=nx; bb!=b; bb=cir[bb])
    vin[bb]=y;
  cir[px]=ny; acir[ny]=px;
  cir[py]=nx; acir[nx]=py;
  if (pbrin[y]==-b)
    pbrin[y]=nx;
  cir[b]=-b; acir[b]=-b; cir[-b]=b; acir[-b]=b; vin[-b]=x;
  pbrin[x]=b;
  DeleteVertex(x);
  //Remove loops at v
  tvertex v = (y==n) ? x : y;
  Set().erase(PROP_NLOOPS);
  RemoveLoops();
  if(connected) Prop1<int> c(Set(),PROP_CONNECTED);
  Set().erase(PROP_SIMPLE);
  Set().erase(PROP_BIPARTITE);
  Set().erase(PROP_HYPERGRAPH);
  //Prop1<int> maptype(Set(),PROP_MAPTYPE);maptype() =  PROP_MAPTYPE_UNKNOWN ; 
  return v;
  }
tvertex TopologicalGraph::BissectEdge(const tedge &e)
  {//the edge e becomes the one corresponding to first tbrin
  tedge e1 = e;
  tbrin b12 =  e1.secondtbrin();
  tbrin cir_b12 = cir[b12];
  tbrin acir_b12 = acir[b12];
  tvertex v2 = vin[b12];
  // create new vertex
  incsize(tvertex());
  tvertex v = nv();
  pbrin[v] = b12;
  vin[b12] = v;
  // create new edge
  incsize(tedge());
  tedge e2 = ne();
  tbrin b21 = e2.firsttbrin();
  tbrin  b22 = e2.secondtbrin();
  vin[b21] = v; vin[b22] = v2;
  // create cir at v
  cir[b12] = acir[b12] = b21;
  cir[b21] = acir[b21] = b12;
  // modify cir at v2
  if (acir_b12==b12)
      cir_b12=acir_b12=b22;
  cir[acir_b12] = b22; cir[b22] = cir_b12;
  acir[cir_b12] = b22; acir[b22] = acir_b12;
  // check pbrin at v2
  if(pbrin[v2] == b12)pbrin[v2] = b22;
  Set().erase(PROP_TRICONNECTED);
  Set().erase(PROP_FOURCONNECTED);
  Set().erase(PROP_SYM);
  Set().erase(PROP_HYPERGRAPH);
  Set().erase(PROP_BIPARTITE);
  //Prop1<int> maptype(Set(),PROP_MAPTYPE);maptype() =  PROP_MAPTYPE_UNKNOWN ; 
  // Orient if necessary
  if(PE().exist(PROP_ORIENTED))
      {Prop<bool> oriented(Set(tedge()),PROP_ORIENTED);
      oriented[e2]=oriented[e1];
      }
  return v;
  }
tedge TopologicalGraph::NewEdge(const tvertex &vv1,const tvertex &vv2,tedge )
// insertion "quelconque"
  {tedge e;
  tvertex v1=vv1, v2=vv2;
  incsize(e);
  tbrin b = ne();
  InsertBrin(v1,b);
  InsertBrin(v2,-b);
  Set().erase(PROP_SIMPLE);
  planarMap() = 0;
  Set().erase(PROP_PLANAR);
  Set().erase(PROP_SYM);
  if (Set().exist(PROP_HYPERGRAPH))
      { Prop<bool> ise(Set(tvertex()),PROP_HYPEREDGE);
      if ((ise[v1]^ise[v2])==false)
          { Set().erase(PROP_HYPERGRAPH);
          Set().erase(PROP_BIPARTITE);
          }
      }
  else
      Set().erase(PROP_BIPARTITE);
  //Prop1<int> maptype(Set(),PROP_MAPTYPE);maptype() =  PROP_MAPTYPE_UNKNOWN ; 
  return ne();
  }
void TopologicalGraph::DeleteEdge(const tedge &e)
  {tbrin b = Abs(e());
  UnlinkBrin(-b);
  UnlinkBrin(b);
  SwapEdge((tedge)b());
  if(extbrin().GetEdge()== ne()) extbrin()=1;
  decsize(tedge());
  Set().erase(PROP_CONNECTED);
  Set().erase(PROP_BICONNECTED);
  Set().erase(PROP_TRICONNECTED);
  Set().erase(PROP_FOURCONNECTED);
  //Prop1<int> maptype(Set(),PROP_MAPTYPE);maptype() =  PROP_MAPTYPE_UNKNOWN ; 
  Set().erase(PROP_SYM);
  if(planarMap() < 0)planarMap() = 0;
  }
void TopologicalGraph::MoveBrin(const tbrin &b, const tvertex &to)
  { UnlinkBrin(b);
  InsertBrin(to,b);
  Set().erase(PROP_SIMPLE);
   planarMap() = 0;
  Set().erase(PROP_CONNECTED);
  Set().erase(PROP_BICONNECTED);
  Set().erase(PROP_TRICONNECTED);
  Set().erase(PROP_FOURCONNECTED);
  Set().erase(PROP_SYM);
  //Prop1<int> maptype(Set(),PROP_MAPTYPE);maptype() =  PROP_MAPTYPE_UNKNOWN ; 
  if (Set().exist(PROP_HYPERGRAPH))
      { Prop<bool> ise(Set(tvertex()),PROP_HYPEREDGE);
      if ((ise[vin[b]]^ise[vin[-b]])==false)
          { Set().erase(PROP_HYPERGRAPH);
          Set().erase(PROP_BIPARTITE);
          }
      }
  else
      Set().erase(PROP_BIPARTITE);
  }
void TopologicalGraph::PrivateReverseEdge(const tedge &e)
  {tbrin b = e.firsttbrin();
  if (cir[b]!=b) {cir[acir[b]]=-b; acir[cir[b]]=-b;}
  if (cir[-b]!=-b) {cir[acir[-b]]=b; acir[cir[-b]]=b;}
  tbrin cir_b=cir[b];
  tbrin acir_b=acir[b];
  cir[b] = (cir[-b]==-b)? b : cir[-b];
  acir[b] = (acir[-b]==-b)? b : acir[-b];
  cir[-b] = (tbrin)((cir_b()==b())? -b() : cir_b());
  acir[-b] = (tbrin)((acir_b()==b())? -b() : acir_b());
  tvertex vin_b=vin[b];
  vin[b]=vin[-b];
  vin[-b]=vin_b;
  pbrin[vin_b]=-b;
  pbrin[vin[b]]=b;
  if (extbrin().GetEdge()==e) extbrin()=-extbrin();
  }
void TopologicalGraph::SwitchOrientations()
  {Prop<bool> reoriented(Set(tedge()),PROP_REORIENTED); 
  for (tedge e=1; e<=ne();e++)
      if (reoriented[e])
          PrivateReverseEdge(e);
  }
void TopologicalGraph::ReverseEdge(const tedge &e)
  {PrivateReverseEdge(e);
  Prop<bool> reoriented(Set(tedge()),PROP_REORIENTED,false);
  reoriented[e] = reoriented[e] ? false : true;
  }
void TopologicalGraph::FixOrientation()
  {Prop<bool> reoriented(Set(tedge()),PROP_REORIENTED); 
  reoriented.clear();
  }
void TopologicalGraph::RestoreOrientation()
  {SwitchOrientations();
  FixOrientation();
  }
tvertex TopologicalGraph::NewVertex()
  {incsize(tvertex());
  pbrin[nv()] = 0;
  Set().erase(PROP_CONNECTED);
  Set().erase(PROP_BICONNECTED);
  Set().erase(PROP_TRICONNECTED);
  Set().erase(PROP_FOURCONNECTED);
  Set().erase(PROP_SYM);
  return nv();
  }
void TopologicalGraph::DeleteVertex(const tvertex &v)
  {if (Set().exist(PROP_HYPERGRAPH))
      {Prop<bool> ise(Set(tvertex()),PROP_HYPEREDGE);
      Prop1<int> nhv(Set(),PROP_NV);
      Prop1<int> nhe(Set(),PROP_NE);
      if (ise[v]) --nhe();
      else --nhv();
      }
  while(pbrin[v]!=0)
      DeleteEdge(pbrin[v].GetEdge());
  SwapVertex(v);
  decsize(tvertex());
//   Set().erase(PROP_CONNECTED);
//   Set().erase(PROP_BICONNECTED);
//   Set().erase(PROP_TRICONNECTED);
//   Set().erase(PROP_FOURCONNECTED);
  }
void TopologicalGraph::SwapEdge(const tedge &e)
  {// on ne recalcule que pour ne
  if (e==ne())return;
  PB().CopyIndex(e(),ne());
  PB().CopyIndex(-e(),-ne());
  PE().CopyIndex(e(),ne());
  // ne() became e
  tbrin b0,b1;
  b0 = NewSwapLabel(acir[e](),e());
  b1 = NewSwapLabel(cir[e](),e());
  acir[e] = b0;
  cir[b0] = e.firsttbrin();
  cir[e] = b1;
  acir[b1] = e.firsttbrin();
  b0 = NewSwapLabel(acir[-e](),e());
  b1 = NewSwapLabel(cir[-e](),e());
  acir[-e] = b0;
  cir[b0] = -e;
  cir[-e] = b1;
  acir[b1] = -e;
  if(pbrin[vin[-e]] == -ne()) pbrin[vin[-e]]=e.secondtbrin();
  if(pbrin[vin[e]] == ne()) pbrin[vin[e]]=e.firsttbrin();
  // cas particulier pour extbrin
  if (extbrin()==e()) extbrin()=ne();
  else if (extbrin()==-e()) extbrin()=-ne();
  else if (extbrin()==ne()) extbrin()=e();
  else if (extbrin()==-ne()) extbrin()=-e();
  }
void TopologicalGraph::SwapVertex(const tvertex &v)
  {if (v==nv()) return;
  PV().CopyIndex(v(),nv());
  // nv() became v
  tbrin b0 = pbrin[v];
  tbrin b = b0;
  if(b0!=0)
      do
          {vin[b]= v;
          b = cir[b];
          } while(b!=b0);
  }
void TopologicalGraph::UnlinkBrin(const tbrin &b)
  {tvertex v = vin[b];
  if (b == pbrin[v])
      {tbrin b2 = cir[b];
      if(b == b2)
          {pbrin[v]=0; return;}
      pbrin[v] = b2;
      }
  cir[acir[b]] = cir[b];
  acir[cir[b]] = acir[b];
  }
void TopologicalGraph::InsertBrin(const tvertex &v,const tbrin &b)
  {vin[b] = v;
  if(!pbrin[v])
      {pbrin[v]=b; cir[b]=acir[b]=b;}
  else
      {tbrin bref = pbrin[v];
      acir[b] = acir[bref]; acir[bref] = cir[acir[b]] = b; cir[b] = bref;
      }
  }
void TopologicalGraph::MoveBrinToFirst(const tbrin &b)
{tvertex v=vin[b];
  tbrin b0=pbrin[v];
  if (b==b0)
    return;
  // unlink b
  cir[acir[b]] = cir[b];
  acir[cir[b]] = acir[b];
  // relink before pbrin
  acir[b]=acir[b0]; cir[b]=b0;
  cir[acir[b]]=b; acir[b0]=b;
  pbrin[v]=b;
  Prop1<int> maptype(Set(),PROP_MAPTYPE);
  maptype() = PROP_MAPTYPE_ARBITRARY;
}
int TopologicalGraph::Degree(const tvertex &v)
  {tbrin b0,b;
  if((b0 = pbrin[v]) == 0)return 0;
  int deg = 0;
  b = b0;
  do
      {b = cir[b];++deg;
      }while(b != b0);
  return deg;
  }
int TopologicalGraph::InDegree(const tvertex &v)
  {tbrin b0,b;
  if((b0 = pbrin[v]) == 0)return 0;
  int deg = 0;
  b = b0;
  do
      {if(b < 0)deg++; b=cir[b];
      }while(b != b0);
  return deg;
  }
int TopologicalGraph::OutDegree(const tvertex &v)
  {tbrin b0,b;
  if((b0 = pbrin[v]) == 0)return 0;
  int deg = 0;
  b = b0;
  do
      {if(b > 0)deg++; b=cir[b];
      }while(b != b0);
  return deg;
  }
int TopologicalGraph::FaceWalkLength(const tbrin &b0)
  {if(!b0)return 0;
  int len = 0;
  tbrin b = b0;
  do
      ++len;
  while((b = cir[-b]) != b0);
  return len;
  }
bool TopologicalGraph::CheckNoLoops()
  {if(Set().exist(PROP_NLOOPS))
      {Prop1<int> nloops(Set(),PROP_NLOOPS);
      return (nloops() == 0) ? true : false;
      }
  else if(ne() == 0)
      {Prop1<int> nloops(Set(),PROP_NLOOPS);
      nloops() = 0;
      return true;
      }		
  if(debug())DebugPrintf("  Executing CheckNoLoops");
  Prop1<int> nloops(Set(),PROP_NLOOPS);
  int n = 0;
  for(tedge e = ne();e >= 1;e--)
      if(vin[e] == vin[-e])++n;  
  nloops() = n;
  return true;
  }
int TopologicalGraph::RemoveLoops()
  {if(Set().exist(PROP_NLOOPS))
       {Prop1<int> numloops(Set(),PROP_NLOOPS);
       return numloops();
       }
  else if(ne() == 0)
      {Prop1<int> numloops(Set(),PROP_NLOOPS);
      numloops() = 0;
      return 0;
      }		
  if(debug())DebugPrintf("  Executing RemoveLoops");
  int n = 0;
  Prop<int> nloops(Set(tvertex()),PROP_NLOOPS); nloops.clear();
  nloops.SetName("loops");
  for(tedge e = ne();e >= 1;e--)
      if(vin[e] == vin[-e]){++n; ++nloops[vin[e]]; DeleteEdge(e);}
  Prop1<int> numloops(Set(),PROP_NLOOPS);
  numloops() = 0;
  if(debug())DebugPrintf("  End RemoveLoops: %d loops",n);
  return n;
  }
int TopologicalGraph::RemoveIsolatedVertices()
  {int n = 0;
  for(tvertex v = nv();v >= 1;v--)
      if(!Degree(v)){++n;DeleteVertex(v);}
  return n;
  }
bool TopologicalGraph::DebugCir()
  {int i=0;
  if(vin[0]!=0) {DebugPrintf("DC vin[0]=%d",vin[0]());return false;}
  if(cir[0]!=0) {DebugPrintf("DC cir[0]=%d",cir[0]());return false;}
  if(acir[0]!=0) {DebugPrintf("DC acir[0]=%d",acir[0]());return false;}
  
  for(tvertex v = 1;v <= nv();v++)
      {int degree = Degree(v);
      if(!degree){DebugPrintf("DC isolated vertex v=%d",v());continue;}
      if(!pbrin[v]){DebugPrintf("DC no pbrin at v=%d",v());return false;}
      tbrin b0 = pbrin[v];
      tbrin b = b0;
      do 
          {if(cir[b] == 0){DebugPrintf("DC cir[%d] = 0",b());return false;}
          if(acir[b] == 0){DebugPrintf("DC acir[%d] = 0",b());return false;}
          if(!cir.InRange(b())){DebugPrintf("DC cir not in range b=%d",b());return false;}
          if(!acir.InRange(cir[b]())){DebugPrintf("DC acir not in range b=%d",b());return false;}
          if(vin[b]!=v){DebugPrintf("DC vin[b]=%d v=%d b=%d degree=%d",vin[b](),v(),b(),degree);return false;}
          if(vin[-b]==v){DebugPrintf("DC %d is a loop at v=%d",b(),v());return false;}
          if(acir[cir[b]]!=b)
              {DebugPrintf("DC at v=%d b=%d cir=%d acir[cir]=%d",v(),b(),cir[b](),acir[cir[b]]());
              b = b0;
              DebugPrintf("DC Around v=%d",v());
              do
                  {DebugPrintf("    %d cir:%d acir:%d",b(),cir[b](),acir[b]());
                  }while((b = cir[b])!=b0);  
              return false;
              }
          if(cir[acir[b]]!=b)
              {DebugPrintf("DC1 at v=%d b=%d cir=%d acir[cir]=%d",v(),b(),cir[b](),acir[cir[b]]());
              b = b0;
              DebugPrintf("DC1 Around v=%d",v());
              do
                  {DebugPrintf("    %d cir:%d acir:%d",b(),cir[b](),acir[b]());
                  }while((b = cir[b])!=b0);  
              return false;
              }
              
          b = cir[b];
          i++;
          }while (b!=b0);
      }
  if(i != 2*ne()) 
     {DebugPrintf("Did not reached some brins: m=%d, sums of the degrees=%d",ne(),i);
     return false;
     }
  if(debug())DebugPrintf("DebugCir OK");
  return true;
  }
int TopologicalGraph::NumFaces()
  {tbrin brin_depart,brin_courant;
  int NumberOrbits=0;
  int m = ne();
  svector<char> already_met(-m,m,char(0));

  // Parcourt de toutes les faces 
  already_met[0]=(char)1;
  for(brin_depart=-m; brin_depart<=m; brin_depart++)
      {if(already_met[brin_depart])continue;
      brin_courant=brin_depart;
      do
          {already_met[brin_courant] = (char)1;
          brin_courant = cir[-brin_courant];
          }
      while (!already_met[brin_courant]);
      ++NumberOrbits;
      }
  return NumberOrbits;
  }
int TopologicalGraph::ComputeGenus()
  {if(!CheckConnected())
      genus = -1;
  else
      genus = ((ne() - nv() + 2) - NumFaces())/2;
  return genus;
  }
int TopologicalGraph::FaceNumberIsthmus(const tbrin &b0)
  {if(!b0)return 0;
  svector<int>mark(1,ne(),0);
  tbrin b = b0;
  do
      mark[b.GetEdge()]++;
  while((b = cir[-b]) != b0);

  int n = 0;
  b = b0;
  do
      if(mark[b.GetEdge()] == 2)++n;
  while((b = cir[-b]) != b0);
  return n/2;
  }
int TopologicalGraph::Rotate(tedge e)
  {tbrin b = e.firsttbrin();
  if (cir[-b].GetEdge()==e || cir[b].GetEdge()==e)
      {DPRINTF(("Switch of %d not allowed",e()));
      return 1;   // No rotation allowed
      }
  if (pbrin[vin[-b]] == -b) pbrin[vin[-b]]=cir[-b];
  if (pbrin[vin[b]] == b) pbrin[vin[b]]=cir[b];
  cir[acir[-b]] = cir[-b]; acir[cir[-b]] = acir[-b];
  cir[acir[b]] = cir[b]; acir[cir[b]] = acir[b];
  tbrin b1,b2;
  b1 = -cir[-b]; b2 = cir[b1];
  cir[b1] = -b; cir[-b] = b2; acir[b2] = -b; acir[-b] = b1; vin[-b] = vin[b1];
  b1 = -cir[b]; b2 = cir[b1];
  cir[b1] = b; cir[b] = b2; acir[b2] = b; acir[b] = b1; vin[b] = vin[b1];
  return 0;
  }
bool TopologicalGraph::CheckRegular(int &d)
  {d = Degree(1);
  if(nv() == 1)return true;
  for(tvertex v = 2; v <= nv();v++)
      if(Degree(v) != d)return false;
  return true;
  }
bool TopologicalGraph::MinMaxDegree(int& dmin,int& dmax)
  {dmin = nv(); dmax = 0;
  int d;
  for(tvertex v = 1; v <= nv();v++)
      {d = Degree(v);
      if(d > dmax)dmax = d;
      if(d < dmin)dmin = d;
      }
  if(dmin != dmax)return false;
  return true;
  }
