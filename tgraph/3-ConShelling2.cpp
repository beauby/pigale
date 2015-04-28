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



#include  <TAXI/KantShel.h>
#include  <TAXI/color.h>
#include  <TAXI/Tmessage.h>
#include  <TAXI/Tflist.h>

int KantShelling::FindNext(tbrin &left, tbrin &right)
// return = 0: if there is no vertex nor face to shell.
//        = 1: if a vertex v has been shelled. left (resp. right)
//             will be the leftmost (resp. rightmost) brin
//             incident to v.
//        = k (>1): a face has been shelled. k is the length
//             of the shelled chain {v_1,...,v_p}.
//             left=(v_1,v_2), right=(v_{p-1},v_p).
// MarkedV = 1 if the vertex is saturated -> Red
// MarkedE =1 if the edge had been packed -> Red or Violet
  {int i,count;
  tbrin b,b0,LastB,StopB;
  tvertex v;

  Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);
  Prop<short> vcolor(G.Set(tvertex()),PROP_COLOR);
  Prop<int> ewidth(G.Set(tedge()),PROP_WIDTH);

  do
      {if(Candidates.empty()) // shell the last face.
          {if(!MarkedV[G.vin[-G.cir[FirstBrin]]()])  
              {count=0;
              left=G.cir[FirstBrin];
              right=left;
              do 
                  {count++;right=G.cir[-right];
                  MarkedE[right.GetEdge()] = 1;
                  } 
              while (right!=-FirstBrin);
              right=-G.acir[right];
              MarkedE[left.GetEdge()] = 1;
              MarkedV[G.vin[-G.cir[FirstBrin]]()]=1;
              
//               if(debug())
//                   {for(tedge e = 1;e <= G.ne();e++) {ecolor[e]=Black;ewidth[e] = 1;}
//                   for(tedge e = 1;e <= G.ne();e++)
//                       {if(IsOuterE[e])ecolor[e]=Green; // exterior face
//                       if(MarkedE[e]){ecolor[e]=Red;ewidth[e] = 4;}
//                       }
//                   for(tvertex w=1;w <= G.nv();w++)
//                       if(MarkedV[w])vcolor[w] = Red;
//                   ecolor[FirstBrin.GetEdge()()] = Yellow;
//                    if(debug())cout << "last  path length: " << count << endl;
//                    //if(debug())Twait("end packing");
//                   DrawGraph();
//                   }
              return count;
              }
          // return FirstBrin at the last time.
          left = FirstBrin;
          //All edges should be red except the FisrtBrin
          int nMark = 1;
          for(tedge e = 1;e <= G.ne();e++)
              if(MarkedE[e]) ++nMark;
          if(nMark != G.ne())error= nMark - G.ne();
          return 0; // no more vertex or face to shell.
          }
      i = Candidates.pop();
      }while (i > 0 &&  ((MarkedF[i] || Brin2Face[-FirstBrin] == i) || (i < 0 && MarkedV[-i])) );
  // skip marked faces, the face incident to v_1 and v_2, and marked vertices

  // list of the faces which were not separating faces before the shelling.  
  IntList NonSepFaces; 
  // list of the faces which become separating faces by the shelling.
  IntList NewSepFaces; 
  // list of the vertices which become incident to F_out by the shelling.
  IntList NewOuterVertices;
  
//   if(debug())
//         {for(tedge e = 1;e <= G.ne();e++)
//             {if(ecolor[e] == Violet)continue;
//             if(IsOuterE[e])ecolor[e]=Green; // exterior face
//             if(MarkedE[e]){ecolor[e]=Red;ewidth[e] = 3;}
//             }
//         for(tvertex w=1;w < G.nv();w++)
//             {if(MarkedV[w])vcolor[w] = Red;
//             else vcolor[w] = Yellow;
//             }
//         ecolor[FirstBrin.GetEdge()()] = Yellow;
//         static int firstTime = 1;
//         if(firstTime)
//             {firstTime = 0;
//              cout <<"v1: " << v_1<< "  v2: " <<v_2 << "  vn: "<< v_n << endl;
//             DrawGraph();// red: treated green: exterior face
//             //Twait("Init");
//             }
//         }

  if (i > 0) // shell a path  ======================================= 
      {int f=i; 
      MarkedF[f]=1;
      // Let (v_1,...,v_p) the chain to shell of length count
      // vin[left] = v_1  vin[-right] = v_p
      right = Face2Brin[f];
      while (!IsOuterE[right.GetEdge()()]) right = G.cir[-right];
      while ( IsOuterE[right.GetEdge()()]) right = G.cir[-right];
      right = -G.acir[right];
      left = G.cir[-right];
      while(!IsOuterE[left.GetEdge()()])
          left = G.cir[-left];
      count = 1; // length of the chain
      b = left;
      do
          count++;
      while((b = G.cir[-b]) != right);

//       if(debug())
//           {ecolor[right.GetEdge()()] = Orange;ewidth[right.GetEdge()()] = 3; 
//           ecolor[left.GetEdge()()] = Blue; ewidth[right.GetEdge()()] = 3; 
//           cout << "***Packing  chain length: "<< count << "*************************"<<endl;
//           cout << " v_left = " << G.vin[left] << " v_left = " << G.vin[-left] << endl;
//           cout << " v_right = " << G.vin[right] << " v_right = " << G.vin[-right]<< endl;
//           }
      
      // update IsOuterV, IsOuterE.
      // update new face 
      b=-G.acir[left];
      LastB=G.cir[-right];
      while (1) 
          {IsOuterE[b.GetEdge()]=1;
          if (b==LastB) break;
          IsOuterV[G.vin[b]()] = 1;
          b=-G.acir[b];
          }
      //if(debug()){DrawGraph();Twait("Packing chain: blue orange");}   

      // update MarkedV[v], MarkedE[e].
      b = right; //LastB =-G.acir[left];
      while (b != left) 
          {MarkedV[G.vin[b]()] = 1;
//           if(debug())
//               {vcolor[G.vin[b]()] = Red;
//               ecolor[b.GetEdge()()] = Violet;ewidth[b.GetEdge()()] = 3;
//               MarkedE[b.GetEdge()()] = 1;
//               }
          b = -G.acir[b];
          }
      MarkedE[left.GetEdge()()] = 1;

//       if(debug())
//           {ecolor[left.GetEdge()()] = Violet;ewidth[b.GetEdge()()] = 3;
//           DrawGraph();Twait("Packing chain: done");}   
      // update visited[v].
      _visited(G.vin[left](),1);    _visited(G.vin[-right](),1);

      // set NewOuterVertices, NonSepFaces.
      // check faces which have a vertex on the new face
      b=-G.acir[left];      LastB = G.cir[-right];
      while (b != LastB)
          {NewOuterVertices.push(G.vin[b]());
          //StopB = G.acir[b]; //hub
          StopB = b;
          b = G.cir[b]; 
          do
              {f = Brin2Face[b];
//               if(debug())cout << "f:  " << f << endl;
              if(outv[f] < 3 && (outv[f] != 2 || oute[f] != 0)) 
                  {if(!NonSepFaces.InList(f))
                      {NonSepFaces.push(f);
//                        if(debug())cout << "pushing NonSepFace: "<< f << endl;
                      }
                  }
              b = G.cir[b];
              } while (b != StopB);
          b = -G.acir[b];
          //b = -StopB;//hub
          }

      // for all v newly incident to F_out, 
      // for all f incident to v, outv[f]++.
      // update oute[f].
      b=-G.acir[left];
      LastB=G.cir[-right];
      while (b!=LastB)
          {_oute(Brin2Face[-b],1);
          b0=b;
          do
              {f=Brin2Face[b]; 
              if (!MarkedF[f]) _outv(f,1);
              b=G.cir[b];
              } while (b != b0);
          b=-G.acir[b];  // to the next brin...
          }  
      _oute(Brin2Face[-b],1);

      // set NewSepFaces.
      NonSepFaces.ToHead();
      while ((f=~NonSepFaces) != 0)
          {if (outv[f] >= 3 || (outv[f] == 2 && oute[f] == 0))
              {NewSepFaces.push(f);
//                if(debug())cout << "pushing NewSepFaces f: "<< f  <<  endl;
              }
          ++NonSepFaces;
          }         
      // for all f \in NewSepFaces, for all v incident to f, sepf[v]++;
      UpdateSepf1(NewSepFaces);

      // update sepf[v] for v=v_1,v_p.
      _sepf(G.vin[left](),-1);      _sepf(G.vin[-right](),-1);
      
      if(-G.acir[left] == G.cir[-right]()) // if {v_1,v_p} \in E...
          {int f=Brin2Face[G.acir[left]()];  
          // f is the face (!= F_out) incident to the edge {v_1,v_p}.
          if (outv[f] == 2 && oute[f] == 1)
              {_sepf(G.vin[left](),-1);_sepf(G.vin[-right](),-1);}
          }

      // for all v \in NewOuterVertices, update sepf[v].
      UpdateSepf2(NewOuterVertices);
      return count;
      }
  else   // shell a vertex ========================================
      {v = -i;
      MarkedV[v]=1;
//        if(debug())
//            {vcolor[v] = Red;
//            cout << "packing vertex:" <<-i << "*************************"<< endl;
//            }
      // set leftmost & rightmost brins incident to v.
      // G.vin[left]() = G.vin[right]() = v()
      if (v == v_n) 
          {left=-G.acir[FirstBrin];  right=G.acir[left];}
      else
          {left = G.FirstBrin(v);
          while (!MarkedV[G.vin[-left]()]) left = G.cir[left];
          while ( MarkedV[G.vin[-left]()]) left = G.cir[left];
          right  = G.acir[left];
          while (MarkedV[G.vin[-right]()]) right = G.acir[right];
          }
      
//       if(debug())
//           {ecolor[left.GetEdge()()] = Blue;
//           ecolor[right.GetEdge()()] = Orange; 
//           ewidth[left.GetEdge()()] = 3; 
//           ewidth[right.GetEdge()()] = 3; 
//           cout << "***Packing vertex = " << -i << endl;
//           cout  << " v_left = " << G.vin[-left] << "  v_right = " << G.vin[-right]<< endl;
//            DrawGraph();Twait("packing vertex");
//           }
      
      // MarkedE[e] -> 1 for all incident edges of v
      b=left;
      while(!MarkedE[b.GetEdge()])
          {MarkedE[b.GetEdge()]=1;
//            if(debug())
//                {if(b!=left && b !=right)ecolor[b.GetEdge()] = Pink;
//                else ecolor[b.GetEdge()] = Red;
//                }
          b=G.cir[b];
          }
      
      // update MarkedF[f], IsOuterV[v], IsOuterE[e].
      // visited : vertices adjacent to v => _visited has at least a  MarkedE
      // Mark edges of the new face under the v umbrella
      _visited(G.vin[-left](),1);
      _visited(G.vin[-right](),1);
      b=-G.acir[-left];
      int f;
      while (1)
          {f = Brin2Face[b()];
          MarkedF[f]=1;
//           if(debug())cout << "marking face:" << f << endl;
          IsOuterV[G.vin[b]()] = 1;
          IsOuterE[b.GetEdge()()] = 1;
          //tsu _visited(G.vin[b](),1);
          b = G.acir[b];
          if(-b == right())break;
          if(G.vin[-b] == v)//hub
              {_visited(G.vin[b](),1); 
              b = G.acir[b];
              }
          b = -b;
          }

      // set NewOuterVertices, NonSepFaces.
      b = -G.acir[-left];
      LastB = G.cir[-right];
      while (b != LastB)  //id precedent
              {NewOuterVertices.push(G.vin[b]());
//               if(debug())vcolor[G.vin[b]] = Green;
              StopB = b;
              do
                  {StopB = G.acir[StopB];
                  } while (G.vin[-StopB] == v); // once or twice
              //StopB = G.cir[StopB]; //hub
//                if(debug())cout << "--------b:"<< G.vin[b]() << "  "<< G.vin[StopB]()<< endl;
              while(b != StopB)
                  {b = G.cir[b];
                  f = Brin2Face[b];
                  if(outv[f] < 3 && (outv[f] != 2 || oute[f] != 0)) 
                      if(!NonSepFaces.InList(f)) 
                          {NonSepFaces.push(f);
//                           if(debug())cout << "pushing NonSepFace: "<< f << endl;
                          }
                  }
              b=-StopB;
              }
//   if(debug()){ DrawGraph();Twait("vertex:packed");}

      // for all v newly incident to F_out, 
      //   for all f incident to v, outv[f]++.
      // update oute[f].
      b=-G.acir[-left];
      LastB=G.cir[-right];
      while (b!=LastB)
          { _oute(Brin2Face[-b],1);
          b0=b;
          do
              {f=Brin2Face[b]; 
              if (!MarkedF[f]) _outv(f,1);
              }while((b = G.cir[b]) != b0);
          b = G.acir[b];
          if(G.vin[-b] == v)b = G.acir[b];
          b = -b;
          }       
      _oute(Brin2Face[-b],1);

      // set NewSepFaces.
      NonSepFaces.ToHead();
      while ((f = ~NonSepFaces) != 0)
          {if(outv[f] >=3 || (outv[f] == 2 && oute[f] == 0)) 
              {NewSepFaces.push(f);
//                 if(debug())cout << "pushing NewSepFaces f: "<< f  <<  endl;
              }
          ++NonSepFaces;
          }        
      // for all f \in NewSepFaces, for all v incident to f, sepf[v]++;
      UpdateSepf1(NewSepFaces);
      // for all v \in NewOuterVertices, update sepf[v].
      UpdateSepf2(NewOuterVertices);
      return 1;
      }
  }





LMCShelling::LMCShelling(Graph &G0, tbrin bref) : KantShelling(G0, bref), LeftBrin(0,G0.nv(),0), RightBrin(0,G0.nv(),0), NbBrin(0,G0.nv(),0), NewLeftBrin(0,G0.nv(),0), NewRightBrin(0,G0.nv(),0), NewNbBrin(0,G0.nv(),0),current(0) {
  tbrin left, right, b;
  tvertex LeftVertex, RightVertex;
  int i = 1;
  int new_current;
  int nb_sets;
  do {
    NbBrin[i] = KantShelling::FindNext(left,right);
    LeftBrin[i] = left;
    RightBrin[i] = right;
    NewNbBrin[i] = NbBrin[i];
    NewLeftBrin[i] = LeftBrin[i];
    NewRightBrin[i] = RightBrin[i];

    i++;
  } while(NbBrin[i-1] != 0);
  nb_sets = i-2;

  stack<tvertex>  OuterFace; // List of vertices of on the outerface

  // for each vertex v, the list of set-index such that it is its rightvertex
  //IntList RightVertexSets[G0.nv()+1];
  IntList * RightVertexSets = new  IntList[G0.nv()+1];

  for(i = 1; i <= nb_sets; i++) {
    RightVertex = G.vin[-RightBrin[i]];
    RightVertexSets[RightVertex()].push(i);
  }

  OuterFace.push(G.vin[-RightBrin[nb_sets]]);

  new_current = nb_sets;

  while (!OuterFace.empty()) {
    RightVertex = OuterFace.pop();
    OuterFace.push(RightVertex); // Keep RightVertex on the top of the stack

    if (!RightVertexSets[RightVertex()].empty()) {
      int set_index = RightVertexSets[RightVertex()].pop();
      NewRightBrin[new_current] = RightBrin[set_index];
      NewLeftBrin[new_current] = LeftBrin[set_index];
      NewNbBrin[new_current] = NbBrin[set_index];

      b=G.acir[RightBrin[set_index]]; 
      if (NbBrin[set_index] != 1)
	while (b !=-LeftBrin[set_index]) {
	  OuterFace.push(G.vin[b]);
	  b=G.acir[-b];
	}
      OuterFace.push(G.vin[b]);
      new_current --;
    }
    else 
      OuterFace.pop();
  }
  assert (new_current == 0);
  current=1;
  delete [] RightVertexSets;
}

int LMCShelling::FindNext(tbrin &left, tbrin &right) {
  left = NewLeftBrin[current];
  right = NewRightBrin[current];
  current ++;
  return NewNbBrin[current-1];
}

