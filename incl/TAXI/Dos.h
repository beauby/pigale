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
#ifndef _DOS_H_INCLUDED_
#define _DOS_H_INCLUDED_
#include <TAXI/graphs.h>
class DoubleOccurenceSequence
{
public:
    int n;
    int m;
    int size;
    TopologicalGraph &G0;
private:
    svector<tbrin> Dos;
    svector<int> DosInv;
    svector<tbrin> _First;
    svector<tbrin> treein;
    Prop<bool> _IsTree;
public:
    Prop<tvertex> vin;
    Prop<tbrin> cir;
    Prop<tbrin> acir;
private:

    void SetNames()
        { Dos.SetName("Dos");
        DosInv.SetName("DosInv");
        _First.SetName("_First");
        treein.SetName("treein");
        //_IsTree.SetName("_IsTree");
        //vin.SetName("vin");
        }
    
public:
    DoubleOccurenceSequence(TopologicalGraph &G, tbrin b0) :
        n(G.nv()), m(G.ne()),
        size(2*G.ne()), G0(G),Dos(1,2*G.ne()),
        DosInv(-G.ne(),G.ne()), _First(0,G.ne()), treein(1,G.nv()), 
        _IsTree(G.Set(tedge()),PROP_ISTREE), vin(G.Set(tbrin()),PROP_VIN),
        cir(G.Set(tbrin()),PROP_CIR), acir(G.Set(tbrin()),PROP_ACIR)
        {SetNames();
        Compute(b0);
        }
    void Compute(tbrin b0)
        {
        int i = 1;
        tedge e;
        _First.clear();
        tbrin b =Dos[i] = b0; DosInv[b]=i;
        treein[vin[b0]] = 0;
        while(++i <= size)
            {e = b.GetEdge();
            if (_First[e]==0) _First[e]=b;
            if(_IsTree[e])
                { b = -b;
                treein[vin[b]]=b;
                }
            b = cir[b];
            Dos[i] = b; DosInv[b]=i;
            }
        }
    tbrin operator[] (int i) { return Dos[i]; }
    int Index(tbrin b)  { return DosInv[b]; }
    tbrin TreeIn(tvertex v) { return treein[v];}
    tvertex Father(tvertex v) { return vin[-treein[v]];}
    tbrin First(tedge e)  { return _First[e];  }
    tbrin Second(tedge e) { return -_First[e]; }
    tvertex FirstVertex(tedge e)   { return vin[_First[e]]; }
    bool IsTree(tedge ee)  { return _IsTree[ee]; }
    int IsFirst(tbrin b)   { return b==_First[b.GetEdge()]; }
    int IsSecond(tbrin b)  { return b!=_First[b.GetEdge()]; }
    // si on décale pour un départ en newb0, conversion des indices:
    int OldToNew(int i, tbrin newb0)  { return (DosInv[newb0]+i-2)%size+1;}
    int NewToOld(int i, tbrin newb0)  { return (i-DosInv[newb0])%size+1;}
    
    // GetSide(v,w,b0) : point de départ <- b0
    //   retour : -1 si v totalement avant w
    //             1 si v totalement apres v
    //            -2 si v est un descendant de w
    //             2 si v est un ancetre de w
    //             0 si v = w (ou erreur)
    int GetSide(tvertex v,tvertex w,tbrin b0)
        {tedge ev=treein[v].GetEdge();
        tedge ew=treein[w].GetEdge();
        int iv1=OldToNew(Index(First(ev)),b0);
        int iv2=OldToNew(Index(Second(ev)),b0);
        int iw1=OldToNew(Index(First(ew)),b0);
        int iw2=OldToNew(Index(Second(ew)),b0);
        // (iv1,iv2) et (iw1,iw2) ne doivent pas etre entrelaces!
#ifdef TDEBUG
        if (((iv1<iw1) && (iv2<iw2)) || ((iw1<iv1) && (iw2<iv2)))
            {
            DPRINTF(("Interlaced Tree Pairs!!\n"));
            myabort();
            }
#endif
        if (iw2<iv1) return -1;
        else if (iv2<iw1) return 1;
        else if (iw1<iv1) return -2;
        else if (iv1<iw1) return 2;
        else return 0;
        }
    void ComputeSuc(svector<tedge> &suc)
        { suc.clear();
        tedge es = 0;
        int i;
        tbrin b;
        tedge ee;
        for(i = size;i >= 1;--i)
            {b = Dos[i];
            ee = b.GetEdge();
            if(_IsTree[ee])
                {suc[b] = es; continue;}
            if(ee != es)
                {if(!IsFirst(b))
                    {suc[ee] = es;es = ee;}
                }
            else
                {while(es != 0 && (i<=Index(First(es))))
                    es = suc[es];
                }
            }
        }
    tbrin ComputeStart()
        {
        //delta = 1/2(variation somme des longueurs)
        int i;
        tbrin b;
        int Delta,DeltaMin;
        int i0 = 1;
        Delta = DeltaMin = 0;
        for(i = 1;i <= size;i++)
            {b = Dos[i];
            if(_IsTree[b.GetEdge()])continue;
            if(DosInv[-b] < i)
                Delta -= m - Abs(i-DosInv[-b]);
            else
                Delta += m - Abs(i-DosInv[-b]);
            if(Delta < DeltaMin){DeltaMin = Delta; i0 = i;}
            }
        return Dos[i0+1];
        }
    void MoveStart()
        {tbrin b;
        tbrin b0=Dos[1];
        if ((b=ComputeStart()) != b0)
            Compute(b);
        }

    int ComputeLevel(svector<int> &level, tbrin b0) 
        {
        svector<int> foc(1,m); foc.SetName("foc"); foc.clear();
        int i,j;
        tbrin b;
        tedge ee;
        int height = 0;
        int lmax=1;
        level[vin[b0]] = 0;
        for(j = 1 ; j <=2*m ; j++)
            {i=(j+DosInv[b0]-2)%size+1;
            b = Dos[i];
            ee = b.GetEdge();
            if(!_IsTree[ee])continue;
            ++foc[ee];
            if(foc[ee] == 1)
                {level[vin[-b]] = ++height;
                if(lmax < height)lmax=height;
                }
            else
                --height;
            }
        return lmax;
        }
    // b and b2 should be adjacent cotree brins with same status
    // (first or second)
    void SwapBrins(tbrin b, tbrin b2)
    {
#ifdef TDEBUG
    if (_IsTree[b.GetEdge()])
        {DPRINTF(("Brin %d belongs to the tree", b()));
        myabort();
        }
    if (_IsTree[b2.GetEdge()])
        {DPRINTF(("Brin %d belongs to the tree", b2()));
        myabort();
        }
    if (vin[b]!=vin[b2])
        {DPRINTF(("Brin %d is incident to %d, but brin %d is incident to %d",
                 b(), vin[b](), b2(), vin[b2]()));
        myabort();
        }
#endif
    tbrin a,c,a2,c2;
    a=acir[b]; if (a==b2) a=b;
    c=cir[b]; if (c==b2) c=b;
    a2=acir[b2]; if (a2==b) a2=b2;
    c2=cir[b2]; if (c2==b) c2=b2;
    cir[a2]=b; cir[b]=c2; acir[c2]=b; acir[b]=a2;
    cir[a]=b2; cir[b2]=c; acir[c]=b2; acir[b2]=a;
    // swap indices.
    int i=DosInv[b]; int i2=DosInv[b2];
    Dos[i]=b2; DosInv[b2]=i;
    Dos[i2]=b; DosInv[b]=i2;
    // First should remain unchanged!!!
#ifdef TDEBUG
    if (i2<DosInv[_First(b.GetEdge())])
        {DPRINTF(("Brin %d becomes first brin", b()));
        myabort();
        }
    if (DosInv[-_First(b.GetEdge())]<i2)
        {DPRINTF(("Brin %d becomes second brin", b()));
        myabort();
        }
    if (i<DosInv[_First(b2.GetEdge())])
        {DPRINTF(("Brin %d becomes first brin", b2()));
        myabort();
        }
    if (DosInv[-_First(b2.GetEdge())]<i)
        {DPRINTF(("Brin %d becomes second brin", b2()));
        myabort();
        }
#endif
    }
};
#endif
