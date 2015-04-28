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


#include <TAXI/Tstring.h>
#include <TAXI/graphs.h>
#include <TAXI/bicon.h>
#include <TAXI/Tmessage.h>
#include <TAXI/color.h>

bool TopologicalGraph::FindPlanarMap()
  {if(planarMap() < 0){if(debug())DebugPrintf("  EXIST planarMap < 0");return false;}
  bool Connect= CheckConnected();

  if(planarMap() == 1) 
      {if(debug())DebugPrintf("  EXIST planarMap = 1");
      return true;
      }
  if(Connect && ComputeGenus() == 0)
      {if(debug())DebugPrintf("Good Genus");
      planarMap() = 1;return true;
      }
  if(Connect && Set(tvertex()).exist(PROP_COORD)) // Geometric Graph
      {GeometricGraph GG(*this);
      if(debug())DebugPrintf("Executing geometric cir");
      Prop1<int> maptype(Set(),PROP_MAPTYPE,0);
      int _maptype = maptype();
      svector<tbrin> cir0,acir0;
      cir0 = cir; acir0 = acir;
      if(GG.ComputeGeometricCir() == 0)
          {if(debug())DebugPrintf("Using geometric map");
          planarMap() = 1;return true;
          }
      maptype() =  _maptype;
      cir.vector() = cir0; acir.vector() = acir0; 
      }
  if(debug())DebugPrintf("FindPlanarMap:LRALGO");
  int ret = Planarity();
  if(ret  == 1){planarMap() = 1; return true;}
  else planarMap() = -1;
  return false;
  }
bool TopologicalGraph::CheckBiconnected()
  {if(debug())DebugPrintf("   CheckBionnected");
  if(Set().exist(PROP_BICONNECTED))return true;
  if(nv() < 3 || ne() < 3)return false;
  if(debug())DebugPrintf("Executing CheckBionnected");
  int m = ne();
  int n = nv();
  if (m==0) return true;
  svector<tvertex> nvin(-m,m);   nvin.SetName("TG:Bicon:nvin");
  svector<tvertex> low(0,n);     low.SetName("TG:Bicon:low"); 

  if(!DFS(nvin)) // not connected ...
      return false;
  _Bicon Bicon(n);
  int ret = bicon(n,m,nvin,Bicon,low);
  if(ret)
      {Prop1<int> isbicon(Set(),PROP_BICONNECTED);
      Prop1<int> is_con(Set(),PROP_CONNECTED);
      return true;
      }
  return false;
  }
int TopologicalGraph::Simplify()
// returns the # o multiple edges + #  loops
  {if(Set().exist(PROP_SIMPLE))return 0;
  if(debug())DebugPrintf("  Executing Simplify");
  // Remove Loops
  int n = RemoveLoops();
  if(!ne()){Prop1<int> simple(Set(),PROP_SIMPLE);return n;}
  
  svector<tedge>link(0,ne()); link.clear(); link.SetName("TG:Simplify:link");
  svector<tedge>top1(1,nv()); top1.clear(); top1.SetName("TG:Simplify:top1");
  svector<tedge>top2(1,nv()); top2.clear(); top2.SetName("TG:Simplify:top2");
  tvertex u,v,w;
  tedge e,e0,next;
  //First sort with respect to biggest label
  for(e = ne();e >= 1;e--)
      {v = (vin[e] < vin[-e]) ? vin[-e] : vin[e];
      link[e] = top1[v];top1[v] = e;
      }
  // Then sort with respect to smallest label
  for(u = nv();u > 1;u--)
      {e = top1[u];
      while(e!=0)
          {next = link[e];     //as link is modified
          v = (vin[e] < vin[-e]) ? vin[e] : vin[-e];
          link[e] = top2[v]; top2[v] = e;
          e = next;
          }
      }
  // Erase Multiple edges, but backup multiplicity
  Prop<int> multiplicity(Set(tedge()),PROP_MULTIPLICITY);
  for (e=1; e<=ne(); ++e) multiplicity[e]=1;

  for(v = nv()-1;v >= 1;v--)
      {e0 = top2[v];
      u = vin[e0];if(u == v)u = vin[-e0];
      e=link[e0];
      link[e0]=0;
      while(e!=0)
          {next=link[e];
	  w = vin[e];if(w == v)w = vin[-e];
          if(u == w)
	    {++n; 
	    ++multiplicity[e0]; 
	    link[e] = 1;
	    }
          else
	    {u=w;
	    link[e0=e] = 0;
	    }
	  e=next;
          }
      }

  bool erased = false;
  for(e = ne();e >= 1;e--)
      if(link[e]!=0){DeleteEdge(e);erased = true;}

  if(!erased)Set(tedge()).erase(PROP_MULTIPLICITY);
  Prop1<int> simple(Set(),PROP_SIMPLE);  
  return n;
  }
int TopologicalGraph::ExpandEdges()
{ 
  int nadded=0;
  int morig=ne();
  tedge e;
  int i;
  if (!Set(tedge()).exist(PROP_MULTIPLICITY)) return 0;
  Prop<int>  multiplicity(Set(tedge()),PROP_MULTIPLICITY);
  for (e=1; e<=morig; e++)
    {for (i=2; i<=multiplicity[e]; ++i)
      NewEdge(e.firsttbrin(), e.secondtbrin());
    nadded+=multiplicity[e]-1;
    }
  Set(tedge()).erase(PROP_MULTIPLICITY);
  // add loops
  if (!Set(tvertex()).exist(PROP_NLOOPS))
    return nadded;
  tvertex v;
  Prop<int> nloops(Set(tvertex()),PROP_NLOOPS);
  for (v=1; v<=nv(); ++v)
    for (i=1; i<=nloops[v]; ++i)
      {++nadded;
      NewEdge(v,v);
      }
  Set(tvertex()).erase(PROP_NLOOPS);
  return nadded;
}

bool TopologicalGraph::CheckSimple()
  {if(debug())DebugPrintf("   CheckSimple");
  if(Set().exist(PROP_SIMPLE))return true;
  if(ne() <= 1){Prop1<int> simple(Set(),PROP_SIMPLE);return true;}
  if(debug())DebugPrintf("Executing CheckSimple");
  if(!CheckNoLoops())return false;
  
  svector<tedge>link(1,ne()); link.clear(); link.SetName("CheckSimple:link");
  svector<tedge>top1(1,nv()); top1.clear(); top1.SetName("CheckSimple:link");
  svector<tedge>top2(1,nv()); top2.clear(); top2.SetName("CheckSimple:link");
  tvertex u,v,w;
  tedge e,next;
  //First sort with respect to biggest label
  for(e = ne();e >= 1;e--)
      {v = (vin[e] < vin[-e]) ? vin[-e] : vin[e];
      link[e] = top1[v];top1[v] = e;
      }
  // Then sort with respect to smallest label
  for(u = nv();u > 1;u--)
      {e = top1[u];
      while(e!=0)
          {next = link[e];     //as link is modified
          v = (vin[e] < vin[-e]) ? vin[e] : vin[-e];
          link[e] = top2[v]; top2[v] = e;
          e = next;
          }
      }
  // Check Multiple edges
  for(v = nv()-1;v >= 1;v--)
      {e = top2[v];
      while(e!=0)
          {next = link[e];
          u = vin[e];    if(u == v)u = vin[-e];
          w = vin[next]; if(w == v)w = vin[-next];
          if(u == w)
              return false;
          else
              link[e] = 0;
          e = next;
          }
      }
  Prop1<int> simple(Set(),PROP_SIMPLE);
  return true;
  }
int TopologicalGraph::BFS(svector<int> &comp)
  {comp.clear();
  if(!nv()) return -1;
  tvertex v,w;
  tbrin b,b0;
  int ncc = 0;
  svector<tvertex> stack(1,nv()); stack.SetName("TG:BFS:stack");
  int rank =0;
  int max_rank = 0;
  tvertex v0 = 1;

  while (max_rank < nv())
      {while (comp[v0]) v0++;  
      comp[v0] = ++ncc;
      ++max_rank;
      stack[rank + 1] = v0;

      while(rank < max_rank)
          {v = stack[++rank];  
          b = b0 = pbrin[v];
          if (b0!=0)
              do {w = vin[-b]; 
              if(!comp[w])
                  {comp[w] = ncc; 
                  stack[++max_rank] = w;
                  }
              }while((b = cir[b])!= b0);
          }
      }
  return ncc;
  }
int TopologicalGraph::MakeConnectedVertex()
//returns the initial number of connected components
  {if(debug())DebugPrintf("   CheckPropConnected");
  if(Set().exist(PROP_CONNECTED))return 1;
  Prop1<int>is_connected(Set(),PROP_CONNECTED);
  if(!nv()) return 0;
  if(debug())DebugPrintf("Executing MakeConnectedVertex");
  bool simple = Set().exist(PROP_SIMPLE);
  bool bipartite = Set().exist(PROP_BIPARTITE);
  tvertex v,w;
  tbrin b,b0;
  int ncc = 0;
  svector<tvertex> stack(1,nv()); stack.SetName("TP:stack");
  svector<tvertex> comp(0,nv()+1); comp.clear(); comp.SetName("TP:Comp");
  int rank =0;
  int max_rank = 0;
  tvertex v0 = 1;
  tvertex previous_root = 1;
  int n = nv();
  int m = ne();
  tvertex vv = NewVertex();
  NewEdge(vv,v0); 
  comp[vv] = v0;
  while (max_rank < n)
      {while (comp[v0]!=0) v0++;
      comp[v0] = v0;
      ++ncc;
      if(ncc > 1) NewEdge(vv,v0); 
      previous_root = v0;
      ++max_rank;
      stack[rank + 1] = v0;

      while(rank < max_rank)
          {v = stack[++rank];
          b = b0 = pbrin[v];
          if (b0!=0)
              do {w = vin[-b];
              if(!comp[w])
                  {comp[w] = previous_root;
                  stack[++max_rank] = w;
                  }
              }while((b = cir[b])!= b0);
          }
      }
  if(ne() == m+1)// the graph was connected
     { DeleteVertex(vv);
     if(debug())DebugPrintf("End  MakeConnectedVertex  m=%d",ne());
     return 1;
     }
 if(simple)Prop1<int> simple(Set(),PROP_SIMPLE);
 if(bipartite)Prop1<int> bipartite(Set(),PROP_BIPARTITE);
 if(Set(tvertex()).exist(PROP_COORD)) // Geometric Graph
     {Prop<Tpoint> vcoord(Set(tvertex()),PROP_COORD);
     int deg; 
     tvertex w;
     Tpoint p(.0,.0);
     deg = 0;
     Forall_adj_brins_of_G(b,vv)
       {w = vin[-b]; ++deg;
       p += vcoord[w];
       }
     vcoord[vv] = p/(double)deg;
     }
  if(debug())DebugPrintf("End  MakeConnectedVertex  m=%d",ne());
  return ncc - 1;
  }
int TopologicalGraph::MakeConnected(bool mark_cc)
//returns the initial number of connected components
  {if(debug())DebugPrintf("   CheckPropConnected");
  if(Set().exist(PROP_CONNECTED))return 1;
  Prop1<int>is_connected(Set(),PROP_CONNECTED);
  if(!nv()) return 0;
  if(debug())DebugPrintf("Executing MakeConnected");
  int simple = 0,bipartite = 0;
  if(Set().exist(PROP_SIMPLE))simple= true;
  if(Set().exist(PROP_BIPARTITE))bipartite= true;
  tvertex v,w;
  tbrin b,b0;
  int ncc = 0;
  svector<tvertex> stack(1,nv()); stack.SetName("TP:makecon:stack");
  svector<tvertex> comp(0,nv()); comp.clear(); comp.SetName("TP:makecon:Comp");
  int rank =0;
  int max_rank = 0;
  tvertex v0 = 1;
  tvertex previous_root = 1;

  while (max_rank < nv())
      {while (comp[v0]!=0) v0++;
      comp[v0] = v0;
      ++ncc;
      if(ncc > 1) NewEdge(previous_root,v0); 
      previous_root = v0;
      ++max_rank;
      stack[rank + 1] = v0;

      while(rank < max_rank)
          {v = stack[++rank];
          b = b0 = pbrin[v];
          if (b0!=0)
              do {w = vin[-b];
              if(!comp[w])
                  {comp[w] = previous_root;
                  stack[++max_rank] = w;
                  }
              }while((b = cir[b])!= b0);
          }
      }
 if(simple)Prop1<int> simple(Set(),PROP_SIMPLE);
 if(bipartite)Prop1<int> bipartite(Set(),PROP_BIPARTITE);
  if (mark_cc)
     {Prop<tvertex> rep(Set(tvertex()),PROP_REPRESENTATIVEV);
     comp.Tswap(rep);
     }
  if(debug())DebugPrintf("End  MakeConnected m=%d",ne());
  return ncc - 1;
  }
int TopologicalGraph::ColorConnectedComponents()
//returns the initial number of connected components
  {if(debug())DebugPrintf("   CheckConnected");
  if(Set().exist(PROP_CONNECTED))return true;
  if(!nv()) return 0;
  if(debug())DebugPrintf("Executing ColorConnectedComponents");
  tvertex v,w;
  tbrin b,b0;
  int ncc = 0;
  svector<tvertex> stack(1,nv()); stack.SetName("TP:color:stack");
  svector<tvertex> comp(0,nv()); comp.clear(); comp.SetName("TP:color:Comp");
  Prop<short> vcolor(Set(tvertex()),PROP_COLOR);
  int rank =0;
  int max_rank = 0;
  tvertex v0 = 1;
  tvertex previous_root = 1;
  short col;
  while (max_rank < nv())
      {while (comp[v0]!=0) v0++;
      comp[v0] = v0;
      ++ncc;
      col = (ncc + Yellow-2) %16 + 1;
      previous_root = v0;
      ++max_rank;
      stack[rank + 1] = v0;
      vcolor[v0] = col;

      while(rank < max_rank)
          {v = stack[++rank];
          b = b0 = pbrin[v];
          if (b0!=0)
              do {w = vin[-b];
              if(!comp[w])
                  {comp[w] = previous_root;
                  stack[++max_rank] = w;
                  vcolor[w] = col;
                  }
              }while((b = cir[b])!= b0);
          }
      }
  if(debug())DebugPrintf("END ColorConnectedComponents");
  return ncc;
  }
bool TopologicalGraph::CheckBipartite(bool Color)
  {if(debug())DebugPrintf("    CheckBipartite Color=%d",(int)Color);
  if(Set().exist(PROP_BIPARTITE) && !Color)return true;
  if(!nv()) return false; 
  if((nv() == 1) || !ne() || (!Color && ne() <= 3))
  {Prop1<int>is_bipartite(Set(),PROP_BIPARTITE);return true;}
  if(debug())DebugPrintf("Executing CheckBipartite");
  svector<tvertex> stack(1,nv()); stack.SetName("stack");
  svector<bool> vcol(1,nv()); vcol.SetName("vcol");
  svector<int> comp(1,nv()); comp.clear(); comp.SetName("Comp");
  tvertex v,w;
  tbrin b,b0;
  int ncc = 0;
  int rank =0;
  int max_rank = 0;
  tvertex v0 = 1;
  bool bipartite = true;

  while (max_rank < nv())
      {// find a root
      while (comp[v0]) v0++;
      comp[v0] = ++ncc;
      vcol[v0] = true;
      ++max_rank;
      stack[rank + 1] = v0;

      while(rank < max_rank)
          {v = stack[++rank];
          b = b0 = pbrin[v];
          if (b0!=0)
              do 
                  {w = vin[-b];
                  if(!comp[w])  // tree edge
                      {comp[w] = ncc;
                      stack[++max_rank] = w;
                      vcol[w] = !vcol[v];
                      }
                  else          // cotree
                      if(vcol[v] == vcol[w])bipartite = false;
                  }while((b = cir[b])!= b0);
          if(!bipartite)return false;
          }
      }

  if(ncc == 1)
      Prop1<int>is_connected(Set(),PROP_CONNECTED);
  if(!bipartite)return false;
  if(Color)
      {Prop<short> vcolor(Set(tvertex()),PROP_COLOR);
      for(v = 1;v <= nv();v++)
          vcolor[v] = (vcol[v] == true) ? Red : Blue;
      }
  // counts red/blue vertices
  Prop1<int>nred(Set(),PROP_NV);
  nred()=0;
  for(v = 1;v <= nv();v++)
      if (vcol[v]) ++nred();
  Prop1<int>nblue(Set(),PROP_NE);
  nblue()=nv()-nred();
  
  Prop1<int>is_bipartite(Set(),PROP_BIPARTITE);
  if(debug())DebugPrintf("END   CheckBipartite");
  return true;
  }
// Checks a connected graph is bipartite and make it a hypergraph
// v0 is assumed to be a (hyper)edge (resp. vertex) if v0ise=true
// (resp. false).
// this is a very simple BFS from v0
bool TopologicalGraph::CheckHypergraph(tvertex v0, bool v0ise)
  {if (Set().exist(PROP_HYPERGRAPH)) return true;
  tbrin b,b0;
  tvertex v,w;
  int n=nv();
  svector<tvertex> queue(0,n);
  svector<bool> ise(0,n);
  svector<bool> done(0,n); done.clear();
  int top=0;
  int bot=0;
  int nhv=0; int nhe=0;
  bool bipartite=true;
  queue[top++]=v0;
  if ((ise[v0]=v0ise)==true) ++nhe;
  else ++nhv;
  done[v0]=true;
  while (bipartite && (bot<top))
      {
          v=queue[bot++];
          b=b0=pbrin[v];
          do
              {w=vin[-b];
              if (!done[w])
                  {done[w]=true;
                  queue[top++]=w;
                  if ((ise[w]=!ise[v])==true) ++nhe;
                  else ++nhv;
                  }
              else if (ise[w]==ise[v])
                  {bipartite=false;
                  break;
                  }
              b=cir[b];
              } while (b!=b0);
      }
  if (!bipartite || (top<n)) return false;
  Prop1<int> is_connected(Set(),PROP_CONNECTED);
  Prop1<int> is_bipartite(Set(),PROP_BIPARTITE);
  Prop1<int> is_hypergraph(Set(),PROP_HYPERGRAPH);
  Prop1<int> h_nv(Set(),PROP_NV); h_nv()=nhv;
  Prop1<int> h_ne(Set(),PROP_NE); h_ne()=nhe;
  Set(tvertex()).swload(PROP_HYPEREDGE,ise);
  return true;
  }


bool TopologicalGraph::CheckConnected()
  {if(debug())DebugPrintf("   CheckConnected");
  if(Set().exist(PROP_CONNECTED))return true;
  if(nv() < 2 || ne() < nv() - 1) return false;
  if(debug())DebugPrintf("Executing CheckConnected");
  tvertex v,w;
  tbrin b,b0;
  int ncc = 0;
  svector<tvertex> stack(1,nv()); stack.SetName("stack");
  svector<int> comp(1,nv()); comp.clear(); comp.SetName("Comp");
  Prop<bool> IsTree(Set(tedge()),PROP_ISTREE); IsTree.clear();
  int rank =0;
  int max_rank = 0;
  tvertex v0 = 1;

  while (max_rank < nv())
      {while (comp[v0]) v0++;
      comp[v0] = ++ncc;
      //if(debug() && ncc != 1)DebugPrintf("New comp at vertex %d",v0());
      if(ncc > 1)return false;
      ++max_rank;
      stack[rank + 1] = v0;

      while(rank < max_rank)
          {v = stack[++rank];
          b = b0 = pbrin[v];
          if (b0!=0)
              do
                  {w = vin[-b];
                  if(!comp[w])
                      {comp[w] = ncc;
                      //DebugPrintf("Added vertex %d to component %d",w(),ncc);
                      stack[++max_rank] = w;
                      IsTree[b.GetEdge()] = true;
                      }
                  }while((b = cir[b])!= b0);
          }
      }
  if(ncc == 1)
      {Prop1<int> isconnected(Set(),PROP_CONNECTED);
      return true;
      }
  return false;
  }
void TopologicalGraph::ZigZag(tbrin start)
  {tbrin b1,b2;
  tedge e;
  b2 = cir[-start];
  b1 = acir[start];
  if (b1 == -b2) return;    // face of length 2
  while (1)
      {b1 = acir[-b1];
      if (b1 == -b2) break;
      e=NewEdge(b1,b2);
      b2 = cir[-b2];
      if (b1 == -b2) break;
      e=NewEdge(b1,b2);
      }
  }

svector<tbrin> &TopologicalGraph::ComputeFpbrin()
  {tbrin depart,courant;
  int NumberOrbits;
  int m = ne();
  int n = nv();
  svector<tbrin> *Fpbrin = new svector<tbrin>(1,m-n+2);
  (*Fpbrin).SetName("Fpbrin");
  svector<char> met(-m,m); met.clear();
  met.SetName("met");

  /* Parcourt de toutes les faces */
  NumberOrbits = 0;met[0] = (char)1;

  for(depart = -m;depart <= m;depart++)
      {if(met[depart])continue;
//hub          for(courant = depart;!met[courant];(courant = cir[-courant])!=0)
      for(courant = depart;courant() && !met[courant];(courant = cir[-courant]))
          met[courant] = (char)1;
      if(courant == depart)
          (*Fpbrin)[++NumberOrbits] = depart;     //[] -> ()
      }
    
  genus =  ((m -n + 2) - NumberOrbits)/2;
  if(genus!=0) (*Fpbrin).resize(1,NumberOrbits);
  return *Fpbrin;
  }
int TopologicalGraph::LongestFace(tbrin& b,int& len)
  {if(!CheckConnected() || !CheckPlanar())return -1;
  svector<tbrin> & Fpbrin = ComputeFpbrin();
  len = 0;
  int l;
  for (int i=1; i <= Fpbrin.n(); i++)
      {l = FaceLength(Fpbrin[i]);
      if(l > len){b = Fpbrin[i];len = l;}
      }
  delete &Fpbrin;
  return 0;
  }
int TopologicalGraph::LongestFaceWalk(tbrin& b,int& len)
  {if(!CheckConnected() || !CheckPlanar())return -1;
  svector<tbrin> & Fpbrin = ComputeFpbrin();
  len = 0;
  int l;
  for (int i=1; i <= Fpbrin.n(); i++)
      {l = FaceWalkLength(Fpbrin[i]);
      if(l > len){b = Fpbrin[i];len = l;}
      }
  delete &Fpbrin;
  return 0;
  }
void TopologicalGraph::ZigZag()
  {svector<tbrin> & Fpbrin = ComputeFpbrin();
  for (int i=1; i <= Fpbrin.n(); i++)
      ZigZag(Fpbrin[i]);
  delete &Fpbrin;
  }
int TopologicalGraph::ZigZagTriangulate()
  {if(nv() < 3)return -1;
  if(!CheckSimple())return -1;
  int Orgm = ne();
  MakeConnected();
  if(!CheckPlanar()) return -1;
  if(ne() == 3*nv() - 6)return 0;
  if(debug())DebugPrintf("Executing ZigZagTriangulate");
  bool simple = Set().exist(PROP_SIMPLE);

  ZigZag();

  // Switch Loops
  tedge e;
  for(e = 1; e <= ne(); e++)
      if (vin[e]==vin[-e])Rotate(e);

  // Switch Multiple Edges
  svector<tedge>link(1,ne()); link.clear();link.SetName("link");
  svector<tedge>top1(1,nv()); top1.clear();top1.SetName("top1");
  svector<tedge>top2(1,nv()); top2.clear();top2.SetName("top2");
  tvertex u,v,w;
  tedge next;
  //First sort with respect to biggest label
  for(e = ne();e >= 1;e--)
      {v = (vin[e] < vin[-e]) ? vin[-e] : vin[e];
      link[e] = top1[v];top1[v] = e;
      }
  //Then sort with respect to smallest label
  for(u = nv();u > 1;u--)
      {e = top1[u];
      while(e!=0)
          {next = link[e];     //as link is modified
          v = (vin[e] < vin[-e]) ? vin[e] : vin[-e];
          link[e] = top2[v]; top2[v] = e;
          e = next;
          }
      }
  // Switch Multiple edges
  for(v = nv()-1;v >= 1;v--)
      {e = top2[v];
      while(e!=0)
          {next = link[e];
          u = vin[e];if(u == v)u = vin[-e];
          w = vin[next];if(w == v)w = vin[-next];
          if(u == w)
              link[e] = 1;
          else
              link[e] = 0;
          e = next;
          }
      }
  for(e = ne();e > Orgm;e--)
      if(link[e] != 0)Rotate(e);
  if(ne() != 3*nv() - 6)return -2;
  if(simple)Prop1<int> simple(Set(),PROP_SIMPLE);
  return 0;
  }
tedge TopologicalGraph::IdentifyEdge(tvertex &v1,tvertex &v2)
  {tedge e = 0;
  if(v1 > nv() || v2 > nv())return e;
  tbrin b0 = pbrin[v1];
  if(b0 == 0) return e;
  tbrin b = b0;
  do
      if(vin[-b] == v2){e = b.GetEdge();break;}
  while((b = cir[b]) != b0);
  return e;
  }
int TopologicalGraph::VertexQuadrangulate()
  {if(debug())DebugPrintf("VertexQuadrangulate");
  if(!CheckSimple())return -1;
  if(!CheckBipartite(true))return -1;  
  if(!CheckPlanar())return -1;
  if(!CheckConnected())
      {MakeConnectedVertex();
      if(!CheckBipartite(true))return -1; 
      }
  if(debug())DebugPrintf("Executing VertexQuadrangulate");
  int n0 = nv();
  // We add Red vertices
  _VertexQuadrangulate(true);
  // The graph was not two connected, we add blue vertices
  if( ne() != 2*nv() - 4 || !CheckBiconnected())
      _VertexQuadrangulate(false);
 
  tvertex v;
  if(ne() != 2*nv() - 4 && !debug())
      {for(v = nv(); v > n0;v--)
          DeleteVertex(v);
      return -2;    
      }
  return 0;
  }
 
int TopologicalGraph::_VertexQuadrangulate(bool First) 
  {svector<tbrin> & Fpbrin = ComputeFpbrin();
  Prop<short> vcolor(Set(tvertex()),PROP_COLOR);
  svector<tbrin> vertexbrin(1,nv()); 
  vertexbrin.SetName("TP:vertexbrin");
  vertexbrin.clear();
  //vertexbrin is used not to add multiple edges
  tvertex v,v0;
  tbrin nb,b0,b,mark;
  v0 = nv();
  int m0 = ne();
  int col1,col2;
  if(First)
      {col1 = Blue; col2 = Red;}
  else
      {col1 = Red; col2 = Blue;}
  
  for (int  i=1; i <= Fpbrin.n(); i++)
      {b0 = Fpbrin[i];
      if(FaceLength(b0) == 4 && FaceWalkLength(b0) == 4)continue;
      if(vcolor[vin(b0)] != col1)b0 = cir[-b0];
      mark = b = b0;
      vertexbrin[vin[b0]] = mark;
      // Creating a new vertex and an edge incident to it
      v = NewVertex();   vcolor[v] = col2;
      incsize(tedge()); nb = ne();
      // New edge  before b0 
      acir[nb] = acir[b0]; acir[b0] = cir[acir[nb]] = nb; cir[nb] = b0;
      vin[nb] = vin[b0];

      
      nb = -nb;
      vin[nb] = v; pbrin[v] = nb; cir[nb] = nb; acir[nb] = nb;
      // Quadrangulate the face
      b0 = -nb;       // to end the face properly
      // New edge before  nb,before b
      while((b = cir[-cir[-b]]) != b0)
          {if(vertexbrin[vin[b]] == mark)continue;
          vertexbrin[vin[b]] = mark;
          incsize(tedge());
          tbrin bb = ne();
          acir[bb]  = acir[nb]; acir[nb] = cir[acir[bb]] = bb; cir[bb] = nb;
          acir[-bb] = acir[b]; acir[b] = cir[acir[-bb]] = -bb; cir[-bb] = b;
          vin[bb] = vin[nb];    vin[-bb] = vin[b];
          nb = bb;    // for next insertion
          }
      }
  delete &Fpbrin;
  // Geometric graph
  if(Set(tvertex()).exist(PROP_COORD)) // Geometric Graph
      {Prop<Tpoint> vcoord(Set(tvertex()),PROP_COORD);
      int deg; 
      tvertex w;
      for(v = v0 + 1;v <= nv();v++)
          {Tpoint p(.0,.0);
          deg = 0;
          Forall_adj_brins_of_G(b,v)
              {w = vin[-b]; ++deg;
              p += vcoord[w];
              }
          vcoord[v] = p/(double)deg;
          }
      }
  if(Set(tvertex()).exist(PROP_LABEL)) // Geometric Graph
      {Prop<int> vlabel(Set(tvertex()),PROP_LABEL);
      for(int i = v0() + 1;i <= nv();i++)vlabel[i] = i;
      }
  if(Set(tedge()).exist(PROP_LABEL)) // Geometric Graph
      {Prop<int> elabel(Set(tedge()),PROP_LABEL);
      for(int i  = m0 + 1;i <= ne();i++)elabel[i] = i;
      }
  return 0;
  }
int TopologicalGraph::VertexTriangulate()
// Precondition: simple plane 2-connected  graph
  {if(debug())DebugPrintf("VertexTriangulate");
  if(!CheckSimple())return -1;
  tvertex v,v00 = nv();
  MakeConnectedVertex();
  if(!CheckPlanar())
      {for(v = nv(); v > v00;v--) DeleteVertex(v);
      return -1;
      }
  if(!CheckBiconnected())
      {NpBiconnectVertex();// add vertices
      CheckPlanar();// we need a planar map
      }
  if(debug())DebugPrintf("Executing VertexTriangulate");
  svector<tbrin> & Fpbrin = ComputeFpbrin();
  tbrin nb,b0,b;
  int len;
  tvertex v0 = nv();
  int m0 = ne();
  for (int  i=1; i <= Fpbrin.n(); i++)
      {b = b0 = Fpbrin[i];
      len = FaceWalkLength(b0);
      if(len ==  2) {DebugPrintf("Vertextriangulate:not simple graph ?");return -2;}
      if(len ==  3)continue;
      // Creating a new vertex and an edge incident to it
      v = NewVertex();
      incsize(tedge()); nb = ne();  
      acir[nb] = acir[b0]; acir[b0] = cir[acir[nb]] = nb; cir[nb] = b0;
      vin[nb] = vin[b0];
      nb = -nb; 
      vin[nb] = v; pbrin[v] = nb; cir[nb] = nb; acir[nb] = nb;
      // Triangulate the face
      b0 = -nb;   
      while((b = cir[-b]) != b0) 
          {incsize(tedge());
          tbrin bb = ne();
          acir[bb]  = acir[nb]; acir[nb] = cir[acir[bb]] = bb; cir[bb] = nb;
          acir[-bb] = acir[b]; acir[b] = cir[acir[-bb]] = -bb; cir[-bb] = b;
          vin[bb] = vin[nb];    vin[-bb] = vin[b];
          nb = bb;    // for next insertion
          }
      }
  delete &Fpbrin;
  if(ne() != 3*nv() - 6)
      {DebugPrintf("Error 3:Vertextriangulate n=%d m=%d",ne(),nv());
      setPigaleError(A_ERRORS_VTRIANGULATE);
      for(v = nv(); v > v00;v--)DeleteVertex(v);
      return -3;    
      }
  if(Set(tvertex()).exist(PROP_COORD)) // Geometric Graph
      {Prop<Tpoint> vcoord(Set(tvertex()),PROP_COORD);
      int deg; 
      tvertex w;
      for(v = v0 + 1;v <= nv();v++)
          {Tpoint p(.0,.0);
          deg = 0;
          Forall_adj_brins_of_G(b,v)
              {w = vin[-b]; ++deg;
              p += vcoord[w];
              }
          vcoord[v] = p/(double)deg;
          }
      }
  if(Set(tvertex()).exist(PROP_LABEL)) // Geometric Graph
      {Prop<int> vlabel(Set(tvertex()),PROP_LABEL);
      for(int i = v0() + 1;i <= nv();i++)vlabel[i] = i;
      }
  if(Set(tedge()).exist(PROP_LABEL)) // Geometric Graph
      {Prop<int> elabel(Set(tedge()),PROP_LABEL);
      for(int i  = m0 + 1;i <= ne();i++)elabel[i] = i;
      }
  return 0;
  }

bool TopologicalGraph::CheckAcyclic(int &ns, int &nt)
{if(ne() <= 1){ns = nt = 0;return true;}
  if(debug())DebugPrintf("Executing CheckAcyclic");
  ns=nt=0;
  svector<int> din(1,nv());             din.SetName("Acyclic:din");
  svector<tvertex> stack(1,nv());       stack.SetName("Acyclic:stack");
  int num=0;
  tbrin b,b0;
  tvertex v;
  int stackpos=0;

  // compute degrees
  din.clear();
  for (b=-ne(); b<0;b++)
      din[vin[b]]++;
  // first pushs
  for (v=1; v<=nv();v++)
      if (din[v]==0) 
          {stack[++stackpos]=v;
          ns++;
          ++num;
          }
  while (stackpos!=0)
      {bool sink=true; 
      v=stack[stackpos--];
      b0 = b = pbrin[v];
      do 
          {if (b>0)
              {sink=false;
              if (--din[vin[-b]]==0) 
                  {stack[++stackpos]=vin[-b];
                  ++num;
                  }
              }
          b = cir[b];
          } while(b!=b0);
      if (sink) nt++;
      }
  if(debug())DebugPrintf("END CheckAcyclic");
  return (num==nv());
  }

// tries to extend the orientation of a partially oriented graph in order to make
// it acyclic
bool ExtendAcyclic(TopologicalGraph &G, int &ns, int &nt)
  {if(debug())DebugPrintf("Executing ExtendAcyclic");
  ns=nt=0;
  int n=G.nv();
  int m=G.ne();
  if (m==0) return true;
  svector<int> din(1,n);                     din.SetName("TG:Extend:din");
  svector<int> d0(1,n);                      d0.SetName("TG:Extend:d0");
  svector<tvertex> stack(1,n);               stack.SetName("TG:Extend:stack");
  svector<tvertex> other_stack(1,n);         other_stack.SetName("TG:Extend:other_stack");
  svector<bool> done(1,n); done.clear();     done.SetName("TG:Extend:done");
  Prop<bool> oriented(G.Set(tedge()),PROP_ORIENTED);
  Prop<bool> reoriented(G.Set(tedge()),PROP_REORIENTED); reoriented.clear();

  int num=0;
  tbrin b,b0;
  tvertex v;
  int stackpos=0;
  int other_stackpos=0;
  
  // compute degrees
  din.clear();
  d0.clear();
  
  for (b=-m; b<0;b++)
      if (oriented[b.GetEdge()])
          din[G.vin[b]]++;
      else
          {d0[G.vin[b]]++;
          d0[G.vin[-b]]++;
          }
  // first pushs
  for (v=1; v<=n;v++)
      if (din[v]==0) 
          {if (d0[v]==0)
              {stack[++stackpos]=v;
              ns++;
              ++num;
              }
          else
              other_stack[++other_stackpos]=v;
          }
  while (stackpos!=0 || other_stackpos!=0)
      {bool sink=true;
      if (stackpos!=0)
          v=stack[stackpos--];
      else
          {v=other_stack[other_stackpos--];
          if (done[v]) continue;
          done[v]=true;
          ++num;
          }
      b0 = b = G.pbrin[v];
      do 
          {if (!oriented[b.GetEdge()])
              {if (b.in())
                  reoriented[b.GetEdge()]=true;
              oriented[b.GetEdge()]=true;
              if (--d0[G.vin[-b]])
                  {stack[++stackpos]=G.vin[-b];
                  ++num;
                  }
              }
          if (b.out())
              {sink=false;
              if (--din[G.vin[-b]]==0) 
                  {stack[++stackpos]=G.vin[-b];
                  ++num;
                  }
              }
          b = G.cir[b];
          } while(b!=b0);
      if (sink) nt++;
      }
  G.RestoreOrientation();
  return (num==n);
  }
bool TopologicalGraph::TopSort(svector<tbrin> &topin, svector<tvertex> &order, bool revert)
// topsort does a topological sort.
// It is intended for acyclic digraphs.
// It computes a "dependence tree" -> topin=last incoming brin.
  {if(debug())DebugPrintf("TopSort");
  svector<int> din(1,nv());        din.SetName("TG:TopSort:din");
  svector<tvertex> queue(1,nv());  queue.SetName("TG:TopSort:queue");
  int qstart=1; int qend=1;
  int num=0;
  tbrin b,b0;
  tvertex v;
  // compute degrees
  din.clear();
  for (b=-ne(); b<0;b++)
      din[vin[revert?-b:b]]++;
  // first pushs
  for (v=1; v<=nv();v++)
      if (din[v]==0) 
          {queue[qend++]=v;
          topin[v]=0;
	  order[num++]=v;
          }
  while (qstart<qend)
      { v=queue[qstart++];
      b0 = b = pbrin[v];
      do 
          {if ((revert?-b:b)>0 && --din[vin[-b]]==0) 
              {queue[qend++]=vin[-b];
              topin[vin[-b]]=-b;
	      order[num++]=vin[-b];
              }
          b = cir[b];
          } while(b!=b0);
      }
  return (num==nv());
  }

bool TopologicalGraph::CheckNoC3Sep()
  {// perequisit : G planaire 2connexe simple and >= 6 vertices
  if(debug())DebugPrintf("   CheckNoC3Sep");
  if (nv() < 6) return false;
  if(!CheckPlanar() || !CheckBiconnected() || !CheckSimple())
      {DebugPrintf("Not planar or not Bicconnected or not simple");setPigaleError(A_ERRORS_BAD_INPUT);
      return false;
      }
  if(debug())DebugPrintf("ExecutingCheckNoC3Sep");
  GraphContainer GC(Container());
  TopologicalGraph G0(GC);
  if(G0.VertexTriangulate() != 0){setPigaleError(A_ERRORS_VTRIANGULATE);return false;}
  G0.SchnyderOrient(1);
  Prop<bool> erase(G0.Set(tvertex()),PROP_TMP);
  erase.clear();
  tbrin b=1;
  tvertex v;
  bool res;
  do
      {erase[G0.vin[b]]=true;
      b = -G0.cir[b];
      } while (b!=1);
  for (v=G0.nv(); v>0; v--)
      if (erase[v]) G0.DeleteVertex(v);
  if(G0.nv() <= 1) return false;
  GraphContainer &DGC = *G0.DualGraph();
  {TopologicalGraph DG(DGC);
  int ns,nt;
  res = DG.CheckAcyclic(ns,nt);
  }
  delete &DGC;
  return res;
  }
bool TopologicalGraph::CheckSubdivTriconnected()
  {//prerequisit : G plane
  if(debug())DebugPrintf("   CheckSubDivTriconnected");
  if(Set().exist(PROP_TRICONNECTED))return true;
  if (nv() < 4 || ne() <= nv()) return false;
  if(!CheckConnected())return false;
  if(!FindPlanarMap())return false;
  if(debug())DebugPrintf("Executing SubDivTriconnected");
  GraphContainer *pAGC;
  GraphContainer *pDAGC=0;
  int nsinks;
  bool res;
  svector<bool> save_oriented(0,ne());
  Prop<bool> oriented(Set(tedge()),PROP_ORIENTED,false);
  save_oriented.Tswap(oriented);
  tbrin first = 1;
  if(PseudoBipolarPlan(first,nsinks)) // {s,t} edge 1
      {setPigaleError(-1,"PseudoBipolar");save_oriented.Tswap(oriented);RestoreOrientation();return false;}  
  // If only 1 sink and vin[-1] is a sink  => G is biconnected
  if (nsinks > 1)
      {save_oriented.Tswap(oriented);RestoreOrientation(); return false;}
  // Check that vin[-1] is a sink

  tbrin b = -1;
  bool sink = true;
  do
      if(b() > 0){sink = false;break;}
  while((b = cir[b]) != -1);
  if(!sink)
      {save_oriented.Tswap(oriented);RestoreOrientation(); return false;}

  // Now we know that G is plane and biconnected
  Prop1<int> isbicon(Set(),PROP_BICONNECTED);
  {pAGC = AngleGraph();
  TopologicalGraph AG(*pAGC);

  // Erase  original vertices of degree two
  Prop<bool> OriginalVertex(AG.Set(tvertex()),PROP_MARK);
  tvertex v;
  for (v=AG.nv(); v>0; v--)
      if((OriginalVertex[v] && AG.Degree(v) == 2))
          AG.DeleteVertex(v);

  // Compute InDegrees
  svector<int> InDegrees(1,AG.nv());   InDegrees.SetName("TG:3conSub:InDegree");
  for(v = 1; v <= AG.nv();v++)InDegrees[v] = AG.InDegree(v);
    
  // Find the face lacking 4 incoming edges 
  svector<tbrin> & Fpbrin = AG.ComputeFpbrin();
  tbrin b,b0;
  int SumInDegrees,i;
  for (i = 1; i <= Fpbrin.n(); i++)
      {b = b0 = Fpbrin[i];
      SumInDegrees = 0;
      do
          {SumInDegrees += 2-InDegrees[AG.vin[b]];
          }while((b = AG.cir[-b]) != b0);
      if(SumInDegrees == 4)break;      
      }
  if(i > Fpbrin.n()){setPigaleError(A_ERRORS_SUBDIVTRICON,"subdiv3con");i=1;}
  b0 = Fpbrin[i];
  delete &Fpbrin;
    
  // Erase the 4 vertices of the face with 2 sources
  Prop<bool> erase(AG.Set(tvertex()),PROP_TMP);    erase.clear();
  b = b0;
  do
      {erase[AG.vin[b]] = true;
      }while((b = AG.cir[-b]) != b0);

  for (v=AG.nv(); v>0; v--)
      if(erase[v])AG.DeleteVertex(v);
  AG.RemoveIsolatedVertices();
  if(AG.CheckConnected())
      {GraphContainer *pDAGC = AG.DualGraph();
      TopologicalGraph DAG(*pDAGC);
      int ns,nt;
      res = DAG.CheckAcyclic(ns,nt);
      }
  else
      res = false;
  }
  delete pAGC;delete pDAGC;
  save_oriented.Tswap(oriented);
  RestoreOrientation();
  return res;
  }

struct _tricon_angle
{
  TopologicalGraph & G;
  svector<char> dedge;         
  svector<bool> angle_mark;     
  svector<tedge> elist;         
  tedge elist_bot,elist_top;

  _tricon_angle(TopologicalGraph & GG) 
      : G(GG), dedge(1,G.ne()), angle_mark(-G.ne(),G.ne()),elist(0,G.ne()) 
      {}
  ~_tricon_angle() {}

  void Init()
      {char tmp;
      tbrin b;
      tedge e;
      dedge.SetName("TG:dedge");
      angle_mark.SetName("TG:angle_mark");
      elist.SetName("TG:elist");
      angle_mark.clear();
      elist_bot=elist_top=0;
      for (b=1; b<=G.ne(); b++)
          {tmp=0;
          if (G.cir[b]<0) tmp++;
          if (G.acir[b]>0) tmp++;
          if (G.cir[-b]>0) tmp++;
          if (G.acir[-b]<0) tmp++;
          dedge[b] = tmp;
          if (tmp==0)
              {e=b.GetEdge();
              elist[elist_top++]=e;
              MarkEdge(e);
              }
          }
      }
  void MarkEdge(tedge e)
      {tbrin b=e.firsttbrin();
      angle_mark[b]=true;
      angle_mark[G.cir[b]]=true;
      angle_mark[-b]=true;
      angle_mark[G.cir[-b]]=true;
      }
  void DecreaseEdge(tedge e)
      {if (--dedge[e]==0)
          { MarkEdge(e);
          elist[elist_top++]=e;
          }
      }
  void DecreaseNeighbors(tedge e)
      {tbrin b=e.firsttbrin();
      DecreaseEdge(G.cir[b].GetEdge());
      DecreaseEdge(G.cir[-b].GetEdge());
      DecreaseEdge(G.acir[b].GetEdge());
      DecreaseEdge(G.acir[-b].GetEdge());
      }
  void MarkInvertibleAngles()
      {Init();
      while(elist_bot != elist_top)
          DecreaseNeighbors(elist[elist_bot++]);
      }
  void MarkIncidentVertexAngles(tbrin b)
      {tbrin b0=b;
      do
          {angle_mark[b]=true;
          b=G.cir[b];
          } while (b!=b0);
      }
  void MarkIncidentFaceAngles(tbrin b)
      {tbrin b0=b;
      do
          {angle_mark[b]=true;
          b=G.cir[-b];
          } while (b!=b0);
      }
  bool AllMarked()
      {bool res=true;
      angle_mark[0]=true;
      for (tbrin b=-G.ne(); b<=G.ne(); b++)
          if (!angle_mark[b])
              {res=false;
              // Tprintf("NOT 3-Connected;angle %d not marked",b());
              break;
              }
      return res;
      }
  bool DoIt(tbrin st_brin)
      {MarkInvertibleAngles();
      MarkIncidentVertexAngles(st_brin);
      MarkIncidentVertexAngles(-st_brin);
      MarkIncidentFaceAngles(st_brin);
      MarkIncidentFaceAngles(-st_brin);
      return AllMarked();
      }
};

bool TopologicalGraph::CheckTriconnected()
// if not planar returns false
  {if(debug())DebugPrintf("   CheckTriconnected");
  if(Set().exist(PROP_TRICONNECTED))return true;
  if(!CheckConnected())return false;
  if (nv()<=3 || (ne()<=nv()+2 && nv() !=4)) return false;
  if(!FindPlanarMap())return false;
  if(debug())DebugPrintf("Executing CheckTriconnected");
  int nsinks;
  svector<bool> save_oriented(0,ne());   save_oriented.SetName("TG:save_oriented");
  Prop<bool> oriented(Set(tedge()),PROP_ORIENTED,false);
  save_oriented.Tswap(oriented);
  tbrin first = 1;
  if(PseudoBipolarPlan(first,nsinks)) // {s,t} edge 1
      {setPigaleError(A_ERRORS_BIPOLAR_PSEUDO_PLAN);
      save_oriented.Tswap(oriented);RestoreOrientation();return false;
      }  
  // If only 1 sink and vin[-1] is a sink  => G is biconnected
  if(nsinks > 1){save_oriented.Tswap(oriented);RestoreOrientation();return false;}
  // Check that vin[-1] is a sink
  tbrin b = -1;
  bool sink = true;
  do
      if(b() > 0){sink = false;break;}
  while((b = cir[b]) != -1);

  if(!sink){save_oriented.Tswap(oriented); RestoreOrientation();return false;}

  // G is plane and biconnected
  Prop1<int> isbicon(Set(),PROP_BICONNECTED);

  // Bipolar orientation has to be the minimal one !
  _tricon_angle TriconCheck(*this);
  bool res=TriconCheck.DoIt(first);
    
  if(res)Prop1<int> istricon(Set(),PROP_TRICONNECTED);
  save_oriented.Tswap(oriented); RestoreOrientation();
  if(debug())DebugPrintf("END CheckTriconnected:%d",res);
  return res;
  }

bool TopologicalGraph::CheckSerieParallel()
  {//prerequisit : G  planaire
  if(!FindPlanarMap())return false;
  if(debug())DebugPrintf("Executing CheckSerieParallel");
  svector<bool> save_oriented(0,ne()); save_oriented.SetName("SerieP:save_oriented");
  Prop<bool> oriented(Set(tedge()),PROP_ORIENTED,false);
  oriented.SetName("Serie //:Orient");
  save_oriented.Tswap(oriented);
  tbrin first = 1;
  int nsinks;

  if(PseudoBipolarPlan(first,nsinks)) // {s,t} edge 1
      {setPigaleError(-1);save_oriented.Tswap(oriented); RestoreOrientation();return false;}  
  // If only 1 sink and vin[-1] is a sink  => G is biconnected
  if (nsinks > 1)
      {save_oriented.Tswap(oriented); RestoreOrientation(); return false;}
  // Check that vin[-1] is a sink
  tbrin b = -1;
  bool sink = true;
  do
      if(b() > 0){sink = false;break;}
  while((b = cir[b]) != -1);
  if(!sink)
      {save_oriented.Tswap(oriented); RestoreOrientation(); return false;}

  // G is plane and biconnected
  Prop1<int> isbicon(Set(),PROP_BICONNECTED);
  bool res;
  GraphContainer &DGC = *AngleGraph();
  {TopologicalGraph DG(DGC);
  int ns,nt;
  if(debug())DebugPrintf("Executing CheckSerieParallel:CheckAcyclic");
  res = DG.CheckAcyclic(ns,nt);
  }

  delete &DGC;
  save_oriented.Tswap(oriented); RestoreOrientation();
  if(debug())DebugPrintf("END CheckSerieParallel");
  return res;
  }

GraphContainer * TopologicalGraph::AngleGraph()
  { if(debug())DebugPrintf("Executing AngleGraph");
  if(!CheckConnected() || !FindPlanarMap())
      {DebugPrintf("Could not compute angle graph");
      setPigaleError(-1,"Could not compute angle graph");
      return (GraphContainer *)0;
      }
// #ifdef TDEBUG
//   if(ComputeGenus() != 0)
//       {DebugPrintf("Not planar map !");
//       setPigaleError(-1,"Not planar map !");
//       return (GraphContainer *)0;
//       }
// #endif
  int m = ne();
  int n = nv();
  int nn = m + 2; // No exterior face
  int mm = 2*m; //   - LenExtFace;
  if(debug())DebugPrintf("AngleGraph: n:%d m:%d",nn,mm);
  GraphContainer & Angle = *new GraphContainer;
  Angle.setsize(nn,mm);
  Prop1<tstring> title(Set(),PROP_TITRE);
  Prop1<tstring> titleD(Angle.Set(),PROP_TITRE);
  titleD() = "A-" + title();
  Prop1<tbrin> dextbrin(Angle.Set(),PROP_EXTBRIN);
  dextbrin()=1;
  tvertex u,v;
  tbrin b,bb;
  Prop<tvertex> dvin(Angle.PB(),PROP_VIN); dvin.SetName("angle:vin");
  dvin[0]=0;
  Prop<tbrin> dcir(Angle.PB(),PROP_CIR);dcir.SetName("angle:cir");
  Prop<tbrin> dacir(Angle.PB(),PROP_ACIR);dacir.SetName("angle:cir");
  dcir[0]=0;
  Prop<tbrin> dpbrin(Angle.PV(),PROP_PBRIN);dpbrin.SetName("angle:pbrin");
  dpbrin[0]=0;
  Prop<bool> OriginalVertex(Angle.Set(tvertex()),PROP_MARK);
  OriginalVertex.clear();
  for(v = 1;v <= n;v++) OriginalVertex[v] = true;
  svector<int> mark(-m,m);  mark.clear();   mark.SetName("Mark");mark.SetName("angle:mark");
  tvertex nf = n;
  tedge newe;
  svector<int> dnum(-m,m); dnum.SetName("angle:dnum");
  
  // renum brins (including orientation for Angle graph)
  for (b=-m; b<0 ; b++)
      if (cir[b]<0)
         dnum[b]=-2*b()-1;
      else
         dnum[b]=2*b()+1;
  dnum[0]=0;
  for (b=0; b<=m ; b++)
      if (cir[b]>0)
         dnum[b]=2*b();
      else
         dnum[b]=-2*b();
              
  // cir and vin around original vertices
  for (b=-m; b<=m; b++)
      {
      dcir[dnum[b]]=dnum[cir[b]];
      dvin[dnum[b]]= vin[b];
      }
  // pbrins for original vertices
  for (v=1; v<=n; v++)
      dpbrin[v]=dnum[pbrin[v]];
      
  // cir, vin and pbrin around the faces
 mark[0] = 1;
  tbrin newb;
  for (b = -m; b <= m; b++)
      {if(mark[b]) continue;
      bb = b;
      dpbrin[++nf] = -dnum[b];
      do
          {mark[bb] = 1;
          dvin[-dnum[bb]] = nf;
          newb = acir[-bb];
          dcir[-dnum[bb]] = -dnum[newb];
          bb = newb;
          } while (bb!=b);
      }
  for (b=-mm; b<=mm; b++) dacir[dcir[b]]=b;
  if(debug())DebugPrintf("END Computing Angle Graph");
  return &Angle;
  }
 
GraphContainer * TopologicalGraph::DualGraph()
  {if(debug())DebugPrintf("Executing DualGraph");
  RemoveIsolatedVertices();
  int m = ne();
  int n = nv();
  if(!CheckConnected() || !FindPlanarMap())
      {DebugPrintf("Error Computing the dual:no planar map");
      setPigaleError(-1,"Could not compute angle graph");
      return (GraphContainer *)0;
      }
  GraphContainer & Dual = *new GraphContainer;
  Dual.setsize(m-n+2,m);
  Prop1<tstring> title(Set(),PROP_TITRE);
  Prop1<tstring> titleD(Dual.Set(),PROP_TITRE);
  titleD() = "D-" + title();

  tvertex u,v;
  tbrin b,bb;
  Prop<tvertex> dvin(Dual.PB(),PROP_VIN);    dvin.clear(); dvin.SetName("dual:dvin");
  tvertex nf = 0;
  for (b = -m; b <= m; b++)
      {if(dvin[b]() || !b()) continue;
      bb = b;
      ++nf;
      do
          {dvin[bb] = nf;
          v = vin[bb];
          bb = -cir[bb];
          } while(bb != b);
      }

  if (nf != m-n+2) 
      {DebugPrintf("m=%d n=%d",m,n);
      setPigaleError(A_ERRORS_DUAL);
      DebugPrintf("Error Computing the dual: nf != m-n+2 %d!=%d",nf(),m-n+2);
      delete &Dual;return (GraphContainer *)0;
      }
  if(debug())DebugPrintf("END DualGraph");
  return &Dual;
  }

// Sorts the circular order of the brins in increasing order of
// colore. Colors are assumed to be small integers from 0 to ncolore.
// Circular order of parallel edges with the same color is preserved.
// The new pbrin is the first brin met from pbrin in cir order having
// the smallest color.

void SortCir(TopologicalGraph &G, svector<tbrin> &ncir, svector<tbrin>
	     &npbrin, svector<int> &colore, int ncolore)
  {if(debug())DebugPrintf("Executing SortCir");
  // Color sort of cir -> ncir
  tvertex v,w;
  tbrin b,b0,bb;
  int ec;
  int n=G.nv();
  svector<tbrin> top(0,ncolore); top.clear();   top.SetName("TG:SortCir:top");
  ncir.clear();
  for (v=n; v>0; --v)
    {b=b0=G.acir[G.pbrin[v]]; // will end with pbrin
    do
      {ec = colore[b.GetEdge()];
      ncir[b]=top[ec];
      top[ec]=b;
      } while ((b=G.acir[b])!=b0); // use acir !
    }
  npbrin.clear();
  // here, npbrin is the last brin
  for (ec=ncolore; ec>=0; --ec)
    {b0=b=top[ec];
    v=G.vin[b];
    do
      {bb=b;
      w=v;
      b=ncir[b];
      v=G.vin[b];
      if (v!=w)
          { // Interval from b0 to bb to insert at w
          if (npbrin[w]!=0)
              {ncir[bb]=ncir[npbrin[w]];
              ncir[npbrin[w]]=b0;
              }
          else
              ncir[bb]=b0;
          npbrin[w]=bb;
          b0=b;
          }
      } while (v!=0);
    }
  // readjust npbrin
  for (v=1; v<=n; v++)
    npbrin[v]=ncir[npbrin[v]];
  }
void ConnectToRoot(TopologicalGraph &G, tvertex root)
  {if(debug())DebugPrintf("   ConnecteToRoot %d",root());
  if(G.Set().exist(PROP_CONNECTED))return;
  Prop1<int>is_connected(G.Set(),PROP_CONNECTED);
  if(!G.nv()) return;
  if(debug())DebugPrintf("Executing ConnecteToRoot");
  tvertex v,w;
  tbrin b,b0;
  int ncc = 0;
  svector<tvertex> stack(1,G.nv()); stack.SetName("stack");
  svector<tvertex> comp(0,G.nv()); comp.clear(); comp.SetName("Comp");
  int rank =0;
  int max_rank = 0;
  tvertex v0 = root;

  while (max_rank < G.nv())
      {while (comp[v0]!=0) v0++;
      comp[v0] = root;
      ++ncc;
      if(ncc > 1) G.NewEdge(root,v0); 
      ++max_rank;
      stack[rank + 1] = v0;

      while(rank < max_rank)
          {v = stack[++rank];
          b = b0 = G.pbrin[v];
          if (b0!=0)
              do {w = G.vin[-b];
              if(!comp[w])
                  {comp[w] = root;
                  stack[++max_rank] = w;
                  }
              }while((b = G.cir[b])!= b0);
          }
      }
  }
