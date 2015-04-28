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

/*!
\file
\brief Handlers used to call the algorithms
*/

#include <QT/Action_def.h>
#include "pigaleWindow.h" 
#include <QT/Misc.h>
#include <QT/Handler.h>

void BFSOrientTree(TopologicalGraph &G, tvertex v0);

int OrientHandler(GraphContainer &GC,int action)
  {GeometricGraph G(GC);
  Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED);
  int ret = 2;
  int i = 0;
  tedge e;
  tbrin first;
  switch(action)
      {case  A_ORIENT_E:
          {Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED);
          eoriented.definit(true);
          for(e=1;e <= G.ne();e++)eoriented[e] = true;
          }
          ret = 1;
          break;
      case  A_ORIENT_NOE:
          {Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED);
	    eoriented.definit(false); eoriented.clear();
          }
          ret = 1;
          break;
      case A_ORIENT_SHOW:
          ColorPoles(G);
          ret = 1;
          break;
      case A_REORIENT_COLOR:
          {short ecol=0; G.ecolor.getinit(ecol);
          for(tedge e = 1; e <= G.ne() ;e++)
              if(G.ecolor[e] == ecol)
                  {G.ReverseEdge(e);eoriented[e] = true;}
          }
          G.FixOrientation();
          break;
      case A_ORIENT_INF:
          i = G.InfOrient();
          G.FixOrientation();
          Tprintf("G is %d inf-oriented",i-1);
          break;
      case A_ORIENT_TRICON:
          i = G.Tricon3orient();
          G.FixOrientation();
          if(i != 0){Tprintf("err=%d",i);return -1;}
          break;
      case A_ORIENT_BIPAR:
          i = DecompMaxBip(G);
          if(i != 0){Tprintf("err=%d",i);return -1;}
          G.FixOrientation();
          break;
      case A_ORIENT_SCHNYDER:
          i = G.SchnyderOrient(0);
          G.FixOrientation();
          if(i != 0)Tprintf("i=%d",i);
          break;
      case A_ORIENT_BIPOLAR:
          first = G.extbrin();
          i = G.BipolarPlan(first);
          if(i != 0){Tprintf("err=%d",ret);return -1;}
          G.FixOrientation();
          if(first.GetEdge() <= G.ne())
              {G.ecolor(first.GetEdge()) = Red;
              G.ewidth(first.GetEdge()) = 2;
              }
          Tprintf("st=%d  (%d %d)",first(),G.vin[first](),G.vin[-first]());
          break;
      case A_ORIENT_BIPOLAR_NP:
          first = G.extbrin();
          i = NPBipolar(G,first);
          if(i != 0){Tprintf("err=%d",i);return -1;}
          G.FixOrientation();
          if(first.GetEdge() <= G.ne())
              {G.ecolor(first.GetEdge()) = Red;
              G.ewidth(first.GetEdge()) = 2;
              }
          Tprintf("st=%d  (%d %d)",first(),G.vin[first](),G.vin[-first]());
          break;
      case A_ORIENT_BFS:
          BFSOrientTree(G,tvertex(1));
          G.RestoreOrientation();
          break;
      default:
          return 0;
      }
  return ret;
  }
int AlgoHandler(GraphContainer &GC,int action,int nn)
  {GeometricGraph G(GC);
  int ret = 0;
  int i;
  tedge e; tvertex v;
  switch(action)
      {case A_ALGO_KURATOWSKI:
          ret = G.Kuratowski();
          Tprintf("Kuratowski");
          break;
      case A_ALGO_COTREE_CRITICAL:
          G.CotreeCritical(i);
          Tprintf("Cotre Critical case:%d",i);
	  //if(ret){Tprintf("Error Cotre Critical");return -1;}
          // Compute cycle dimension of the Cotree Critical
          {int nblack ,mblack;
          tbrin b,b0;
          tvertex v;
          bool noblack;
          Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);
          nblack = mblack = 0;
          ForAllVertices(v,G)
            {b = b0 = G.FirstBrin(v);
            if(!b)continue; // Isolated vertex
            noblack = true;
            do
                {if(ecolor[b.GetEdge()] == Black)
                    {++mblack;noblack=false;}
                }while((b = G.cir[b]) != b0);
            if(noblack == false)++nblack;
            }
          mblack /= 2;
          Tprintf("Critical cycle dimension=%d",mblack-nblack+1);
          }
          return 1;
          break;
      case A_ALGO_NPSET:
          //G.Simplify();
          i = FindNPSet(G);
          if(i == 0)return 0;
          Tprintf("Max.planar (fast)");
          Tprintf("Need to erase %d edges",i);
          return 1;
          break;
      case A_ALGO_MAX_PLANAR:
          //G.Simplify();
          i = G.MaxPlanar();
          if(i == 0)return 0;
          if(i < 0){Tprintf("Max.planar (slow) Error:%d",i);return i;}
          Tprintf("Max.planar (slow)");
          Tprintf("Need to erase %d edges",i);
          return 1;
          break;
      case A_ALGO_RESET_COLORS:
          {short ecol=0; G.ecolor.getinit(ecol);
          short vcol=0;  G.vcolor.getinit(vcol);
          int width=0; G.ewidth.getinit(width);
          ForAllEdges(e,G) {G.ecolor[e] = ecol; G.ewidth[e] = width;}
          ForAllVertices(v,G) G.vcolor[v] = vcol;
          }
          break;
      case A_ALGO_NETCUT: // Netcut
          G.Simplify();
          ret = split(G,nn,staticData::UseDistance());
          break;
      case A_ALGO_GEOMETRIC_CIR:
          if(G.ComputeGeometricCir() == 0)
              G.extbrin() = G.FindExteriorFace();
          Tprintf("Geometric map");
          break;
      case A_ALGO_LRALGO_CIR :
          G.Planarity( G.extbrin());
          Tprintf("LR-algo map");
          break;
      case A_ALGO_SYM://symetrie
          ret = Embed3d(G,staticData::UseDistance());
          if(ret >=0)return 5;
          return 5;
          break;
      case A_ALGO_COLOR_BIPARTI://color bipartite
          G.CheckBipartite(true);
          break;
      case A_ALGO_COLOR_EXT://color exterior face
          i = G.ColorExteriorface();
          ret = (i > 0) ? 0:1;
          break;
      case A_ALGO_COLOR_CONNECTED://color connected components
          G.ColorConnectedComponents();
          ret = 0;
          break;
      case A_ALGO_COLOR_NON_CRITIC://show non critical edges
          {tedge e;
          ForAllEdges(e,G)
            {G.ecolor[e] = Black; G.ewidth[e] = 1;}
          tvertex v0,v1;
          int planar,m;
          m = G.ne();
          for(e = m; e >= 1;e--)
              {v0 = G.vin[e]; v1 = G.vin[-e];
              G.DeleteEdge(e);
              planar = G.TestPlanar();
              G.NewEdge(v0,v1);
              if(!planar)
                  {G.ecolor[m] = Red; G.ewidth[m] = 2;}
              }
          }
          return 1;
          break;
      default: 
          return 0;
      }
  if(ret)return -1;
  return 1;
  }
int EmbedHandler(GraphContainer &GC,int action,int &drawing)
  {TopologicalGraph G(GC);
  GeometricGraph G0(G);
  int ret;
  int err = 0;
  drawing = 0;
  switch(action)
      {case A_EMBED:
          return 2;  //only used by the server (Information+gw->update)
      case A_EMBED_SCHNYDER_E:ret = 1;
          err = G.Schnyder(staticData::SchnyderRect(),staticData::SchnyderColor()
                           ,staticData::SchnyderLongestFace(),0);
          break;
      case A_EMBED_SCHNYDER_V:ret = 1;
          err = G.SchnyderV(staticData::SchnyderRect(),staticData::SchnyderColor()
                            ,staticData::SchnyderLongestFace(),0);
          break;
      case A_EMBED_FPP:ret = 1;
          err = EmbedFPP(G,staticData::SchnyderRect(),staticData::SchnyderLongestFace());
          break;
      case A_EMBED_CD:ret = 1;
          err = EmbedCCD(G,false);
          break;
      case A_EMBED_CCD:ret = 1;
          err = EmbedCCD(G,true);
          break;
      case A_EMBED_TUTTE_CIRCLE:ret = 1;
          EmbedTutteCircle(G,staticData::SchnyderLongestFace());
          break;
      case A_EMBED_TUTTE:ret = 1;
          G0.Tutte();
          break;
      case A_EMBED_POLREC_DFS:ret = 3;
          err = EmbedPolrecDFS(G);
          drawing = 8;
          break;
      case A_EMBED_POLREC_BFS:ret = 3;
          err = EmbedPolrecBFS(G);
          drawing = 8;
          break;
      case A_EMBED_POLREC_DFSLR:ret = 3;
          err = EmbedPolrecLR(G);
          drawing = 8;
          break;
      case A_EMBED_VISION:ret = 3;
          err = EmbedVision(G);
          drawing = 0;
          break;
      case A_EMBED_POLYLINE:ret = 3;
          err = EmbedPolyline(G);
          drawing = 6;
          break;
      case A_EMBED_CURVES:ret = 3;
          err = EmbedCurves(G);
          drawing = 7;
          break;
      case A_EMBED_CONTACT_BIP:ret = 3;
          err = EmbedContactBip(G0);
          drawing = 3;
          break;
      case A_EMBED_BIP_2PAGES:ret = 3;
          err = EmbedContactBip(G0);
          drawing = 11;
          break;
      case A_EMBED_POLAR:ret = 3;
          err = Polar(G);
          drawing = 4;
          break;
      case A_EMBED_FPP_RECTI:ret = 3;
          err = EmbedFPPRecti(G,staticData::SchnyderLongestFace());
          drawing = 1;
          break;
      case A_EMBED_GVISION:ret = 3;
          err = EmbedGVision(G);
          drawing = 2;
          break;
      case A_EMBED_T_CONTACT:ret = 3;
          err = EmbedTContact(G,staticData::SchnyderLongestFace());
          drawing = 5;
          break;
      case A_EMBED_TRIANGLE:ret = 3;
          err = EmbedTriangle(G);
          drawing = 9;
          break;
      case A_EMBED_3d:ret = 4; //Embed3d
          err = Embed3d(G,staticData::UseDistance());
          break;
      case A_EMBED_3dSCHNYDER:ret = 4; //Embed3d
          err = Embed3dSchnyder(G,staticData::SchnyderLongestFace());
          break;
      case  A_EMBED_SPRING :ret = 6;
          break;
      case  A_EMBED_SPRING_PM :ret = 7;
          break;
      case  A_EMBED_JACQUARD:ret = 8;
          break;
      default:
          Tprintf("Handler.cpp unknown action:%d",action);
          return 0;
      }
  if(err > 0)err = -err;
  if(err){Tprintf("embed error:%d",err);return err;}
  return ret;
  }
int AugmentHandler(GraphContainer &GC,int action)
  {GeometricGraph  G(GC);
  Prop1<tstring> title(G.Set(),PROP_TITRE);
  int ret = 0;
  int i;
  switch(action)
      {case A_AUGMENT_CONNECT: 
          i = G.MakeConnected();
          if(debug())DebugPrintf("Added %d Edges",i);
          title() = "K-" + title();
          break;
      case A_AUGMENT_CONNECT_V: 
          i = G.MakeConnectedVertex();
          if(debug())DebugPrintf("Added %d Edges",i);
          title() = "KV-" + title();
          break;
      case A_AUGMENT_BICONNECT:
          G.Set().erase(PROP_BICONNECTED);
          ret = G.Biconnect();
          if(ret != 0){Tprintf("ret=%d",ret);break;}
          title() = "2K-" + title();
          break;
      case A_AUGMENT_BICONNECT_6:
          G.Set().erase(PROP_BICONNECTED);
          ret = G.Opt6Biconnect();
          if(ret != 0){Tprintf("ret=%d",ret);break;}
          title() = "2Kopt-" + title();
          break;
      case A_AUGMENT_BICONNECT_NP:
          G.Set().erase(PROP_BICONNECTED);
          ret = G.NpBiconnect();
          if(ret != 0){Tprintf("ret=%d",ret);break;}
          title() = "2KNP-" + title();
          break;
      case A_AUGMENT_BICONNECT_NP_V:
          G.Set().erase(PROP_BICONNECTED);
          ret = G.NpBiconnectVertex();
          if(ret != 0){Tprintf("ret=%d",ret);break;}
          title() = "2KNPV-" + title();
          break;
      case A_AUGMENT_TRIANGULATE_V:
          ret = G.VertexTriangulate();
          if(ret != 0){Tprintf("ret=%d",ret);break;}
          title() = "VT-" + title();
          break;
      case A_AUGMENT_TRIANGULATE_ZZ:
          ret = G.ZigZagTriangulate();
          if(ret < 0){Tprintf("ret=%d",ret);break;}
          title() = "ZZ-" + title();
          break;
      case A_AUGMENT_TRIANGULATE_3C:
          ret = G.TriconTriangulate();
          if(ret != 0){Tprintf("ret=%d",ret);break;}
          title() = "3CT-" + title();
          break;
      case A_AUGMENT_QUADRANGULATE_V:
          ret =   G.VertexQuadrangulate();
          if(ret != 0){Tprintf("ret=%d",ret);break;}
          title() = "VQ-" + title();
          break;
      case A_AUGMENT_BISSECT_ALL_E:
          {int m = G.ne();
          for(tedge e = 1;e <= m;e++)G.BissectEdge(e);
          }
          break;
      default:
          return 0;
      }
  if(ret)return -1;
  return 2;
  }
int DualHandler(GraphContainer &GC,int action)
  {GeometricGraph G(GC);
  GraphContainer *pGC;
  switch(action)
      {case A_GRAPH_DUAL:
          pGC = G.DualGraph();
          if (pGC) G.Tswap(*pGC);
          delete pGC;
          Tprintf("Dual");
          break;
      case  A_GRAPH_DUAL_G:
          pGC = G.GeometricDualGraph();
          if (pGC) G.Tswap(*pGC);
          delete pGC;
          Tprintf("Dual geometric");
          break;
      case A_GRAPH_ANGLE:
          pGC = G.AngleGraph();
          if (pGC) G.Tswap(*pGC);
          delete pGC;
          Tprintf("Angle");
          break;
      case A_GRAPH_ANGLE_G:
          pGC = G.GeometricAngleGraph();
          if (pGC) G.Tswap(*pGC);
          delete pGC;
          Tprintf("Angle geometric");
          break;
      default:
          break;
      }
  {TopologicalGraph GG(GC); GG.RemoveLoops();}
  return 2;
  }
int RemoveHandler(GraphContainer &GC,int action)
  {TopologicalGraph G(GC);
  GeometricGraph GG(GC);
  int n;
  tvertex v;
  tedge e;
  switch(action)
      {case A_REMOVE_ISOLATED_V:
          n = G.RemoveIsolatedVertices();
          break;
      case A_REMOVE_LOOPS:
          G.RemoveLoops();
          break;
      case A_REMOVE_MULTIPLE_E:
          G.Simplify();
          break;
      case  A_REMOVE_BRIDGES:
          G.RemoveIsthmus();
          break;
      case A_REMOVE_COLOR_V://Erase Color Vertices
	  {short vcol=0; GG.vcolor.getinit(vcol);
          for(v= GG.nv() ;v > 0;v--)
              if(GG.vcolor[v] == vcol)GG.DeleteVertex(v);
	  }
          break;
      case A_REMOVE_COLOR_E://Erase Color Edges
	  {short ecol=0; GG.ecolor.getinit(ecol);
          for(e = GG.ne() ;e > 0;e--)
              if(GG.ecolor[e] == ecol)GG.DeleteEdge(e);
	  }
          break;
      case A_REMOVE_THICK_E://Erase Thick Edges
	  {int ewidth=0; GG.ewidth.getinit(ewidth);
          for(e = GG.ne() ;e > 0;e--)
              if(GG.ewidth[e] == ewidth)GG.DeleteEdge(e);
	  }
          break;
      default:
          return 0;
      }
  return 20;
  }
int GenerateHandler(GraphContainer &GCC,int action,int n1_gen,int n2_gen,int m_gen)
  {GraphContainer *GC=(GraphContainer *)0;
  switch(action)
      {case  A_GENERATE_GRID:
          GC = GenerateGrid(n1_gen,n2_gen);
          break;
      case A_GENERATE_COMPLETE:
          GC = GenerateCompleteGraph(n1_gen);
          break;
      case A_GENERATE_COMPLETE_BIP:
          GC = GenerateCompleteBiGraph(n1_gen,n2_gen);
          break;
      case  A_GENERATE_RANDOM:
          //GC = GenerateRandomGraph(n1_gen,m_gen,staticData::RandomEraseMultipleEdges());
          GC = GenerateRandomGraph(n1_gen,m_gen);
          break;
          // Schaeffer generator
      case A_GENERATE_P:
          GC = GenerateSchaeffer(m_gen,1,1,staticData::RandomEraseMultipleEdges()
                                 ,staticData::RandomUseGeneratedCir());
          break;
      case A_GENERATE_P_2C:
          GC = GenerateSchaeffer(m_gen,1,2,staticData::RandomEraseMultipleEdges()
                                 ,staticData::RandomUseGeneratedCir());
          break;
      case A_GENERATE_P_3C:
          GC = GenerateSchaeffer(m_gen,1,3,staticData::RandomEraseMultipleEdges()
                                 ,staticData::RandomUseGeneratedCir());
          break;
      case A_GENERATE_P_3R_2C:
          GC = GenerateSchaeffer(m_gen,3,2,staticData::RandomEraseMultipleEdges()
                                 ,staticData::RandomUseGeneratedCir());
          break;
      case A_GENERATE_P_3R_3C:
          GC = GenerateSchaeffer(m_gen,3,3,staticData::RandomEraseMultipleEdges()
                                 ,staticData::RandomUseGeneratedCir());
          break;
      case A_GENERATE_P_3R_D4C:
          GC = GenerateSchaeffer(m_gen,3,4,staticData::RandomEraseMultipleEdges()
                                 ,staticData::RandomUseGeneratedCir());
          break;
      case A_GENERATE_P_4R_C:
          GC = GenerateSchaeffer(m_gen,4,2,staticData::RandomEraseMultipleEdges()
                                 ,staticData::RandomUseGeneratedCir());
          break;
      case A_GENERATE_P_4R_2C:
          GC = GenerateSchaeffer(m_gen,4,4,staticData::RandomEraseMultipleEdges()
                                 ,staticData::RandomUseGeneratedCir());
          break;
      case A_GENERATE_P_4R_3C:
          GC = GenerateSchaeffer(m_gen,4,6,staticData::RandomEraseMultipleEdges()
                                 ,staticData::RandomUseGeneratedCir());
          break;
      case A_GENERATE_P_4R_BIP:
          GC = GenerateSchaeffer(m_gen,4,0,staticData::RandomEraseMultipleEdges()
                                 ,staticData::RandomUseGeneratedCir());
          break;
      case A_GENERATE_P_BIP:
          GC = GenerateSchaeffer(m_gen,2,1,staticData::RandomEraseMultipleEdges()
                                 ,staticData::RandomUseGeneratedCir());
          break;
      case A_GENERATE_P_BIP_2C:
          GC = GenerateSchaeffer(m_gen,2,2,staticData::RandomEraseMultipleEdges()
                                 ,staticData::RandomUseGeneratedCir());
          break;
      case A_GENERATE_P_BIP_3C:
          GC = GenerateSchaeffer(m_gen,2,3,staticData::RandomEraseMultipleEdges()
                                 ,staticData::RandomUseGeneratedCir());
          break;
      case  A_GENERATE_P_OUTER_N:
          GC = GenerateRandomOuterplanarGraph(n1_gen,staticData::RandomEraseMultipleEdges());
          break;
      case  A_GENERATE_P_OUTER_NM:
          GC = GenerateRandomOuterplanarGraph(n1_gen, m_gen,staticData::RandomEraseMultipleEdges());
          break;
      default:
          return 0;
      }
  if(GC)
      {GCC.Tswap(*GC);
      GeometricGraph GG(GCC);
      }
  else {setPigaleError(-1,"Generator Error");DebugPrintf("Error generating:%d",action);}
  delete GC;

  return 2;
  }
