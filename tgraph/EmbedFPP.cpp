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
#include <TAXI/SchPack.h>
#include <TAXI/MaxPath.h>

#define CONE 0
#define HOR_LEFT 1
#define HOR_RIGHT 2

int EmbedFPP_Rect(TopologicalGraph &G,bool schnyderLongestFace);


class Constraints: public TopologicalGraph
{public:
  Constraints(Graph &G);
  ~Constraints();
  void init(tbrin ee);
  void add(tbrin ee);
  void deplace(tvertex start,int length,tvertex iv);
  tvertex up(tvertex vi,tvertex iv);
  int monot_up(tbrin lee,tbrin ree);
  tbrin monot_dwn(tbrin lee,tbrin ree);
  void ComputeCoord(tvertex iv,tvertex iv1,tvertex iv2,int forme);
  void ComputeCoord_Rect(tvertex iv,tvertex iv1,tvertex iv2,int forme);

  svector<int>x,y;
  bool ok;
private:
  svector<tvertex>push1,push2,push3,push_from,push_by;
};
Constraints:: ~Constraints()
  {}
Constraints:: Constraints(Graph &G) :
    TopologicalGraph(G),ok(true)
  {push1.resize(0,nv());         push1.clear();       push1.SetName("push1");
  push2.resize(0,nv());          push2.clear();       push2.SetName("push2");
  push3.resize(0,nv());          push3.clear();       push3.SetName("push3");
  push_from.resize(1,nv());      push_from.clear();   push_from.SetName("push_from"); 
  push_by.resize(1,nv());        push_by.clear();     push_by.SetName("push_by"); 
  x.resize(1,nv());              x.clear();           x.SetName("x");
  y.resize(1,nv());              y.clear();           y.SetName("y");  
  }
void Constraints::init(tbrin ee)
  {tvertex iv1,iv2,iv3;
  iv1 = vin[ee];iv2 = vin[-ee];iv3 = vin[-acir[ee]];
  push1[iv3] = iv2;push_from[iv2] = iv3;push_by[iv2] = 0;
  }
void Constraints::add(tbrin ee)
//  iv2=G.vin[-ee] poussera le sommet iv1=G.vin[ee]
  {tvertex iv1,iv2,iv3;
  iv1 = vin[ee];iv2 = vin[-ee];
  iv3 = push_from[iv1];
  push_from[iv1] = iv2;
  if(push1[iv3] == iv1)push1[iv3]=0;
  if(push2[iv3] == iv1)push2[iv3]=0;
  if(push3[iv3] == iv1)push3[iv3]=0;
  if(push1[iv2] != 0  && push2[iv2] !=0 && push3[iv2] !=0)
      {ok = false;}
  if(push1[iv2] == 0)
      push1[iv2] = iv1;
  else if(push2[iv2] == 0)
      push2[iv2] = iv1;
  else
      push3[iv2] = iv1;
  }
void Constraints::deplace(tvertex start,int length,tvertex iv)
  {tvertex vi,vii;
  vi = start;
  if(push_by[vi] != iv)
      {push_by[vi] = iv;x[vi] += length;}
  for(;;)
      {if((vii = up(vi,iv)) != 0)
	  {vi = vii;x[vi] += length;}
      else if(vi == start)
	  return;
      else
	  vi = push_from[vi];
      }
  }
tvertex Constraints::up(tvertex vi,tvertex iv)
  {tvertex vi_up;

  if(((vi_up = push1[vi]) != 0) && (push_by[vi_up] != iv))
      {push_by[vi_up] = iv;return(vi_up);}
  if(((vi_up = push2[vi]) != 0) && (push_by[vi_up] != iv))
      {push_by[vi_up] = iv;return(vi_up);}
  if(((vi_up = push3[vi]) != 0) && (push_by[vi_up] != iv))
      {push_by[vi_up] = iv;return(vi_up);}
  return 0;
  }
int Constraints::monot_up(tbrin lee,tbrin ree)
  {for(tbrin ee = lee;ee != ree;ee = acir[-acir[ee]])
      if(y[vin[ee]] > y[vin[acir[-acir[ee]]]])return 0;
  return 1;
  }
tbrin Constraints::monot_dwn(tbrin lee,tbrin ree)
  {tbrin  ee = acir[-acir[lee]];
  for(;;)
      {if(ee == ree)
	  return(ee);
      else if(y[vin[ee]] <= y[vin[lee]])
	  {lee = ee;ee = acir[-acir[ee]];}
      else
	  return lee;
      }
  }
void Constraints::ComputeCoord(tvertex iv,tvertex iv1,tvertex iv2,int forme)
 {int x1,x2,y1,y2;
 x1 = x[iv1];y1 = y[iv1];x2 = x[iv2];y2 = y[iv2];
 if(forme == HOR_LEFT)
     {y[iv] = y2;
     x[iv] = x1 + y2 - y1;
     }
 else if(forme == HOR_RIGHT)
     {y[iv] = y1;
     x[iv] = x2 + y2 -y1;
     }
 else
     {x[iv] = (x1 + x2 + y2 -y1)/2;
     y[iv] = (y1 + y2 + x2 -x1)/2;
     }
 }
void Constraints::ComputeCoord_Rect(tvertex iv,tvertex iv1,tvertex iv2,int forme)
 {int x1,x2,y1,y2;
 x1 = x[iv1];y1 = y[iv1];x2 = x[iv2];y2 = y[iv2];
 if(forme == HOR_LEFT)
     {y[iv] = y2;
     x[iv] = x1;
     }
 else if(forme == HOR_RIGHT)
     {y[iv] = y1;
     x[iv] = x2 + y2 -y1;
     }
 else
     {x[iv] = x1;
     y[iv] = x2 - x1 + y2 ;
     }
 }
//***********************************************************************
/*    x[iv],y[iv] tableau des coordonnees
        push_by[iv] indique le sommet que l'on place et qui entraine que l'on
                doive pousser iv
        push1[iv], push2[iv], push3[iv] indiquent quels sommets iv pousse
        push_from[iv] est le sommet qui pousse iv
*/


int EmbedFPP(TopologicalGraph &G,bool schnyderRect,bool schnyderLongestFace)
  {if(G.nv() < 3)return -1;
  if(!G.CheckSimple())return -1;
  if(schnyderRect)return EmbedFPP_Rect(G,schnyderLongestFace);
  int OldNumEdge = G.ne();
  PSet1  propSave(G.Set());
  G.MakeConnected();
  if(!G.CheckPlanar())return -1;
  bool MaxPlanar = (G.ne() != 3 * G.nv() - 6) ? false : true;
  int len;
  tbrin ee;
  if(schnyderLongestFace && !MaxPlanar)
      {G.LongestFace(ee,len);G.extbrin() = ee;}
  else
      ee = G.extbrin();
  if(!MaxPlanar && G.ZigZagTriangulate() < 0)return -2;
  tbrin left,right,new_push;
  tvertex iv,iv1,iv2,iv3;
  tvertex ivl,nivl,ivr,pivr;
  int move_l,move_r,forme;
  //init with the leftmost brin incident to tha last vertxex to be packed
  SchnyderPacking SP(G,-G.acir[ee]);
  // Skip first two vertices
  SP.FindVertex();  SP.FindVertex();
  Constraints cs(G);           cs.init(ee);
  iv1 = G.vin[ee];iv2 = G.vin[-ee];iv3 = G.vin[-G.acir[ee]];
  cs.x[iv1] = 1;cs.x[iv2] = 3;
  for(tvertex ivn = 3;ivn <= G.nv();ivn++)
      {if((iv = SP.FindVertex(left,right)) == 0)return -3;
      left = -left; right = -right;
      // sur le front on trouve dans l'ordre: ivl,nivl,pivr,ivr
      ivl = G.vin[left];nivl = G.vin[G.acir[-G.acir[left]]];
      ivr = G.vin[right];pivr = G.vin[G.cir[-G.cir[right]]];
      move_l = move_r = 0;
      forme = CONE;
      /* determination de la forme du front */
      if(cs.y[nivl] > cs.y[ivl])
	  {new_push = G.acir[-G.acir[left]];
	  move_l = 1;
	  if((cs.y[pivr] < cs.y[ivr]) && cs.monot_up(left,right))
	      forme = HOR_LEFT;
	  else if(cs.y[pivr] > cs.y[ivr])
	      move_r = 1;
	  }
      else if(cs.y[nivl] < cs.y[ivl])
	  {if((new_push = cs.monot_dwn(left,right)) == right)
	      forme = HOR_RIGHT;
	  if(cs.y[pivr] > cs.y[ivr])
	      move_r = 1;
	  }
      else if((cs.y[pivr] < cs.y[ivr]) && cs.monot_up(left,right))
	  {forme = HOR_LEFT;new_push = cs.monot_dwn(left,right);
	  }
      else
	  {if(cs.y[pivr] > cs.y[ivr])
	      move_r = 1;
	  new_push = cs.monot_dwn(left,right);
	  }

      /* calcul du deplacement des sommets */
      //if(!minimisation)forme = CONE;
      if(forme == CONE)
	  {int dx = cs.x[ivl]+cs.x[ivr]+cs.y[ivl]+cs.y[ivr]+move_l+move_r;
	  if(dx%2 != 0)++move_r;
	  }
      move_r += move_l;
      if(move_r)cs.deplace(ivr,move_r,iv);
      if(move_l)cs.deplace(nivl,move_l,iv);
      /* modification des pousseurs (cet ordre est capital) */
      cs.add(new_push);cs.add(-left);
      cs.ComputeCoord(iv,G.vin[left],G.vin[right],forme);
      }
  if(!cs.ok)return -4;
  Prop<Tpoint> Coord(G.Set(tvertex()),PROP_COORD);
  for(tvertex v = 1;v <= G.nv();v++)
      {Coord[v].x() = cs.x[v];
       Coord[v].y() = cs.y[v];
      }
  // Erase added edges
  for(tedge e = G.ne();e > OldNumEdge;e--)G.DeleteEdge(e);
  G.Set() =  propSave;
  return 0;
  }

int EmbedFPP_Rect(TopologicalGraph &G,bool schnyderLongestFace)
  {int OldNumEdge = G.ne();
  PSet1  propSave(G.Set());
  G.MakeConnected();
  if(!G.CheckPlanar())return -5;
  bool MaxPlanar = (G.ne() != 3 * G.nv() - 6) ? false : true;
  int len;
  tbrin ee;
  if(schnyderLongestFace && !MaxPlanar)
      {G.LongestFace(ee,len);G.extbrin() = ee;}
  else
      ee = G.extbrin();
  if(!MaxPlanar && G.ZigZagTriangulate() < 0)return -2;
  tbrin left,right,new_push;
  tvertex iv,iv1,iv2,iv3;
  tvertex ivl,nivl,ivr,pivr;
  int move_l,move_r,forme;

  //init with the leftmost brin incident to tha last vertxex to be packed
  SchnyderPacking SP(G,-G.acir[ee]);
  // Skip first two vertices
  SP.FindVertex();  SP.FindVertex();
  Constraints cs(G);           cs.init(ee);
  iv1 = G.vin[ee];iv2 = G.vin[-ee];iv3 = G.vin[-G.acir[ee]];
  cs.x[iv1] = 1;cs.x[iv2] = 2;
  cs.y[iv1] = cs.y[iv2] = 1;
  for(tvertex ivn = 3;ivn <= G.nv();ivn++)
      {if((iv = SP.FindVertex(left,right)) == 0)return -3;
      left = -left; right = -right;
      // sur le front on trouve dans l'ordre: ivl,nivl,pivr,ivr
      ivl = G.vin[left];nivl = G.vin[G.acir[-G.acir[left]]];
      ivr = G.vin[right];pivr = G.vin[G.cir[-G.cir[right]]];
      move_l = move_r = 0;
      forme = CONE;
      /* determination de la forme du front */
      if(cs.y[nivl] > cs.y[ivl])
	  {new_push = G.acir[-G.acir[left]];
	  move_l = 1;
	  if((cs.y[pivr] < cs.y[ivr]) && cs.monot_up(left,right))
	      forme = HOR_LEFT;
	  else if(cs.y[pivr] > cs.y[ivr])
	      move_r = 1;
	  }
      else if(cs.y[nivl] < cs.y[ivl])
	  {if((new_push = cs.monot_dwn(left,right)) == right)
	      forme = HOR_RIGHT;
	  if(cs.y[pivr] > cs.y[ivr])
	      move_r = 1;
	  }
      else if((cs.y[pivr] < cs.y[ivr]) && cs.monot_up(left,right))
	  {forme = HOR_LEFT;new_push = cs.monot_dwn(left,right);
	  }
      else
	  {if(cs.y[pivr] > cs.y[ivr])
	      move_r = 1;
	  new_push = cs.monot_dwn(left,right);
	  }

      /* calcul du deplacement des sommets */
      move_r += move_l;
      //printf("add:%d L=%d R=%d\n",iv(),move_l,move_r);
      if(move_r)cs.deplace(ivr,move_r,iv);
      if(move_l)cs.deplace(nivl,move_l,iv);
      /* modification des pousseurs (cet ordre est capital) */
      cs.add(new_push);cs.add(-left);
      cs.ComputeCoord_Rect(iv,G.vin[left],G.vin[right],forme);
      //printf("x=%d y=%d\n",cs.x[iv],cs.y[iv]);
      }
  //printf("******************************\n");
  if(!cs.ok)return -4;
  Prop<Tpoint> Coord(G.Set(tvertex()),PROP_COORD);
  for(tvertex v = 1;v <= G.nv();v++)
      {//printf("%d x=%d y=%d\n",v(),cs.x[v],cs.y[v]);
      Coord[v].x() = cs.x[v];
      Coord[v].y() = cs.y[v];
      }
  //printf("******************************\n");
  // Erase added edges
  for(tedge e = G.ne();e > OldNumEdge;e--)G.DeleteEdge(e);
  G.Set() =  propSave;
  return 0;
  }


//***************************************************************************************
//***************************************************************************************
class Recti: public TopologicalGraph
{public:
  Recti(Graph &G);
  ~Recti();
  int  hauteur(tbrin lee,tbrin ree);
  void con_iv(tbrin lee,tbrin ree);
  void con1_ee(tbrin ee,tbrin ree);
  void con2_ee(tbrin ee);
  void addrecti(tedge je,tedge jf,int len);
  void resolve(void);
  void solve(tedge je);
  int  lrver(void);

  svector<int> length,xliv,xriv,y,xje;
  tvertex iv1;
  int m_origin;

private:
  svector<tedge> push0_by,push1_by,push2_by,back;
 
};
Recti::~Recti()
  {}
Recti::Recti(Graph &G) :
    TopologicalGraph(G)
  {push0_by.resize(0,ne()); push0_by.clear(); push0_by.SetName("push0_by");
  push1_by.resize(0,ne());  push1_by.clear(); push1_by.SetName("push1_by");
  push2_by.resize(0,ne());  push2_by.clear(); push2_by.SetName("push2_by");
  length.resize(0,ne());    length.clear();   length.SetName("length");
  back.resize(0,ne());      back.clear();     back.SetName("length");
  xje.resize(0,ne());       xje.clear();      xje.SetName("xje");
  xliv.resize(0,nv());      xliv.clear();     xliv.SetName("xliv");
  xriv.resize(0,nv());      xriv.clear();     xriv.SetName("xriv");
  y.resize(0,nv());         y.clear();        y.SetName("y");
  }
int Recti::hauteur(tbrin lee,tbrin ree)
  {int h = 0;
  while((lee = -cir[-lee]) != ree)
      {h = Max(h,y[vin[lee]]);
      }
  return h;
  }
void Recti::con_iv(tbrin lee,tbrin ree)
  {tbrin ee = lee;
  while(ee != ree)
      {con1_ee(lee,ree);
      ee = lee;lee = -cir[-lee];
      }
  }
void Recti::con1_ee(tbrin ee,tbrin ree)
  {tbrin gg;
  int h,hee,hff,hgg;
  tbrin ff = cir[ee];
  h = y[vin[-ee]];hee = y[vin[ee]];hff = y[vin[-ff]];
  
  if(hee == h && ee == ree)
      {gg = acir[ee];
      if(y[vin[-gg]])addrecti(ff.GetEdge(),gg.GetEdge(),0);
      addrecti(acir[-ee].GetEdge(),cir[ee].GetEdge(),2);
      return;
      }
  else if(hee == h)
      {addrecti(acir[ee].GetEdge(),cir[-ee].GetEdge(),2);
      return;
      }
  else if(hff && (hff < hee))
      {addrecti(ff.GetEdge(),ee.GetEdge(),0);
      }
  else if(hff && (hff ==  hee) && (vin[ee] != iv1))
      {gg = cir[-ff];hgg = y[vin[-gg]];
      if(hgg > hee){addrecti(gg.GetEdge(),ee.GetEdge(),2);}

      ff = cir[ff];
      hff = y[vin[-ff]];
      if(hff && (hff < hee))addrecti(ff.GetEdge(),ee.GetEdge(),0);
      }
  con2_ee(ee);
  }

void Recti::con2_ee(tbrin ee)
  {tbrin ff,gg;
  int hee,hff,hgg;
  hee = y[vin[ee]];
  ff = acir[ee];hff = y[vin[-ff]];
  if(hff == 0)return;
  if((hff > hee) && (hff != y[vin[ff]]) && (hff != y[vin[-ee]]))
      {addrecti(ee.GetEdge(),ff.GetEdge(),1);return;}
  gg = acir[-ff];hgg = y[vin[-gg]];
  if((hgg > hff) && hff != hee)
      {addrecti(ee.GetEdge(),gg.GetEdge(),1);
      addrecti(ff.GetEdge(),gg.GetEdge(),1);
      }
  gg = cir[-ff];hgg = y[vin[-gg]];
  if(hgg && (hff == hee) && (hgg < hee))
      {addrecti(ee.GetEdge(),gg.GetEdge(),2);}
  }
void Recti::addrecti(tedge je,tedge jf,int len)
  {if(len)
      {if(push1_by[jf] == 0)
	  {push1_by[jf] = je;length[je] = len;}
      else
	  {push2_by[jf] = je;length[je] = len;}
      }
  else
      push0_by[jf] = je;
  }
void Recti::resolve(void)
  {for(tedge je = 1;je <= ne();je++)
      {if(vin[je] == 0)continue;
      if(xje[je] != 0)continue;
      solve(je);
      }
  }
void Recti::solve(tedge je)
  {tedge jje;
  for(;;)
      {if((jje = push1_by[je]) != 0)
	  {back[jje] =je;push1_by[je] = 0;
	  je = jje;
	  }
      else if((jje = push2_by[je]) != 0)
	  {back[jje] =je;push2_by[je] = 0;
	  je = jje;
	  }
      else if((jje = push0_by[je]) != 0)
	  {back[jje] =je;
	  je = jje;
	  }
      else if((jje = back[je]) != 0)
	  {if(push0_by[jje] == je)
	      {push0_by[jje] = 0;
	      xje[jje] = Max(xje[jje],xje[je]);
	      je = jje;
	      }
	  else
	      {xje[jje] = Max(xje[jje],xje[je] + length[je]);
	      je = jje;
	      }
	  }
      else
	  return;
      }
  }
int Recti::lrver(void)
  {tbrin ref,ee;
  int xx,xmax = 0;
  bool vertical;
  for(tvertex iv = 1;iv <= nv();iv++)
      {ref = ee = pbrin[iv];
      // Check exist vertical edges around iv from original graph
      vertical = false;
      do    
	  {if(y[vin[ee]] == y[vin[-ee]])continue;
	  if(ee.GetEdge() <= m_origin)
	      vertical = true;
	  }while(!vertical && (ee = acir[ee]) != ref);

      ee = ref;
      xliv[iv] = 0x7FFF;
      do    
	  {if(y[vin[ee]] == y[vin[-ee]])continue;
	  if(vertical && ee.GetEdge() > m_origin)continue;
	  xx = xje[ee.GetEdge()];
	  xliv[iv] = Min(xliv[iv],xx);
	  xriv[iv] = Max(xriv[iv],xx);
	  xmax = Max(xmax,xriv[iv]);
	  }while((ee = acir[ee]) != ref);
      }
  return(xmax);
  }


int EmbedFPPRecti(TopologicalGraph &G,bool schnyderLongestFace)
  {if(G.nv() < 3)return -1;
  if(!G.CheckSimple())return -2;
  int OldNumEdge = G.ne();
 PSet1  propSave(G.Set());
  G.MakeConnected();
  if(!G.CheckPlanar())return -3;
  bool MaxPlanar = (G.ne() != 3 * G.nv() - 6) ? false : true;
  int len;
  tbrin ee;
  if(schnyderLongestFace && !MaxPlanar)
      G.LongestFace(ee,len);
  else
      ee = G.extbrin();
  if(!MaxPlanar && G.ZigZagTriangulate() < 0)return -4;
  tvertex iv,iv1,iv2,iv3;

  //init with the leftmost brin incident to tha last vertxex to be packed
  SchnyderPacking SP(G,-G.acir[ee]);
  // Skip first two vertices
  SP.FindVertex();  SP.FindVertex();
  Recti recti(G);              
  recti.m_origin = OldNumEdge;
  recti.iv1 = iv1 = G.vin[ee];iv2 = G.vin[-ee];iv3 = G.vin[-G.acir[ee]];
  recti.y[iv1] = recti.y[iv2] = 1;recti.xje[G.acir[ee].GetEdge()] = 1;
  int h,h0;
  tbrin left,nleft,pright,right;
  tvertex ivl,nivl,pivr,ivr;
  for(int i = 3;i <= G.nv();i++)
      {if((iv = SP.FindVertex(left,right)) == 0)return -5;
      left = -left; right = -right;
      ivl = G.vin[left];nleft = G.acir[-G.acir[left]];nivl = G.vin[nleft];
      ivr = G.vin[right];pright = G.cir[-G.cir[right]];pivr = G.vin[pright];
      // calcul de la hauteur du nouveau sommet iv 
      h0 = Max(recti.y[ivl],recti.y[ivr]);
      if((h = recti.hauteur(left,right)) != 0)
	  {h += 2;h = Max(h,h0);}
      else if(recti.y[ivl] != recti.y[ivr])
	  h = h0;
      else
	  h = recti.y[ivl] + 2;
      recti.y[iv] = h;
      // calcul des pousseurs introduits sur les aretes 
      recti.con_iv(left,right);
      }
  // resolution des contraintes
  recti.resolve();
  int xmax = recti.lrver();
  
  // Erase added edges
  for(tedge e = G.ne();e > OldNumEdge;e--)
      G.DeleteEdge(e);
  G.Set() =  propSave;
  // Defines properties for the drawing
  Prop<int> pxliv(G.Set(tvertex()),PROP_DRAW_INT_1);
  Prop<int> pxriv(G.Set(tvertex()),PROP_DRAW_INT_2);
  Prop<int> py(G.Set(tvertex()),PROP_DRAW_INT_3);
  Prop<int> pxje(G.Set(tedge()),PROP_DRAW_INT_4);
  pxliv.vector() = recti.xliv;  pxriv.vector() = recti.xriv;  
  py.vector() = recti.y;  pxje.vector() = recti.xje;
  // define the boundaries
  Prop1<Tpoint> pmin(G.Set(),PROP_POINT_MIN);
  Prop1<Tpoint> pmax(G.Set(),PROP_POINT_MAX);
  pmin() = Tpoint(.0,-1.);
  pmax() = Tpoint((double)xmax+1.,(double)recti.y[iv3]+ 1.);
  return 0;
  }
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Representation by contact of T 

typedef struct  {tvertex lvertex;
                tvertex rvertex;
                tvertex hvertex; // the highest vertex incident
                }Tcontact;

int EmbedTContact(TopologicalGraph &G,bool schnyderLongestFace)
  {if(debug())DebugPrintf("EmbedTContact");
  if(G.nv() < 3){if(debug())DebugPrintf(" n < 3");return -1;}
  if(!G.CheckSimple()){if(debug())DebugPrintf("not simple");return -1;}
  int morg = G.ne();
  PSet1  propSave(G.Set());
  G.MakeConnected();
  //int OldNumVertex = G.nv(); // cannot vertextriangulate if not 2-connected
  if(!G.CheckPlanar()){if(debug())DebugPrintf("not planar");return -1;}
  bool MaxPlanar = (G.ne() != 3 * G.nv() - 6) ? false : true;
  int len;
  tbrin e0;
  if(schnyderLongestFace && !MaxPlanar)
      G.LongestFace(e0,len);
  else
      e0 = G.extbrin();
  if(!MaxPlanar && G.ZigZagTriangulate() < 0)
      {if(debug())DebugPrintf("ERROR ZigZag");return -2;}
  // Initializations
  int n = G.nv();   
  tvertex iv1,iv2,iv3,iv;
  iv1 = G.vin[e0];iv2 = G.vin[-e0];iv3 = G.vin[-G.acir[e0]];
  //init with the leftmost brin incident to tha last vertxex to be packed
  SchnyderPacking SP(G,-G.acir[e0]);
  // Skip first two vertices
  SP.FindVertex();  SP.FindVertex();
  svector<Tcontact> T_vertex(1,n);      T_vertex.SetName("T_vertex");
  svector<int> Hor(1,n);  Hor.clear();  Hor.SetName("Tcontact:Hor");
  svector<int> Hor2(1,n); Hor2.clear(); Hor2.SetName("Tcontact:Hor2");
  svector<int> Ver(1,n); Ver.clear(); Ver.SetName("Tcontact:Ver");
  Hor[iv1] = 1; Hor[iv2] = 0;  Ver[iv1] = 1;
  Hor2[iv1] = Hor2[iv2] = 1; 
  MaxPath vertical(n,2*n-4);

  // use the Schnyder packing to add constraints between verticals
  tbrin left,right,b;
  for(tvertex ivn = 3;ivn <= G.nv();ivn++)
      {if((iv = SP.FindVertex(left,right)) == 0)return -3;
      // First compute Hor[iv]: left and right are incident to iv
      b = left;
      Hor[iv] = Hor2[iv] = Max(Hor[iv],Hor[G.vin[-b]()] + 1);
      vertical.insert(G.vin[-b](),iv(),1);
      T_vertex[iv].lvertex =  T_vertex[iv].rvertex =  T_vertex[iv].hvertex = 0;
      if(b.GetEdge() <= morg) {T_vertex[iv].lvertex = T_vertex[iv].rvertex = G.vin[-b];}    
 
      while((b = G.cir[b]) != G.cir[right]) // for packed b incident to iv 
            {Hor[iv] = Max(Hor[iv],Hor[G.vin[-b]()] + 1);
	    if(b.GetEdge() <= morg)
		{T_vertex[iv].rvertex = G.vin[-b];
		if(T_vertex[iv].lvertex == 0)T_vertex[iv].lvertex = G.vin[-b];
		T_vertex[G.vin[-b]].hvertex = iv;
		}
            }

      b = right;
      // Now Hor[iv] is correct
      Hor2[iv] = Hor[iv];
      vertical.insert(iv(),G.vin[-b](),1);
      // Modify Hor2 under iv
      b = left;
      if(b.GetEdge() <= morg) Hor2[G.vin[-b]] = Max(Hor2[G.vin[-b]],Hor[iv]);
      while((b = G.cir[b]) != right) 
	  if(b.GetEdge() <= morg) Hor2[G.vin[-b]] = Max(Hor2[G.vin[-b]],Hor[iv]);
      if(b.GetEdge() <= morg) Hor2[G.vin[-b]] = Max(Hor2[G.vin[-b]],Hor[iv]);
      }
 
  // Solve constraints
  vertical.solve(Ver);
  Hor2[iv3] = Hor[iv3];

  // Modifications for drawing
  T_vertex[iv1].lvertex = iv1;  T_vertex[iv1].rvertex = iv2;  
  T_vertex[iv2].lvertex = iv2;  T_vertex[iv2].rvertex = iv2;  
  T_vertex[iv1].hvertex = 0;    T_vertex[iv2].hvertex = iv3;

  // Erase triangulation edges
  for(tedge e = G.ne(); e > morg;e--)G.DeleteEdge(e);
  G.Set() =  propSave;
  // define the boundaries
  Prop1<Tpoint> pmin(G.Set(),PROP_POINT_MIN);
  Prop1<Tpoint> pmax(G.Set(),PROP_POINT_MAX);
  Prop1<double> sizetext(G.Set(),PROP_DRAW_DBLE_1);

  double epsilon = .1;    // free distance for contacts
  double yminsize = .25;  // minsize of verticals above horizontals
  double xminsize = .0;   // minsize of horizontals (.125)
  sizetext() = .5;        // maximal textsize 
  pmin() = Tpoint(1.-xminsize,.0);
  pmax() = Tpoint(Ver[iv2] + sizetext()/2,Hor[iv3] + sizetext());

  // compute horizontals and verticals
  Prop<Tpoint> hp1(G.Set(tvertex()),PROP_DRAW_POINT_1);
  Prop<Tpoint> hp2(G.Set(tvertex()),PROP_DRAW_POINT_2);
  Prop<Tpoint> vp1(G.Set(tvertex()),PROP_DRAW_POINT_3);
  Prop<Tpoint> vp2(G.Set(tvertex()),PROP_DRAW_POINT_4);
  Prop<Tpoint> txt(G.Set(tvertex()),PROP_DRAW_POINT_5);
  // Compute horizontals
  int x1,x2,xv,v;
  for(v = 1;v <= n;v++)
      {if(v == iv2()){hp1[v].x() = -1.;continue;} // no horizontal
       xv = Ver[v];
      hp1[v].y() =  hp2[v].y() = (double)Hor[v];
      if(T_vertex[v].lvertex == 0)// optimization of verticals
	  {if( G.Degree(v) == 1) 
	      {tvertex w = G.vin[-G.FirstBrin(v)];
	      if(Hor2[w] < Hor[v])Hor2[v] =  Hor2[w] + 1;
	      else if(Hor[v] < Hor[w])Hor[v] = Hor[w] - 1;
	      hp1[v].y() =  hp2[v].y() = (double)Hor[v];
	      }
	  hp1[v].x() = (double)xv - xminsize;  hp2[v].x() = (double)xv + xminsize;continue;
	  }
      x1 = Ver[T_vertex[v].lvertex]; x2 = Ver[T_vertex[v].rvertex];
      if(v == iv3())
	  {if(x1 != x2)
	      {hp1[v].x() = (double)x1 + epsilon; hp2[v].x() = (double)x2 - epsilon;}
	  else
	      {hp1[v].x() = (double)x2 - xminsize; hp2[v].x() = (double)x2 + xminsize;}
	  continue;
	  }
      // Optimizations of vertices represented by an horizontal
      if(Hor[v] == Hor2[v]  && x1 == x2)
	  {if(x1 < xv)Ver[v] = xv = x1 + 1;
	  else if(xv < x2)Ver[v] = xv = x2 -1;
	  }
      if(v == iv1() && G.Degree(iv1) == 1)
	  Ver[v] = xv = x1 = Ver[iv2]-1;
      // general case
      x1 = Min(x1,xv); x2 = Max(x2,xv);
      hp1[v].x() = (x1 != xv) ? (double)x1 + epsilon :(double) x1 - xminsize;
      hp2[v].x() = (x2 != xv) ? (double)x2 - epsilon : (double)x2 + xminsize;
      }
  // Compute verticals and text position
  double y1,y2;
  tvertex hv;
  for(v = 1;v <= n;v++)
      {xv = Ver[v]; y1 = Hor[v]; y2 = Hor2[v];
      hv = T_vertex[v].hvertex;
      if(v == iv3())
	  vp1[v].x() = -1.;
      else
	  {vp1[v].x() = vp2[v].x() = (double)xv;
	  vp1[v].y() = (double)y1;
	  if(y1 == y2)
	      vp2[v].y() = (double)y2;
	  //vp2[v].y() = (double)y2 + yminsize;
	  else if(hv != 0  && hp1[hv].x() <=(double) xv &&  hp2[hv].x() >= (double)xv)
	      vp2[v].y() = (double)y2 - epsilon;
	  else
	      vp2[v].y() = (double)y2 + yminsize;
	  }
      // Text
      txt[v].x() = (double)xv;   txt[v].y() = (double)y1;
      }
  // special cases
  txt[iv3].x() = (hp1[iv3].x() + hp2[iv3].x())/2 ;
  return 0;
  }

static double xIntersection(Tpoint & p1, Tpoint &p2, double y)
// returns the x-coordinate of the horizontal "y" and the line defined by (p1,p2)
  {return  (p1.x() + (p2.x() -p1.x())*(y -p1.y())/(p2.y() - p1.y()));
  }
int EmbedTriangle(TopologicalGraph &G)
  {if(G.nv() < 3)return -1;
  if(!G.CheckSimple())return -1;
  if(!G.CheckPlanar())return -1;
  PSet1  propSave(G.Set());
  bool MaxPlanar = (G.ne() != 3 * G.nv() - 6) ? false : true;
  int nOrigin = G.nv();
  if(!MaxPlanar)G.VertexTriangulate();
  if(G.ne() != 3 * G.nv() - 6)return -1;
  int n = G.nv();

  tbrin ee = G.extbrin();
  tvertex iv1 = G.vin[ee];
  tvertex iv2 = G.vin[-ee];
  tvertex iv3 = G.vin[-G.acir[ee]];
  svector<tvertex> leftFather(1,n),rightFather(1,n),topFather(1,n);
  svector<tvertex>order(1,n); 
  svector<int> hauteur(0,n); hauteur.clear();

  topFather[iv1] =  topFather[iv2] = iv3;  topFather[iv3] = 0;
  order[1] = iv1;  order[2] = iv2;
  hauteur[iv2] = 1;
  SchnyderPacking SP(G,-G.acir[ee]);
  // Skip first two vertices
  SP.FindVertex();  SP.FindVertex();
  tbrin left,right;
  tvertex iv;
  for(tvertex ivn = 3;ivn <= G.nv();ivn++)
      {if((iv = SP.FindVertex(left,right)) == 0)return -3;
      leftFather[iv] = G.vin[-left];
      rightFather[iv] = G.vin[-right];
      order[ivn] = iv;
      ee = left;
      do
          {hauteur[iv] = Max(hauteur[iv],hauteur[G.vin[-ee]]+1);
          if(ee != left)topFather[G.vin[-ee]]= iv;
          }while((ee = G.cir[ee]) != right);
      hauteur[iv] = Max(hauteur[iv],hauteur[G.vin[-ee]]+1);
      }
  
  double ymax =  hauteur[order[n]];
  // Properties defining the traiangles
  Prop<Tpoint> pleft(G.Set(tvertex()),PROP_DRAW_POINT_1);
  Prop<Tpoint> pright(G.Set(tvertex()),PROP_DRAW_POINT_2);
  Prop<Tpoint> ptop(G.Set(tvertex()),PROP_DRAW_POINT_3);

  // triangle iv1
  pright[iv1].x() = .5; pright[iv1].y() = 1.;
  pleft[iv1].x() = .5; pleft[iv1].y() = 1.;
  ptop[iv1].x() = 0.; ptop[iv1].y() = ymax;
  // triangle iv2
  pleft[iv2].x() = .5; pleft[iv2].y() = 1.;
  pright[iv2].x() = .5; pright[iv2].y() = 1.;
  ptop[iv2].x() = 1.; ptop[iv2].y() = ymax;
  // triangle iv3
  pleft[iv3].x() = .0; pleft[iv3].y() = ymax;
  pright[iv3].x() = 1.; pright[iv3].y() = ymax;
  ptop[iv3].x() = .5; ptop[iv3].y() = ymax;

  // compute y-coordinates of all other triangles
  for(int i = 3; i < n;i++)
      {tvertex iv= order[i];
      pleft[iv].y() =  pright[iv].y() = hauteur[iv];
      ptop[iv].y() = hauteur[topFather[iv]];
      }
  // compute x-coordinates of all other triangles
    for(int i = 3; i < n;i++)
      {tvertex iv= order[i];
      tvertex ivl = leftFather[iv];
      pleft[iv].x() = xIntersection(pright(ivl),ptop[ivl],pleft[iv].y());
      tvertex ivr = rightFather[iv];
      pright[iv].x() = xIntersection(pleft[ivr],ptop[ivr],pright[iv].y());
      ptop[iv].x() = .5*(pleft[iv].x() + pright[iv].x());
      }
    
    if(nOrigin != n)
        {//erase added vertices
        for(tvertex iv = n; iv > nOrigin;iv--)G.DeleteVertex(iv);
        G.Set() =  propSave;
        }

    // define the boundaries
    Prop1<Tpoint> pmin(G.Set(),PROP_POINT_MIN);
    Prop1<Tpoint> pmax(G.Set(),PROP_POINT_MAX);
    const double border = .002;
    pmin() = Tpoint(-border,.0);
    pmax() = Tpoint(1.-border,ymax + 1.);
    return(0);
    }
