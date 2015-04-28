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

#ifndef NLRALGO_H
#define NLRALGO_H
#include <TAXI/graphs.h>

#define GAUCHE (char)0
#define DROITE (char)1
#define AUTRE   (char)2
#define PLANAR  (char)0
#define NONPLANAR (char)1

#define FIN 2
#define AGAIN 1
#define STOP 0

struct _DFSHist {
    svector<tedge> Link;
    tedge LeftTop,LeftBot,RightTop,RightBot,Cove,NP;
    int Thick;
    _DFSHist(int m) :  Link(0,m),LeftTop(0),LeftBot(0),RightTop(0),
    RightBot(0),Cove(0),NP(0),Thick(0)
        {Link.clear();
        Link.SetName("Link _DFSHist nlralgo.h");
        }
    ~_DFSHist() {}
};
struct _DFSLrSort {
    svector<tedge> tel;
    svector<tedge> tref;
    svector<tedge> linkt;
    svector<int> num;
	 _DFSLrSort(int n, int m) : tel(0,n), tref(0,n), linkt(0,m),
     num(n,m)
        {tel.clear(); tref.clear(); linkt.clear(); num.clear();
          tel.SetName("tel _DFSLrSort nlralgo.h");
          tref.SetName("tref _DFSLrSort nlralgo.h");
          linkt.SetName("linkt _DFSLrSort nlralgo.h");
          num.SetName("num _DFSLrSort nlralgo.h");
          }
	 ~_DFSLrSort() {}
};

#ifdef PRIVATE

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

class _DFSTwit
    {
     tvertex _forkvertex;   //vertex 
     svector<tvertex> ForkLink;
     svector<int> ForkNumTwin;
     public:
	 int _current;
	 int _fork;
	 int _erase_pile;
	 int isnotplanar;
	 svector<_twin> tw;
	 svector<tvertex> &vin;

	 _DFSHist &Hist;

	 public :
    _DFSTwit(int ne,int nv, svector<tvertex> &_vin, _DFSHist &rHist) :
          _forkvertex(0),ForkLink(0,nv),ForkNumTwin(0,nv),
         _current(0),_fork(0),_erase_pile(0),isnotplanar(0),tw(0,ne),vin(_vin),Hist(rHist)
         {ForkNumTwin[0] = 0; ForkLink[0] = 0;
         tw.SetName("tw:nlralgo.h");
         ForkLink.SetName("ForkLink:nlralgo.h");
         ForkNumTwin.SetName("ForkNumTwin:nlralgo.h");
         tw[0].lbot()=tw[0].ltop()=tw[0].rbot()=tw[0].rtop()=0;
         }
	 


	 ~_DFSTwit() {}

     _twin & Twin() {return tw[_current];}           // twin courrant
     _twin & PrevTwin() {return tw[_current-1];}     // twin precedent
     _twin & NextTwin() {return tw[_current+1];}     // twin precedent
     _twin & FirstTwin() {return tw[_fork+1];}       // premier twin
     _twin & ForkTwin() {return tw[_fork];}          // twin dembranchement
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
     void Obstruct(tedge& TeftTop,tedge& TeftBot,tedge& RightTop,tedge& RightBot,tedge& Cov);

  
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
         {return(vin[rtp]<=vin[lbt])&&((vin[rtp]==vin[cv])||(vin[-rtp]==vin[-cv]));}
     tedge Couve() // Toujours kk+1
         {
         tedge l = FirstTwin().lbot();
         tedge r = FirstTwin().rbot();
         if(r() && (vin[r]< vin[l])) return r ;
         else return l;
         }

     int Cover(const tedge &cv) // ik = pile -1
         {return((vin[cv]<vin[PrevTwin().ltop()]) && (vin[-cv]!=vin[-PrevTwin().ltop()]));}
};

#endif
#endif
