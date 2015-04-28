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

#include <TAXI/Dos.h>
//
// Polar Drawing Computation
//
// Computation by Polar() (with reduction) and Polar0() (without reduction)
//
// Results:
//    Window size -> Prop1<double> PROP_DRAW_DBLE_1 (x) and PROP_DRAW_DBLE_2 (y)
//    Vertex coordinates -> Prop<Tpoint> PROP_DRAW_COORD
//    Edge path ->           
//             Epoint1 : PROP_DRAW_POINT_1
//             Epoint2 : PROP_DRAW_POINT_2
//             Epoint11: PROP_DRAW_POINT_3
//             Epoint12: PROP_DRAW_POINT_4
//             Epoint21: PROP_DRAW_POINT_5
//             Epoint22: PROP_DRAW_POINT_6
//             Erho    : PROP_DRAW_DBLE_1
//             Etheta1 : PROP_DRAW_DBLE_2
//             Etheta2 : PROP_DRAW_DBLE_3
//        if Erho=-1 -> seg from Epoint1 to Epoint2
//        else
//           if Epoint11!=(-1,-1) -> seg path:  Epoint1, Epoint11, Epoint12
//           else                    seg from   Epoint1 to Epoint12
//           if Epoint21!=(-1,-1) -> seg path:  Epoint2, Epoint21, Epoint22
//           else                    seg from   Epoint2 to Epoint22
//           Arc at rho=Erho from angle Etheta1 to angle Etheta2


static double Phi=1.;

static double epush = .5;
static double vpush = .5;
static double extpush=.1;

static double eweight=extpush;
static double beta=1.;


void InvertSomeCir(TopologicalGraph &G, svector<bool> &Invert)
  {
  int m=G.ne();
  svector<tbrin> ncir(-m,m);
  svector<tbrin> nacir(-m,m);
  ncir[0]=nacir[0]=0;
  for (tbrin b=-m; b<=m;b++)
      {if (m==0) continue;
      if (Invert(G.vin[b]))
          { ncir[b]=G.acir[b];
          nacir[b]=G.cir[b];
          }
      else
          {ncir[b]=G.cir[b];
          nacir[b]=G.acir[b];
          }
      }
  ncir.Tswap(G.cir);
  nacir.Tswap(G.acir);
  }

double ComputeCotreeHeights(DoubleOccurenceSequence &Dos, svector<double> &h,
                            svector<int> &level, int )
  {int m=Dos.m;
  int i;
  tbrin b;
  tedge ee,es;
    
  // Compute succ
  svector<tedge> suc(-m,m);      suc.SetName("suc");
  Dos.ComputeSuc(suc);

  svector<double> inith(0,m);
    
  // Init heights of cotree edges
  inith.clear();
  for(ee = 1;ee <= m;++ee)
      if(!Dos.IsTree(ee))
          inith[ee] = (double)Max(level[Dos.vin[ee.firsttbrin()]],
                                  level[Dos.vin[ee.secondtbrin()]]); 
    
  h=inith;
  // second traversal: compute heights
  for(i = 1;i <= 2*m;++i)
      {b = Dos[i];
      ee = b.GetEdge();
      if(Dos.IsTree(ee))
          {es = suc[b];
          if(es == 0)continue;
          if(Dos.vin[b] == Dos.vin[es] || Dos.vin[b] == Dos.vin[-es])
              h[es] = Max((double)level[Dos.vin[b]],h[es]); 
          else
              h[es] = Max((double)level[Dos.vin[b]] + vpush/beta,h[es]); 
          }
      else if (Dos.IsSecond(b))
          {es = suc[ee];
          if (es==0)
              h[0]=Max(h[0],h[ee]);
          else if (h[ee]==inith[ee])
              h[es] = Max(h[ee] + epush/beta,h[es]);
          else
              h[es]=Max(h[ee] + extpush/beta,h[es]);
          }
      }
  return h[0];
  }

double levelrho(double level,double levelmax)
  {return level/levelmax;}

static double levelweight(double level)
  {return 2*asin((Phi/2.)/level);}

void ComputeWeights(TopologicalGraph &G, svector<int> &level,
                    svector<double> &h,
                    svector<double> &weight,
                    svector<double> &weightunder)
  {int n = G.nv(); int m=G.ne();
  Prop<bool> IsTree(G.Set(tedge()),PROP_ISTREE);
  weightunder.clear();
  tvertex v,w;
  tbrin b,b0;
  svector<bool> reached(1,n); reached.SetName("reached");
  reached.clear();
    
  v = 1;  b = b0 = G.pbrin[v]; reached[1]=true;
  do
      {if(IsTree[b.GetEdge()])
          {b = -b;          
          w=G.vin[b];
          if(reached[w])
              {//backtrack
                  weight[v]=weightunder[v];
                  if (weight[v]<=levelweight(level[v]))
                      weight[v]=levelweight(level[v]);
                  // weight[v]+=levelweight(level[v]+1);
                  if (b==-b0) b=0;
                  else
                      weightunder[w]+=weight[v];
              }
          else
              {//montée
                  reached[w]=true;
              }
          v=w;
          }
      else if (h[b.GetEdge()]>level[v])
          {
              weight[n+m+1+b()]=levelweight(level[v]+2)*eweight;
              weightunder[v]+=weight[n+m+1+b()];
          }
      else
          weight[m+n+1+b()]=0;
      if(b()!=0)b = G.cir[b];
      }
  while(b()!=0);
  }


// not finished

void ComputeFather(TopologicalGraph &G, svector<tvertex> &father)
  {int n = G.nv(); int m=G.ne();
  Prop<bool> IsTree(G.Set(tedge()),PROP_ISTREE);
  tvertex v,w;
  tbrin b,b0;
  svector<bool> reached(1,n); reached.SetName("reached");
  reached.clear();
  v = 1;  b = b0 = G.pbrin[v]; father[1]=0; reached[1]=true;
  do
      {if(IsTree[b.GetEdge()])
          {b = -b;          
          w=G.vin[b];
          if(reached[w])  
              {//backtrack
                  if (b==-b0) b=0;
              }
          else
              {//montée
                  father[w]=v;
                  reached[w]=true;
              }
          v=w;
          }
      else
          father[n+m+1+b()]=v;
      if(b()!=0)
          b = G.cir[b];
      }
  while(b()!=0);
  }

void ComputeListe(TopologicalGraph &G, svector<int> &liste)
  {int n=G.nv(); int m=G.ne();
  Prop<bool> IsTree(G.Set(tedge()),PROP_ISTREE);
  svector<bool> reached(1,n); reached.SetName("reached");
  reached.clear();
  tvertex v,w;
  tbrin b,b0;
    
  int j=1;
  liste[1]=1; // on commence par le sommet 1
  v = 1;  b = b0 = G.pbrin[v]; reached[1]=true;
  do
      {if(IsTree[b.GetEdge()])
          {b = -b;          
          w=G.vin[b]; 
          if(reached[w])  
              {//backtrack
                  if (b==-b0) b=0;
              }
          else
              {//montée
                  liste[++j]=w();
                  reached[w]=true;
              }
          v=w;
          }
      else // cotree
          {liste[++j]=m+n+1+b();
          }      
      if(b()!=0)b = G.cir[b];
      }
  while(b()!=0);
  }

void ComputeAngles(TopologicalGraph &G, svector<int> &level, int
                   levelmax,
                   svector<int>&liste, svector<tvertex> &father,
                   svector<double> &weight, svector<double>&weightunder,
                   svector<double> &base, svector<double> &angle)
  {int n = G.nv(); int m=G.ne();
    
  // x occupe (avec ses fils) un angle de base[x] à base[x]+angle[x]
    
  svector<double> basel(1,levelmax+1); basel.clear(); basel.SetName("basel");
  angle[1]=weightunder[1];
  base[1]=0;
  basel[1]=0;
  int ll=1;
  double a;
  int l;
  int x;
  int i;
    
  for (i=2; i<=2*m-n+2; i++)
      {x=liste[i];
      l=level[father[x]]+1;
      // level n'est calculé que pour les sommets et x!=1
      if (l>ll)basel[l]=base[father[x]];//on augmente de niveau
      // base[x]=basel[l];
      //angle[x]=a (cf ->)
      a=weight[x]/weightunder[father[x]]*angle[father[x]];
        
      // restrict !
      angle[x]=weight[x];
      base[x]=basel[l]+(a-angle[x])/2;

      basel[l]+=a;
      ll=l;
      }
  }



double pigaleComputeAngles(TopologicalGraph &G, svector<int> &level, int
                       levelmax,svector<double> &h,
                       svector<double> &base, svector<double> &angle)
  {int n = G.nv(); int m=G.ne();
  
  svector<double> weightunder(1,n); weightunder.SetName("weightunder");  
  svector<double> weight(1,2*m+n+1); weight.SetName("weight");
  ComputeWeights(G,level,h,weight,weightunder);
  
  svector<tvertex> father(1,2*m+n+1); father[1]=0; father.SetName("father");
  ComputeFather(G,father);
  
  svector<int> liste(1,2*m-n+2); liste.SetName("liste");
  ComputeListe(G,liste);

  ComputeAngles(G,level,levelmax,liste,father,weight,weightunder,base,angle);
  // recomputes to normalize.
  beta=1;
  tvertex v;
  
  for (v=2;v<=n;v++)
      beta=Max(beta,angle[v]/(2*acos((double)level[v]/(level[v]+0.5))));
  beta=Max(beta,angle[1]/(2*PI));
  int x;
  
  for (x=1;x<=2*m+n+1;x++)
      {angle[x]/=beta;
      base[x]/=beta;      
      }
  return beta; 
  }
// 0 value is reserved!
template <class T>
class PStack
{
    svector<T> StackTop;
    svector<T> StackNext;
public:
    PStack(int na,int nb,int a,int b): StackTop(na,nb),StackNext(a,b)
        {StackTop.clear();
        StackNext.clear();
        }
    void Push(int n, const T& x)
        {
#ifdef TDEBUG
        if (x==T(0))
            {DPRINTF(("Value 0 reserved (element)!"));
            myabort();
            }
        if (StackNext[x]!=T(0))
            {DPRINTF(("Element %d already in a stack!",x()));
            myabort();
            }
        if (n==0)
            {DPRINTF(("Value 0 reserved (stack)!"));
            myabort();
            }
#endif
        StackNext[x]=StackTop[n];
        StackTop[n]=x;
        }
    T Pop(int n)
        {
#ifdef TDEBUG
        if (n==0)
            {DPRINTF(("Value 0 reserved (stack)!"));
            myabort();
            }
        if (StackTop[n]==T(0))
            {DPRINTF(("Stack %d is empty!",n));
            myabort();
            }
#endif
        T res = StackTop[n];
        StackTop[n]=StackNext[res];
        StackNext[res]=T(0);
        return res;
        }
    bool InStack(int n, const T& x) // x should be there or nowhere!
        {
#ifdef TDEBUG
        if (x==T(0))
            {DPRINTF(("Value 0 reserved (element)!"));
            myabort();
            }
        if (n==0)
            {DPRINTF(("Value 0 reserved (stack)!"));
            myabort();
            }        
#endif
        if (StackTop[n]==x) return true;
        bool found = false;
        for (T y=StackTop[n]; y!=T(0); y=StackNext[y])
            if (x==y)
                { found=true;
                break;
                }
        return found;
        }
    T Next(const T &x)  { return StackNext[x]; }
    T Top(int n)  { return StackTop[n]; }
    // cannot be used to remove the first!
    T RemoveNext(const T &x) 
        { T y = StackNext[x];
        StackNext[x]=StackNext[y];
        StackNext[y]=T(0);
        return y;
        }
    bool Remove(int n, const T&x)
        {
#ifdef DEBUG
        if (x==T(0))
            {DPRINTF(("Value 0 reserved (element)!"));
            myabort();
            }
        if (n==0)
            {DPRINTF(("Value 0 reserved (stack)!"));
            myabort();
            }        
#endif
        if (StackTop[n]==x)
            { StackTop[n]=StackNext[x];
            StackNext[x]=T(0);
            return true;
            }
        bool found = false;
        for (T y=StackTop[n]; StackNext[y]!=T(0); y=StackNext[y])
            if (x==StackNext[y])
                { found=true;
                StackNext[y]=StackNext[x];
                StackNext[x]=0;
                break;
                }
        return found;
        }
    bool TryPop(int n, const T& x)
        {
#ifdef TDEBUG
        if (x==T(0))
            {DPRINTF(("Value 0 reserved (element)!"));
            myabort();
            }
        if (n==0)
            {DPRINTF(("Value 0 reserved (stack)!"));
            myabort();
            }
#endif
        if (StackTop[n]!=x) return false;
        StackTop[n]=StackNext[x];
        StackNext[x]=T(0);
        return true;
        }
    bool Exchange(int n, const T& x,const T& y)
        {
#ifdef TDEBUG
        if (x==T(0) || y==T(0))
            {DPRINTF(("Value 0 reserved (element)!"));
            myabort();
            }
        if (n==0)
            {DPRINTF(("Value 0 reserved (stack)!"));
            myabort();
            }
#endif
        T zz = StackTop[n];
        int found=0;
        if (zz==x)
            {++found;
            StackTop[n]=y;
            }
        else if (zz==y)
            {++found;
            StackTop[n]=x;
            }
        for (T z=zz; StackNext[z]!=T(0);)
            {zz=StackNext[z];
            if (zz==x)
                { StackNext[z]=y;
                if (++found==2) break;
                }
            else if (zz==y)
                { StackNext[z]=x;
                if (++found==2) break;
                }
            z=zz;
            }
        if (found!=2)
            {DPRINTF(("Error!!"));
            myabort();
            }
        if (StackNext[x]==y) StackNext[x]=x;
        if (StackNext[y]==x) StackNext[y]=y;
        Tswap(StackNext[x],StackNext[y]);
        return true;
        }      
    bool IsEmpty(int n)
        {
#ifdef TDEBUG
        if (n==0)
            {DPRINTF(("Value 0 reserved (stack)!"));
            myabort();
            }
#endif
        return (StackTop[n]==T(0));
        }
};


bool MarkBadCir(DoubleOccurenceSequence &Dos, svector<bool> &Bad)
  {int m=Dos.m;
  int n=Dos.n;
  bool BadExists=false;
    
  PStack<tbrin> PS(1,n,-m,m);
  Bad.clear();
  tbrin b;
  int i;
    
  for (i=1; i<=2*m; i++)
      { b = Dos[i];
      if (Dos.IsTree(b.GetEdge())) continue;
      if (Dos.IsFirst(b))
          { PS.Push(Dos.vin[b](),b);
          PS.Push(Dos.vin[-b](),-b);
          }
      else
          {if (!PS.TryPop(Dos.vin[b](),b))
              {Bad[Dos.vin[b]]=true;
              BadExists=true;
              }
          if (!PS.TryPop(Dos.vin[-b](),-b))
              {Bad[Dos.vin[-b]]=true;
              BadExists=true;
              }      
          }
      }
  return BadExists;
  }


bool SwapBadBrins(DoubleOccurenceSequence &Dos)
  {int m=Dos.m;
  int n=Dos.n;
  bool BadExists=false;
    
  PStack<tbrin> PS(1,n,-m,m);
  tbrin b,b2,bb,bb2;
  int i;
  tvertex v;
    
  for (i=1; i<=2*m; i++)
      { b = Dos[i];
      if (Dos.IsTree(b.GetEdge())) continue;
      if (Dos.IsFirst(b))
          { PS.Push(Dos.vin[b](),b);
          PS.Push(Dos.vin[-b](),-b);
          // DPRINTF(("Push %d on vertex %d",b(),Dos.vin[b]()));
          // DPRINTF(("Push %d on vertex %d",-b(),Dos.vin[-b]()));            
          }
      else
          {//DPRINTF(("Now brins %d and %d",b(),-b()));
          if ((b2=PS.Top((v=Dos.vin[b])()))!=b) // b is a second brin
              {BadExists=true;
              while (Dos.IsFirst(b2))
                  {// Swap b2 with b
                      // DPRINTF(("Swap %d (second) and %d (first)",b(),b2()));
                      Dos.SwapBrins(b,b2);
                      b2=PS.Next(b2);
                      // at least we may find b
                  }
              // now, b2 is a second brin
              // If b2 is different from b, exchange b2 and b
              if (b!=b2)
                  {// DPRINTF(("Swap %d (second) and %d (second)",b(),b2()));
                      Dos.SwapBrins(b,b2);
                  }
              // DPRINTF(("Delete %d and continue with it",b2()));
              PS.Remove(v(),b2);
              b=b2;
              }
          else
              { PS.Pop(Dos.vin[b]());
              // DPRINTF(("Pop %d on vertex %d",b(),Dos.vin[b]()));
              }
          if ((b2=PS.Top((v=Dos.vin[-b])()))!=-b) // -b is a first brin
              {BadExists=true;
#ifdef TDEBUG
              if (Dos.IsSecond(b2))
                  {DPRINTF(("%d is a second brin ???",b2()));
                  myabort();
                  }
#endif
              // DPRINTF(("Exchange %d and %d",-b(),b2()));
              Dos.SwapBrins(-b,b2);
              PS.Exchange(v(),-b,b2);
              }
          PS.Pop(v());
          // DPRINTF(("Pop %d on vertex %d",-b(),v()));
          }
      }
  return BadExists;
  }

// POMPOMPOM
int Polar0(TopologicalGraph &G)
  {tvertex v,w;
  int m = G.ne();  int n = G.nv();

  // Compute Height of Cotree edges and vertices
  svector<double> h(0,m);             h.SetName("h");
  svector<int> level(1,n);            level.SetName("level");
  svector<double> myangle(1,2*m+n+1); myangle.SetName("my angle");
  svector<double> mybase(1,2*m+n+1);  mybase.SetName("my base");
  Prop1<double> nw(G.Set(),PROP_DRAW_DBLE_1);

  double maxh,nbeta;
      
  tbrin b0=G.pbrin[1];
  DoubleOccurenceSequence Dos(G,b0);

  // Compute level of vertices
  int lmax=Dos.ComputeLevel(level,b0);

  Dos.MoveStart();
  maxh=Max(lmax+0.2,ComputeCotreeHeights(Dos,h,level,lmax));
  nbeta=pigaleComputeAngles(G, level, lmax,h, mybase, myangle);
  
  // recompute heights.
  maxh=Max(lmax+0.2,ComputeCotreeHeights(Dos,h,level,lmax));
  
  // compute node width
  nw() = (.5+75./maxh/beta)/600.;

  // Compute coords of vertices
  Prop<Tpoint> Vcoord(G.Set(tvertex()),PROP_DRAW_COORD);
  Vcoord.SetName("Vcoord");
  
  Vcoord[1].x() = Vcoord[1].y() = 0;
  for(v = 2;v <= n;v++)
      {Vcoord[v].x()=
	 levelrho(level[v],maxh)*cos((mybase[v]+myangle[v]/2)); 
      Vcoord[v].y()=
	levelrho(level[v],maxh)*sin((mybase[v]+myangle[v]/2)); 
      }
  
  // Prepare Drawing

  Prop<Tpoint> Epoint1(G.Set(tedge()),PROP_DRAW_POINT_1);
  Prop<Tpoint> Epoint2(G.Set(tedge()),PROP_DRAW_POINT_2);
  Prop<Tpoint> Epoint11(G.Set(tedge()),PROP_DRAW_POINT_3);
  Prop<Tpoint> Epoint12(G.Set(tedge()),PROP_DRAW_POINT_4);
  Prop<Tpoint> Epoint21(G.Set(tedge()),PROP_DRAW_POINT_5);
  Prop<Tpoint> Epoint22(G.Set(tedge()),PROP_DRAW_POINT_6);
  Prop<double> Erho   (G.Set(tedge()),PROP_DRAW_DBLE_1);
  Prop<double> Etheta1(G.Set(tedge()),PROP_DRAW_DBLE_2);
  Prop<double> Etheta2(G.Set(tedge()),PROP_DRAW_DBLE_3);
  // pour Qt
  Prop1<Tpoint> pmin(G.Set(),PROP_POINT_MIN);
  Prop1<Tpoint> pmax(G.Set(),PROP_POINT_MAX);

  tbrin bv,bw;
  tedge ee;
  double theta1,theta2,rho;

  for (ee=1; ee<=m; ee++)
      {bv=Dos.First(ee);
      bw=Dos.Second(ee);
      v=Dos.vin[bv];
      w=Dos.vin[bw];
      Epoint1[ee]=Vcoord[v];
      Epoint2[ee]=Vcoord[w];
      
      if (Dos.IsTree(ee))  // tree edges
          {
              Erho[ee]=-1;
              continue;
          }
      Tpoint pi;
      double rhoi;
      
      theta1 = mybase[m+n+1+bv()]+myangle[m+n+1+bv()]/2;
      rho = levelrho(h[ee],maxh);
      Tpoint p = Tpoint(rho*cos(theta1),rho*sin(theta1));
      rhoi=levelrho(level[v]+1.0,maxh);
      pi=Tpoint(rhoi*cos(theta1),rhoi*sin(theta1));
      if (level[v]+1.0<h[ee])
          {Epoint11[ee]=pi;
          Epoint12[ee]=p;
          }
      else if (h[ee]!=(double)level[v])
          {Epoint11[ee]=Tpoint(-1,-1);
          Epoint12[ee]=p;
          }
      else
          {Epoint11[ee]=Epoint12[ee]=Tpoint(-1,-1);
          theta1=mybase[v]+myangle[v]/2;
          }
      theta2 = mybase[m+n+1+bw()]+myangle[m+n+1+bw()]/2;
      p = Tpoint(rho*cos(theta2),rho*sin(theta2));
      rhoi=levelrho(level[w]+1.0,maxh);
      pi=Tpoint(rhoi*cos(theta2),rhoi*sin(theta2));
      if (level[w]+1.0<h[ee])
          { Epoint21[ee]=pi;
          Epoint22[ee]=p;
          }
      else if (h[ee]!=(double)level[w])
          {Epoint21[ee]=Tpoint(-1,-1);
          Epoint22[ee]=p;
          }
      else
          {Epoint21[ee]=Epoint22[ee]=Tpoint(-1,-1);
          theta2 = mybase[w]+myangle[w]/2;
          }
      
      if(theta2 < theta1) theta2 += 2*PI;
      Erho[ee]=rho;
      Etheta1[ee]=theta1;
      Etheta2[ee]=theta2;
      }
  pmax().x() = pmax().y() = 1.05;
  pmin().x() = pmin().y() = -1.05;
  return 0;
  }

int Polar(TopologicalGraph &G)
  { 
  // Mark the edges of a BFS tree
  G.Set().erase(PROP_CONNECTED);
  if(!G.CheckConnected())
      {setPigaleError(-1,"Polar: graph not connected");
      return -1;
      }
  
  //G.Planarity();
  
  int n = G.nv();
  tbrin b0=G.pbrin[1];
  DoubleOccurenceSequence Dos(G,b0);
  svector<bool> Invert(1,n);
  if (SwapBadBrins(Dos))
      {if(debug())DebugPrintf("Swaped some brins");
      Dos.Compute(b0);
      if (MarkBadCir(Dos,Invert))
          DebugPrintf("Polar:dos still bad!");
      }
  Polar0(G);
  return 0;
  }
