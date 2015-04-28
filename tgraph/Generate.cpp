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


#include <TAXI/Tpoint.h>
#include <TAXI/Tdebug.h>
#include <TAXI/Tmessage.h>
#include <TAXI/graphs.h>
#include <TAXI/random.h>
#include <TAXI/Tstack.h>
#include <limits>

GraphContainer *GenerateGrid(int a, int b)
  {if(debug())DebugPrintf("\nGenerateGrid");   
  GraphContainer &GC = *new GraphContainer;
  int n=a*b;
  int m=2*n-a-b;
  GC.setsize(n,m);
  Prop1<tstring> title(GC.Set(),PROP_TITRE);
  char t[256];
  sprintf(t,"%d x %d Grid", a,b);
  title() = t;
  Prop<tvertex> vin(GC.PB(),PROP_VIN); vin[0]=0;
  Prop<Tpoint> vcoord(GC.PV(),PROP_COORD);
  Prop<int> vlabel(GC.PV(),PROP_LABEL);
  Prop<int> elabel(GC.PE(),PROP_LABEL);
  Prop<tbrin> cir(GC.PB(),PROP_CIR); cir[0]=0;
  Prop<tbrin> acir(GC.PB(),PROP_ACIR); acir[0]=0;
  Prop<tbrin> pbrin(GC.PV(),PROP_PBRIN); pbrin[0]=0;
  tvertex v;
  tedge e;
  for (v=0; v<=n; v++)
    vlabel[v]=v();
  for (e=0; e<=m; e++)
    elabel[e]=e();
  if(n > 1)
      {int aa=a-1; int bb=b-1;
      int x; int y;
      tbrin bs[4];
      for (int j=0; j<b; j++)
          {for (int i=0; i<a; i++)
              {int nb=0;
              x=i*b+j+1; y=i+j*a+aa*b+1; v=i+a*j+1;
              vcoord[v]=Tpoint(i,j);
              if (i<aa)        bs[nb++]=x;
              if (j<bb)        bs[nb++]=y;
              if (i>0)        bs[nb++]=b-x;
              if (j>0)        bs[nb++]=a-y;
              nb--;
              for (int k=0; k<nb;k++)
                  {cir[bs[k]]=bs[k+1];acir[bs[k+1]]=bs[k];vin[bs[k]]=v;}
              cir[bs[nb]]=bs[0];acir[bs[0]]=bs[nb];vin[bs[nb]]=v;
              pbrin[v]=bs[0];
              }
          }
      }
      else
          {pbrin[1]=0;vcoord[1] = Tpoint(1.,1.);}
  TopologicalGraph TG(GC);
  TG.planarMap() = 1;
  return &GC;
}
GraphContainer *GenerateCompleteGraph(int a)
  {if(debug())DebugPrintf("\nGenerateCompleteGraph");   
  GraphContainer &GC = *new GraphContainer;
  int n=a;
  int m=a*(a-1)/2;
  GC.setsize(n,m);
  Prop1<tstring> title(GC.Set(),PROP_TITRE);
  char t[256];
  sprintf(t,"K%d", a);
  title() = t;
  Prop<tvertex> vin(GC.PB(),PROP_VIN); vin[0]=0;
  Prop<Tpoint> vcoord(GC.PV(),PROP_COORD);
  Prop<int> vlabel(GC.PV(),PROP_LABEL);
  Prop<int> elabel(GC.PE(),PROP_LABEL);
  tvertex v;
  tedge e;
  vlabel[0]=0;
  double angle = 2.*acos(-1.)/a;
  for (v=1; v<=n; v++)
    {vlabel[v]=v();
    vcoord[v]=Tpoint(cos(angle*(v()-1)),sin(angle*(v()-1)));
    }
  for (e=0; e<=m; e++)
    elabel[e]=e();
  tbrin b=1;
  for (v=1; v<a; v++)
    for (tvertex vv=v+1; vv<=a; vv++)
      {vin[b]=v;vin[-b]=vv; b++;}
  TopologicalGraph TG(GC);
  if(a > 4)TG.planarMap() = -1;
  return &GC;
}
GraphContainer *GenerateCompleteBiGraph(int a,int b)
  {if(debug())DebugPrintf("\nGenerateCompleteBiGraph");   
  GraphContainer &GC = *new GraphContainer;
  int n=a+b;
  int m=a*b;
  GC.setsize(n,m);
  Prop1<tstring> title(GC.Set(),PROP_TITRE);
  char t[256];
  sprintf(t,"K%d,%d", a,b);
  title() = t;
  Prop<tvertex> vin(GC.PB(),PROP_VIN); vin[0]=0;
  Prop<Tpoint> vcoord(GC.PV(),PROP_COORD);
  Prop<int> vlabel(GC.PV(),PROP_LABEL);
  Prop<int> elabel(GC.PE(),PROP_LABEL);
  tvertex v;
  tedge e;
  vlabel[0]=0;
  for (v=1; v<=a; v++)
    {vlabel[v]=v();
    vcoord[v]=Tpoint(0,v()-a/2.0);
    }
  for (v=a+1; v<=a+b; v++)
    {vlabel[v]=v();
    vcoord[v]=Tpoint(1,v()-b/2.0-a);
    }
  for (e=0; e<=m; e++)
    elabel[e]=e();
  tbrin bb=1;
  for (v=1; v<=a; v++)
    for (tvertex vv=a+1; vv<=a+b; vv++)
      {vin[bb]=v;vin[-bb]=vv; bb++;}
  TopologicalGraph TG(GC);
  if(a >= 3 && b >=3 )TG.planarMap() = -1;
  return &GC;
}
GraphContainer *GenerateRandomGraph(int a,int b,bool randomEraseMultipleEdges)
  {if(debug())DebugPrintf("\nGenerateRandomGraph seed:%d",randomSetSeed());  
  GraphContainer &GC = *new GraphContainer;
  int n = a;
  int m = (n > 1) ? b : 0;
  GC.setsize(n,m);
  Prop1<tstring> title(GC.Set(),PROP_TITRE);
  char titre[256];
  sprintf(titre,"Random_%ld",randomSetSeed());
  title() = titre;
  Prop<tvertex> vin(GC.PB(),PROP_VIN); vin[0]=0;
  Prop<Tpoint> vcoord(GC.PV(),PROP_COORD);
  Prop<int> vlabel(GC.PV(),PROP_LABEL);
  Prop<int> elabel(GC.PE(),PROP_LABEL);

  tvertex v,w;
  tedge e;
  vlabel[0]=0;
 double angle = 2.*acos(-1.)/n;
  for(v=1; v <= n; v++)
    {vlabel[v]=v();
    vcoord[v]=Tpoint(cos(angle*(v()-1)),sin(angle*(v()-1)));
    }
  for (e=0; e<=m; e++)
    elabel[e]=e();

  randomStart();
  for(tbrin bb = 1;bb <= m;bb++)
      {v = randomGet(n);
      while((w = randomGet(n)) == v){;}
      vin[bb] = v;vin[-bb] = w;
      }    
  randomEnd();
  TopologicalGraph TG(GC);
  if(randomEraseMultipleEdges)TG.Simplify();
  return &GC;
}
GraphContainer *GenerateRandomGraph(int a,int b)
  {if(debug())DebugPrintf("\nGenerateRandomGraph seed:%d n:%d m:%d",randomSetSeed(),a,b); 
  //DebugPrintf("\n max int:%d",std::numeric_limits<int>::max() ); 
  //DebugPrintf("\n max long:%ld",std::numeric_limits<long>::max() ); 
  GraphContainer &GC = *new GraphContainer;
  int n = a;
  unsigned int maxm = (unsigned int)n*((unsigned int)n-1)/2;  
  
  int m = (n > 1) ? b : 0; 
  //m = Min(m,maxm);
  GC.setsize(n,m);
  Prop1<tstring> title(GC.Set(),PROP_TITRE);
  char titre[256];
  sprintf(titre,"Random_%ld",randomSetSeed());
  title() = titre;
  Prop<tvertex> vin(GC.PB(),PROP_VIN); vin[0]=0;
  Prop<Tpoint> vcoord(GC.PV(),PROP_COORD);
  Prop<int> vlabel(GC.PV(),PROP_LABEL);
  vlabel[0]=0;
  double angle = 2.*acos(-1.)/n;
  
  for(tvertex v = 1; v <= n; v++)
    {vlabel[v]=v();
    vcoord[v]=Tpoint(cos(angle*(v()-1)),sin(angle*(v()-1)));
    }
    
  randomStart();
  int mm = 0;
  double p = randomMax()/(double)maxm; 
  unsigned long pro = (unsigned long)(m*p +.5);
  //DebugPrintf("\nmax:%ld pro:%ld maxm:%u",randomMax(),pro,maxm);
  //unsigned long cumul = 0;
  for(tvertex v = 1;v < n;v++)
      for(tvertex w = v+1;w <= n;w++)
      {unsigned long p  = randomGet();
      //cumul += p;
      if(p <= pro)
          {if(mm < m)
              {++mm;
              vin[mm] = v;vin[-mm] = w;
              }
          else
              {++mm;
              vin(mm) = v;vin(-mm) = w;
              }
          }
      }  
      //cumul /= maxm;
  randomEnd();
  GC.setsize(n,mm);
  Prop<int> elabel(GC.PE(),PROP_LABEL);
  for(tedge e= 0; e <=mm; e++)
    elabel[e]=e();
  TopologicalGraph TG(GC);
  
  //if(debug())DebugPrintf("\n%d -> %d   %f:",m,TG.ne(),(double)cumul/randomMax()/.005 -100);
  
  return &GC;
}
///////////////// GEN OUTERPLANAR /////////////////////////////
// Compute the length of the last branch of a well-orderly spanning tree of a random outerplanar map with the correct probability
// parameters
// n (input) : number of vertices of the random outerplanar map
// return : the length of the last branch
int gen_i(int n) 
  {int i = 1;
  bool again = (randomGet((2*((2*n-2)-i)*(i+1)))<= (2*n-2-2*i)*(i+2));
  while(again) 
      { i++;
      if (randomGet(10000)/1000 % 2 == 0) 
          i = 1;
      again = (randomGet((2*((2*n-2)-i)*(i+1))) <= (2*n-2-2*i)*(i+2));
      }
  return i;
  }
// Compute the length of the last branch of a well-orderly spanning tree of a random outerplanar map with n vertices and m edges with the correct probability
// parameters
// n (input) : number of vertices of the random outerplanar map
// m (input) : number of edges of the random outerplanar map
// return : the length of the last branch
int gen_i_m(int n, int m) 
  { int i = 1;
  if (n == 3 && m == 3)
      return 1;
  bool again = (randomGet(2*((2*n-2)-i)*(i+1))<= ((2*n-2)-2*i)*(i+2));
  while(again) 
      {i++;
        if (n != (i+1))
            if (randomGet(n-i-1) <= (m-n+1)) 
                 i = 1;
        again = (randomGet(2*((2*n-2)-i)*(i+1)) <= ((2*n-2)-2*i)*(i+2));
      }
  return i;
  }
// Generate a Dyck path with exactly i South-East steps at the end.
// Parameters :
// word (output) : the Dyck path. This array must be allocated
// i (input) : number of South-East steps at the end
// length( input) : length of the dyck path.
void gen_Dyck_i(int *word, int i, int length) 
  {assert(length%2 == 0);
  int k=0;
  for(k = length; k>= length-i;k--)
      word[k] = length-k;
  word[length-i-1]=i-1;
  for(k=length-i-1;k>=1; k--) 
      {if (randomGet(2*k*(word[k]+1)) <= (k-word[k])*(word[k]+2))
          word[k-1] = word[k] + 1;
      else
          word[k-1] = word[k] - 1;
      }
  word[0] = 0;
  }
// Generate a random binary string
// int *word (output) : the binary string. This array must be allocated
// int length : length of the array "word".
void gen_random(int *word, int length) {
  for(int i=0; i< length;i++)
    word[i] = (randomGet(10000)/1000 % 2);
}
// Generate a random binary string with k "1" and "length-k "0"
// Parameters
// int *word (output) : the binary string. This array must be allocated
// int length : length of the array "word".
// int k : number of "1" in the binary string
void gen_random(int *word, int length, int k) 
  { int h = 0;
  for(int i=0; i< length;i++) 
      {if (randomGet(length-i) <= (k-h)) 
          {word[i] = 1;
          h++;
          }
      else
          word[i] = 0;
      }
  assert(h==k);
  }
// Create the outerplanar map encode with the Dyck path and the additionnal edges
// Parameters 
// Dyck (input) : the Dyck path encoding the well-orderly tree of the outerplanar map.
// add_edges (input) : the vector of additional edges of the outerplanar map
// l (input) : the length of the last branch of the well-orderly tree
// n (input) : number of vertices of the outerplanar map
// seed (input) : the Id of the outerplanar map. If Dyck and add_edges have been randomly generated, seed should be computed with randomSetSeed();
GraphContainer * create_outerplanar(int *Dyck, int *add_edges, int i, int n, long seed,bool randomEraseMultipleEdges)
{int m =n-1;
  int k_m = 0;
  int j;
  for (j=n-1;j>=0;j--) 
      if (add_edges[j] == 1)
          m++;
  if(debug())DebugPrintf("\nGenerateRandom OuterPlanarGraph");  
  GraphContainer &GC = *new GraphContainer;
  GC.setsize(n,m);
  Prop1<tstring> title(GC.Set(),PROP_TITRE);
  char titre[256];
  sprintf(titre,"Outerplanar_%ld",seed);
  title() = titre;
  Prop<tvertex> vin(GC.PB(),PROP_VIN);
  Prop<Tpoint> vcoord(GC.PV(),PROP_COORD);
  Prop<int> vlabel(GC.PV(),PROP_LABEL);
  Prop<int> elabel(GC.PE(),PROP_LABEL);
  tvertex v,w,k;
  tedge e=0;
  vlabel[0]=0;
  for(v=1; v <= n; v++)
      vlabel[v]=v();

  int t;
  double incr = (2 * 3.14) / n;
  double angle = (3* 3.14) / 2;
  double x = cos(angle);
  double y = sin(angle);
  stack<tvertex> Stk;
  tvertex P1 = 0;

  k=n;
  // New node
  x =  cos(angle);
  y =  sin(angle);
  vcoord[k]=Tpoint(x,y);
  Stk.push(k);
  for (t=2*n-3; t>=0;t--) 
      {assert(Dyck[t]>=0);
      assert(Dyck[t]!=Dyck[t+1]);
      if (Dyck[t]>Dyck[t+1]) 
          {// New node
          k--;
          assert(!Stk.empty());
          e++;
          elabel[e]=e();
          k_m++;
          vin[k_m] = k;
          vin[-k_m] = Stk.peep();
          Stk.push(k);
          if (k <2*n-2-i) 
              {if (add_edges[ k()- 1 ] == 1) 
                  {e++;
                  elabel[e]=e();
                  k_m++;
                  vin[k_m] = k;
                  vin[-k_m] = P1;
                  }
              }
          }
      else 
          {P1 = Stk.pop();
          angle += incr;
          x = cos(angle);
          y = sin(angle);
          vcoord[P1]=Tpoint(x,y);
          }
      }
  assert(k == 1);
  delete [] Dyck; delete [] add_edges;
  GC.setsize(n,k_m); // as the graph may have less edges
  TopologicalGraph TG(GC);
  if(randomEraseMultipleEdges)
      TG.Simplify();
  return &GC;
}
// Compute a random outerplanar map with n vertices
GraphContainer *GenerateRandomOuterplanarGraph(int n,bool randomEraseMultipleEdges) 
  {int *add_edges = new int[n-1];
  int *Dyck = new int[2*n-1];
  for(int k=0;k<=n-2;k++)
      add_edges[k]=0;
  for(int k=0;k<=2*n-2;k++)
      Dyck[k]=0;
  long seed = randomSetSeed();
  randomStart();
  int i = gen_i(n);
  gen_Dyck_i(Dyck, i, 2*n-2);
  gen_random(add_edges, n-i-1);
  randomEnd();
  return create_outerplanar(Dyck, add_edges, i, n, seed,randomEraseMultipleEdges);
  }
// Compute a random outerplanar map with n vertices and m edges.
GraphContainer *GenerateRandomOuterplanarGraph(int n,int m,bool randomEraseMultipleEdges)
  { int i,k;
  int *add_edges = new int[n-1];
  int *Dyck = new int[2*n-1];
  for(k=0;k<=n-2;k++)
      add_edges[k]=0;
  for(k=0;k<=2*n-2;k++)
      Dyck[k]=0;
  long seed = randomSetSeed();
  randomStart();
  if (m<n-1)
      m = n-1;
  if (m > 2*n-3)
      m=2*n-3;
  i = gen_i_m(n,m);
  assert(i<=2*n-2-m); 
  gen_Dyck_i(Dyck, i, 2*n-2);
  gen_random(add_edges, n-i-1, m-(n-1));
  randomEnd();
  return create_outerplanar(Dyck, add_edges, i, n, seed,randomEraseMultipleEdges);
  }





