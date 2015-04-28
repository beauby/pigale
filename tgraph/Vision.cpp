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
#include <TAXI/Tdebug.h>
#include <TAXI/color.h>
#include <TAXI/MaxPath.h>
#include <TAXI/Channel.h>

int BipPlanarize(TopologicalGraph &G, svector<tbrin> &topin,
                 svector<tvertex> &order, svector<tvertex> &orig,
                 tbrin bst); 
 
// finds the leftmost tbrin at s which goes to t
tbrin FindLeftBrin(TopologicalGraph &G, tvertex s, tvertex t)
{ int n=G.nv();
  tbrin b,b0;
  tbrin bst=b0=G.pbrin[s]; 
  if (n>2)
    {if (G.vin[-bst]==t)
      {while (G.vin[-(b=G.cir[bst])]==t)
	if ((bst=b)==b0) break; // all edges from s to t ?
      }
    else
      do
	{if ((bst=G.acir[bst])==b0)
	  break; // no edges from s to t
	}
      while (G.vin[-bst]!=t);
    }
  return (G.vin[-bst]==t)?bst:0; // 0 : not found
}

// finds a tbrin at s which goes to t

tbrin FindBrin(TopologicalGraph &G, tvertex s, tvertex t)
{int n=G.nv();
  tbrin b;
  tbrin bst=G.pbrin[s]; 
  if (n>2)
    {b=bst;
    while (G.vin[-bst]!=t)
      if ((bst=G.cir[bst])==b)
	break;  // no edges from s to t
    }
  return (G.vin[-bst]==t)?bst:0; // 0 : not found
}


int ComputeExtremities(TopologicalGraph &G,svector<tvertex> &orig,
		       svector<int> &x, 
		       svector<int> &x1,svector<int> &x2,
		       svector<int> &x1m, svector<int> &x2m, int morg)
{
  tvertex v;
  tedge e;
  int n=G.nv();
  int m=G.ne();

  int maxxval=0;
  for (e=1; e<=m;e++)
    if (x[e]>maxxval) maxxval=x[e];
  
  x2.clear();x2m.clear();
  for (v=1; v<=n;v++)
      x1[v]=x1m[v]=maxxval+1;
  
  for (e=1;e<=morg;e++)
      {int xe=x[e];
      if (G.vin[e.firsttbrin()]==orig[e.firsttbrin()])
          {
          v=G.vin[e.firsttbrin()];
          if (x1[v]>xe) x1[v]=xe;
          if (x2[v]<xe) x2[v]=xe;
          }
      else
          {
          v=orig[e.firsttbrin()];
          if (x1m[v]>xe) x1m[v]=xe;
          if (x2m[v]<xe) x2m[v]=xe;
          }
      if (G.vin[e.secondtbrin()]==orig[e.secondtbrin()])
	{
          v=G.vin[e.secondtbrin()];
          if (x1[v]>xe) x1[v]=xe;
          if (x2[v]<xe) x2[v]=xe;
          }
      else
          {
          v=orig[e.secondtbrin()];
          if (x1m[v]>xe) x1m[v]=xe;
          if (x2m[v]<xe) x2m[v]=xe;
          }
      if (G.vin[e.secondtbrin()]==14 || orig[e.secondtbrin()]==14)
          {if(debug())DebugPrintf("b:%d, vin: %d orig:%d x=%d -> x1=%d, x2=%d",
		     e.secondtbrin()(), G.vin[e.secondtbrin()](),
		     orig[e.secondtbrin()](),
		     xe,x1[14],x2[14]
		     );
	}
      }
  // For isolated vertices
  for (v=1; v <= n;v++)
      if(x1[v] == maxxval + 1)
          {int xe = x[G.FirstBrin(v).GetEdge()];
          x1[v] = x2[v] = x1m[v] = x2m[v] = xe;
          }
  for(v=1;v<=n;v++)
      {if (x1m[v]>x1[v]) x1m[v]=x1[v];
      if (x2m[v]<x2[v]) x2m[v]=x2[v];
      }
  return maxxval;
}


  // sort parallel edges
void SortParallelEdges(TopologicalGraph &G, svector<tvertex> &orig,
		       svector<int> &x2, svector<int> &y, 
		       svector<int> &x)
{
  int n=G.nv();
  int m=G.ne();
  tvertex v;
  tedge e;

  svector<tedge> up(0,n);
  svector<tedge> link(0,m);
  svector<tedge> left(0,m), tot(0,m), right(0,m);
  svector<int> xvals(0,m);
  tvertex upv;
  for (upv=1; upv <=n; upv++)
    { up.clear();
    link.clear();
    for (e=1;e<=m;e++)
      if (G.vin[e.secondtbrin()]==upv)
          {v=G.vin[e.firsttbrin()];
          link[e]=up[v]; up[v]=e;
          }
    for (v=1; v<n; v++)
        {// fills sorted
        int nl,nt,nr,nv,i,j;
        nl=nt=nr=nv=0;
        e=up[v];
        while(e!=0)
            {xvals[nv++]=x[e];
            if (orig[e.secondtbrin()]==upv)
                {
                tot[nt++]=e;
                }
            else
                {int xv=x2[orig[e.secondtbrin()]];
                if (xv<x[e]) // goes left
                    {for (j=0; j<nl; j++)
                        if (y[orig[e.secondtbrin()]]<y[orig[left[j].secondtbrin()]])
                            break;
                    for (i=nl; i>j; i--)
                        left[i]=left[i-1];
                    left[j]=e;
                    nl++;                   
                    }
                else
                    {for (j=0; j<nr; j++)
                        if (y[orig[e.secondtbrin()]]>y[orig[right[j].secondtbrin()]])
                            break;
                    for (i=nr; i>j; i--)
                        right[i]=right[i-1];
                    right[j]=e;
                    nr++;
                    }
                }
            e=link[e];
            }
        // sort xvals
        for (i=0; i<nv-1;i++)
            {int minv=xvals[i];
            int mini=i;
            for (j=i+1; j<nv; j++)
                { if (xvals[j]<minv)
                    {minv=xvals[j];
                    mini=j;
                    }
                }
            if (mini!=i)
                {xvals[mini]=xvals[i];
                xvals[i]=minv;
                }
            }
        
        // change x values
        i=0;
        for (j=0; j<nl; j++)
            x[left[j]]=xvals[i++];
        for (j=0; j<nt; j++)
            x[tot[j]]=xvals[i++];
        for (j=0; j<nr; j++)
            x[right[j]]=xvals[i++];
        }
    }
}

int Vision(TopologicalGraph &xG,int morg)
  {
  GraphContainer GC(xG.Container());
  TopologicalGraph G(GC);
  int n=G.nv();
  int m=G.ne();
  tedge e;
  tbrin b,b0;
  tvertex v;
  int i;
  bool stadded=false;

  // Computes vertex order, topsort tree, s and t and bst

  svector<tbrin> topin(0,n);
  svector<tvertex> order(0,n-1);
  if (!G.TopSort(topin,order))
      return -1;
  tvertex s=order[0];
  tvertex t=order[n-1];
  tbrin bst = FindBrin(G,s,t);  
  if (bst==0) // no st-edge
    {bst=G.NewEdge(s,t).firsttbrin();stadded=true; ++m;} // add an st-edge

  if (!FindBrin(G,s,order[1]))
      return -2;
    

  // Compute y coords (using non planarized bipolar orientation)

  Prop<int> y(xG.Set(tvertex()),PROP_DRAW_INT_5); y.clear();
  MaxPath *MP=new MaxPath(n,m);
  for (e=1; e <= m; e++)
      MP->insert(G.vin[e.firsttbrin()](),G.vin[e.secondtbrin()](),1);
  MP->solve(y);
  delete MP;
  int maxyval=y[t];
  

  // Recompute order according to y values:
  svector<tvertex> topv(0,n); topv.clear();
  svector<tvertex> link(0,n); link.clear();
  int yv;
  for (i=0; i<n; i++)
    {v=order[i];
    yv=y[v];
    link[v]=topv[yv];
    topv[yv]=v;
    }
  for (yv=n; yv>=0; yv--)
    { if ((v=topv[yv])!=0)
      do
	{
	  order[--i]=v;
	  v=link[v];
	} while (v!=0);
    }
      

  // Planarization

  svector<tvertex> orig(0,n); 
  if (BipPlanarize(G,topin,order,orig,bst)!=0)
    return -3;
  // move orig for added edges
  for (e = morg+1; e<=m; e++)
    {orig[m]=G.vin[m]; orig[-m]=G.vin[-m];}
    
#ifdef TDEBUG
  if (!G.TestPlanar())
    return -4;
#endif
  G.Planarity();
#ifdef TDEBUG
  if (G.ComputeGenus()!=0)
    return -5;
#endif
  bst = FindLeftBrin(G,s,t);
  
  // recompute maxpath for vertices 

  // Compute y coords (using non planarized bipolar orientation)

  y.clear();
  MP=new MaxPath(n,2*m);
  for (e=1; e<=m; e++)
      {MP->insert(G.vin[e.firsttbrin()](),orig[e.secondtbrin()](),1);
      if (orig[e.secondtbrin()]!=G.vin[e.secondtbrin()])
          MP->insert(orig[e.secondtbrin()](),G.vin[e.secondtbrin()](),1);
      }
  MP->solve(y);
  delete MP;
  maxyval=y[t];
  

  // Recompute order according to y values:
  topv.clear();
  link.clear();
  for (i=0; i<n; i++)
      {v=order[i];
      yv=y[v];
      link[v]=topv[yv];
      topv[yv]=v;
      }
  for (yv=n; yv>=0; yv--)
      { if ((v=topv[yv])!=0)
          do
              {
              order[--i]=v;
              v=link[v];
              } while (v!=0);
      }

  
  // compute MaxPath for edges

  svector<int> x(0,m); x.clear();
  MP=new MaxPath(m,2*m);
  svector<tbrin> &Fpbrin=G.ComputeFpbrin();
  for (i=1; i <= Fpbrin.n(); i++)
      { b0=Fpbrin[i];
      if (b0.out())
          while ((b=-G.acir[b0]).out())
              b0=b;
      else
          do
              {b0=G.cir[-b0];}
          while(b0.in());
      if (b0==G.cir[bst])
          continue;
      // référence : e
      e=(G.acir[b0]).GetEdge();
      b=b0;
      if (e<=morg)
	{while (b.out())
	  {if ((!stadded || b!=bst) && b<=morg)
              MP->insert(b.GetEdge()(),e(),1);
	    b=G.cir[-b];
          }
	  while (b.GetEdge()!=e)
	    { if (e<=morg) MP->insert(e(),b.GetEdge()(),0);
	      b=G.cir[-b];
	    }
	}
      }
  MP->solve(x);
  delete &Fpbrin;
  delete MP;

  
//   for (tedge e=G.ne(); e>morg; e--)
//     {
//     G.DeleteEdge(G.ne()); --m;
//     }


  // computes extremities of vertices
  
  Prop<int> x1(xG.Set(tvertex()),PROP_DRAW_INT_1);
  Prop<int> x2(xG.Set(tvertex()),PROP_DRAW_INT_2);
  Prop<int> x1m(xG.Set(tvertex()),PROP_DRAW_INT_3);
  Prop<int> x2m(xG.Set(tvertex()),PROP_DRAW_INT_4);
  
  int maxxval=ComputeExtremities(G,orig,x,x1,x2,x1m,x2m,morg);

  // Performs channel routing
  ChannelRouter CR(n,0,maxxval);
  svector<int> channel(0,n);
  channel.clear();
  svector<int> offset(0,maxyval+1);
  offset[0]=0;
  for (i=0; i<=maxyval; i++)
    {CR.clear();
    for (v=1;v<=n;v++)
      if (y[v]==i)
	CR.insert(v(),x1m[v],x2m[v]);
    offset[i+1]=CR.solve(channel)+offset[i];
    }
  for (v=1; v<=n; v++)
    y[v]=offset[y[v]+1]-channel[v];

  // Sort parallel edges and change x

  SortParallelEdges(G,orig,x2,y,x);

  // recompute true x1,x2, etc

  maxxval=ComputeExtremities(G,orig,x,x1,x2,x1m,x2m,morg);
  maxyval=y[t];


  // NEW 27/04/2005
  
  // Recompute cir

  svector<tedge> first_x(0,maxxval); first_x.clear();
  svector<tedge> next_x(0,m); next_x.clear();
  for (e=1; e<=m; e++)
    { int xe=x[e];
      next_x[e]=first_x[xe];
      first_x[xe]=e;
    }
  G.pbrin.clear();
  
  // Incoming
  for (int xe=0;xe<=maxxval; xe++)
    { e = first_x[xe];
      while (e!=0)
	{ b = e.secondtbrin();
	  v=G.vin[b];
	  //G.vin[b]=v;
	  if (G.pbrin[v]==0) 
	    {G.pbrin[v]=G.cir[b]=G.acir[b]=b;}
	  else 
	    {G.cir[b]=G.pbrin[v]; G.acir[b]=G.acir[G.pbrin[v]];
	      G.acir[G.cir[b]]=b; G.cir[G.acir[b]]=b;
	    }
	  e=next_x[e];
	}
    }
  // Outgoing
  for (int xe=maxxval;xe>=0; xe--)
    { e = first_x[xe];
      while (e!=0)
	{ b = e.firsttbrin();
	  v=G.vin[b];
	  if (G.pbrin[v]==0) 
	    {G.pbrin[v]=G.cir[b]=G.acir[b]=b;}
	  else 
	    {G.cir[b]=G.pbrin[v]; G.acir[b]=G.acir[G.pbrin[v]];
	      G.acir[G.cir[b]]=b; G.cir[G.acir[b]]=b;
	    }
	  e=next_x[e];
	}
    }
  if (!G.DebugCir()) myabort();

  for (b0=1; b0<=morg;b0++)
    { 
      if (x2[orig[-b0]]<=x[b0]) continue;
      b=G.acir[b0];
      tbrin bb=0;
      while (1)
	{ if (b<0 || b==b0) break;
	  if (b<=morg && (x[b]>x2[orig[-b0]] || x[b]<x[b0])) break;
	  if (b<=morg && y[orig[-b0]]<=y[orig[-b]]) bb=b;
	  b=G.acir[b];
	}
      if (bb==0) continue;
      // Make a room
      int xe=x[bb];
      for (tedge f=1; f<=m; f++)
	if (x[f]>xe) x[f]++;
      for (v=1; v<=n; v++)
	{ if (x1[v]>xe) x1[v]++;
	  if (x2[v]>xe) x2[v]++;
	}
      x[b0]=xe+1;
      G.acir[G.cir[b0]]=G.acir[b0]; G.cir[G.acir[b0]]=G.cir[b0];
      G.cir[b0]=bb; G.acir[b0]=G.acir[bb]; 
      G.cir[G.acir[b0]]=G.acir[G.cir[b0]]=b0;
      Prop<short> ecolor(xG.Set(tedge()),PROP_COLOR);
      ecolor[b0]=Red;
    }
  if (!G.DebugCir()) myabort();
  maxxval=ComputeExtremities(G,orig,x,x1,x2,x1m,x2m,morg);


  // Recompute cir

  first_x.resize(0,maxxval);
  first_x.clear();
  next_x.clear();
  for (e=1; e<=m; e++)
    { int xe=x[e];
      next_x[e]=first_x[xe];
      first_x[xe]=e;
    }
  G.pbrin.clear();
  
  // Incoming
  for (int xe=0;xe<=maxxval; xe++)
    { e = first_x[xe];
      while (e!=0)
	{ b = e.secondtbrin();
	  v=G.vin[b];
	  //G.vin[b]=v;
	  if (G.pbrin[v]==0) 
	    {G.pbrin[v]=G.cir[b]=G.acir[b]=b;}
	  else 
	    {G.cir[b]=G.pbrin[v]; G.acir[b]=G.acir[G.pbrin[v]];
	      G.acir[G.cir[b]]=b; G.cir[G.acir[b]]=b;
	    }
	  e=next_x[e];
	}
    }
  // Outgoing
  for (int xe=maxxval;xe>=0; xe--)
    { e = first_x[xe];
      while (e!=0)
	{ b = e.firsttbrin();
	  v=G.vin[b];
	  if (G.pbrin[v]==0) 
	    {G.pbrin[v]=G.cir[b]=G.acir[b]=b;}
	  else 
	    {G.cir[b]=G.pbrin[v]; G.acir[b]=G.acir[G.pbrin[v]];
	      G.acir[G.cir[b]]=b; G.cir[G.acir[b]]=b;
	    }
	  e=next_x[e];
	}
    }
  if (!G.DebugCir()) myabort();

  for (b0=1; b0<=morg;b0++)
    { 
      if (x1[orig[-b0]]>=x[b0]) continue;
      b=G.cir[b0];
      tbrin bb=0;
      while (1)
	{ if (b<0 || b==b0) break;
	  if (b<=morg && (x[b]<x1[orig[-b0]] || x[b]>x[b0])) break;
	  if (b<=morg && y[orig[-b0]]<=y[orig[-b]]) bb=b;
	  b=G.cir[b];
	}
      if (bb==0) continue;
      // Make a room
      int xe=x[bb];
      for (tedge f=1; f<=m; f++)
	if (x[f]>=xe) x[f]++;
      for (v=1; v<=n; v++)
	{ if (x1[v]>=xe) x1[v]++;
	  if (x2[v]>=xe) x2[v]++;
	}
      x[b0]=xe;
      G.acir[G.cir[b0]]=G.acir[b0]; G.cir[G.acir[b0]]=G.cir[b0];
      G.acir[b0]=bb; G.cir[b0]=G.cir[bb]; 
      G.cir[G.acir[b0]]=G.acir[G.cir[b0]]=b0;
      Prop<short> ecolor(xG.Set(tedge()),PROP_COLOR);
      ecolor[b0]=Red;
    }
  if (!G.DebugCir()) myabort();
  maxxval=ComputeExtremities(G,orig,x,x1,x2,x1m,x2m,morg);

  
  // END NEW 27/04/2005

  Prop1<int> maxx(xG.Set(),PROP_DRAW_INT_1);
  Prop1<int> maxy(xG.Set(),PROP_DRAW_INT_2);
  maxx()=maxxval;
  maxy()=maxyval;
  Prop1<Tpoint> pmin(xG.Set(),PROP_POINT_MIN);
  Prop1<Tpoint> pmax(xG.Set(),PROP_POINT_MAX);
  pmin() = Tpoint(-1,-1);
  pmax() = Tpoint(maxxval+1,maxyval+1);

  Prop<Tpoint> P1(xG.Set(tedge()),PROP_DRAW_POINT_1);
  Prop<Tpoint> P2(xG.Set(tedge()),PROP_DRAW_POINT_2);
  for (e=1; e<=morg; e++)
    {
      if (orig[e.firsttbrin()]==G.vin[e.firsttbrin()])
	{P1[e]=Tpoint(x[e],y[orig[e.firsttbrin()]]);
	if (orig[e.secondtbrin()]==G.vin[e.secondtbrin()])
	  P2[e]=Tpoint(x[e],y[orig[e.secondtbrin()]]);
	else
	  P2[e]=Tpoint(x1[orig[e.secondtbrin()]],y[orig[e.secondtbrin()]]);
	}
      else
	{P1[e]=Tpoint(x[e],y[orig[e.secondtbrin()]]);
	P2[e]=Tpoint(x2[orig[e.firsttbrin()]],y[orig[e.firsttbrin()]]);
	}
      }
  return 0;
  }

int NPBipolar(TopologicalGraph &G, tbrin bst);

int EmbedGVision(TopologicalGraph &G)
{ int morg = G.ne();
  if(!G.CheckConnected())G.MakeConnected();
  if(!G.CheckBiconnected())G.NpBiconnect();
  NPBipolar(G,tbrin(1));
  int ret=Vision(G,morg);
  while (G.ne()>morg) G.DeleteEdge(G.ne());
  return ret;
}
