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



#include  <TAXI/SWShelling.h>
#include  <TAXI/color.h>
#include  <TAXI/Tmessage.h>
#include  <TAXI/Tflist.h>


#include  <TAXI/SWShelling.h>
#include  <TAXI/color.h>
#include  <TAXI/Tmessage.h>
#include  <TAXI/Tflist.h>



// for all f \in NewSepFaces, for all v incident to f, sepf[v]++;
// for all f \in NewNonSepFaces, for all v incident to f, sepf[v]--;
void SWShelling::UpdateSepf1(IntList &NewSepFaces, IntList &NewNonSepFaces) {
  tbrin b, b0;
  int f;
  NewSepFaces.ToHead();
  while ((f=~NewSepFaces)!=0) {
    b0=b=Face2Brin[f];
    do {
      sepf[G.vin[b]()]++; 
      b=G.cir[-b];
    } while (b!=b0);
    ++NewSepFaces;
  }
  NewNonSepFaces.ToHead();
  while ((f=~NewNonSepFaces)!=0) {
    b0=b=Face2Brin[f];
    do {
      sepf[G.vin[b]()]--; 
      b=G.cir[-b];
    } while (b!=b0);
    ++NewNonSepFaces;
  }
}

// for all v \in NewOuterVertices, recalculate sepf[v].
void SWShelling::UpdateSepf2(IntList &NewOuterVertices) {

  tbrin b, b0;
  int f;
  int vi, count;

  NewOuterVertices.ToHead();
  while ((vi=~NewOuterVertices)!=0) {
    count=0;
    b0=b=G.FirstBrin(vi);
    do {
      f=Brin2Face[b];
      if(!MarkedF[f] && (outv[f] - oute[f]> 1)) 
	count++;
      b=G.cir[b];
    } while (b!=b0);
    sepf[vi] = count; 
    ++NewOuterVertices;
  }
} 

void SWShelling::SetAdjFaces() {
  /* set Brin2Face, Face2Brin, IsOuterV, IsOuterE.
     b = Face2Brin[num]
     b and acir[b] defines an angle
     Moving along a face: cir[-b] or -acir[b]
  */
  tbrin b, b0;
  // Mark the vertices incident to the last face
  Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);
  Prop<int> ewidth(G.Set(tedge()),PROP_WIDTH);
  IntList Brins;  // Brins: list of all brins 
  b = b0 = FirstBrin();
  do {
    BelongToLastFace[G.vin[b]()] = 1;
    sepf[G.vin[b]()] = 1;
//     if(debug()) {
//       ecolor[b.GetEdge()] = Green2;
//       ewidth[b.GetEdge()] = 3;
//     }
  } while((b = -G.cir[b]) != b0);

  
  for(int i = -G.ne();i <= G.ne();i++)
    if(i)Brins.push(i);
  Brin2Face[FirstBrin()]=1;			  
  IsOuterV[v_1] = 1;
  LeftBorderBrin[v_1] = FirstBrin();
  RightBorderBrin[v_1] = G.acir[FirstBrin]();


  IsOuterE[FirstBrin.GetEdge()] = 1;
  Brins.del(FirstBrin());


    LeftBorderBrin[v_n()]  = -G.acir[FirstBrin]();
    RightBorderBrin[v_n()] = -G.cir[-FirstBrin]();
    IsOuterV[v_n] = 1;

    LeftBorderBrin[v_2()] = G.cir[-FirstBrin]();
    RightBorderBrin[v_2()] = -FirstBrin();
    IsOuterV[v_2] = 1;
  // the outer face is indexed 1
  b = G.cir[-FirstBrin];
  do {
    Brins.del(b());
    Brin2Face[b()] = 1;
    IsOuterE[b.GetEdge()] = 1;
    IsOuterV[G.vin[b]()] = 1;
    LeftBorderBrin[G.vin[b]] = b();
    RightBorderBrin[G.vin[-b]] = -b();
  } while ((b = G.cir[-b])  != FirstBrin);
  Face2Brin[1]=FirstBrin();

  // indexing other faces.
  int FaceIndex=2;
  while (!Brins.empty()) {
    b0 = b =Brins.first();
//     if(debug())cout << "face:" << FaceIndex << endl;
    do {
      Brins.del(b());
      Brin2Face[b]=FaceIndex;
//       if(debug())
// 	cout << G.vin[b]() << " " <<endl;
    } while((b = G.cir[-b]) != b0);
    
    Face2Brin[FaceIndex]=b();
    FaceIndex++;
  }
}



//retrun the right brin of the NextCandidate.
//If there is no more candidate it returns FirstBrin
tvertex SWShelling::NextCandidate(tvertex lastVertexFind) {
  tbrin bRight = RightBorderBrin[lastVertexFind];
  tvertex current, next_current;

  assert(IsOuterV[G.vin[bRight]]);
  assert(!MarkedV[G.vin[bRight]]);
  assert(IsOuterE[bRight.GetEdge()]);



  //FindRight
  current = G.vin[-bRight];
  assert(IsOuterV[current]);
  if (current != v_2 && current != v_1) {
    next_current = G.vin[-RightBorderBrin[current]];
    while (next_current == G.vin[-G.cir[LeftBorderBrin[current]]]) {
      current = next_current;
      assert(IsOuterV[current]);
      next_current = G.vin[-RightBorderBrin[current]];
    }
	   //return current;
  }

  //FindLeft
    //  current = lastVertexFind;
  do {
    assert(IsOuterV[current]);
    if (visited[current] && sepf[current] == 0 && current != v_2 && current != v_1)
      return current;
    current = G.vin[-LeftBorderBrin[current]];
  } while (current != v_1);

  return current;
}



tbrin  SWShelling::GetLeftConnection(tvertex Candidate, int &length) {
  tbrin bnext1, bnext2;
  bnext1 = -LeftBorderBrin[Candidate];
  bnext2 = -G.acir[bnext1];
  length = 1;
  while (bnext2 == -LeftBorderBrin[G.vin[bnext1]] && G.vin[bnext1] != v_1 ) {
    assert(IsOuterV[G.vin[bnext2]]);
    bnext1 = bnext2;
    bnext2 = -G.acir[bnext1];
    length++;
  }
  return bnext1;
}

// return = 0: if there is no vertex nor face to shell.
//        = k (>=1): the number of vertices on the leftConnection
int SWShelling::FindNext(tbrin &left, tbrin &right, tbrin &LeftConnection, int &leftActive, int& rightActive) {

  int count, f;
  tbrin b,b0,LastB,StopB;
  tvertex  Candidate;

  Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);
  Prop<short> vcolor(G.Set(tvertex()),PROP_COLOR);
  Prop<int> ewidth(G.Set(tedge()),PROP_WIDTH);

  if (LastFace) {
    assert(G.nv() -2 == nb_shelled);
    return 0;
  }
  Candidate = NextCandidate(lastVertexFind);
  left = LeftBorderBrin[Candidate];
  right = RightBorderBrin[Candidate];
  LeftConnection = GetLeftConnection(Candidate, count);

  leftActive = visited[G.vin[LeftConnection]];
  rightActive = visited[G.vin[-right]];

  if (right == FirstBrin)
    assert(true);
  assert (Candidate != v_1);
  assert (Candidate != v_2);
  assert (!MarkedV[Candidate]);


  
//   if(debug()) {
//     for(tedge e = 1;e <= G.ne();e++) {
//       if(ecolor[e] == Violet)continue;
//       if(IsOuterE[e])ecolor[e]=Green; // exterior face
//       if(MarkedE[e]){ecolor[e]=Red;ewidth[e] = 3;}
//     }
//     for(tvertex w=1;w < G.nv();w++) {
//       if (MarkedV[w])
// 	vcolor[w] = Red;
//       else 
// 	vcolor[w] = Yellow;
//     }
//     ecolor[FirstBrin.GetEdge()()] = Yellow;
//     static int firstTime = 1;
//     if(firstTime) {
//       {firstTime = 0;
//       cout <<"v1: " << v_1<< "  v2: " <<v_2 << "  vn: "<< v_n << endl;
//       DrawGraph();// red: treated green: exterior face
//       Twait("Init");
//       }
//     }
//   }

  if (BelongToLastFace[Candidate]) {
    LastFace = 1;
    MarkedV[Candidate] = 1;
    tbrin bnext1, bnext2;
    bnext1  = -LeftBorderBrin[Candidate];
    bnext2 = -G.acir[bnext1];
    while (G.vin[bnext1] != v_1 ) {
      assert(IsOuterV[G.vin[bnext2]]);
      bnext1 = bnext2;
      bnext2 = -G.acir[bnext1];
      MarkedV[G.vin[bnext1]] = 1;
    }
      
    nb_shelled += count;
    lastVertexFind = count;
    return count;
  }

  ///////////////////////////////////////////////////////////
  //Update the Border of G_k, oute, outf, sepf, MarkedV, MarkedE, IsOuterV, IsOuterE

  // list of the vertices which become incident to F_out by the shelling.
  IntList NewOuterVertices;
  // list of the faces which were not separating faces before the shelling.  
  IntList SepFaces; 
  // list of the faces which were not separating faces before the shelling.  
  IntList NonSepFaces; 
  // list of the faces which become separating faces by the shelling.
  IntList NewSepFaces;
  // list of the faces which become non-separating faces by the shelling.
  IntList NewNonSepFaces; 
  

  MarkedV[Candidate]=1;
//   if(debug()) {
//     vcolor[Candidate] = Red;
//     cout << "packing vertex:" <<-i << "*************************"<< endl;
//   }
    
//   if(debug()) {
//     ecolor[left.GetEdge()()] = Blue;
//     ecolor[right.GetEdge()()] = Orange; 
//     ewidth[left.GetEdge()()] = 3; 
//     ewidth[right.GetEdge()()] = 3; 
//     cout << "***Packing vertex = " << -i << endl;
//     cout  << " v_left = " << G.vin[-left] << "  v_right = " << G.vin[-right]<< endl;
//     DrawGraph();Twait("packing vertex");
//   }

  b = left;
  while(!MarkedE[b.GetEdge()]) {
    MarkedE[b.GetEdge()]=1;
//     if(debug()) {
//       if(b!= left && b !=right)
// 	ecolor[b.GetEdge()] = Pink;
//       else 
// 	ecolor[b.GetEdge()] = Red;
//     }
    b=G.cir[b];
  }

  // Mark All edges of the leftconnection.
  // ...
  // update MarkedV[v], MarkedE[e].
  b = -left;
  while (b != LeftConnection) {
    MarkedV[G.vin[b]()] = 1;
//     if(debug()) {
//       vcolor[G.vin[b]()] = Red;
//       ecolor[b.GetEdge()()] = Violet;ewidth[b.GetEdge()()] = 3;
//     }
    MarkedE[b.GetEdge()()] = 1;
    b = -G.acir[b];
  }
  MarkedE[LeftConnection.GetEdge()()];

      
  // update MarkedF[f], IsOuterV[v], IsOuterE[e].
  // visited : vertices adjacent to v => visited has at least a  MarkedE
  // Mark edges of the new face under the v umbrella
  visited[G.vin[LeftConnection]()] = 1;
  visited[G.vin[-right]()] = 1;
  b=-G.acir[LeftConnection];
  while (1) {
    f = Brin2Face[b()];
    MarkedF[f]=1;
//     if(debug())
//       cout << "marking face:" << f << endl;
    IsOuterV[G.vin[b]()] = 1;
    IsOuterE[b.GetEdge()()] = 1;
    LeftBorderBrin[G.vin[b]] = b();
    RightBorderBrin[G.vin[-b]] = -b();

    b = G.acir[b];
    if(-b == right())
      break;
    if(G.vin[-b] == Candidate) {
      visited[G.vin[b]()] = 1; 
      b = G.acir[b];
    }
    b = -b;
  }
      
  // set NewOuterVertices, NonSepFaces.
  b = -G.acir[LeftConnection];
  LastB = G.cir[-right];
    
  f = Brin2Face[-b];
  if (outv[f]-oute[f] <= 1) {
    if (!NonSepFaces.InList(f)) {
      NonSepFaces.push(f);
//       if(debug()) cout << "pushing NonSepFace: "<< f << endl;
    }
  }
  else {
    if (!SepFaces.InList(f)) {
      SepFaces.push(f);
//       if(debug()) cout << "pushing SepFace: "<< f << endl;
    }
  }
    
      
  while (b != LastB) { 
    NewOuterVertices.push(G.vin[b]());
//     if(debug())vcolor[G.vin[b]] = Green;
    StopB = b;
    do {
      StopB = G.acir[StopB];
    } while (G.vin[-StopB] == Candidate); // once or twice
//     if(debug())cout << "--------b:"<< G.vin[b]() << "  "<< G.vin[StopB]()<< endl;
    while(b != StopB) {
      b = G.cir[b];
      f = Brin2Face[b];
      if (outv[f]-oute[f] <= 1) {
	if (!NonSepFaces.InList(f)) {
	  NonSepFaces.push(f);
// 	  if(debug()) cout << "pushing NonSepFace: "<< f << endl;
	}
      }
      else {
	if (!SepFaces.InList(f)) {
	  SepFaces.push(f);
// 	  if(debug()) cout << "pushing SepFace: "<< f << endl;
	}
      }
    }
    b=-StopB;
  }
//   if(debug()){ DrawGraph();Twait("vertex:packed");}
    
  // for all v newly incident to F_out, 
  //   for all f incident to v, outv[f]++.
  // update oute[f].
  b = -G.acir[LeftConnection];
  LastB=G.cir[-right];
  while (b!=LastB) { 
    oute[Brin2Face[-b]] ++;
    LeftBorderBrin[G.vin[b]] = b();
    RightBorderBrin[G.vin[-b]] = -b();
    b0=b;
    do {
      f=Brin2Face[b]; 
      if (!MarkedF[f]) outv[f] ++;
    }while((b = G.cir[b]) != b0);
    b = G.acir[b];
    if(G.vin[-b] == Candidate)
      b = G.acir[b];
    b = -b;
  }       
  oute[Brin2Face[-b]] ++;
  LeftBorderBrin[G.vin[b]] = b();
  RightBorderBrin[G.vin[-b]] = -b();
      
  // set NewSepFaces.
  NonSepFaces.ToHead();
  while ((f = ~NonSepFaces) != 0) {
    if(outv[f]- oute[f] > 1) {
      NewSepFaces.push(f);
//       if(debug()) cout << "pushing NewSepFaces f: "<< f << " " << G.vin[Face2Brin[f]] <<" " << G.vin[-Face2Brin[f]]<< " " << G.vin[-G.acir[Face2Brin[f]]] <<  endl; 
    }
    ++NonSepFaces;
  }        
  
  SepFaces.ToHead();
  while ((f = ~SepFaces) != 0) {
    if(outv[f]- oute[f] <= 1) {
      NewNonSepFaces.push(f);
//       if(debug()) cout << "pushing NewNonSepFaces f: " << f  << " " << G.vin[Face2Brin[f]] <<" " << G.vin[-Face2Brin[f]]<< " " << G.vin[-G.acir[Face2Brin[f]]] <<  endl;
    }
    ++SepFaces;
  }     
  // for all f \in NewSepFaces, for all v incident to f, sepf[v]++;
  // for all f \in NewNonSepFaces, for all v incident to f, sepf[v]--;
  UpdateSepf1(NewSepFaces, NewNonSepFaces);

  // for all v \in NewOuterVertices, update sepf[v].
  UpdateSepf2(NewOuterVertices);
  
  nb_shelled += count;
  return count;
}
  
  
