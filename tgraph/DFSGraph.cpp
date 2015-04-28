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
#include <TAXI/DFSLow.h>
#include <TAXI/Tdebug.h>

#include <TAXI/graphs.h>

int DFSGraph::DoDFS(tbrin b0)
  {if(debug())DebugPrintf("Executing DFSGraph:DoDFS");
  //cout<<"DoDFS nv:"<<nv()<<" ne:"<<ne()<<" v:"<<Gvin[b0]<< " "<<Gvin[-b0]<<endl;
  if(ne() < 1)return 0;  
  Prop<tbrin> cir(G.Set(tbrin()),PROP_CIR);
  Prop<tbrin> acir(G.Set(tbrin()),PROP_ACIR);
 /* 
  TopologicalGraph TG(G);
  if(!TG.DebugCir())
  	{cout<<"BAD CIR"<<endl;myabort();}
  else cout<<"CIR OK"<<endl;
  */
  // compute nvin, uptree
  // modify cir to link b0 to 0
  cir[0] = b0; acir[0] = acir[b0]; cir[acir[b0]] = 0;
  svector<tbrin> tb(0,nv()); tb.clear(); tb.SetName("tb:DoDFS");
  svector<int> dfsnum(0,nv());dfsnum.SetName("dfsnum:DoDFS");
  svector<tedge> tout(0,nv()); tout.SetName("tout:DoDFS");
  svector<tedge> ue(0,ne()); ue.SetName("ue:DoDFS");
  nvin[0] = 0;
  _ib[0] = 0; //initial brin value
  tbrin b = b0;
  tedge y = 1;
  tedge z = ne();
  tvertex v = Gvin[b0];
  tb[v] = b0;
  dfsnum[v] = 1;
  tvertex w;
  do
      {w = Gvin[-b];
      if (tb[w]!=0)            // w previously reached
          {if(b == tb[v])    // backtrak on tree
              {b.cross();
              v = w;
              }
          else if(dfsnum[v] < dfsnum[w]) // cotree low (secon time met)
              {nvin[z.firsttbrin()] = dfsnum[v];
              nvin[z.secondtbrin()] = dfsnum[w];
              _ib[z] = b;
              if(z<nv())
                 {cout<<"z<nv b:"<<b()<< " v:"<<dfsnum[v]<< " w:"<<dfsnum[w]<<endl;
                 myabort();
                 }
              uptree[z] = ue[b.GetEdge()];
              z--;
              }
          else // cotree high (first time discover)
	          ue[b.GetEdge()] = tout[w]; // lowest tree edge in fondamental circuit
          }
      else   // w has not yet been reached: define new tree edge
          {if (w==0) break;
          _ib[y] = b;
          b.cross();
          tb[w] = b;
          tout[v] = y(); // last outgoing tree edge from v
          nvin[y.firsttbrin()] = dfsnum[v];
          y = nvin[y.secondtbrin()]() = dfsnum[w] = y()+1;
          v = w;
          }
      b = cir[b];
      } while(1);
  // restore cir: remove 0 
  cir[0] = 0; cir[acir[0]] = b0; acir[0] = 0;
  if(y != nv()) 
      {cout<<"DFSGraph::DoDFS: "<<y()<<" ERROR!= "<<nv()<<endl;return 1;}
  Prop1<int> connected(G.Set(),PROP_CONNECTED);
  if(debug())DebugPrintf("    END DFSGraph:DoDFS");
  //cout<<"END DoDFS"<<endl;
  return 0;
  }

int DFSGraph::bicon()
// cotree edges are oriented from the root !!
  {if(debug())DebugPrintf("Executing DFSGraph:bicon");
  Prop<tvertex> low(Set(tvertex()),PROP_LOW);low.SetName("DFSGraph:low");
  Prop<tedge> elow(Set(tvertex()),PROP_ELOW);elow.SetName("DFSGraph:elow");
  Prop<int> status(Set(tvertex()),PROP_TSTATUS);status.SetName("DFSGraph:status");
  status.clear();
  low.clear();
  int nbre_fine = 0;
  tvertex nfrom,nto,nformer;
  tedge z;
  tvertex newv;

  elow[0]=0;
  low[1]=1; //elow[1]=0;
  for (z=nv();z<=ne();z++)
      {nfrom = nvin[z.firsttbrin()];
      nto = nvin[z.secondtbrin()];       
      if(!low[nfrom]){low[nfrom]=nfrom; elow[nfrom]=0;}
      nformer=0;
      while(!low[nto])
          {status[nto-1]=PROP_TSTATUS_THIN; ++nbre_fine;
          low[nto]=nfrom; elow[nto]=z;
          nformer=nto; nto=nvin[nto-1];
          }
      if(nto==nfrom)//fin du backtrack
          {
          
          if(!nformer)//impossible
              {cout<<"hubert nformer nto:"<<nto()<<" m:"<<ne()<<endl; 
              abort();
              }
          
          status[nformer-1]=PROP_TSTATUS_LEAF;
          nbre_fine--;
          }
      else if (low[nto]!=nfrom)
          {while(status[nto-1]==PROP_TSTATUS_THIN)
              {nformer = nto;
              nto=nvin[nto-1];
              if (nto==nfrom) break;
              status[nformer-1]=PROP_TSTATUS_THICK;
              }
          }
      }
  // initialization of unintialized lows
  for (newv=2; newv<=nv();newv++)
      if(!low[newv]){low[newv]=newv; elow[newv]=0;}
  if (status[1]==PROP_TSTATUS_LEAF)
      {tvertex v = nv();
      tbrin b;
      do {b = treein(v);
      v = nvin[b];
      } while (v!=1);
      if (b==tbrin(1))
          {status[1]=PROP_TSTATUS_THIN;
          nbre_fine++;
          }
      }
  return (nbre_fine == nv()-1);
  }

int DFSGraph::Planarity()
  {if(debug())DebugPrintf("Executing DFSGraph:Planarity");
  DFSLow DL(*this);
  return DL.Planarity();
  }
int DFSGraph::TestPlanar()
  {if(debug())DebugPrintf("Executing DFSGraph:TestPlanar");
  DFSLow DL(*this);
  return DL.TestPlanar();
  }

void DFSGraph::ComputeDegrees()
  {if(debug())DebugPrintf("Executing DFSGraph:ComputeDegrees");
  Prop<int> degree(Set(tvertex()),PROP_DEGREE);
  degree.clear();
  degree.SetName("degree");
  for (tbrin b=-ne();b<=ne();b++)
      degree[nvin[b]]++;
  }

void DFSGraph::ShrinkSubdivision()
  {if(!Set(tvertex()).defined(PROP_DEGREE))
      ComputeDegrees();
  Prop<int> degree(Set(tvertex()),PROP_DEGREE);
  svector<tvertex> newvin(-ne(),ne());
  tvertex newv;
  tedge newe;
  tvertex v;
  tedge e;
  svector<tvertex> Newv(1,nv());
  newv = 1;
  for (v=1; v <= nv(); v++)
      {Newv[v]=1;
      if(degree[v] !=2) break;
      }
  for (v++; v <= nv(); v++)
      {if (degree[v]==2)
          Newv[v]=Newv[nvin[v()-1]];
      else
          {Newv[v]=++newv;
          newvin[newv()-1]=Newv[nvin[v()-1]];
          newvin[1-newv()]=newv;
          _ib[newv()-1]=_ib[v()-1];
          }
      }

  for (newe=newv(), e=nv(); e<=ne(); newe++, e++)
      {newvin[newe]=Newv[nvin[e]];
      newvin[-newe]=Newv[nvin[-e]];
      _ib[newe]=_ib[e];
      }
  newe--;
  // Calcul de PROP_NEW pour G
  Prop<tedge> NewEdgeShrink(G.Set(tedge()),PROP_NEW);
  NewEdgeShrink.clear();
  // Cote racine
  tedge f=0;
  tedge cotree_root=0;
  // le coarbre
  for (e=G.nv(); e<=G.ne();e++)
      {f = e()-G.nv()+newv();
      if (nvin[e]==1) cotree_root=e;
      NewEdgeShrink[e]=f;
      v = nvin[-e];
      while (degree[v]==2)
          {NewEdgeShrink[v()-1]=f;
          v = nvin[v()-1];
          }
      }
  if(degree[1]==2)
      {f = cotree_root()-G.nv()+newv(); //new value
      v = 1;
      do 
          {NewEdgeShrink[v()]=f;
          v++;
          }while (degree[v]==2);
      }
  // l'arbre qui reste.
  for (v = G.nv(); v >= 2; v--)
      {if(NewEdgeShrink[v()-1] != 0) continue;
      else if (degree[nvin[1-v()]]!=2)
          f = Newv[v]()-1;
      NewEdgeShrink[v()-1]=f;
      }
  newvin[0] = 0;
  newvin.Tswap(nvin);
  setsize(newv(),newe());
  }

void DFSGraph::DeleteCotreeEdge(tedge e)
  {tedge f;
  for (f=e; f<ne(); f++)
      {nvin[f]=nvin[f()+1];
      nvin[-f]=nvin[-f()-1];
      _ib[f]=_ib[f()+1];
      }
  setsize(tedge(),ne()-1);
  }
void DFSGraph::ShrinkCotree()
  {tedge e;
  tedge newe = 0;
  Prop<bool> mark(Set(tedge()),PROP_MARK,true);
  for (e=nv();e<=ne();e++)
      if (mark[e])
          {newe++;
          nvin[newe]=nvin[e];
          nvin[-newe]=nvin[-e];
          _ib[newe]=_ib[e];
          }
  setsize(tedge(),newe());
  }
void DFSGraph::MarkFundTree()
  {tedge e;
  tvertex vfrom, vto;
  Prop<bool> mark(Set(tedge()),PROP_MARK,true);
  for (e=nv(); e<=ne(); e++)
      {if (mark[e])
          {vfrom = nvin[e];
          vto = nvin[-e];
          while (!mark[treein(vto)] && (vfrom!=vto))
              { mark[treein(vto)] = true;
              vto = father(vto);
              }
          }
      }
/*
   int k = 0;  
   for (e=1; e<=ne(); e++)
      {if (mark[e])++k;}
   cout <<"MarkFundTree:"<<k<<endl;
*/ 
 }

void DFSGraph::Shrink()
  {tedge e;
  tedge newe;
  tvertex v;
  Prop<bool> mark(Set(tedge()),PROP_MARK,true);
  svector<tvertex> newv(0,nv());
  // shrink tree edges
  e=1;
  while (!mark[e]) 
      { if (++e >= nv()) 
          {
          setsize(0,0);
          return;
          }
      }
  newv[nvin[e]] = 1;
  newe = 0;
  v = 1;
  for (;e<=nv()-1;e++)
      if (mark[e])
          {newe++;
          v++;
          newv[e()+1] = v;
          nvin[-newe] = v;
          nvin[newe]= newv[nvin[e]];
          _ib[newe]=_ib[e];
          }
  for (e=nv();e<=ne();e++)
      if (mark[e])
          {newe++;
          nvin[newe]=newv[nvin[e]];
          nvin[-newe]=newv[nvin[-e]];
          _ib[newe]=_ib[e];
          }
  setsize(v(),newe());
  //cout<<"shrink ne:"<<newe()<<endl;
  }

int DFSLow::TestPlanar()
  {_LrSort LrSort(nv(),ne());
  LralgoSort(LrSort);
  _FastHist Hist(ne());
  return Lralgo(LrSort,Hist);
  }

void DFSLow::LralgoSort(_LrSort &LrSort)
  {
  svector<tedge> thin(0,nv()); thin.clear(); thin.SetName("thin:_DFS");
  svector<tedge> thick(0,nv()); thick.clear(); thick.SetName("thick:_DFS");
  tedge je,nextje;
  tedge pje=0;
  tvertex iv;

  TEdgeStackPartition Stack(LrSort.linkt);
  // filling piles thin and thick (sort by Bicon.low)
  for(je = 1;je < nv();je++) // tree edges 
      {iv = low[je+1];
      if(status[je] == PROP_TSTATUS_THICK)
          Stack.Push(thick[iv],je);
      else
          Stack.Push(thin[iv],je);
      }
  for(je = nv();je <= ne();je++) // cotree edges 
      Stack.Push(thin[nvin[je.firsttbrin()]],je);

  // making the list of edges: filling LrSort.tel 
  for(iv = nv();iv >= 1;iv--)
      {je = thick[iv];
      while(je!=0)
          { nextje = Stack.Next(je);
          Stack.Push(LrSort.tel[nvin[je.firsttbrin()]],je);
          je = nextje;
          }
      je = thin[iv];
      while(je!=0)
          { nextje = Stack.Next(je);
          if(je < nv())
              Stack.Push(LrSort.tel[nvin[je.firsttbrin()]],je);
          else
              Stack.Push(LrSort.tel[nvin[je.secondtbrin()]],je);
          je = nextje;
          }
      }

  // assigning a tremaux reference tree edge to each non-terminal vertex:
  // filling LrSort.tref
  for(iv=1;iv <= nv();iv++)
      {if ((je = LrSort.tel[iv])==0) continue;
      if (je <nv()) 
          {LrSort.tref[iv]=je; Stack.Pop(LrSort.tel[iv]); continue;}
      while(je >= nv())
          {pje =je;	je = Stack.Next(je);}
      if(je == 0)continue;
      LrSort.tref[iv] = je;
      /* a tree edge is found */
      Stack.PopNext(pje);
      }
  }
int DFSLow::Lralgo( _LrSort &LrSort, _FastHist &Hist)
  {
  _FastTwit Twit(ne(),nv(),nvin, Hist);
  svector<tedge> &ctel = LrSort.tel;
  tvertex vi, vii;
  tedge ej;
  // Subcalls do not need destructor call. Hence, we may use setjmp/longjmp facility.
  int ncotree;
  int ret_val;
  if ((ret_val = setjmp(Twit.env))!=0)
      { return Twit.planar();
      }
  // Going up in the tree along LrSort.tref edges
  ncotree = 0;
  vi = 1;
  while(LrSort.tref[vi]!=0) vi=treetarget(LrSort.tref[vi]); 
  for(;;)
      {if(ctel[vi] == 0)                                    // No Edge
          {if(vi == tvertex(1))return Twit.planar();        // No bactracking from 1
          if(!Twit.planar())return Twit.planar();
          vii =vi;
          vi = nvin[treein(vi)];                            // Bactracking to the father of vi
          Twit.Deletion(vi);                                // Delete cotree edges
          if(LrSort.tref[vi] == treein(vii)())              // Backtracking along a reference edge
              continue;
          else if(status[treein(vii)] > PROP_TSTATUS_LEAF)  // Backtracking to a fork
              {if(status[treein(vii)] == PROP_TSTATUS_THIN) // Backtacking along a thin edge
                  {ej = Twit.Twin().Firstbot();
                  Twit.Thin(ej);                            // Merge THIN
                  }
              else                                          // Backtacking along a thick edge
                  {Twit.Thick();                            // Merge THICK
                  if(!Twit.planar())return Twit.planar();
                  ej=Twit.Twin().lbot();
                  }
              Twit.NextFork();                              // Looking for nex fork
              Twit.Fusion(ej);                              // fusion
              if(!Twit.planar())return Twit.planar();
              }
          else                                              // Backtraking along an articulation
              Twit.NextFork();
          }
      else if(ctel[vi] < nv())                                 // Going up in the tree
          {vii = treetarget(ctel[vi]);
          Twit.NewFork(vi);                                  // New tree fork
          ctel[vi] = LrSort.linkt[ctel[vi]];
          vi = vii;                                         // updating current vertex
          while(LrSort.tref[vi]!=0)vi = treetarget(LrSort.tref[vi]);
          // Going up in the tree along LrSort.tref edges
          continue;
          }
      else
          {ej = ctel[vi];                                    // Treating a new cotree edge
          ctel[vi] = LrSort.linkt[ctel[vi]];
          Twit.NewTwin(ej);                                  // Create a new Twin
          LrSort.num[ej] = ++ncotree;
          if(!Twit.FirstLink())
              {Twit.Fusion(ej);                              // Fusion
              if(!Twit.planar())return Twit.planar();
              }
          }
      }
  }
int DFSGraph::MarkKuratowski()
  {// On suppose que l'on a un coarbre critique.
  if ((nv() < 6) && (ne() < 9)) 
  	{DebugPrintf("MarkKuratowski nv: ne:",nv(),ne());
  	cout<<"nv:"<<nv()<<" ne:"<<ne()<<endl;
  	setPigaleError(-1,"MarkKuratowski: PLANAR !!");
  	return -1;
  	}
  Prop<bool> keep(Set(tedge()),PROP_MARK);
  keep.SetName("keep:MarkKura");
  // K33 ou K5
  tedge e;
  if ((nv() == 6) && ((ne() == 9)  || ((nv() == 5) && (ne() == 10)))) 
      {for(e = 1;e <= ne();e++)keep[e] = true;
      return 0;
      }
      
  tbrin b;
  tvertex v,v1,v2;
  tbrin ibe;
  tvertex vv1,vv2;
  TopologicalGraph TG(*this);

  //if at most 8  vertices: use quadratic algo
  // take a copy of the graph
  if(debug())DebugPrintf("MarkKuratov:n=%d m=%d",nv(),ne());
  
  if (nv() <= 8)
      {if(debug())DebugPrintf("MarkKuratov <= 8:n=%d m=%d",nv(),ne());
      //cout<<"Q algo  nv:"<<nv()<<" ne:"<<ne()<<endl;
      GraphContainer GC(Container());
      TopologicalGraph G0(GC);
      for(e = 1;e <= G0.ne();e++)keep[e] = true;
      int toerased = G0.ne() - G0.nv() - 3;
      int erased = 0;
      for(e = G0.nv() - 1;e > 0;e--)
          {v1 = G0.vin[e];v2 = G0.vin[-e];
          G0.DeleteEdge(e);
          if(G0.TestPlanar() == 0)
              {keep[e] = false;++erased; // bizarre mais juste
              if(erased == toerased)break;
              }
          else
              G0.NewEdge(v1,v2);
          }
      Set(tbrin()).erase(PROP_CIR);
      Set(tbrin()).erase(PROP_ACIR);
      Set(tbrin()).erase(PROP_PBRIN);
      if(erased != toerased)
          {DebugPrintf("Not Critical erased=%d",erased);return 3;}
      return 0;
      }
  //cout <<"Q  nv:"<<nv()<<" ne:"<<ne()<<endl; 
  // find cotree edge whose fundamnetal cycle is of length > 4
  for (e = ne();e != 0;e--)
      if( (father(father(nvin[-e])) != nvin[e]) && (father(father(father(nvin[-e]))) != nvin[e]))break;
      
  if(e == 0)   
      {DebugPrintf("MarkKuratowski:zero edge");
      setPigaleError(-1,"MarkKuratowski");
  	  return -1;
  	  }
  keep.clear();
  v1 = nvin[e];    v2 = nvin[-e];
  
  keep[e]=true;
  ibe = _ib[e];
  DeleteCotreeEdge(e);
  // now the graph TG is planar
  Planarity();
  svector<int> mark(0,nv()); mark.clear();
  Forall_adj_brins(b,v1,TG)
      mark[nvin[-b]] = 1;
  
  Forall_adj_brins(b,v2,TG)
      {if (mark[nvin[-b]])
          mark[nvin[-b]]=0;
      else
          mark[nvin[-b]]=2;
      }
  int mrk,fl,f1,f2;
  //cout<<"linear algo ********************************* nv:"<<nv()<<endl;
  for(e = 1; e <= ne();e++)
      {mrk = mark[nvin[-e]] | mark[nvin[e]];
      f1 = TG.FaceLength(e());
      f2 = TG.FaceLength(-e());
      fl = f1 + f2 - 2;
      // bug en opt si fl = TG.FaceLength(e())+TG.FaceLength(e())-2;
      /*
      if(debug())DebugPrintf(" lengths %d %d mrk=%d %d -> %d fl=%d"
                             ,TG.FaceLength(e()),TG.FaceLength(-e()),mark[nvin[e]],mark[nvin[-e]]
                             ,mrk,fl);
      */
      if(mrk  == 3 &&  fl == nv())
          break;
      }
  //if(debug()) DebugPrintf("v1=%d v2=%d",v1(),v2());
  if(e > ne()) 
      {DebugPrintf("MarkKuratowski:No second edge");
      setPigaleError(-2,"MarkKuratowski");
      cout<<"e:"<<e<<" ne:"<<ne()<<endl;
      return -2;
      }
  if (mark[nvin[e]]==1)
      {vv1 = TG.vin[e]; vv2 = TG.vin[-e]; }
  else
      {vv2 = TG.vin[e]; vv1 = TG.vin[-e]; }
  if(debug()) DebugPrintf("second edge : %d %d",vv1(),vv2());
  keep[e]=true;
  b = e();
  do
      keep[b.GetEdge()]=true;
  while((b = TG.cir[-b]) != e());
  b = -e();
  do
      keep[b.GetEdge()]=true;
  while((b = TG.cir[-b]) != -e());
  Set(tbrin()).erase(PROP_CIR);
  Set(tbrin()).erase(PROP_ACIR);
  Set(tbrin()).erase(PROP_PBRIN);

  // find last edge incident to neither v1 or v2 or vv1 or vv2
  tvertex vvv1=0;
  tvertex vvv2=0;
  for (e=1; e<=ne(); e++)
      {if (keep[e]) continue;
      vvv1= nvin[e];
      vvv2=nvin[-e];
      if ((vvv1!=v1) && (vvv1!=v2) && (vvv1!=vv1) && (vvv1!=vv2)
          && (vvv2!=v1) && (vvv2!=v2) && (vvv2!=vv1) && (vvv2!=vv2))
          break;
      }
  if (e>ne()) 
      {DebugPrintf("No last edge");
      setPigaleError(-3,"MarkKuratowski");
      return -3;
      }
  if (debug()) DebugPrintf("last edge : %d %d",vvv1(),vvv2());
  keep[e]=true;
  keep[NewCotreeEdge(v1,v2,ibe)]=true;
  return 0;
  }
int DFSGraph::Kuratowski()
//mark a Kuratowski subdivision of a CotreeCritical graph
  {GraphContainer GC2;
  //cout<<"DFSGraph::Kuratowski()"<<endl;
  DFSGraph DG2(GC2,*this);
  DG2.ShrinkSubdivision();
  int kret = DG2.MarkKuratowski();
  if(kret!=0){DebugPrintf("MarKuratovski returned:%d",kret);return -1;}
  Prop<bool> keep(Set(tedge()),PROP_MARK); 
  Prop<tedge> NewEdgeShrink(Set(tedge()),PROP_NEW); //computed in ShrinkSubdivision
  Prop<bool> keep2(DG2.Set(tedge()),PROP_MARK,true); // computed in MarkKuratowsi 
  // Transfert sur le graphe de depart.
  for (tedge e=1; e<=ne(); e++)
      keep[e]=keep2[NewEdgeShrink[e]];
  return 0;
  }
