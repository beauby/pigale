


#include "GraphWidget.h"
#include "pigaleWindow.h"
#include <QT/Action_def.h> 
#include <QT/Misc.h> 

using namespace std;
/*! \file 
\ingroup pigaleWindow
\brief used to add custom functions

This file allow a user to write his own functions: test1, test2, test3.
These functions takes as argument:<br>
 <b>GraphContainer &GC</b> a reference on the current GraphContainer<br>
 <b>int &drawing</b> which can specify a drawing function defines in pigalePaint.cpp<br>
The return value tells Pigale what to do after the completion of the function:<br>
 -1:error 0:(No-Redraw,No-Info) 1:(Redraw,No-Info) 2:(Redraw,Info) 20:(Update drawing,Info)<br>
 3:(Drawing) 4:(3d) 5:symetrie 6-7-8:Springs Embedders<br><br>
The  <b>initMenuTest() </b> allows the user to set the names of his functions in the main menu.
*/
#ifndef _MSC_VER
inline double abs(double x) {if(x>=0) return x; else return -x;}
#endif
static int Test1(GraphContainer &GC,int &drawing);
static int Test2(GraphContainer &GC,int &drawing);
static int Test3(GraphContainer &GC,int &drawing);

void pigaleWindow:: initMenuTest()
  {setUserMenu(1,"Speed: TestPlanar/TestSinglePassPlanar()  (1000x)");
  //setUserMenu(2,"1000xTesNewtPlanarity");
  setUserMenu(2,"Terminaux");
  setUserMenu(3,"Properties");
  }
int Test(GraphContainer &GC,int action,int &drawing)
  {//cout <<"test:"<<action<<endl;
  if(debug())DebugPrintf("Executing Test:%d",action);
  int ret;
  if(action == 1)ret= Test1(GC,drawing);
  else if(action == 2)ret = Test2(GC,drawing);
  else ret =  Test3(GC,drawing);
  if(debug())DebugPrintf("    END executing Test:%d",action);
  return ret;
  }

int Test1(GraphContainer &GC,int &drawing)
  {int repeat= 1000;
  drawing = 0;
  TopologicalGraph G(GC);
  bool _debug = debug(); 
  QTime timer;timer.start();
  int r0 = 0;
  long seed = randomSetSeed();
  for(int i = 0;i < repeat;i++){shuffleCir(G);r0 += G.TestPlanar();}
  double Time1 = timer.elapsed(); // millisec
  if(Time1 < 10)
      {Tprintf("too short time to measure");
      debug() = _debug;
      return 0;
      }

  timer.start();
  int r1 = 0;
  randomSetSeed() = seed; // to get same graphs
  for(int i = 0;i < repeat;i++){shuffleCir(G);r1 += G.TestSinglePassPlanar();}
  double Time2 = timer.elapsed();
  Tprintf("times:%f %f (%f)", Time1,Time2,Time2/Time1); 
  if(r0 != r1){Tprintf("ERROR: %d %d",r0,r1);return 1;}
  debug() = _debug;
  return 0;
  }


int DFSterminal(TopologicalGraph &G,tbrin b0,int & h)
  {// count terminal vertices of a DFS tree in a connected graph
  tvertex v,w; 
  int n = G.nv(); 
  int m = G.ne(); 
  svector<tvertex> nvin(-m,m); 
  svector<tbrin> tb(0,n); tb.clear(); 
  svector<int> dfsnum(0,n); 
  svector<int> height(1,n);height.clear();
  tbrin b = b0;
  tedge y = 1;
  tvertex root =  v = G.vin[b0];
  tb[v]= b0;
  dfsnum[v] = 1;
  for(;;)
      {w = G.vin[-b];
      if(tb[w]!=0)            
          {if(b == tb[v])    // backtracking to w
              {b.cross();
              height(w) = Max(height(w),height(v)+1);
              v = w;
              if(v == root && y == n)break;
              }
          }
          
      else                   // discovering 
          {b.cross();
          tb[w] =b ;
          nvin[y.firsttbrin()] = dfsnum[v];
          y = dfsnum[w] = y() + 1;
          v = w;
          }
      b = G.cir[b];
      }
  h = height(root);
  // Compute # of terminal vertices
  svector<bool> terminal(0,n);  terminal.SetName("GDFSRenu:terminal");
  for(int i = 1;i<= n;i++)
      terminal[i] = true;
  for(int i = 1;i< n;i++)
      terminal[nvin[i]] = false;
  int nter = 0;
  for(int i = 1;i<= n;i++)
      if(terminal[i] == true)++nter;
  return nter;
  }

int Test2(GraphContainer &GC,int &drawing)
  {drawing = 0;
  TopologicalGraph G(GC);
  // compute isolated vertices
  int isolated = 0;
  for(tvertex v = 1;v <= G.nv();v++)
      if(!G.Degree(v))isolated++;
  G.MakeConnectedVertex();
  int nt = 0;
  int maxi = 100;
  int max_nter = 0;
  int min_nter = G.nv();
  int max_h = 0;
  int min_h = G.nv();
  double hauteur = .0;
  for(int i=1; i <= maxi;i++)
      {shuffleCir(G);
      randomStart();
      tbrin b0 = randomGet(G.ne()); 
      randomEnd();
      int h;
      int nter = DFSterminal(G,b0,h);
      max_nter = Max(max_nter,nter);
      min_nter = Min(min_nter,nter);
      nt += nter;
      hauteur += (double)h/maxi;
      max_h = Max(max_h,h);
      min_h = Min(min_h,h);
      }
  Tprintf("nter=%.2f / %d (%d,%d) prop=%2.2f",(double)nt/maxi,G.nv(),min_nter,max_nter,(100.*nt)/(maxi*G.nv()));
  Tprintf("hauteur %d %d (%d) prop=%2.2f",min_h,max_h,(int)(hauteur+.5),100.*hauteur/G.nv());
  if(isolated)Tprintf("isolated=%d",isolated);
  return 1;
  }

int Test3(GraphContainer &GC,int &drawing)
// display  the properties of the current graph that would be saved in a tgf file.
  {drawing = 0;
  TopologicalGraph G(GC);
  Tprintf("\nVertices:");
  int i;
  for (i=G.Set(tvertex()).PStart(); i<G.Set(tvertex()).PEnd(); i++)
      if (G.Set(tvertex()).exist(i))
          {if (G.Set(tvertex()).defined(i)) 
              Tprintf("\n%d %s \n (%s) -> %d bytes",i,PropName(1,i),PropDesc(1,i),
                     (1+G.nv())*G.Set(tvertex())(i)->size_elmt());
          else
              Tprintf("\n%d %s \n (%s)",i,PropName(1,i),PropDesc(1,i));
          }
  Tprintf("\nEdges:");
  for (i=G.Set(tedge()).PStart(); i<G.Set(tedge()).PEnd(); i++)
      if (G.Set(tedge()).exist(i))
          {if (G.Set(tedge()).defined(i)) 
              Tprintf("\n%d %s \n (%s) -> %d bytes",i,PropName(2,i),PropDesc(2,i),
                     (1+G.ne())*G.Set(tedge())(i)->size_elmt());
          else
              Tprintf("\n%d %s \n (%s)",i,PropName(2,i),PropDesc(2,i));
          }
  Tprintf("\nBrins (half-edges):");
  for (i=G.Set(tbrin()).PStart(); i<G.Set(tbrin()).PEnd(); i++)
      if (G.Set(tbrin()).exist(i))
          {if (G.Set(tbrin()).defined(i)) 
              Tprintf("\n%d %s \n (%s) -> %d bytes",i,PropName(3,i),PropDesc(3,i),
                     (1+2*G.ne())*G.Set(tbrin())(i)->size_elmt());
          else
              Tprintf("\n%d %s \n (%s)",i,PropName(3,i),PropDesc(3,i));
          }
  Tprintf("\nGeneral:");
  for (i=G.Set().PStart(); i<G.Set().PEnd(); i++)
      if (G.Set().exist(i))
          Tprintf("\n%d %s \n (%s)",i,PropName(0,i),PropDesc(0,i));
  return 0;
  }












