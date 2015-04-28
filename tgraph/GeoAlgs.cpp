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
#include <TAXI/smatrix.h>
#include <TAXI/Tstring.h>
#include <TAXI/Tdebug.h>
#include <TAXI/Tpoint.h>
#include <TAXI/graphs.h>
#include <TAXI/color.h>
#include <TAXI/Tmessage.h>

//GEOMETRICGRAPH *************************************************

struct OrderedBrin
{double angle;
  tbrin brin;
  OrderedBrin & operator=(const OrderedBrin &x)
      {angle=x.angle; brin=x.brin;return *this;}
};
int compare(const void *b1, const void *b2)
  {if (((OrderedBrin *)b1)->angle > ((OrderedBrin *)b2)->angle) return -1;
  else if (((OrderedBrin *)b1)->angle == ((OrderedBrin *)b2)->angle) return 0;
  else return 1;
  }
void GeometricGraph::init()
  {ecolor.SetName("ecolor");ewidth.SetName("ewidth");elabel.SetName("elabel");
  vcolor.SetName("vcolor");vlabel.SetName("vlabel");vcoord.SetName("vcoord");

  if(!nv()) maxvlabel=0;
  else
      {maxvlabel=vlabel[1];
      for (tvertex v=2; v<=nv(); v++)
          if (maxvlabel < vlabel[v]) maxvlabel=vlabel[v];
      }
  if(!ne()) maxelabel=0;
  else
      {maxelabel=elabel[1];
      for (tedge e=2; e <= ne(); e++)
          if (maxelabel < elabel[e]) maxelabel=elabel[e];
      }
  }
tedge GeometricGraph::FindEdge(const Tpoint &p,double node_radius) const
  {Tpoint ps, pt;
  double distance;

  for (tedge e=1; e<=ne(); e++)
      {ps = vcoord[vin[-e]]; pt = vcoord[vin[e]];
      distance = dist_seg((const Tpoint &)p,(const Tpoint &)ps,(const Tpoint &)pt);
      if(distance < 0.25 * node_radius) return e;
      }
  return 0;
  }
tedge GeometricGraph::FindEdge(const Tpoint &p) const
  {Tpoint ps, pt;
  double d,d_min = DBL_MAX;
  tedge e0=0;
  if(!ne()) return 0;
  for (tedge e=1; e<=ne(); e++)
      {ps = vcoord[vin[-e]]; pt = vcoord[vin[e]];
      d = dist_seg((const Tpoint &)p,(const Tpoint &)ps,(const Tpoint &)pt);
      if(d < d_min){d_min = d;e0 = e;}
      }
  return e0;
  }
tvertex GeometricGraph::FindVertex(const Tpoint & p,double node_radius) const
  {for (tvertex v=1; v<=nv(); v++)
      {Tpoint q = vcoord[v];
      if(Distance((const Tpoint &)p,(const Tpoint &)q) < node_radius) return v;
      }
  return 0;
  }
int GeometricGraph::ComputeGeometricCir()
  {if(debug())DebugPrintf("    ComputeGeometricCir");
  tvertex v;
  tbrin b0,b,opp;
  Tpoint p;
  int degree;
  for(v = 1;v <= nv();v++)
      {degree = Degree(v);
      svector<OrderedBrin> ob(degree);
      if((b0 = pbrin[v]) == 0)continue;
      // Put adjacent brins in the array.
      int i = 0; b = b0;
      p = vcoord[v];
      do
          {ob[i].brin = b;
          ob[i].angle = Angle(vcoord[vin[-b]]-p);
          b = cir[b]; i++;
          }while (b != b0);

      if(!i)continue;
      qsort(ob.begin(), degree, sizeof(OrderedBrin), compare);

      // rewrite cir and acir
      // cir is COUNTERCLOCKWISE.
      for (i = 0; i < degree - 1; i++)
          {cir[ob[i+1].brin] = ob[i].brin;
          acir[ob[i].brin] = ob[i+1].brin;
          }
      cir[ob[0].brin] = ob[degree - 1].brin;
      acir[ob[degree - 1].brin] = ob[0].brin;
      }
  int genus = ComputeGenus();
  Prop1<int> maptype(Set(),PROP_MAPTYPE);
  maptype() = PROP_MAPTYPE_GEOMETRIC;
  if(genus == 0){planarMap() = 1; extbrin() = FindExteriorFace();}
  else planarMap() = -1;
  return genus;
  }
void ComputeGeometricCir(GeometricGraph &G,svector<tbrin> &cir)
  {tvertex v;
  tbrin b0,b,opp;
  Tpoint p;
  int degree;
  
  for(v = 1;v <=G. nv();v++)
      {degree = G.Degree(v);
      svector<OrderedBrin> ob(degree);
      
      if((b0 = G.pbrin[v]) == 0)continue;
      // Put adjacent brins in the array.
      int i = 0; b = b0;
      p = G.vcoord[v];
      do
          {ob[i].brin = b;
          ob[i].angle = Angle(G.vcoord[G.vin[-b]]-p);
          b = G.cir[b]; i++;
          }while (b != b0);

      if(!i)continue;
      qsort(ob.begin(), degree, sizeof(OrderedBrin), compare);

      // rewrite cir which is COUNTERCLOCKWISE.
      for (i = 0; i < degree - 1; i++)
          cir[ob[i+1].brin] = ob[i].brin;
      cir[ob[0].brin] = ob[degree - 1].brin;
      }
  cir[0] = 0;
  }
tbrin GeometricGraph::FindExteriorFace()
  {//find leftmost vertex with edges
  if(!ne())return 0;
  double x = DBL_MAX;
  tvertex lv=0;
  for(tvertex v = 1; v <= nv();v++)
      if(vcoord[v].x() < x && (pbrin[v]!=0)){x = vcoord[v].x();lv = v;}

  //find most vertical brin at lv
  double x0 = 0;
  tbrin b,b0,lb;
  b = lb = b0 = pbrin[lv];
  Tpoint plv = vcoord[lv];
  bool positif;
  do
      {x = Angle(vcoord[vin[-b]] - plv);
      positif = (x <= .25) ? true : false;
      if(x > .5)x = 1 - x;
      if(x > x0)
          {x0 = x; lb = b;
          if(positif)lb = -lb;
          }
      }while((b = cir[b]) != b0);
  return cir[-lb];
  }
tbrin  GeometricGraph::FindExteriorFace(Tpoint& pp)
// set extbrin() or returns 0
  {tedge e0 = FindEdge(pp);
  if(e0 == 0)return 0;
  tvertex v1 = vin[e0];	  tvertex v2 = vin[-e0];
  bool right = (Determinant(pp - vcoord[v1],vcoord[v2] - vcoord[v1]) > 0) ? true:false;
  tvertex vh = v2;
  if(ComputeGeometricCir())return 0;
  if(vcoord[v1].y() > vcoord[v2].y()
     || ((vcoord[v1].y() == vcoord[v2].y()) && (vcoord[v1].x() > vcoord[v2].x())) )
      {vh = v1;
      right  = !right;
      }
  tbrin b = ((right && vh == v2) || (!right && vh == v1)) ? e0() : -e0();
  extbrin() = b;
  return b;
  }
int GeometricGraph::ColorExteriorface()
  {if(!FindPlanarMap())
      {DebugPrintf("Exterior face of non planar graph");return -1;}
  short ecol=0;  ecolor.getinit(ecol);
  int width=0; ewidth.getinit(width);
  tedge e;
  for(e=1; e<= ne();e++){ecolor[e] = ecol; ewidth[e] = width;}
  tbrin b0 = extbrin();
  tbrin b = b0;
  int len = 1;
  while((b = cir[-b]) != b0)
      {e = b.GetEdge();
      ecolor[e] = Red;      ewidth[e] = 2;
      ++len;
      }
  e = b0.GetEdge();
  ecolor[e] = Green; ewidth[e] = 2;
  return len;
  }
double GeometricGraph::MinMaxCoords(double &xmin,double &xmax, double &ymin,double &ymax)
  {if(nv() < 1)return .0;
  tvertex v = 1;
  xmin = xmax = vcoord[v].x();
  ymin = ymax = vcoord[v].y();
  for(v = 2; v <= nv();v++)
      {xmin = Min(xmin,vcoord[v].x());xmax = Max(xmax,vcoord[v].x());
      ymin = Min(ymin,vcoord[v].y());ymax = Max(ymax,vcoord[v].y());
      }
  return Max(xmax-xmin,ymax-ymin);
  }

GraphContainer * GeometricGraph::GeometricDualGraph()
  {if(!CheckConnected() || !CheckPlanar())
      return (GraphContainer *)0;
  tbrin extb = FindExteriorFace();
  if (!extb) return (GraphContainer *)0;
  int LenExtFace = FaceLength(extb);
  GraphContainer & Dual = *new GraphContainer;
  int m = ne();
  int n = nv();
  int nn = m-n+1;
  int mm =m - LenExtFace;
  Dual.setsize(nn,mm);
  Prop1<tstring> title(Set(),PROP_TITRE);
  Prop1<tstring> titleD(Dual.Set(),PROP_TITRE);
  titleD() = "D-" + title();

  tvertex u,v;
  tbrin b,bb;
  Prop<tvertex> dvin(Dual.PB(),PROP_VIN);      dvin.clear();
  Prop<Tpoint> dcoord(Dual.PV(),PROP_COORD);   dcoord.clear();
  Prop<int> dvlabel(Dual.PV(),PROP_LABEL);
  Prop<int> delabel(Dual.PE(),PROP_LABEL);
  svector<int> fdegree(1,nn);    fdegree.clear();
  svector<int> mark(-m,m);       mark.clear();
  svector<tedge> DualEdge(1,m);  DualEdge.clear();

  // mark the exterior face with 2
  mark[0] = 2;
  b = extb;
  do
      mark[b] = 2;
  while((b = cir[-b]) != extb);

  //DualEdge indicates the corresponding dual edge
  tvertex nf = 0;
  tedge nb = 0;
  tedge absbb;
  int md = 0;
  for (b = -m; b <= m; b++)
      {if(mark[b])continue;
      bb = b;
      ++nf;
      do
          {if(mark[bb] != 2)mark[bb] = 1;
          absbb = bb.GetEdge();
          if(mark[bb] != 2 &&  mark[-bb] != 2)
              {if(!DualEdge[absbb])          
                  {dvin[++nb] = nf;DualEdge[absbb] = nb;++md;}
              else
                  dvin[-DualEdge[absbb]] = nf;
              }
          fdegree[nf]++;
          v = vin[bb];
          dcoord[nf] += vcoord[v];
          }while((bb = cir[-bb]) != b);
      }

  if(mm != md)
      {delete & Dual; return (GraphContainer *)0;}

  // calculation of coordinates
  for (v=1; v<=Dual.nv(); v++)
      dcoord[v] /= (double)fdegree[v];
  for (v=1; v<=Dual.nv(); v++)
      dvlabel[v]=v();
  for (tedge e=1; e<=Dual.ne(); e++)
      delabel[e]=e();

  return &Dual;
  }
GraphContainer * GeometricGraph::DualGraph()
  {if(!CheckConnected() || !CheckPlanar()) return (GraphContainer *)0;
  GraphContainer & Dual = *new GraphContainer;
  int m = ne();
  int n = nv();
  Dual.setsize(m-n+2,m);
  Prop1<tstring> title(Set(),PROP_TITRE);
  Prop1<tstring> titleD(Dual.Set(),PROP_TITRE);
  titleD() = "D-" + title();

  tvertex u,v;
  tbrin b,bb;
  Prop<tvertex> dvin(Dual.PB(),PROP_VIN);    dvin.clear();
  Prop<Tpoint> dcoord(Dual.PV(),PROP_COORD); dcoord.clear();
  Prop<int> dvlabel(Dual.PV(),PROP_LABEL);
  Prop<int> delabel(Dual.PE(),PROP_LABEL);
  svector<int> fdegree(1,m-n+2);             fdegree.clear();
  tvertex nf = 0;
  for (b = -m; b <= m; b++)
      {if(dvin[b]() || !b()) continue;
      bb = b;
      ++nf;
      do
          {dvin[bb] = nf;
          fdegree[nf]++;
          v = vin[bb];
          dcoord[nf] += vcoord[v];
          bb = -cir[bb];
          } while(bb != b);
      }

  if (nf != m-n+2) 
      {delete &Dual;return (GraphContainer *)0;}

  // Computation of coordinates
  for (v=1; v<=Dual.nv(); v++)
      dcoord[v] /= (double)fdegree[v];
  for (v=1; v<=Dual.nv(); v++)
      dvlabel[v]=v();
  for (tedge e=1; e<=Dual.ne(); e++)
      delabel[e]=e();

  return &Dual;
  }

GraphContainer * GeometricGraph::GeometricAngleGraph()
  {if(!CheckConnected() || !CheckPlanar())return (GraphContainer *)0;
  GraphContainer & Angle = *new GraphContainer;
  tbrin extb = FindExteriorFace();
  int LenExtFace = FaceWalkLength(extb);
  int m = ne();
  int n = nv();
  int nn = m + 1; //No exterior face
  int mm = 2*m- LenExtFace;
  //    if(!mm)return (Graph *)0;
  Angle.setsize(nn,mm);
  Prop1<tstring> title(Set(),PROP_TITRE);
  Prop1<tstring> titleD(Angle.Set(),PROP_TITRE);
  titleD() = "A-" + title();

  tvertex u,v;
  tbrin b,bb;
  Prop<tvertex> dvin(Angle.PB(),PROP_VIN);     dvin.clear();
  Prop<Tpoint> dcoord(Angle.PV(),PROP_COORD);  dcoord.clear();
  Prop<int> dvlabel(Angle.PV(),PROP_LABEL);         
  Prop<int> delabel(Angle.PE(),PROP_LABEL);
  svector<int> fdegree(n,m+1);                 fdegree.clear();
  svector<int> mark(-m,m);                     mark.clear();

  tvertex nf = n;
  // mark the exterior face
  mark[0] = 1;
  b = extb;
  do
      mark[b] = 1;
  while((b = cir[-b]) != extb);

  //  brins compute coord vertices in faces
  tbrin nb = 0;
  for (b = -m; b <= m; b++)
      {if(mark[b]) continue;
      bb = b;
      ++nf;
      do 
          {mark[bb] = 1;
          fdegree[nf]++;
          v = vin[bb];
          dcoord[nf] += vcoord[v];
          //Create an edge
          dvin[++nb] = nf;
          dvin[-nb] = v;
          }while((bb = cir[-bb]) != b);
      }

  if(nb != mm){delete &Angle; return (GraphContainer *)0;}

  // Computation  of coordinates
  for (v=1; v <= n; v++)
      dcoord[v] = vcoord[v];
  for (v=n+1; v<=Angle.nv(); v++)
      dcoord[v] /= (double)fdegree[v];
  for (v=1; v<=Angle.nv(); v++)
      dvlabel[v]=v();
  for (tedge e=1; e<=Angle.ne(); e++)
      delabel[e]=e();

  return &Angle;
  }
GraphContainer * GeometricGraph::AngleGraph()
  {if(!CheckConnected() || !CheckPlanar())return (GraphContainer *)0;
  int m = ne();
  int n = nv();
  int nn = m + 2; //No exterior face
  int mm = 2*m; //   - LenExtFace;
  GraphContainer & Angle = *new GraphContainer;
  Angle.setsize(nn,mm);
  Prop1<tstring> title(Set(),PROP_TITRE);
  Prop1<tstring> titleD(Angle.Set(),PROP_TITRE);
  titleD() = "A-" + title();

  tvertex u,v;
  tbrin b,bb;
  Prop<tvertex> dvin(Angle.PB(),PROP_VIN);    dvin.clear();
  Prop<Tpoint> dcoord(Angle.PV(),PROP_COORD); dcoord.clear();
  Prop<int> dvlabel(Angle.PV(),PROP_LABEL);   
  Prop<int> delabel(Angle.PE(),PROP_LABEL);   
  svector<int> fdegree(n,nn); fdegree.clear();      
  svector<int> mark(-m,m);  mark.clear();   mark.SetName("Mark");
  tvertex nf = n;
  tedge newe;
  mark[0] = 1;

  //  brins compute coord vertices in faces
  for (b = -m; b <= m; b++)
      {if(mark[b]) continue;
      bb = b;
      ++nf;
      do
          {mark[bb] = 1;
          fdegree[nf]++;
          v = vin[bb];
          dcoord[nf] += vcoord[v];
          // edge number
          newe = (bb<0) ? -2*bb()-1 : 2*bb();
          // orientation
          if (bb()*acir[bb]() > 0) // extremal angle
              {dvin[newe] = v; dvin[-newe] = nf;}
          else
              {dvin[-newe] = v; dvin[newe] = nf;}
          }while((bb = cir[-bb]) != b);
      }

  // Computation  of coordinates
  for (v=1; v <= n; v++)
      dcoord[v] = vcoord[v];
  for (v=n+1; v<=Angle.nv(); v++)
      dcoord[v] /= (double)fdegree[v];
  for (v=1; v<=Angle.nv(); v++)
      dvlabel[v]=v();
  for (tedge e=1; e<=Angle.ne(); e++)
      delabel[e]=e();

  return &Angle;
  }

int GeometricGraph::Tutte()
  {if(NumberOfConnectedComponents()!=1){return 0;}
  RemoveLoops();
  if(ComputeGeometricCir()) // if genus is non-zero...
      return 0;

  // mark all the vertices incident to the outer face.
  tbrin FBrin = FindExteriorFace();
  tbrin b = FBrin;
  svector<int> marked(1,nv(),0);
  do  {marked[vin[b]]=1; b=cir[-b];} while(b != FBrin);

  SMatrix M(nv());
  svector<double> a(nv()), x(nv()), y(nv());
  double d;
  tvertex v;
  M.clear();
  a.clear();
  
  ForAllVerticesOfG(v)
      {if (marked[v]) 
          {M[v()-1][v()-1]= 1.; a[v()-1]=vcoord[v].x();}
      else 
          {M[v()-1][v()-1] = 1.; d = -1.0/Degree(v);
          Forall_adj_brins_of_G(b,v) 
              M[v()-1][vin[-b]()-1] = d; 
          }
      }
  int ok;
  SMatrix Inv = M.Inverse(ok);
  
  x = Inv * a;
  ForAllVerticesOfG(v) if (marked[v]) a[v()-1]=vcoord[v].y();
  y = Inv * a;
  ForAllVerticesOfG(v) vcoord[v] = Tpoint(x[v()-1], y[v()-1]);
  return ok;
  }

void ColorPoles(GeometricGraph &G)
  {for(tvertex v = 1; v <= G.nv();v++)
      {int Out = G.OutDegree(v);
      int In = G.InDegree(v);
      if(In == 0 && Out == 0)G.vcolor[v] = Pink;
      else if(In == 0)G.vcolor[v] = Green;
      else if(Out == 0)G.vcolor[v] = Red;
      else G.vcolor[v] = Yellow;
      }
  }

//  CheckCoordNotOverlap
void HeapSort(int (*f)(int a,int b),int first,int nelements,int *heap);

static bool Equal(double x, double y);
static bool Less(double x, double y);
static int Cmp(int i,int j);
static double Epsilon = 2.;
static double *xcoord, *ycoord;

bool Equal(double x, double y)
  {if(fabs(x-y) <= Epsilon)return true;
  return false;
  }
bool Less(double x, double y)
  {if(!Equal(x,y) && x < y)return true;
  return false;
  }
int Cmp(int i,int j)
  {double ax = xcoord[i];
  double ay = ycoord[i];
  double bx = xcoord[j];
  double by = ycoord[j];
  if(Less(ax,bx))return 1;
  if(Equal(ax,bx) && Less(ay,by))return 1;
  return 0;
  }
bool CheckCoordNotOverlap(GeometricGraph & G)
  {bool ok = true;
  int n = G.nv();
  int *heap = new int[n+1];xcoord = new double[n+1];ycoord = new double[n+1];
  int i;
  for(i = 1;i <= n;i++)
      {xcoord[i] = G.vcoord[i].x();
      ycoord[i] = G.vcoord[i].y();
      }
  HeapSort (Cmp,1,n,heap);
  double pos,prevpos;
  prevpos = G.vcoord[heap[0] + 1].x();
  int i0 = 0;
  for(i = 1;i < n;i++)
      {pos = G.vcoord[heap[i] + 1].x();
      if(Equal(pos,prevpos) &&  G.vcoord[heap[i] + 1].y() == G.vcoord[heap[i0] + 1].y())
          {ok = false;   
          break;
          }
      prevpos = pos;
      i0 = i;
      }
  delete [] heap;delete [] xcoord;delete [] ycoord;
  return ok;
  }

