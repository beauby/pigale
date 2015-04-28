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



#include <Pigale.h>
#include <TAXI/MaxPath.h>

class DoccGraph : public TopologicalGraph
{public:
  svector<tbrin> iel,dos;
  svector<tedge> suc;
  svector<tvertex> nvin,ivl;  
  svector<int> dosInv,dosx,foc,hauteur,lver,rver;
  int hmax;
  int morg;
  bool ok;

 private:
  void init();
  MaxPath *MPATH; 
  int makeDosDFS();
  int makeDosBFS();
  int makeDosLR();
  int edgeHeightCountLR();
 
  tbrin  rightBrinPR(tedge e)
      {// si e a son brin bas a gauche -> brin haut
      // si e a son brin bas a droite     -> brin bas
      return(dosInv[e.firsttbrin() ] > dosInv[e.secondtbrin()] ? e.firsttbrin()  : e.secondtbrin());
      }
  bool edgeFlipped(tedge je)
      {return (hauteur[je] > ( hauteur[nvin[je.secondtbrin()]-1] +1));
      }
  void  edgePush(tedge je,tedge js)
      {if(hauteur[js] <= hauteur[je]) hauteur[js] =  hauteur[je] +1;
      }
  bool  edgeLower(tedge js, tedge sucje);
  void insertConstraint(tedge je,tedge js,int len = 1);

 public:
  DoccGraph(GraphAccess &G,int _morg,int type) : TopologicalGraph(G),hmax(0),morg(_morg),ok(true)
      {init();
      if(type == 0)
          makeDosDFS();
      else  if(type == 1)
          makeDosLR();
      else  if(type == 2)
          makeDosBFS();
      }
  ~DoccGraph(){}
 void vertexHeight();
  void edgeHeight();
  void edgeMinHeight();
  void edgeHeightLR();
  void  vertexLeftRight();
  int drawPR();
  int  checkListe(svector<tedge> &liste,int ml);
  int checkDrawing();
};
void DoccGraph::insertConstraint(tedge je,tedge js,int len)
      {if(js == 0)js = ne()+1;
      MPATH->insert(je(),js(),len);
      }
void DoccGraph::init()
  {int m = ne(); int n = nv();
  nvin.resize(-m,m); nvin.SetName("DosG:nvin");
  iel.resize(-m,m); iel.SetName("DosG:iel");
  ivl.resize(1,n); ivl.SetName("DosG:ivl");
  dos.resize(1,2*m);  dos.SetName("DosG:dos");
 dosInv.resize(-m,m);  dosInv.SetName("DosG:dosInv");
 dosx.resize(1,2*m);  dosx.SetName("DosG:dosx"); 
  suc.resize(-n+1,m);  suc.SetName("DosG:suc");  
  foc.resize(n,m);  foc.SetName("DosG:foc"); 
  hauteur.resize(0,m+1);  hauteur.SetName("DosG:hauteur");
  lver.resize(1,n);  lver.SetName("DosG:lver");lver.clear();
  rver.resize(1,n);  rver.SetName("DosG:rver");
  }
int DoccGraph::makeDosLR()
  {Prop<bool> isTree(Set(tedge()),PROP_ISTREE_LR);
#ifdef TDEBUG
  Prop<short> ecolor(Set(tedge()),PROP_COLOR);
  Prop<int> vlabel(Set(tvertex()),PROP_LABEL);
  Prop<int> elabel(Set(tedge()),PROP_LABEL);
  Prop<int> ewidth(Set(tedge()),PROP_WIDTH);
  for(tedge a =1; a <= ne();a++) 
      {ecolor[a] = Black;
      if(isTree(a))ewidth[a] = 3;
      else ewidth[a] = 1;
      }
#endif
  tbrin b0 = extbrin();
  svector<int> dfsnum(1,nv()); dfsnum.SetName("dfsnum");

 // Renum the vertices as they are discovered
  dfsnum.clear();
  tvertex v = vin[b0];
  int num = 1;
  dfsnum[v] = 1;  ivl[1] = v; 
  tbrin b = b0;
  do
      {tedge je = b.GetEdge();
      tvertex w=vin[-b];
      if(!isTree(je))
          ;
      else if (dfsnum[w] )  // Backtraking on tree
          {b.cross();  v = w;
          }
      else  // Climbing on  tree
          {dfsnum[w] = ++num;  ivl[num] = w;
          b.cross(); v = w;
          }
      b = cir[b];
      } while(b != b0);
 
#ifdef TDEBUG
  for(tvertex v = 1; v <= nv();v++)
      vlabel[v] = (int) dfsnum[v];
 #endif

  // Relabel thes edges and compute Dos
  svector<bool> visited(1,nv()); visited.SetName("visited");
  svector<tbrin> nel(-ne(),ne());  nel.SetName("nel");
  iel[0] = nel[0] = 0;
  nvin[0]=0;
  int occ = 0;
  tbrin y = 1; 
  tbrin z = nv();
  nel.clear();
 visited.clear();
 visited[v] = true;
 b = b0;
  do
      {tedge je = b.GetEdge();
      tvertex w=vin[-b];
      if(!isTree(je))  
          {if (nel[b] != 0)                                                                                                    // cotree -> 2 occ
              dos[++occ] = nel[b];
          else                                                                                                                   // cotree -> 1 occ
              {if (dfsnum[v] < dfsnum[w]) // lower cotree
                  {iel[z] = b;                             nel[b] = z;
                  iel[-z] = -b ;                           nel[-b] = -z;
                  nvin[z] = dfsnum[v]; nvin[-z] = dfsnum[w]; 
                  }
              else // upper cotree
                  {iel[-z] = b ;                           nel[b] = -z;
                  iel[z] = -b;                             nel[-b] = z;
                  nvin[z] = dfsnum[w]; nvin[-z] = dfsnum[v]; 
                  }
              dos[++occ] = nel[b];
              z++;
              }
          }
      else if (visited[w])                                                                                                      // tree 2 occ
          {dos[++occ] = nel[b];
          b.cross();
          v = w;
          }
      else                                                                                                                             // tree 1 occ
          {iel[y]=b;                 nel[b] = y; 
           iel[-y] = -b;             nel[-b] = -y;  
           dos[++occ] = nel[b];
          nvin[y]=dfsnum[v];  nvin[-y] = dfsnum[w] ;
          visited[w] = true;
          b.cross();
          y ++; 
          v = w;
#ifdef TDEBUG
          ecolor[b.GetEdge()] = Red;
#endif
          }
      b = cir[b];
      } while(b != b0);

#ifdef TDEBUG
  // elabel < 0 <=> 1 occ up cotree
  for(tedge je = 1; je <= ne();je++)
      elabel[je] = (int) nel[je].GetEdge()();
      
  for(tedge je = 1; je <= ne();je++)
      if(nvin[je] > nvin[-je])ok = false;
   if (y != nv() || occ != 2*ne()){printf("makeDosPR error\n"); ok = false;}
  if(!ok)return -1;

#endif
  if (y != nv() || occ != 2*ne()){setPigaleError(-1,"makeDosPR"); ok = false;return -1;}

  // Compute dosInv  
  dosInv[0] = 0;
  for(int i = 1;i <= 2*ne();++i)       
      dosInv[dos[i]] = i;
  return 0;
  }
int DoccGraph::makeDosBFS()
  {svector<tvertex> stack(1,nv());  stack.SetName("stack");
  svector<int> bfsnum(0,nv());  bfsnum.SetName("dfsnum");
  svector<tbrin> nel(-ne(),ne());  nel.SetName("nel");
#ifdef TDEBUG
  Prop<short> ecolor(Set(tedge()),PROP_COLOR);
  Prop<int> vlabel(Set(tvertex()),PROP_LABEL);
  Prop<int> elabel(Set(tedge()),PROP_LABEL);
#endif
  bfsnum.clear();nel.clear();
  iel[0] =  0;
  nvin[0]=0;
  tbrin y = 0;
  tbrin z = nv()-1;
  tvertex v = vin[extbrin()];
  bfsnum[v] = 1;
  ivl[1] = v;  
  stack[1] = v;
#ifdef TDEBUG
  vlabel[v] = 1;
#endif
  bfsnum[v]=1;
  int rank =0;
  int max_rank = 1;
  tbrin b,b0;
  while(rank < nv())
      {v = stack[++rank];  
      //printf("v=%d rank=%d\n",v(),rank);
      b = b0 = pbrin[v];
      //printf("b=%d\n",b());
      do 
          {if(nel[b]())continue;
          tvertex w = vin[-b]; 
          if(bfsnum[w] == 0) // tree
              {++y; ++max_rank;
              //printf("tree %d  %d %d\n",y(),vin[b](),vin[-b]());
              bfsnum[w] = max_rank; ivl[max_rank] = w;
              nvin[y] = bfsnum[v];  nvin[-y] = bfsnum[w]; 
              iel[y]=b;                 nel[b] = y; 
              iel[-y] = -b;             nel[-b] = -y;
              stack[max_rank] = w;
#ifdef TDEBUG
              vlabel[w] = (int) bfsnum[w] ; 
              elabel[b.GetEdge()] = (int)nel[b]();          
              ecolor[b.GetEdge()] = Red;
#endif
              }
          else // cotree
              {++z;
              //printf("cotree %d  %d %d\n",z(),vin[b](),vin[-b]());
              nvin[z] = bfsnum[v];  nvin[-z] = bfsnum[w]; 
              iel[z]=b;                 nel[b] = z; 
              iel[-z] = -b;             nel[-b] = -z;
#ifdef TDEBUG
              elabel[b.GetEdge()] = (int)z();
              ecolor[b.GetEdge()] = Blue;
#endif
              }
          }while((b = cir[b])!= b0);
      }
  
  if (y != nv()-1 || z != ne()){DebugPrintf("DFSRenum z=%d y=%d n=%d",z(),y(),nv());ok = false; return -1;}
  // compute dos sequence
  int occ = 0;
  b = b0 = extbrin();
  do
      {dos[++occ] = nel[b];
      if(nel[b].GetEdge() < nv())b = b.cross();
      }while((b = acir[b])!= b0);

  if(occ != 2*ne()){DebugPrintf("Bad double occurrence sequence (%d != %d)",occ,2*ne());ok = false; return -1;}
 // Compute dosInv  
  dosInv[0] = 0;
  for(int i = 1;i <= 2*ne();++i)       
      dosInv[dos[i]] = i;
  return 0;
  }
int DoccGraph::makeDosDFS()
  {svector<tbrin> tb(0,nv()); tb.clear(); tb.SetName("tb");
  svector<int> dfsnum(0,nv());  dfsnum.SetName("dfsnum");
  svector<tbrin> nel(-ne(),ne());  nel.SetName("nel");
#ifdef TDEBUG
  Prop<short> ecolor(Set(tedge()),PROP_COLOR);
  Prop<int> vlabel(Set(tvertex()),PROP_LABEL);
  Prop<int> elabel(Set(tedge()),PROP_LABEL);
#endif
  iel[0] = nel[0] = 0;
  nvin[0]=0;
  //tbrin b0 = acir[extbrin()];
  tbrin b0 = extbrin();
  tbrin b = b0;
  tbrin y = 1;
  tbrin z = nv();
  tvertex w;
  tvertex v = vin[b0];
  ivl[1] = v;  
#ifdef TDEBUG
  vlabel[v] = 1;
#endif
  tb[v]= b0;
  dfsnum[v]=1;
  int occ = 0;
  do
      {w=vin[-b];
      if(tb[w]!=0)            // w deja connu ?
          {if (b==tb[v])    // on descend sur l'abre 
              {dos[++occ] = nel[b];
              b.cross();
              v=w;
              }
          else if (dfsnum[v]<dfsnum[w]) // coarbre bas 
              {dos[++occ] = nel[b];
              }
          else // coarbre haut decouvert en 1
              {iel[-z] = b;                             nel[b] = -z;
              iel[z()] = -b ;                           nel[-b] = z;
              nvin[z]=dfsnum[w];              nvin[-z]=dfsnum[v];
              dos[++occ] = nel[b];
#ifdef TDEBUG
              elabel[b.GetEdge()] = (int)z();
#endif
              z++;
              }
          }
      else   // arbre bas 
          {iel[y]=b;                nel[b] = y; 
          nel[-b] = -y;            iel[-y] = -b;
          dos[++occ] = nel[b];
          b.cross();
          tb[w]=b;
          nvin[y]=dfsnum[v];
          y = nvin[-y]() = dfsnum[w] = y()+1;
          ivl[y] = (tvertex) w;  
#ifdef TDEBUG
          vlabel[w] = (int) y(); 
          elabel[b.GetEdge()] = (int)nel[b]();          
          ecolor[b.GetEdge()] = Red;
#endif
          v = w;
          }
      b = cir[b];
      } while(b != b0);

  if (y != nv() || z != ne()+1){DebugPrintf("GDFSRenum z=%d y=%d n=%d",z(),y(),nv());ok = false; return -1;}

  // Compute dosInv  
  dosInv[0] = 0;
  for(int i = 1;i <= 2*ne();++i)       
      dosInv[dos[i]] = i;
  return 0;
  }
void  DoccGraph::vertexHeight()
  {int hh = 0;
  tbrin last = 0;
  int ix = 1;
  hauteur.clear();
  for(int i = 1;i <= 2*ne();++i)
      {tedge je = dos[i].GetEdge();
      if(je < nv())
          {if(dos[i] > 0)
              {hh += 2;
              hmax = max(hmax,hh);
              }
          else
              {hauteur[je] = hh;
              hh -= 2;
              }
          }
      if(( (last < 0)||(last >= nv()) ) && ( (dos[i] >= 0) || (dos[i] <= -nv())) )++ix;
      dosx[i] = ix;
      last = dos[i];
      }
  hmax += 2;
  }
void  DoccGraph::edgeHeight()
  {tedge js  = 0;
  tbrin es = 0;
  foc.clear();suc.clear();
  MPATH=new MaxPath(ne()+1,2*ne());
  for(int j = 2*ne();j >= 1;--j)
      {tbrin ee = dos[j];
      tedge je = ee.GetEdge();
      if(je  >= nv())     // je  cotree 
          {++foc[je];
          if(je != js)
              {if(foc[je] != 2)  // premiere occurrence (en partant de la droite)
                  {insertConstraint(je,js);
                  suc[je] = js;js = je;es = ee;
                  }
              }
          else  // deuxieme occurrence (en partant de la droite) et je == js
              {while(js > 0 && foc[js] == 2)
                  {insertConstraint(je,js);
                  js = suc[js];es = ee;
                  }
              es = rightBrinPR(js);
              }
          }
      else  // je is tree
          {insertConstraint(je,js);
          suc[ee] = js;
          }
      }

  MPATH->solve(hauteur);
  delete  MPATH;
  hmax = Max(hmax,hauteur[ne()+1] +1);
  }
void  DoccGraph::edgeMinHeight()
  {for(tedge je = nv(); je <= ne(); ++je)
      hauteur[je] = 1+ Max(hauteur[nvin[je] - 1],hauteur[nvin[-je] - 1]);
      //hauteur[je] =  Max(hauteur[nvin[je] - 1],hauteur[nvin[-je] - 1]);
  if(planarMap() != -1)return;
  foc.clear();suc.clear();
  tedge last = 0;
  tedge js = 0;
  for(int j = 1; j <= 2*ne();++j)
      {tbrin ee = dos[j];
      tedge je = ee.GetEdge();
      if(je  >= nv())     // je  cotree 
          {++foc[je];
          if(je != js)
              {if(foc[je] != 2)  {suc[je] = js;js = je;}
              }
          else  
              while(js > 0 && foc[js] == 2)
                  js = suc[js];
          }
      else //tree
          {suc[ee] = js;
          if(ee() > 0)last = je;
          else if(je == last)
              {tedge cte = suc(ee);
              tvertex iv = nvin[-last];
              while(cte > 0 && foc[cte] )
                  {if(foc[cte] == 1 && iv != nvin[-cte])
                      hauteur[cte] = Max(hauteur[cte],hauteur[last]+2);
                  //hauteur[cte] = Max(hauteur[cte],hauteur[last]);
                  cte = suc(cte);
                  }
              }
          }
      }
  }

bool  DoccGraph::edgeLower(tedge a, tedge b)
  {return (Max(hauteur[nvin[a] - 1], hauteur[nvin[-a] - 1]) <  Max(hauteur[nvin[b] - 1], hauteur[nvin[-b] - 1]));
  }

int  DoccGraph::edgeHeightCountLR()
  {int num = 0;
  tedge jss,js  = 0;
  tbrin es = 0; // left  brin of js
  foc.clear();suc.clear();
  for(int j = 1;j <= 2*ne();j++)
      {tbrin ee = dos[j];
      tedge je = ee.GetEdge();
      if(je < nv())continue; 
      ++foc[je];
      if(je == js) // deuxieme occurrence
          {while(js != 0 && foc[js] == 2)
              js = suc[js];
          continue;
          }
      if(foc[je] == 2) continue;
      jss = js;
      do
          {if(js())++num;
          do
              {js = suc[js];
              }while(js() && foc[js] == 2);
          }while(js());
      
      suc[je] = jss;
      js = je;es = ee;
      }
  return num;
  }
void  DoccGraph::edgeHeightLR()
  {int numConstraints = edgeHeightCountLR();
#ifdef TDEBUG
 Prop<short> ecolor(Set(tedge()),PROP_COLOR);
 for(tedge je = nv();je <= ne();je++)
     if(edgeFlipped(je))
         {tedge e = iel[je].GetEdge();
         if(dosInv[je] < dosInv[-je] )ecolor[e] = Blue;
         else ecolor[e] = Red;
         }
#endif
  tedge jss,js  = 0;
  tbrin es = 0; // left  brin of js
  foc.clear();suc.clear();
  MPATH=new MaxPath(ne()+1,numConstraints);

  for(int j = 1;j <= 2*ne();j++)
      {tbrin ee = dos[j];
      tedge je = ee.GetEdge();
      if(je < nv())continue; 
      ++foc[je];
      if(je == js) // deuxieme occurrence
          {while(js != 0 && foc[js] == 2)
              js = suc[js];
          //printf("->2OCC je=%d suc=%d\n",je(),js());
          continue;
          }
      if(foc[je] == 2) continue;
      jss = js;
      do
          {es = (js() != 0) ? -rightBrinPR(js) : 0; 
          if(es() &&  dosInv[-ee] < dosInv[-es])
              {insertConstraint(je,js);
              //printf("%d p %d (%d %d)\n",je(),js(), hauteur[je],hauteur[js]);
              }
          else if(js()  &&  hauteur[je] < hauteur[js])
              {insertConstraint(je,js);
              //printf("%d p< %d (%d %d)\n",je(),js(), hauteur[je],hauteur[js]);
              }
          else  if(js() && hauteur[je] > hauteur[js])
              {insertConstraint(js,je);
              //printf("%d Rpush> %d \n",js(),je());
              }
          else if(js() && ee() > 0 && es() > 0 &&  edgeFlipped(je) && !edgeFlipped(js))
              {insertConstraint(js,je);
              //printf("%d  RA %d \n",es(),ee());
              }
          else if(js() && ee() < 0 && es() > 0 &&  edgeFlipped(je) && !edgeFlipped(js) )
              {insertConstraint(js,je);
              //printf("%d  RB %d \n",es(),ee());
              }
          else if(js())
               {insertConstraint(je,js);
               //printf("%d  p== %d \n",ee(),es());
               }
          do
              {js = suc[js];
              }while(js() && foc[js] == 2);
          }while(js());
      
      suc[je] = jss;
      js = je;es = ee;
      }
  
  MPATH->solve(hauteur);
#ifdef TDEBUG
  bool nocircuits =  MPATH->verify(hauteur);
  if(!nocircuits){ok = false;setPigaleError(-1,"There is a CIRCUIT");}
  if(!nocircuits)printf("THERE IS A CIRCUIT----------------------------\n");
 hmax = Max(hmax,hauteur[ne()+1] +1);
#else
 for(tedge je = nv();je <= ne();je++)
      hmax = Max(hmax,hauteur[je] +1);
#endif
  delete  MPATH;
  }

 void  DoccGraph::vertexLeftRight()
  {for(int i = 1;i <= 2*ne();++i)
      {tbrin ee = dos[i];
      tedge je = ee.GetEdge();
      if(iel[je].GetEdge() > morg)continue;
      tvertex v = nvin[ee];
      if(lver[v] == 0)
          {if(je() < nv())
              lver[v] = dosInv[je];
          else
              lver[v] = i;
          rver[v] = lver[v];
          }
      else
          {if(je() < nv())
              rver[v] = Max(rver[v],dosInv[je]);
          else
              rver[v] = i;
          }
      // as some tree edges might be erased
      if(je() < nv())
          {v = nvin[-ee];
           if(lver[v] == 0) rver[v] = lver[v] = dosInv[je];
          }
      }
  // for vertices originally isolated
for(int i = 1;i <= 2*ne();++i)
      {tbrin ee = dos[i];
      tedge je = ee.GetEdge();
      tvertex v = nvin[ee];
      if(lver[v] != 0)continue;
      if(lver[v] == 0)
          {if(je() < nv())
              lver[v] = dosInv[je];
          else
              lver[v] = i;
          rver[v] = lver[v];
          }
      }
  }
int  DoccGraph::checkListe(svector<tedge> &liste,int ml)
  {int i0,i1,j0,j1;
  Prop<short> ecolor(Set(tedge()),PROP_COLOR);
  for(int i = 1;i < ml;i++)
      for(int j = i+1;j <= ml;j++)
          {if(dosInv[liste[i]] < dosInv[-liste[i]])
              {i0 = dosInv[liste[i]];  i1 = dosInv[-liste[i]];}
          else
              {i0 = dosInv[-liste[i]];  i1 = dosInv[liste[i]];}
          if(dosInv[liste[j]] < dosInv[-liste[j]])
              {j0 = dosInv[liste[j]];  j1 = dosInv[-liste[j]];}
          else
              {j0 = dosInv[-liste[j]];  j1 = dosInv[liste[j]];}

          if((j0 > i0 && j0 < i1) || (j1 > i0 && j1 < i1))
              {printf("crossing:%d %d\n",liste[i](),liste[j]());
              ecolor[iel[liste[i]].GetEdge()]  = Red;
              ecolor[iel[liste[j]].GetEdge()]  = Blue;
              setPigaleError(-1,"polrec:bad drawing X");
              ok = false;
              return -1;
              }
          if((i0 < j0 && j1 < i1) || (i1 < j1))
              {printf("touching:%d %d\n",liste[i](),liste[j]());
              ecolor[iel[liste[i]].GetEdge()] = Red;
              ecolor[iel[liste[j]].GetEdge()] = Green;
              setPigaleError(-1,"polrec:bad drawing T");
              ok = false;
              return -1;
              }
          }
  return 0;
  }
int  DoccGraph::checkDrawing()
  {// sort cotree edges according to their height
  // recompute hmax
  hmax = 0;
  for(tedge je = nv(); je <= ne();je++)
      hmax = Max(hmax, hauteur[je]);
  svector<tedge> htop(0,hmax); htop.SetName("htop");
  svector<tedge> link(nv(),ne()); link.SetName("link");
  htop.clear();
  for(tedge je = nv(); je <= ne();je++)
      {int h = hauteur[je];
      link[je] = htop[h]; htop[h] = je;
      }
  if( htop[0]())
      {setPigaleError(-1,"polrec:bad drawing");
      ok = false;
      return -1;
      }
  svector<tedge> liste(1,ne()-nv()); liste.SetName("liste");
  for(int h = hmax;h >= 0;--h)
      {int ml = 0;
      while(htop[h]())
          {liste[++ml] =  htop[h];
          htop[h] = link[htop[h]];
          }
      if(ml > 1 && checkListe(liste,ml))return -1;
      }
  return 0;
  }
int  DoccGraph::drawPR()
  {Prop1<Tpoint> pmin(Set(),PROP_POINT_MIN);
  Prop1<Tpoint> pmax(Set(),PROP_POINT_MAX);
  pmin() = Tpoint(.0,-1.5);
  pmax() = Tpoint(dosx[2*ne()]+1.,hmax+.5);
  Prop<Tpoint> p1(Set(tvertex()),PROP_DRAW_POINT_1); p1.SetName("p1");
  Prop<Tpoint> p2(Set(tvertex()),PROP_DRAW_POINT_2); p2.SetName("p2");
  Prop<double> x1(Set(tedge()),PROP_DRAW_DBLE_1); x1.SetName("x1");
  Prop<double> x2(Set(tedge()),PROP_DRAW_DBLE_2); x2.SetName("x2");
  Prop<double> y1(Set(tedge()),PROP_DRAW_DBLE_3); y1.SetName("y1");
  Prop<double> y2(Set(tedge()),PROP_DRAW_DBLE_4); y2.SetName("y2");
  Prop<double> y(Set(tedge()),PROP_DRAW_DBLE_5); y.SetName("y");
  Prop<bool> isTree(Set(tedge()),PROP_ISTREE); isTree.SetName("isTree");
  isTree.clear();
 
  // vertex plotting
  for(tvertex v = 1;v <= nv();++v)
        {double xv1 = (double)dosx[lver[v]] - .45;
        double yv1 =  (double)hauteur[v-1] + .5; // point haut-gauche du sommet
        double xv2 = (double)dosx[rver[v]] + .45;
        p1[ivl[v]] = Tpoint(xv1,yv1);        p2 [ivl[v]] = Tpoint(xv2,yv1);
        }
  // tree plotting 
  for(tedge je = 1;je <= nv()-1 ;++je)
      {tedge ie = iel[je].GetEdge();
      if(ie > ne())continue;
      isTree[ie] = true;
      x1[ie] = (double)dosx[dosInv[je]]; x2[ie] = -1;
      y2[ie] = (double)hauteur[je] -.5;
      y1[ie] =  y2[ie] - 1.;
      }
  // cotree plotting (x1,y1) -> (x1,y) -> (x2,y) -> (x2,y2)
  for(tedge je = nv();je <= ne();++je)
        {tedge ie = iel[je].GetEdge();
        if(ie > ne())continue;
        x1[ie] = (double)dosx[dosInv[je]];
        x2[ie] = (double)dosx[dosInv[-je]];
        y1[ie] = (double)hauteur[nvin[dos[dosInv[je]]] - 1] +.5;
        y2[ie] = (double)hauteur[nvin[dos[dosInv[-je]]] - 1] +.5;
        y[ie] = (double)hauteur[je] +.5-.3;
        if(y[ie] <= Max( y1[ie], y2[ie]))ok = false;
        }
  // delete the edges added by Connexity
  for(tedge e = ne();e > morg;e--) DeleteEdge(e);
  return 0;
  }

int EmbedPolrecGeneral(TopologicalGraph &G,int type)
// type == 0 -> polerc     
// type == 1 -> polercLR
  {int morg = G.ne();
  svector<tbrin> cir0,acir0;
  tbrin b0=0;
  if(type == 1){cir0 = G.cir; acir0 = G.acir;b0 = G.extbrin();}
  G.MakeConnected();
  if(type == 1)G.Planarity(G.extbrin());
  
  DoccGraph DG(G,morg,type);
  if(!DG.ok){setPigaleError(-1,"ERROR: polrec construction");DebugPrintf("ERROR: polrec"); return -1;}
  DG.vertexHeight();
  if(type == 1&& DG.planarMap() == -1)
      {DG.edgeMinHeight();
      DG.edgeHeightLR();
      }
  else 
      DG.edgeHeight();
  DG.vertexLeftRight();
#ifdef TDEBUG
  if(type == 1)DG.checkDrawing();
#endif
  DG.drawPR();
  if(type == 1) {G.cir.vector() = cir0; G.acir.vector() = acir0; G.extbrin() = b0;}
#ifndef TDEBUG
  if(!DG.ok){setPigaleError(-1,"ERROR: polrec");DebugPrintf("ERROR: polrec");return -1;}
#endif
  
  return 0;
  }    
int EmbedPolrecDFS(TopologicalGraph &G)
  {return  EmbedPolrecGeneral(G,0);
  }
int EmbedPolrecLR(TopologicalGraph &G)
  {return  EmbedPolrecGeneral(G,1);
  }
int EmbedPolrecBFS(TopologicalGraph &G)
  {int morg = G.ne();
  G.MakeConnected();
  DoccGraph DG(G,morg,2);
  if(!DG.ok){setPigaleError(-1,"ERROR: polrec construction");DebugPrintf("ERROR: polrec"); return -1;}
  DG.vertexHeight();
  DG.edgeHeight();
  DG.vertexLeftRight();
  DG.drawPR();
  return 0;
  }


