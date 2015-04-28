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

#ifndef __SWSHEL_H__
#define __SWSHEL_H__

#include <TAXI/graphs.h>
#include <TAXI/IntList.h>
#include <TAXI/Tdebug.h>

// Compute an order on vertices as described in:
//@inproceedings{fps-ccs3pg-04
//, author =      "E. Fusy and D. Poulalhon and G.Schaeffer "
//, title =       "Coding, counting and sampling 3-connected planar graphs"
//, booktitle =   "$16^{th}$ ACM-SIAM Sympos. Discrete Algorithms"
//, year =        2005
//, note =        "to appear"
//}
// This order is then used by SchnyderWood() to compute the minimal Schnyder wood of a 
// 3-connected plane graph.
class SWShelling {
 protected:
    TopologicalGraph G;

 private:

    int nf;                          // #faces.
    tbrin FirstBrin;                 // = (v_1,v_2).
    tvertex v_1,v_2,v_n, lastVertexFind;

    svector<int> Brin2Face;          // brin -> face
    svector<int> Face2Brin;          // face -> a brin
    svector<int> IsOuterV, IsOuterE; // 1 iff incident to F_out.                                    

    svector<int> MarkedV, MarkedE, MarkedF, visited, sepf, outv, oute;
    svector<int> BelongToLastFace;
    svector<int> LeftBorderBrin, RightBorderBrin;
    
    int error;
    int LastFace;
    int nb_shelled;


    void UpdateSepf1(IntList &NewSepFaces, IntList &NewNonSepFaces);
    void UpdateSepf2(IntList &NewOuterVertices);
    void SetAdjFaces() ;
    tvertex NextCandidate(tvertex lastVertexFind);
    tbrin  GetLeftConnection(tvertex Candidate, int &length);
    public:

    // b is the brin defined by the vertex v_1 and the edge {v_1,v_2}.
    SWShelling(Graph &G0, tbrin bref) : G(G0), nf(G.NumFaces()), FirstBrin(bref),
      v_1(G.vin[bref]), v_2(G.vin[-bref]), v_n(G.vin[-G.acir[bref]]), lastVertexFind(G.vin[-bref]),
        Brin2Face(-G.ne(),G.ne()),
        Face2Brin(1,nf),
        IsOuterV(1,G.nv(),0), IsOuterE(1,G.ne(),0),
        MarkedV(1,G.nv(),0), MarkedE(1,G.ne(),0), MarkedF(1,nf,0),
        visited(1,G.nv(),0), sepf(1,G.nv(),0), outv(1,nf,0), oute(1,nf,0)
	  ,BelongToLastFace(1,G.nv()), LeftBorderBrin (1, G.nv(), 0), RightBorderBrin(1,G.nv(), 0)
      ,error(0), LastFace(0), nb_shelled(0)
        {
        if (G.ComputeGenus()) {error=1; return;}
        G.CheckTriconnected();
        if(!G.Set().exist(PROP_TRICONNECTED)){error=1; return;}
        BelongToLastFace.clear();
        MarkedV.SetName("MarkedV");
        MarkedE.SetName("MarkedE");
        MarkedF.SetName("MarkedF");
	BelongToLastFace.SetName("BelongTo");
	LeftBorderBrin.SetName("LeftBorder");
	RightBorderBrin.SetName("RightBorder");


        SetAdjFaces(); // set Brin2Face, Face2Brin, IsOuterV, IsOuterE.

        tbrin b, b0;

        // initialize outv.                            
        int iv;
        for (iv = 1;iv <= G.nv();iv++)
            if(IsOuterV[iv])   // if v is incident to the outer face...
                {b0 = b =G.FirstBrin(iv);
                do
                    outv[Brin2Face[b]]++;
                while((b = G.cir[b]) != b0);
                }                                            
            
        // initialize oute.                            
        b = FirstBrin;
	//	oute[Brin2Face[-b]]++;
	b = G.cir[-b]; 
        // Skip  edge {v_1,v_2}. Otherwise, the last face would not be shelled.
        do {
            oute[Brin2Face[-b]]++; 
	} while ((b = G.cir[-b])  != FirstBrin);
        visited[v_n()] = 1;
        }

    // return = 0: if there is no vertex nor face to shell.
    //        = k (>=1): the number of vertices on the leftConnection
    int FindNext(tbrin &left, tbrin &right, tbrin &LeftConnection, int &leftActive, int& rightActive);

    int Error() const { return error; }

};


#endif
