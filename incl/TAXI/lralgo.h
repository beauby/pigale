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

#ifndef LRALGO_H
#define LRALGO_H

#include <TAXI/Tbase.h>
#include <TAXI/graph.h>
#include <TAXI/graphs.h>
#include <TAXI/DFSGraph.h>
#include <TAXI/bicon.h>

#ifndef NLRALGO_H
#define GAUCHE 0
#define DROITE 1
#define AUTRE   2
#define PLANAR  0
#define NONPLANAR 1

#define FIN 2
#define AGAIN 1
#define STOP 0
#endif

struct _Hist {
  svector<tedge> Link;
  svector<int> Dus;
  svector<int> Flip;
  _Hist(int n,int m) :  Link(0,m),Dus(0,m),Flip(0,m)
      {Link.clear(); Dus.fill(0,n-1,0); Flip.clear();
      Link.SetName("Link _Hist lralgo.h");
      Dus.SetName("Dus _Hist lralgo.h");
      Flip.SetName("Flip _Hist lralgo.h");
      }
  _Hist(Graph &G) :  Link(0,G.ne()),Dus(0,G.ne()),Flip(0,G.ne())
      {Link.clear(); Dus.fill(0,G.nv()-1,0); Flip.clear();
      Link.SetName("Link _Hist lralgo.h");
      Dus.SetName("Dus _Hist lralgo.h");
      Flip.SetName("Flip _Hist lralgo.h");
      }
  ~_Hist() {}
};
struct _FastHist {
  svector<tedge> Link;
  tedge LeftTop,LeftBot,RightTop,RightBot,Cove,NP;
  int Thick;
  _FastHist(Graph &G) : Link(0,G.ne()),LeftTop(0),LeftBot(0),RightTop(0),
			RightBot(0),Cove(0),NP(0),Thick(0)
      {Link.clear();
      Link.SetName("Link _FastHist nlralgo.h");
      }
  _FastHist(int m) :  Link(0,m),LeftTop(0),LeftBot(0),RightTop(0),
		      RightBot(0),Cove(0),NP(0),Thick(0)
      {Link.clear();
      Link.SetName("Link _FastHist nlralgo.h");
      }
  ~_FastHist() {}
};
struct _LrSort {
  svector<tedge> tel;
  svector<tedge> tref;
  svector<tedge> linkt;
  svector<int> num;
  _LrSort(int n,int m) : tel(0,n), tref(0,n), linkt(0,m),
			 num(n,m)
      {tel.clear(); tref.clear(); linkt.clear();//num.clear();
      tel.SetName("tel _LrSort lralgo.h");
      tref.SetName("tref _LrSort lralgo.h");
      linkt.SetName("linkt _LrSort lralgo.h");
      num.SetName("num _LrSort lralgo.h");
      }
  _LrSort(Graph &G) : tel(0,G.nv()), tref(0,G.nv()), linkt(0,G.ne()),
		      num(G.nv(),G.ne())
      {tel.clear(); tref.clear(); linkt.clear();//num.clear();
      tel.SetName("tel _LrSort lralgo.h");
      tref.SetName("tref _LrSort lralgo.h");
      linkt.SetName("linkt _LrSort lralgo.h");
      num.SetName("num _LrSort lralgo.h");
      }
  ~_LrSort() {}
};
inline tedge treein(tvertex v) {return tedge(v()-1);}
inline tvertex treetarget(tedge e) {return tvertex(e()+1);}
void LralgoSort(int n,int m, svector<tvertex> &vin, const _Bicon &Bicon ,const svector<tvertex> &low,_LrSort &LrSort);
void NewLralgoSort(int n,int m, svector<tvertex> &vin, const _Bicon &Bicon ,const svector<tvertex> &low,_LrSort &LrSort);
int lralgo(int n, int m, svector<tvertex> &vin,const _Bicon &Bicon, const _LrSort &LrSort, _Hist &Hist,bool OnlyTest=false);
int Newlralgo(int n, int m, svector<tvertex> &vin,const _Bicon &Bicon, const _LrSort &LrSort, _Hist &Hist,bool OnlyTest=false);
int fastlralgo(int n,int m, svector<tvertex> &vin,const _Bicon &Bicon, _LrSort &LrSort, _FastHist &Hist);
int Newfastlralgo(int n,int m, svector<tvertex> &vin,const _Bicon &Bicon, _LrSort &LrSort);

#ifdef PRIVATE
#ifndef NLRALGO_H

#define TW_LEFT    0
#define TW_RIGHT   1
#define TW_TOP     0
#define TW_BOT     1


class TEdgeStackPartition {
  svector<tedge> &linkt;

public :

  TEdgeStackPartition(svector<tedge> &l):linkt(l) {}
  ~TEdgeStackPartition() {}
  void Push(tedge &top,const tedge &e) {linkt[e]=top; top=e;}
  tedge Pop(tedge &top) {tedge e=top; top=linkt[top]; return e;}
  tedge Next(const tedge &e) const {return linkt[e];}
  void PopNext(tedge &e) {linkt[e]=linkt[linkt[e]];}
};

struct _twin {
  tedge _Extr[2][2]; // 1er arg : TOP/BOT ; 2Šme arg : LEFT/RIGHT
  _twin() {}
  _twin(const _twin &t)
      { _Extr[0][0]=t._Extr[0][0];
      _Extr[0][1]=t._Extr[0][1];
      _Extr[1][0]=t._Extr[1][0];
      _Extr[1][1]=t._Extr[1][1];
      }
  ~_twin(){}
  _twin &operator=(const _twin&t)
      { _Extr[0][0]=t._Extr[0][0];
      _Extr[0][1]=t._Extr[0][1];
      _Extr[1][0]=t._Extr[1][0];
      _Extr[1][1]=t._Extr[1][1];
      return *this;
      }
  int operator!() const {return !(_Extr[0][0]()||_Extr[0][1]()||_Extr[1][0]()||_Extr[1][1]());}
  void Permute() // permutation Gauche/Droite
      {tedge tmp;
      tmp=_Extr[0][0]; _Extr[0][0]=_Extr[0][1]; _Extr[0][1]=tmp;
      tmp=_Extr[1][0]; _Extr[1][0]=_Extr[1][1]; _Extr[1][1]=tmp;
      }

  tedge &extr(int where, int side) {return _Extr[where][side];}
  tedge &top(int side) { return _Extr[TW_TOP][side];}
  tedge &bot(int side) { return _Extr[TW_BOT][side];}
  tedge &ltop() { return _Extr[TW_TOP][TW_LEFT];}
  tedge &lbot() { return _Extr[TW_BOT][TW_LEFT];}
  tedge &rtop() { return _Extr[TW_TOP][TW_RIGHT];}
  tedge &rbot() { return _Extr[TW_BOT][TW_RIGHT];}

  tedge Firstbot() {return (tedge)(lbot()()? lbot()():rbot()());}
  int MoreThanOne(int side) {return (top(side)!=bot(side));}
  int NotEmpty(int side) {return (bot(side)());}
  int Empty(int side) {return (bot(side)==tedge(0));}
};
#endif
class _Twit
{
  tvertex _forkvertex;   //vertex 
  svector<tvertex> ForkLink;
  svector<int> ForkNumTwin;
public :
  int _current;
  int _fork;         //twin
  int _erase_pile;
  int isnotplanar;
  svector<_twin> tw;
  svector<tvertex> &vin;

  _Hist &Hist;

public :
	 

  _Twit(int ne,int nv, svector<tvertex> &_vin, _Hist &rHist) :
      _forkvertex(0),ForkLink(0,nv),ForkNumTwin(0,nv),
      _current(0),_fork(0),_erase_pile(0),isnotplanar(0),
      tw(0,ne),vin(_vin),Hist(rHist)
      {ForkNumTwin[0] = 0; ForkLink[0] = 0;
      tw.SetName("tw _Twit:lralgo.h");
      ForkLink.SetName("ForkLink:lralgo.h");
      ForkNumTwin.SetName("ForkNumTwin:lralgo.h");
      tw[0].lbot()=tw[0].ltop()=tw[0].rbot()=tw[0].rtop()=0;
      }
  _Twit(DFSGraph &G, _Hist &rHist) :
      _forkvertex(0),ForkLink(0,G.nv()),ForkNumTwin(0,G.nv()),
      _current(0),_fork(0),_erase_pile(0),isnotplanar(0),tw(0,G.ne()),
      vin(G.nvin),Hist(rHist)
      {ForkNumTwin[0] = 0; ForkLink[0] = 0;
      tw.SetName("tw _Twit:lralgo.h");
      ForkLink.SetName("ForkLink:lralgo.h");
      ForkNumTwin.SetName("ForkNumTwin:lralgo.h");
      tw[0].lbot()=tw[0].ltop()=tw[0].rbot()=tw[0].rtop()=0;
      }

  ~_Twit() {}

  _twin & Twin() {return tw[_current];}           // twin courantt
  _twin & PrevTwin() {return tw[_current-1];}     // twin precedent
  _twin & NextTwin() {return tw[_current+1];}     // twin precedent
  _twin & FirstTwin() {return tw[_fork+1];}       // premier twin
  _twin & ForkTwin() {return tw[_fork];}          // twin fork
  //int piles(){return _erase_pile;} 	
  int planar() {return !isnotplanar;}
  void NextFork()
      {_forkvertex = ForkLink[_forkvertex];
      _fork = ForkNumTwin[_forkvertex];
      }
  void NewFork(tvertex v)
      {ForkLink[v] = _forkvertex;
      ForkNumTwin[v] = _current;
      _forkvertex = v;
      _fork = _current;
      }
  void GotoFirstTwin()
      { _current = _fork+1;}
  void NewTwin(const tedge &ej)
      {_current++;
      Twin().lbot() = Twin().ltop() = ej;
      Twin().rbot() = Twin().rtop() = 0;
      }
  int FirstLink() {return (_current == _fork+1);}
  void Deletion(tvertex vi);
  void Fusion(tedge ej);
  void Thin(tedge ej);
  void Thick();

private :
  int DeleteLeft(tvertex vi);
  int DeleteRight(tvertex vi);
  void Fuse(tedge ej);
  void Flipin(tedge ej);
  void Flipe(tedge ej);
  void Efnp(tedge ej);
  void Align(tedge je,int flipin,tedge ej);
  void Flick();
  void Mflip();
  void Drop();
  int side_flip(const tedge &rtp,const tedge &lbt,const tedge &cv)
      {return(vin[rtp]<=vin[lbt])
       &&((vin[rtp]==vin[cv])||(vin[-rtp]==vin[-cv]));}
  tedge Couve() // Toujours kk+1
      {tedge l = FirstTwin().lbot();
      tedge r = FirstTwin().rbot();
      tedge e = l;
      if(!l || (r() &&  vin[r]< vin[l]))e = r;
      return e;
      }
  int Cover(const tedge &cv) // ik = pile -1
      {return((vin[cv]<vin[PrevTwin().ltop()]) 
	      && (vin[-cv]!=vin[-PrevTwin().ltop()]));}
};

class _FastTwit {

  tvertex _forkvertex;   //vertex 
  svector<tvertex> ForkLink;
  svector<int> ForkNumTwin;
     
public :
  int _current;
  int _fork;
  int _erase_pile;
  int isnotplanar;
  svector<_twin> tw;
  svector<tvertex> &vin;

  _FastHist &Hist;
  jmp_buf env;

public :
	 
  _FastTwit(int ne,int nv, svector<tvertex> &_vin, _FastHist &rHist) :
      _forkvertex(0),ForkLink(0,nv),ForkNumTwin(0,nv),
      _current(0),_fork(0),_erase_pile(0),isnotplanar(0),tw(0,ne),vin(_vin),Hist(rHist)
      {ForkNumTwin[0] = 0;ForkLink[0] = 0;
      tw.SetName("tw_Fast:lralgo.h");
      ForkLink.SetName("ForkLink_Fast:lralgo.h");
      ForkNumTwin.SetName("ForkNumTwin_Fast:lralgo.h");
      tw[0].lbot()=tw[0].ltop()=tw[0].rbot()=tw[0].rtop()=0;
      }


  ~_FastTwit() {}

  _twin & Twin() {return tw[_current];}           // twin courrant
  _twin & PrevTwin() {return tw[_current-1];}     // twin precedent
  _twin & NextTwin() {return tw[_current+1];}     // twin precedent
  _twin & FirstTwin() {return tw[_fork+1];}       // premier twin
  _twin & ForkTwin() {return tw[_fork];}          // twin fork
  _twin & Twin1(){return tw[ForkNumTwin[ForkLink[_forkvertex]]+1];}
  int piles(){return _erase_pile;} 	
  int planar() {return !isnotplanar;}
  void NextFork()
      {_forkvertex = ForkLink[_forkvertex];
      _fork = ForkNumTwin[_forkvertex];
      }
  void NewFork(tvertex v)
      {ForkLink[v] = _forkvertex;
      ForkNumTwin[v] = _current;
      _forkvertex = v;
      _fork = _current;
      }
  void GotoFirstTwin()
      { _current = _fork+1;}
  void NewTwin(const tedge &ej)
      {_current++; 
      Twin().lbot() = Twin().ltop() = ej;
      Twin().rbot() = Twin().rtop() = 0;
      }
  int FirstLink() { return (_current == _fork+1);}

  void Deletion(tvertex vi);
  void Fusion(tedge ej);
  void Thin(tedge ej);
  void Thick();

private :
  int DeleteLeft(tvertex vi);
  int DeleteRight(tvertex vi);
  void Fuse(tedge ej);
  void Flipin(tedge ej);
  void Flipe(tedge ej);
  void Align(tedge je,int flipin,tedge ej);
  void Flick();
  void Mflip();
  int side_flip(const tedge &rtp,const tedge &lbt,const tedge &cv)
      {return(vin[rtp]<=vin[lbt])
       &&((vin[rtp]==vin[cv])||(vin[-rtp]==vin[-cv]));}
  tedge Couve() // Toujours kk+1
      {tedge l = FirstTwin().lbot();
      tedge r = FirstTwin().rbot();
      tedge e = l;
      if(!l || (r() &&  vin[r]< vin[l]))e = r;
      return e;
      }
  int Cover(const tedge &cv) // ik = pile -1
      {return((vin[cv]<vin[PrevTwin().ltop()]) 
	      && (vin[-cv]!=vin[-PrevTwin().ltop()]));}
};

/***********************************************************************************/
class _NewFastTwit {

  svector<int> ForkStack;
  int _topfork;
   
public :
  int _current;
  int _fork;
  int isnotplanar;
  svector<_twin> tw;
  svector<tvertex> &vin;
  svector<tedge> Link;
  jmp_buf env;

public :
	 
  _NewFastTwit(int ne,int nv, svector<tvertex> &_vin) :
    ForkStack(0,nv),_topfork(0),_current(0),_fork(0),isnotplanar(0),tw(0,ne),vin(_vin),Link(0,ne)
    { ForkStack[0]=0;
      tw.SetName("tw_NewFast:lralgo.h");
      ForkStack.SetName("ForkStack_NewFast:lralgo.h");
      Link.SetName("link_NewFast:lralgo.h");
      tw[0].lbot()=tw[0].ltop()=tw[0].rbot()=tw[0].rtop()=0;
      Link.clear();
      }

  ~_NewFastTwit() {}

  _twin & Twin() {return tw[_current];}           // twin courrant
  _twin & PrevTwin() {return tw[_current-1];}     // twin precedent
  _twin & FirstTwin() {return tw[_fork+1];}       // premier twin
  _twin & Twin1(){return tw[ForkStack[_topfork-1]+1];}
  int planar() {return !isnotplanar;}
  void PopFork()  {_current=_fork;_fork=ForkStack[--_topfork]; }
  void NewFork()  { ForkStack[++_topfork]= _fork = _current; }
  void NewTwin(const tedge &ej)
      {_current++; 
      Twin().lbot() = Twin().ltop() = ej;
      Twin().rbot() = Twin().rtop() = 0;
      }
  void Deletion(tvertex vi);
  void Fusion(tedge ej);
  void Thin();
  void Thick();

private :
  void FuseTo(tvertex vi);
  void FusePrev(int side);
  void SetNonPlanar() {isnotplanar=1; longjmp(env,1);}
#ifdef TDEBUG
  void Print();
#endif
};
/***********************************************************************************/
class _NewTwit
{
  tvertex _forkvertex;   //vertex 
  svector<tvertex> ForkLink;
  svector<int> ForkNumTwin;
public :
  int _current;
  int _fork;         //twin
  int _erase_pile;
  int isnotplanar;
  svector<_twin> tw;
  svector<tvertex> &vin;

  _Hist &Hist;

public :

  _NewTwit(int ne,int nv, svector<tvertex> &_vin, _Hist &rHist) :
      _forkvertex(0),ForkLink(0,nv),ForkNumTwin(0,nv),
      _current(0),_fork(0),_erase_pile(0),isnotplanar(0),
      tw(0,ne),vin(_vin),Hist(rHist)
      {ForkNumTwin[0] = 0; ForkLink[0] = 0;
      tw.SetName("tw _Twit:lralgo.h");
      ForkLink.SetName("ForkLink:lralgo.h");
      ForkNumTwin.SetName("ForkNumTwin:lralgo.h");
      tw[0].lbot()=tw[0].ltop()=tw[0].rbot()=tw[0].rtop()=0;
      }
  _NewTwit(DFSGraph &G, _Hist &rHist) :
      _forkvertex(0),ForkLink(0,G.nv()),ForkNumTwin(0,G.nv()),
      _current(0),_fork(0),_erase_pile(0),isnotplanar(0),tw(0,G.ne()),
      vin(G.nvin),Hist(rHist)
      {ForkNumTwin[0] = 0; ForkLink[0] = 0;
      tw.SetName("tw _Twit:lralgo.h");
      ForkLink.SetName("ForkLink:lralgo.h");
      ForkNumTwin.SetName("ForkNumTwin:lralgo.h");
      tw[0].lbot()=tw[0].ltop()=tw[0].rbot()=tw[0].rtop()=0;
      }

  ~_NewTwit() {}

  _twin & Twin() {return tw[_current];}           // twin courantt
  _twin & PrevTwin() {return tw[_current-1];}     // twin precedent
  _twin & NextTwin() {return tw[_current+1];}     // twin precedent
  _twin & FirstTwin() {return tw[_fork+1];}       // premier twin
  _twin & ForkTwin() {return tw[_fork];}          // twin fork
  int planar() {return !isnotplanar;}
  void NextFork()
      {_forkvertex = ForkLink[_forkvertex];
      _fork = ForkNumTwin[_forkvertex];
      }
  void NewFork(tvertex v)
      {ForkLink[v] = _forkvertex;
      ForkNumTwin[v] = _current;
      _forkvertex = v;
      _fork = _current;
      }
  void GotoFirstTwin()
      { _current = _fork+1;}
  void NewTwin(const tedge &ej)
      {_current++;
      Twin().lbot() = Twin().ltop() = ej;
      Twin().rbot() = Twin().rtop() = 0;
      }
  int FirstLink() {return (_current == _fork+1);}
  void Deletion(tvertex vi);
  void Fusion(tedge ej);
  void Thin(tedge ej);
  void Thick();

private :
  int DeleteLeft(tvertex vi);
  int DeleteRight(tvertex vi);
  void Fuse(tedge ej);
  void Efnp(tedge ej);
  void Align(tedge je,int flipin,tedge ej);
  void Flick();
  void Mflip();
  void Drop();
  int side_flip(const tedge &rtp,const tedge &lbt,const tedge &cv)
      {return(vin[rtp]<=vin[lbt]  && vin[rtp] == vin[cv]);}
  tedge Couve() // Toujours kk+1
      {tedge l = FirstTwin().lbot();
      tedge r = FirstTwin().rbot();
      tedge e = l;
      if(!l || (r() &&  vin[r]< vin[l]))e = r;
      return e;
      }

};

#endif
#endif
