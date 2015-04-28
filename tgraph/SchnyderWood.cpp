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


#include <TAXI/SchnyderWood.h>
#include <TAXI/Tmessage.h>
#include <TAXI/Tflist.h>
#include <TAXI/KantShel.h>
#include <TAXI/SWShelling.h>
#include <TAXI/Tdebug.h>
#include <TAXI/color.h>



SchnyderWood::SchnyderWood(Graph &G0, tbrin fb)
  : G(G0), FirstBrin(fb),v_1(G.vin[fb]), v_2(G.vin[-fb]), v_n(G.vin[-G.acir[fb]]),
    ParentB(1,G.nv(),(tbrin)0), ParentG(1,G.nv(),(tbrin)0), ParentR(1,G.nv(),(tbrin)0), brin_color(-G.ne(),G.ne(), Black) 
{svector<short> UnmarkedNeighbors(1, G.nv(), short(0));
  tvertex v;
  tbrin left, right, b, bb;
  tedge e;
  int i;
  ParentB.SetName("ParentB");
  ParentG.SetName("ParentG");
  ParentR.SetName("ParentR");
  UnmarkedNeighbors.SetName("UnmarkedNeighbors");


  SWShelling  S2(G, fb);
  tbrin  LeftConnection;
  int leftActive, rightActive;

  assert(ParentG[v_1] == 0);
  ParentG[v_1] = fb;
  assert(brin_color[fb] == Black);
  brin_color[fb] = Green;

  assert(ParentB[v_2] == 0);
  ParentB[v_2] = -fb;
  assert(brin_color[-fb] == Black);
  brin_color[-fb] = Blue;

  // Compute the minimal Schnyder wood of G
  while (1) {
    i=S2.FindNext(left, right, LeftConnection, leftActive, rightActive);
    if (i == 0)
      break;
    tvertex vLeftConnection = G.vin[LeftConnection];
    tvertex vRight = G.vin[-right];
    v = G.vin[left];
    tvertex vCurrent;
    
    // Color left connection
    for (b = -LeftConnection; b != left; b= -G.cir[b]) {
      vCurrent = G.vin[b];

      assert(ParentB[vCurrent] == 0);
      ParentB[vCurrent] = b;
      assert(brin_color[b] == Black);
      brin_color[b] = Blue;

      assert(ParentG[vCurrent] == 0);
      ParentG[vCurrent] = G.cir[b];
      assert(brin_color[G.cir[b]] == Black);
      brin_color[G.cir[b]] = Green;
    }
    // color green parent of v
    assert(ParentG[v] == 0);
    ParentG[v] = right;
    assert(brin_color[right] == Black);
    brin_color[right] = Green;
 
    // color blue parent of v
    assert(ParentB[v] == 0);
    ParentB[v] = left;
    assert(brin_color[left] == Black);
    brin_color[left] = Blue;

    // color red children of v
    for (b=G.cir[left]; b != right; b= G.cir[b]) {
      vCurrent = G.vin[-b];
      assert(ParentR[vCurrent] == 0);
      ParentR[vCurrent] = -b;
      assert(brin_color[-b] == Black);
      brin_color[-b] = Red;
    }

    // Color parentEdge of leftConnection
    if (!leftActive) {
	assert(ParentR[vLeftConnection] == 0);
	ParentR[vLeftConnection] = LeftConnection;
	assert(brin_color[LeftConnection] == Black);
	brin_color[LeftConnection] = Red;
    }

    // Color parentEdge of vRight
    if (!rightActive) {
	assert(ParentR[vRight] == 0);
	ParentR[vRight] = -right;
	assert(brin_color[-right] == Black);
	brin_color[-right] = Red;
    }

  }
  

  ForAllVertices(v,G) {
    if (v != v_1) {
      if (ParentB[v]  != 0)
	assert(brin_color[ParentB[v]] == Blue);
    }
  }
  ForAllVertices(v,G) {
    if (v != v_n) {
      if (ParentR[v]  != 0)
	assert(brin_color[ParentR[v]] == Red);
    }
  }
  ForAllVertices(v,G) {
    if (v != v_2) {
      if (ParentG[v]  != 0)
	assert(brin_color[ParentG[v]] == Green);
    }
  }
  ForAllEdges(e,G) {
    assert(brin_color[e.firsttbrin()] != Black || brin_color[e.secondtbrin()] != Black);
  }

  //Check the vertex property
  ForAllVertices(v,G) {
    if (v != v_1 && v != v_2 && v != v_n) {
      tbrin b_B, b_R, b_G,b;
      b_B = ParentB[v];
      b_G = ParentG[v];
      b_R = ParentR[v];
      for(b =G.acir[b_B]; b != b_R; b = G.acir[b])
	assert(brin_color[b] == Black && brin_color[-b]== Green);
      for(b =G.acir[b_R]; b != b_G; b = G.acir[b])
	assert(brin_color[b] == Black && brin_color[-b]== Blue);
      for(b =G.acir[b_G]; b != b_B; b = G.acir[b])
	assert(brin_color[b] == Black && brin_color[-b]== Red);
    }
  }

  assert (CountDeltaSharpCW() == 0);
  
  int nb_cwR = 0, nb_cwG = 0 , nb_cwB = 0;
  int nb_ccwR = 0, nb_ccwG = 0 , nb_ccwB = 0;

  ForAllVertices(v,G) {
    if(is_cw_elbow(v, Red))
      nb_cwR++;
    if(is_cw_elbow(v, Green))
      nb_cwG++;
    if(is_cw_elbow(v, Blue))
      nb_cwB++;
  }

  ForAllVertices(v,G) {
    if(is_ccw_elbow(v, Red))
      nb_ccwR++;
    if(is_ccw_elbow(v, Green))
      nb_ccwG++;
    if(is_ccw_elbow(v, Blue))
      nb_ccwB++;
      }
}

bool SchnyderWood::IsSimple (tedge e) const {
  if (brin_color[e.firsttbrin()] == Black || brin_color[e.secondtbrin()] == Black)
    return true;
  else
    return false;
}

bool SchnyderWood::IsBlack (tedge e) const {
  return (brin_color[e.firsttbrin()] == Black && brin_color[e.secondtbrin()] == Black);
}

bool SchnyderWood::IsRoot (tvertex v) const {
  return (v == v_1 || v ==v_2 || v == v_n);
}


tbrin SchnyderWood::GetParentBr(tvertex v, short c) const {
  switch(c) {
  case Blue : 
    return ParentB[v];
  case Red :
    return ParentR[v];
  case Green :
    return ParentG[v];
  default : 
    setPigaleError(-1);
    return 0;
  }
}

tedge SchnyderWood::GetParentE (tvertex v, short c) const {
  if (GetParentBr(v,c) == 0)
    return 0;
  else
    return GetParentBr(v,c).GetEdge();
}

tvertex SchnyderWood::GetParentV (tvertex v, short c) const {
  if (v == 0)
    return 0;
  if (GetParentBr(v,c) == 0)
    return 0;
  else
    return G.vin[-GetParentBr(v,c)];
}
tvertex SchnyderWood::GetRoot(short c) const {
  switch(c) {
  case Blue : 
    return v_1;
  case Red :
    return v_n;
  case Green :
    return v_2;
  default : 
    setPigaleError(-1);
    return 0;
  }
}
void SchnyderWood::GetEdgeColors (tbrin b, short &c1, short &c2) const {
 c1 =brin_color[b];
 c2 =brin_color[-b];
}
short SchnyderWood::GetBrinColor (tbrin b) const {
  return brin_color[b];
}


bool SchnyderWood::remove_cw_elbow (tvertex v, short c) {
  if (!is_cw_elbow(v,c))
    return false;
  tbrin b1,b2;
  b1 = GetParentBr(v,c);
  assert (brin_color[-b1] == Black);
  b2 = G.cir[-b1];
  switch(c) {
  case Blue : 
    assert (brin_color[b2] == Red && brin_color[-b2] == Green);
    ParentR[G.vin[b2]] = -b1;
    break;
  case Red :
    assert (brin_color[b2] == Green && brin_color[-b2] == Blue);
    ParentG[G.vin[b2]] = -b1;
    break;
  case Green :
    assert (brin_color[b2] == Blue && brin_color[-b2] == Red);
    ParentB[G.vin[b2]] = -b1;
    break;
  default : 
    setPigaleError(-1);
    return 0;
  }
  brin_color[-b1] = brin_color[b2];
  brin_color[b2] = Black;
  assert(!is_cw_elbow(v,c)); 
  return true;
}


bool SchnyderWood::is_cw_elbow(tvertex v, short c) const {
  tbrin b1,b2;
  if (v == v_1 || v == v_2 || v == v_n)
    return false;
  b1 = GetParentBr(v,c);
  if (brin_color[-b1] != Black)
    return false;
  b2 = G.cir[-b1];
  switch(c) {
  case Blue : 
    if (brin_color[b2] != Red || brin_color[-b2] != Green)
      return false;
    break;
  case Red :
    if (brin_color[b2] != Green || brin_color[-b2] != Blue)
      return false;
    break;
  case Green :
    if (brin_color[b2] != Blue || brin_color[-b2] != Red)
      return false;
    break;
  default : 
    setPigaleError(-1);
    return false;
  }
  return true;
}


bool SchnyderWood::remove_ccw_elbow (tvertex v, short c) {
  if (!is_ccw_elbow(v,c))
    return false;
  tbrin b1,b2;
  b1 = GetParentBr(v,c);
  b2 = G.acir[-b1];
  assert (brin_color[-b1] == Black);
  switch(c) {
  case Blue : 
    assert (brin_color[b2] == Green && brin_color[-b2] == Red);
    ParentG[G.vin[b2]] = -b1;
    break;
  case Red :
    assert (brin_color[b2] == Blue && brin_color[-b2] == Green);
    ParentB[G.vin[b2]] = -b1;
    break;
  case Green :
    assert (brin_color[b2] == Red && brin_color[-b2] == Blue);
    ParentR[G.vin[b2]] = -b1;
    break;
  default : 
    setPigaleError(-1);
    return false;
  }
  brin_color[-b1] = brin_color[b2];
  brin_color[b2] = Black;
  assert(!is_ccw_elbow(v,c)); 
  return true;
}



bool SchnyderWood::is_ccw_elbow(tvertex v, short c)  const {
  if (v == v_1 || v == v_2 || v == v_n)
    return false;
  tbrin b1,b2;
  b1 = GetParentBr(v,c);

  if (brin_color[-b1] != Black)
    return false;
  b2 = G.acir[-b1];
  switch(c) {
  case Blue : 
    if (brin_color[b2] != Green || brin_color[-b2] != Red)
      return false;
    break;
  case Red :
    if (brin_color[b2] != Blue || brin_color[-b2] != Green)
      return false;
    break;
  case Green :
    if (brin_color[b2] != Red || brin_color[-b2] != Blue)
      return false;
    break;
  default : 
    setPigaleError(-1);
    return false;
  }
  return true;
}



bool SchnyderWood::is_cw_face(tvertex v)  const {
  if (v == v_1 || v == v_2 || v == v_n)
    return false;
  tbrin b1,b2,b3;
  b1 = ParentR[v];
  b2 = G.cir[-b1];
  b3 = G.cir[-b2];
  return (brin_color[b2] == Green && brin_color[b3] == Blue && brin_color[-b1] == Black &&
	  brin_color[-b2] == Black && brin_color[-b3] == Black && G.vin[-b3] == v);
}

bool SchnyderWood::reverse_cw_face(tvertex v) {
  if (!is_cw_face(v)) 
    return false;
    tbrin b1,b2,b3;
    tvertex u,w;
  b1 = ParentR[v];
  b2 = G.cir[-b1];
  b3 = G.cir[-b2];
  u= GetParentV(v,Red);  
  w= GetParentV(u,Green);

  ParentR[v] = -b3;
  brin_color[-b3] = Red;
  brin_color[b1] = Black;

  ParentG[u] = -b1;
  brin_color[-b1] = Green;
  brin_color[b2] = Black;
  
  ParentB[w] = -b2;
  brin_color[-b2] = Blue;
  brin_color[b3] = Black;
  return true;
}


bool SchnyderWood::is_ccw_face(tvertex v) const {
  if (v == v_1 || v == v_2 || v == v_n)
    return false;
  tbrin b1,b2,b3;
  b1 = ParentR[v];
  b2 = G.acir[-b1];
  b3 = G.acir[-b2];
  return (brin_color[b2] == Blue && brin_color[b3] == Green && brin_color[-b1] == Black &&
	  brin_color[-b2] == Black && brin_color[-b3] == Black && G.vin[-b3] == v);
}

bool SchnyderWood::reverse_ccw_face(tvertex v) {
  if (!is_ccw_face(v)) 
    return false;
  tbrin b1,b2,b3;
  tvertex u,w;

  b1 = ParentR[v];
  b2 = G.acir[-b1];
  b3 = G.acir[-b2];

  u= GetParentV(v,Red);  
  w= GetParentV(u,Blue);

  ParentR[v] = -b3;
  brin_color[-b3] = Red;
  brin_color[b1] = Black;

  ParentB[u] = -b1;
  brin_color[-b1] = Blue;
  brin_color[b2] = Black;
  
  ParentG[w] = -b2;
  brin_color[-b2] = Green;
  brin_color[b3] = Black;
  return true;
}



int SchnyderWood::CountDeltaSharpCW () const {
  tvertex v;
  int count = 0;
  ForAllVertices(v,G) {
    if (is_cw_face(v))
      count ++;
    if (is_cw_elbow(v,Blue))
      count ++;
    if (is_cw_elbow(v,Red))
      count ++;
    if (is_cw_elbow(v,Green))
    count ++;
  }
  return count;
}

int SchnyderWood::CountDeltaSharpCCW ()  const {
  tvertex v;
  int count = 0;
  ForAllVertices(v,G) {
    if (is_ccw_face(v))
      count ++;
    if (is_ccw_elbow(v,Blue))
      count ++;
    if (is_ccw_elbow(v,Red))
      count ++;
    if (is_ccw_elbow(v,Green))
    count ++;
  }
  return count;
}

bool SchnyderWood::is_ccw_smooth(tvertex v) const {
  if (v == v_n)
      return false;
  tbrin b1, b2, b3, b4;
  b1 = ParentR[v];

  b2 = G.acir[-b1];
  b4  = G.cir[b1];
  while (IsBlack(b4.GetEdge()))
    b4 = G.cir[b4];
  if (brin_color[-b4] == Red)
    return false;
  while (IsBlack(b2.GetEdge()))
    b2 = G.acir[b2];
  while(brin_color[b2] != Blue && b2 != b1) {
    b2 = G.acir[-b2];
    while (IsBlack(b2.GetEdge()))
      b2 = G.acir[b2];
  }
  
  b3 = G.acir[-b2];
  while (IsBlack(b3.GetEdge()))
    b3 = G.acir[b3];
  while(brin_color[b3] != Green && b3 != b1) {
    b3 = G.acir[-b3];
    while (IsBlack(b3.GetEdge()))
      b3 = G.acir[b3];
  }
  return (brin_color[b2] == Blue && brin_color[b3] == Green);
}


int SchnyderWood::CountDeltaSmoothCCW ()  const {
  tvertex v;
  int count = 0;
  ForAllVertices(v,G) {
    if (is_ccw_smooth(v))
      count ++;
  }
  return count;
}

bool SchnyderWood::is_cw_smooth(tvertex v) const {
  if (v == v_n)
      return false;
  tbrin b1, b2, b3, b4;
  b1 = ParentR[v];

  b2 = G.cir[-b1];

  b4  = G.acir[b1];
  while (IsBlack(b4.GetEdge()))
    b4 = G.acir[b4];
  if (brin_color[-b4] == Red)
    return false;

  while (IsBlack(b2.GetEdge()))
    b2 = G.cir[b2];
  while(brin_color[b2] != Green && b2 != b1) {
    b2 = G.cir[-b2];
    while (IsBlack(b2.GetEdge()))
      b2 = G.cir[b2];
  }
  
  b3 = G.cir[-b2];
  while (IsBlack(b3.GetEdge()))
    b3 = G.cir[b3];
  while(brin_color[b3] != Blue && b3 != b1) {
    b3 = G.cir[-b3];
    while (IsBlack(b3.GetEdge()))
      b3 = G.cir[b3];
  }
  return (brin_color[b2] == Green && brin_color[b3] == Blue);
}



int SchnyderWood::CountDeltaSmoothCW ()  const {
  tvertex v;
  int count = 0;
  ForAllVertices(v,G) {
    if (is_cw_smooth(v))
      count ++;
  }
  return count;
}


bool SchnyderWood::cw_merge (tvertex v, short c) {
   if (IsRoot(v))
     return false;
   tbrin b1,b2;
   b1 = GetParentBr(v,c);
   b2 = G.acir[b1];
  if (brin_color[-b1] != Black || brin_color[b2] != Black)
    return false;
  if (IsBlack(b2.GetEdge()))
     return false;
  brin_color[b2] = c;
  brin_color[b1] = Black;
  switch(c) {
  case Blue : 
    ParentB[v] = b2;
    break;
  case Red :
    ParentR[v] = b2;    
    break;
  case Green :
    ParentG[v] = b2;
    break;
  default : 
    setPigaleError(-1);
    return false;
  }
  return true;
}

bool SchnyderWood::ccw_merge (tvertex v, short c) {
   tbrin b1,b2;
   b1 = GetParentBr(v,c);
  b2 = G.cir[b1];
  if (brin_color[-b1] != Black || brin_color[b2] != Black)
    return false;
  brin_color[b2] = c;
  brin_color[b1] = Black;
  switch(c) {
  case Blue : 
    ParentB[v] = b2;
    break;
  case Red :
    ParentR[v] = b2;    
    break;
  case Green :
    ParentG[v] = b2;
    break;
  default : 
    setPigaleError(-1);
    return false;
  }
  return true;
}


void SchnyderWood::CyclicColors(short c, short &c_left, short &c_right) const {
   switch(c) {
  case Blue : 
    c_left = Red; c_right = Green;
    break;
  case Red :
    c_left = Green; c_right = Blue;
    break;
  case Green :
    c_left = Blue; c_right = Red;
    break;
  default : 
    setPigaleError(-1);
  }
}
tvertex SchnyderWood::GetFirstChild( tvertex v, short c)  const {
  short c_left, c_right;
  CyclicColors(c, c_left, c_right);
  tbrin b_next;
  tbrin p_left = GetParentBr(v, c_left);
  if (p_left == 0)
    return 0;
  if (GetParentV( GetParentV(v, c_left),c) == v) {
    assert(!IsSimple(GetParentE(v,c_left)));
    return GetParentV(v, c_left);
  }
  for(b_next = -G.acir[p_left]; IsBlack(b_next.GetEdge()); b_next = -G.acir[-b_next]){;}
  tvertex child = G.vin[b_next];
  if (GetParentV(child,c) == v)
    return child;
  else
    return 0;
}

tvertex SchnyderWood::GetNextChild (tvertex current_child, short c) const  {
  assert(current_child != GetRoot(c));
  tbrin b_next;
  tbrin bp = GetParentBr(current_child, c);
  if (bp == 0)
    return 0;
  for(b_next = -G.acir[-bp]; IsBlack(b_next.GetEdge()); b_next = -G.acir[-b_next]){;}
  tvertex child = G.vin[b_next];
  if (GetParentV(child,c) == GetParentV(current_child,c)) {
    if (child != GetFirstChild(GetParentV(current_child,c),c))
      return child;
    else
      return 0;
  }
  else
    return 0;
}

tvertex SchnyderWood::GetLastChild( tvertex v, short c) const {
  short c_left, c_right;
  CyclicColors(c,c_left,c_right);
  tbrin p_right = GetParentBr(v,c_right);
  if (p_right == 0)
    return 0;

  if (GetParentV( GetParentV(v, c_right),c) == v) {
    assert(!IsSimple(GetParentE(v,c_right)));
    return GetParentV(v, c_right);
  }
  tvertex child = G.vin[-G.cir[p_right]];
  if (GetParentV(child,c) == v)
    return child;
  else
    return 0;
}

tvertex SchnyderWood::GetPreviousChild (tvertex current_child, short c) const {
  assert(current_child != GetRoot(c));
  tbrin bp = GetParentBr(current_child,c);
  if (bp == 0)
    return 0;
  tvertex child = G.vin[-G.cir[-bp]];
  if (GetParentV(child,c) == GetParentV(current_child,c)) {
    if (child != GetLastChild(GetParentV(current_child,c),c))
      return child;
    else
      return 0;
  }
  else
    return 0;
}

  


bool SchnyderWood::is_unpointed_face(tbrin b) const {
  assert(!IsBlack(b.GetEdge()));
  tbrin current_b = b;
  tbrin prev_b = b;
  do {
    prev_b = current_b;
    current_b = G.cir[-current_b];
    while (IsBlack(current_b.GetEdge()))
      current_b = G.cir[current_b];
    if (brin_color[-current_b] == brin_color[prev_b] && brin_color[-current_b] != Black)
      return false;
  } while (b != current_b);
  //  cout << "unpointed " << G.vin[b] << " " << G.vin[-b] << " " << G.vin[prev_b] << endl;
  return true;
}

bool SchnyderWood::isFirstBrinOfFace(tbrin b) const {
  assert(!IsBlack(b.GetEdge()));
  int min_b = abs(b());
  tbrin current_b = b;
  do {
    current_b = G.cir[-current_b];
    while (IsBlack(current_b.GetEdge()))
      current_b = G.cir[current_b];
    if (abs(current_b()) < min_b) 
      return false;
  } while (b != current_b);
  return true;
}

int SchnyderWood::CountUnpointedFaces() const {
  tedge e;
  tbrin b;
  int nb_unpointed = 0;
  ForAllEdges(e,G) {
    if (!IsBlack(e)) {
      b= e.firsttbrin();
      if (isFirstBrinOfFace(b))
	if (is_unpointed_face(b))
	  nb_unpointed ++;
      b= e.secondtbrin();
      if (isFirstBrinOfFace(b))
	if (is_unpointed_face(b))
	  nb_unpointed ++;
    }
  }
  //  assert(nb_unpointed % 2 == 0);
  return nb_unpointed ;
}
