/****************************************************************************
 **
 ** copyright (C) 2001 Hubert de Fraysseix, Patrice Ossona de Mendez.
 ** All rights reserved.
 ** This file is part of the PIGALE Toolkit.
 **
 ** This file may be distributed under the terms of the GNU Public License
 ** appearing in the file LICENSE.HTML included in the packaging of this file.
 **
 ****************************************************************************/


#include <TAXI/Tbase.h>
#include <TAXI/graphs.h>
#include <TAXI/Tprop.h>
#include <TAXI/SchnyderWood.h>
#include <TAXI/Tstack.h>

#define CONE 0
#define HOR_LEFT 1
#define HOR_RIGHT 2

static void CountParents(TopologicalGraph &G,  const SchnyderWood &SW, 
			 short c, tvertex root,
                         svector<int> &Descendants,
                         svector<int> &DoubleEdgeDescent,
                         svector<int> &DistanceRoot) {
  short c_left, c_right;
  SW.CyclicColors(c, c_left, c_right);
  //if (c == Green)    cout << root << endl;
  tvertex v;
  

  if (root == SW.GetRoot(c))
    assert(SW.GetFirstChild(root,c) != 0);
  else 
    if (SW.IsRoot(root))
      return;


  for (v = SW.GetFirstChild(root,c) ; v != 0; v = SW.GetNextChild(v,c)) {
    assert(v != SW.GetRoot(c));
    assert(root != SW.GetRoot(c_left));
    assert(root != SW.GetRoot(c_right));
    DistanceRoot[v] =  DistanceRoot[root]+1;
    CountParents(G, SW, c, v, Descendants, DoubleEdgeDescent, DistanceRoot);
    if (! SW.IsRoot(v)) {
      if (SW.GetParentV(SW.GetParentV(v,c),c_left) == v)
	DoubleEdgeDescent[v]++;
      if (SW.GetParentV(SW.GetParentV(v,c_left),c_right) == v)
	DoubleEdgeDescent[v]++;
      if (SW.GetParentV(SW.GetParentV(v,c_right),c) == v)
	DoubleEdgeDescent[v]++;
    }
    DoubleEdgeDescent[root] += DoubleEdgeDescent[v];
    Descendants[root] += Descendants[v];
  }
}



static void CountVerticesDoubles(TopologicalGraph &G,  const SchnyderWood &SW, 
				 short c, tvertex root,
				 svector<int> &DescendantsLeft,
				 svector<int> &DoubleEdgeDescentLeft,
				 svector<int> &SumDesc_Left,
				 svector<int> &SumDouble_Left,
				 svector<int> &DescendantsRight,
				 svector<int> &DoubleEdgeDescentRight,
				 svector<int> &SumDesc_Right,
				 svector<int> &SumDouble_Right) {
  short c_left, c_right;
  SW.CyclicColors(c, c_left, c_right);
  
  tvertex v;

  if (root == SW.GetRoot(c))
    assert(SW.GetFirstChild(root,c) != 0);
  else 
    if (SW.IsRoot(root))
      return;


  for (v = SW.GetFirstChild(root,c) ; v != 0; v =SW.GetNextChild(v,c)) {
    assert(v != SW.GetRoot(c));
    assert(root != SW.GetRoot(c_left));
    assert(root != SW.GetRoot(c_right));

    if (SW.GetParentV(root, c_left) == v) {
      SumDesc_Left[v] = SumDesc_Left[root] + DescendantsLeft[v] - DescendantsLeft[root];
      SumDouble_Left[v] = SumDouble_Left[root] + DoubleEdgeDescentLeft[v] - DoubleEdgeDescentLeft[root];
    }
    else {
      SumDesc_Left[v] = SumDesc_Left[root] + DescendantsLeft[v];
      SumDouble_Left[v] = SumDouble_Left[root] + DoubleEdgeDescentLeft[v];
    }


    if (SW.GetParentV(root, c_right) == v) {
      SumDesc_Right[v] = SumDesc_Right[root]+ DescendantsRight[v] - DescendantsRight[root];
      SumDouble_Right[v] = SumDouble_Right[root] + DoubleEdgeDescentRight[v] - DoubleEdgeDescentRight[root];
    }
    else {
      SumDesc_Right[v] = SumDesc_Right[root]+ DescendantsRight[v];
      SumDouble_Right[v] = SumDouble_Right[root] + DoubleEdgeDescentRight[v];
    }



    if (!SW.IsRoot(v)) {
      if (SW.GetParentV(SW.GetParentV(v,c),c_left) == v)
	SumDouble_Left[v] --;
      if (SW.GetParentV(SW.GetParentV(v,c_left),c_right) == v)
	SumDouble_Left[v] --;
      if (SW.GetParentV(SW.GetParentV(v,c_right),c) == v)
	SumDouble_Right[v] --;
    }



    CountVerticesDoubles(G, SW, c, v, 
			 DescendantsLeft, DoubleEdgeDescentLeft, SumDesc_Left, SumDouble_Left,
			 DescendantsRight, DoubleEdgeDescentRight, SumDesc_Right, SumDouble_Right);

  }
}


int EmbedCCD(TopologicalGraph &G, bool compact)
  {if(!G.CheckPlanar()){setPigaleError(-1,"not planar: CheckPlanar");return -1;}
  if(! G.CheckTriconnected()){setPigaleError(-1,"not 3-connected: CheckTriconnected");return -1;}
  tvertex v;
  tbrin FirstBrin = G.extbrin();
  SchnyderWood SW(G, FirstBrin);
  int DeltaSmoothCCW_before = SW.CountDeltaSmoothCCW();
  int DeltaSharpCW_before = SW.CountDeltaSharpCW();
  int DeltaSmoothCCW_after = 0;
  assert (SW.CountDeltaSharpCW() == 0);
  int nb_mergesB = 0, nb_mergesR = 0, nb_mergesG = 0;

  int nb_unpointed1, nb_unpointed2;
  nb_unpointed1 = SW.CountUnpointedFaces();

  if (compact) {
    ForAllVertices(v,G) {
      if (SW.cw_merge(v,Blue)) {
        nb_mergesB++;
      }
      if (SW.cw_merge(v,Green)) {
        nb_mergesG++;
      }
      if (SW.cw_merge(v,Red)) {
        nb_mergesR++;
      }
    }
  assert (SW.CountDeltaSharpCW() == 0);
  DeltaSmoothCCW_after = SW.CountDeltaSmoothCCW();
  int DeltaSharpCW_after= SW.CountDeltaSharpCW();
  assert (DeltaSmoothCCW_before == DeltaSmoothCCW_after);
  assert (DeltaSharpCW_before == DeltaSharpCW_after);
  
  int nb_faces =  2 - G.nv() + G.ne();
  int nb_ext_vertices = 1;
  tbrin b;
  for (b =G.cir(-FirstBrin); b != FirstBrin; b= G.cir[-b]) 
    nb_ext_vertices++;
  assert(nb_mergesR + nb_mergesB + nb_mergesG  - (nb_faces -G.nv() + DeltaSmoothCCW_after)== 0);
  }
  if(debug()) {
    nb_unpointed2 = SW.CountUnpointedFaces();
    cout << "n :\t" << G.nv () << "\tm : " << G.ne();
    cout << "\tnb_merges " << nb_mergesR + nb_mergesB + nb_mergesG;
    cout << "\tdelta " <<   DeltaSmoothCCW_after;
    cout << "\tnb_up1 " << nb_unpointed1 << "\t nb_up2 " << nb_unpointed2 << endl;
  }
  // face counting
  svector<int> DescendantsB(1,G.nv(),1);
  svector<int> DescendantsG(1,G.nv(),1);
  svector<int> DescendantsR(1,G.nv(),1);
  DescendantsB.SetName("DescendantsB");
  DescendantsG.SetName("DescendantsG");
  DescendantsR.SetName("DescendantsR");
  
  svector<int> DoubleEdgeDescendantsB(1,G.nv(),0);
  svector<int> DoubleEdgeDescendantsG(1,G.nv(),0);
  svector<int> DoubleEdgeDescendantsR(1,G.nv(),0);
  DoubleEdgeDescendantsB.SetName("DoubleEdgeDescendantsB");
  DoubleEdgeDescendantsG.SetName("DoubleEdgeDescendantsG");
  DoubleEdgeDescendantsR.SetName("DoubleEdgeDescendantsR");

  svector<int> DistanceRootB(1,G.nv(),0);
  svector<int> DistanceRootG(1,G.nv(),0);
  svector<int> DistanceRootR(1,G.nv(),0);
  DistanceRootB.SetName("DistanceRootB");
  DistanceRootG.SetName("DistanceRootG");
  DistanceRootR.SetName("DistanceRootR");

  svector<int> SumDescB_G(1,G.nv(),1);
  svector<int> SumDescR_B(1,G.nv(),1);
  svector<int> SumDescG_R(1,G.nv(),1);
  SumDescB_G.SetName("SumDescB_G");
  SumDescR_B.SetName("SumDescR_B");
  SumDescG_R.SetName("SumDescG_R");

  svector<int> SumDescB_R(1,G.nv(),1);
  svector<int> SumDescG_B(1,G.nv(),1);
  svector<int> SumDescR_G(1,G.nv(),1);
  SumDescB_R.SetName("SumDescB_R");
  SumDescG_B.SetName("SumDescG_B");
  SumDescR_G.SetName("SumDescR_G");


  //LeftDouble edges;

  // number of double edges in the Blue region while treating the tree Green
  svector<int> SumDoubleB_G(1,G.nv(),0);
  svector<int> SumDoubleR_B(1,G.nv(),0);
  svector<int> SumDoubleG_R(1,G.nv(),0);
  SumDoubleB_G.SetName("SumDoubleB_G");
  SumDoubleR_B.SetName("SumDoubleR_B");
  SumDoubleG_R.SetName("SumDoubleG_R");

  svector<int> SumDoubleB_R(1,G.nv(),0);
  svector<int> SumDoubleG_B(1,G.nv(),0);
  svector<int> SumDoubleR_G(1,G.nv(),0);
  SumDoubleB_R.SetName("SumDoubleB_R");
  SumDoubleG_B.SetName("SumDoubleG_B");
  SumDoubleR_G.SetName("SumDoubleR_G");


  CountParents(G, SW, Blue, SW.GetRoot(Blue), DescendantsB, DoubleEdgeDescendantsB, DistanceRootB);

  CountParents(G, SW, Red, SW.GetRoot(Red), DescendantsR, DoubleEdgeDescendantsR, DistanceRootR);
  CountParents(G, SW, Green, SW.GetRoot(Green), DescendantsG, DoubleEdgeDescendantsG, DistanceRootG);

  //Computing around the Blue Tree
  CountVerticesDoubles(G, SW, Blue, SW.GetRoot(Blue), 
		       DescendantsR, DoubleEdgeDescendantsR, SumDescR_B,SumDoubleR_B, 
		       DescendantsG, DoubleEdgeDescendantsG, SumDescG_B, SumDoubleG_B);

  //Computing around the Green Tree
  CountVerticesDoubles(G, SW, Green, SW.GetRoot(Green), 
		       DescendantsB, DoubleEdgeDescendantsB, SumDescB_G,SumDoubleB_G, 
		       DescendantsR, DoubleEdgeDescendantsR, SumDescR_G, SumDoubleR_G);



  //Computing around the Red Tree
  CountVerticesDoubles(G, SW, Red, SW.GetRoot(Red), 
		       DescendantsG, DoubleEdgeDescendantsG, SumDescG_R, SumDoubleG_R, 
		       DescendantsB, DoubleEdgeDescendantsB, SumDescB_R, SumDoubleB_R);

  ForAllVertices(v,G) {
    if (!SW.IsRoot(v)) {
      if (SW.GetParentV(SW.GetParentV(v, Red), Green) == v)
	SumDoubleR_G[v] ++;
      if (SW.GetParentV(SW.GetParentV(v, Green), Blue) == v)
	SumDoubleG_B[v] ++;
      if (SW.GetParentV(SW.GetParentV(v, Blue), Red) == v)
	SumDoubleB_R[v] ++;
    }
  }

  svector<int> x(1,G.nv(),0), y(1,G.nv(),0), z(1,G.nv(),0);
  Prop<short> ecolor2(G.Set(tedge()),PROP_COLOR2,1);
  double a11, a12, a21, a22;
  a11 = 1.0;
  a12 = 0.0;
  a21 = 0.0;
  a22 = 1.0;
  Prop<Tpoint> vcoord(G.Set(tvertex()),PROP_COORD);
  Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);
  Prop<int> ewidth(G.Set(tedge()),PROP_WIDTH);
  tedge e;
  ForAllEdges(e,G) {
    ewidth[e] = 2;
    if (SW.IsSimple(e)) {
      short c1,c2;
      SW.GetEdgeColors(e.firsttbrin(),c1,c2);
      if (c2 != Black) 
	c1 = c2;
      ecolor[e] = c1;
      ecolor2[e] = c1;
      
    }
    else {
      ecolor[e] = SW.GetBrinColor(e.firsttbrin());
      ecolor2[e] = SW.GetBrinColor(e.secondtbrin());
    }
  }

    ForAllVertices(v,G) {
      
    int nb_verticesG = SumDescG_R[v] + SumDescG_B[v] - DescendantsG[v];
    int nb_edgesG = 3* nb_verticesG  - DistanceRootR[v] - DistanceRootB[v] - SumDoubleG_B[v] -SumDoubleG_R[v] + DoubleEdgeDescendantsG[v]- 4;
    int nb_facesG = 2  - nb_verticesG + nb_edgesG;

    int nb_verticesR = SumDescR_G[v] + SumDescR_B[v] - DescendantsR[v];
    int nb_edgesR = 3* nb_verticesR  - DistanceRootG[v] - DistanceRootB[v] - SumDoubleR_B[v] -SumDoubleR_G[v] + DoubleEdgeDescendantsR[v] - 4;
    int nb_facesR = 2  - nb_verticesR + nb_edgesR;


    x[v] = nb_facesG;
    y[v] = nb_facesR;
    if (v == SW.GetRoot(Green)){
      y[v] ++;
      x[v] --;
    }
    if (v == SW.GetRoot(Red)){
      y[v] --;
    }
   if (v == SW.GetRoot(Blue)){
      x[v] ++;
    }
    vcoord[v] = Tpoint(a11*x[v] + a12*y[v], a21*x[v] + a22*y[v]);
  }
  return 0;
}
