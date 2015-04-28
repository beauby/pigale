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
\file gprop.cpp
\brief  Display properties of a grap, enable/disable menus
*/

#include "pigaleWindow.h"
#include <QT/Misc.h>
#include "gprop.h"

#include <QBoxLayout>
#include <QLabel>

Graph_Properties::Graph_Properties(QWidget* parent,QMenuBar *menubar,pigaleWindow * _mw)
    : QWidget(parent)
    ,menu(menubar),allow( A_AUGMENT, A_ORIENT_END,1),_updateMenu(true)
  {mw= _mw;
  //mw = (pigaleWindow*)qApp->mainWidget();
  //MaxNSlow = staticData::MaxNS;  MaxNDisplay = staticData::MaxND;
  setMinimumHeight(180); 
  QVBoxLayout* MainLayout = new QVBoxLayout(this); 
  QHBoxLayout *TxtLayout = new QHBoxLayout(); 
  QHBoxLayout *Layout_NM = new QHBoxLayout();
  QHBoxLayout *LayoutDegrees = new QHBoxLayout();
  QHBoxLayout *LayoutAllButtons = new QHBoxLayout();
  //QHBoxLayout *LayoutAllButtons = new QHBoxLayout(0,8,10,"LayoutAllButtons");
  
  MainLayout->addLayout(TxtLayout);
  MainLayout->addLayout(Layout_NM);
  MainLayout->addLayout(LayoutDegrees);
  MainLayout->addLayout(LayoutAllButtons);
  
  QVBoxLayout *LayoutLeftButtons  = new QVBoxLayout(); 
  QVBoxLayout *LayoutRightButtons = new QVBoxLayout();
  LayoutAllButtons->addLayout(LayoutLeftButtons);
  LayoutAllButtons->addLayout(LayoutRightButtons);

  QLabel*TextLabel3 = new QLabel(this);
  TextLabel3->setText(tr("<b>Graph Properties</b>"));
  TextLabel3->setTextFormat(Qt::RichText);
  TextLabel3->setAlignment(Qt::AlignCenter);
  TxtLayout->addWidget(TextLabel3);
  
  QLabel*TextLabel1 = new QLabel(this);
  TextLabel1->setText("N:  ");
  Layout_NM->addWidget(TextLabel1);
  LE_N = new QLineEdit(this);
  LE_N->setMaximumSize(QSize(60,32767));
  LE_N->setText("0");
  Layout_NM->addWidget(LE_N);
  QLabel *TextLabel1_2 = new QLabel(this);
  TextLabel1_2->setText("M:  ");
  Layout_NM->addWidget(TextLabel1_2);
  LE_M = new QLineEdit(this);
  LE_M->setMaximumSize(QSize(60,32767));
  LE_M->setText("0");
  Layout_NM->addWidget(LE_M);
  LE_M->setReadOnly(true);  LE_N->setReadOnly(true);
  
  QLabel* TextLabelMin = new QLabel(this);
  TextLabelMin->setText("Min:");
  LayoutDegrees->addWidget(TextLabelMin);
  LE_Min = new QLineEdit(this);
  LE_Min->setMaximumSize(QSize(60,32767));
  LE_Min->setText("0");
  LayoutDegrees->addWidget(LE_Min);
  QLabel* TextLabelMax = new QLabel(this);
  TextLabelMax->setText("Max:");
  LayoutDegrees->addWidget(TextLabelMax);
  LE_Max = new QLineEdit(this);
  LE_Max->setMaximumSize(QSize(60,32767));
  LE_Max->setText("0");
  LayoutDegrees->addWidget(LE_Max);
  LE_Min->setReadOnly(true);  LE_Max->setReadOnly(true);

  //***************************************************
  RBConnected = new RoRadioButton(this);
  RBConnected->setText(tr("Connected"));
  RBConnected->setChecked(true);
  LayoutLeftButtons->addWidget(RBConnected);

  RB2Connected = new RoRadioButton(this);
  RB2Connected->setText(tr("2-Connected"));
  LayoutLeftButtons->addWidget(RB2Connected);

  RB3Connected = new RoRadioButton(this);
  RB3Connected->setText(tr("3-Connected"));
  LayoutLeftButtons->addWidget(RB3Connected);

  RBBipartite = new RoRadioButton(this);
  RBBipartite->setText(tr("Bipartite"));
  LayoutLeftButtons->addWidget(RBBipartite);

  RBAcyclic = new RoRadioButton(this);
  RBAcyclic->setText(tr("Acyclic"));
  LayoutLeftButtons->addWidget(RBAcyclic);

  //********************************************
  RBPlanar = new RoRadioButton(this);
  RBPlanar->setText(tr("Planar"));
  LayoutRightButtons->addWidget(RBPlanar);

  RBMxPlanar = new RoRadioButton(this);
  RBMxPlanar->setText(tr("Triangulation"));
  LayoutRightButtons->addWidget(RBMxPlanar);

  RBOuPlanar = new RoRadioButton(this);
  RBOuPlanar->setText(tr("Out. planar"));
  LayoutRightButtons->addWidget(RBOuPlanar);

  RBSerie = new RoRadioButton(this);
  RBSerie->setText(tr("Series-//"));
  LayoutRightButtons->addWidget(RBSerie);

  RBSimple = new RoRadioButton(this);
  RBSimple->setText(tr("Simple"));
  LayoutRightButtons->addWidget(RBSimple);
  }

RoRadioButton::RoRadioButton(QWidget * parent)
    :QRadioButton(parent)
  {setAutoExclusive(false); 
  }
void RoRadioButton::mousePressEvent(QMouseEvent* e)
  {e->accept();
  }
void RoRadioButton::mouseReleaseEvent(QMouseEvent* e)
  {e->accept();
  }
Graph_Properties::~Graph_Properties()
  { }

void Graph_Properties::update(GraphContainer & GC,bool print)
  {GeometricGraph G(GC);
  Prop1<tstring> title(G.Set(),PROP_TITRE);
#ifdef TDEBUG
  if(!G.DebugCir())
      {DebugPrintf("input Cir is wrong");setPigaleError(A_ERRORS_BAD_INPUT);return;}
  LogPrintf("START INFO: n = %d m = %d Name:%s\n",G.nv(),G.ne(),~title());
#endif
  if(print && debug())Tprintf("START INFO:");
// #ifdef TDEBUG
//   if(G.vin[0]() || G.cir[0]() || G.acir[0]())
//       {Tprintf("vin[0]=%d,cir[0]=%d,acir[0]=%d",G.vin[0](),G.cir[0](),G.acir[0]());
//       setPigaleError(-1,"vin[0] or cir[0] or acir[0] != 0");
//       }
//   if(debug())DebugPrintf("START INFO: n = %d m = %d Name:%s",G.nv(),G.ne(),~title());
// #endif
  
  int nloops = G.RemoveLoops();
  if(print && nloops)Tprintf("Graph had %d loops",nloops);
  S = G.CheckSimple();
  P = G.CheckPlanar();
  bool SMALL = (G.nv() < 3);
  bool M = (!SMALL  && (G.ne() == 3*G.nv() - 6));
  T = (P && S && M) ? true : false;     //Triangulation
  A = G.CheckAcyclic(ns,nt);
  B = G.CheckBipartite();
  G.MinMaxDegree(dmin,dmax);
  R = (dmin == dmax);
  C1 = C2 = C3 = false;
  Outer =  Serie = false;
 
  //bool H = G.Set().exist(PROP_HYPERGRAPH);
  bool E;
  MaxBi = P && B && S && (G.ne() == (2*G.nv() - 4)  && (G.nv() >= 4));
 
  if(S && G.nv() == 2 && G.ne() == 1) //One edge graph
      C1 = true;
  if(T && G.nv() == 3)
      C1 = C2 = true;
  else if(T && G.nv() > 3) // MaxPlanar
      C3 = true;
  else if(P && S && !M && dmin > 2  && G.CheckTriconnected())
      C3 = true;
  else if(!T)
      {if(G.CheckBiconnected())
	  C1 = C2 = true;
      else if(G.CheckConnected())
	  C1 = true;
      }
  if(C3)C2 = true;
  if(C2)C1 = true;

  if(P && C2 && !C3) Serie = G.CheckSerieParallel();
  if(P  && !C3 && dmin <= 2 && TestOuterPlanar(G)) Outer = true;
  if((G.nv() == 2 && G.ne() == 1) || (C1 && dmax == 2)) 
      Serie = Outer = true;

  //Modify the enable menus
  //For slow programs or display
  bool NotBigS = !(G.nv() > staticData::MaxNS);
  bool NotBigD = !(G.nv() > staticData::MaxND);
  allowAction(A_EMBED_3d,G.nv() > 3 && NotBigD);                   //Rn embedding
  allowAction(A_ALGO_SYM,!SMALL && NotBigS);                       //sym
  allowAction(A_ALGO_NETCUT,!SMALL && NotBigS);                    //partition
  allowAction(A_ALGO_NPSET,!P && NotBigS && S);                    //maxplanar
  allowAction(A_ALGO_MAX_PLANAR,!P && NotBigS && S);               //maxplanar
  //Augment
  if(print)
      {allowAction(A_AUGMENT_CONNECT,(G.nv() > 1) && !C1);               //make connected 
      allowAction(A_AUGMENT_CONNECT_V,(G.nv() > 1) && !C1);               //make connected 
      allowAction(A_AUGMENT_BICONNECT,!SMALL && P && !C2);              //make 2 connected
      allowAction(A_AUGMENT_BICONNECT_6,!SMALL && P && !C2);            //make 2 connected opt
      allowAction(A_AUGMENT_BICONNECT_NP,!SMALL && !C2);                //make 2 connected NP  
      allowAction(A_AUGMENT_BICONNECT_NP_V, !SMALL &&!C2);                  //make 2 connected NP  
      allowAction(A_AUGMENT_TRIANGULATE_V,!SMALL && P && S && !T);      //vertex triangulate
      allowAction(A_AUGMENT_TRIANGULATE_ZZ,!SMALL && P && S && !T);     //ZigZag 
      allowAction(A_AUGMENT_TRIANGULATE_3C,!SMALL && P && C3 && !T);    //Tricon triangulate opt
      allowAction(A_AUGMENT_QUADRANGULATE_V,(G.nv() > 1) && B && !MaxBi);    //Quadrangulate
      }
  else // macro
      {allowAction(A_AUGMENT_CONNECT,(G.nv() > 1));               //make connected 
      allowAction(A_AUGMENT_CONNECT_V,(G.nv() > 1));               //make connected 
      allowAction(A_AUGMENT_BICONNECT,!SMALL && P );              //make 2 connected
      allowAction(A_AUGMENT_BICONNECT_6,!SMALL && P);            //make 2 connected opt
      allowAction(A_AUGMENT_BICONNECT_NP,!SMALL );                //make 2 connected NP  
      allowAction(A_AUGMENT_BICONNECT_NP_V, !SMALL );                  //make 2 connected NP  
      allowAction(A_AUGMENT_TRIANGULATE_V,!SMALL && P && S );      //vertex triangulate
      allowAction(A_AUGMENT_TRIANGULATE_ZZ,!SMALL && P && S );     //ZigZag 
      allowAction(A_AUGMENT_TRIANGULATE_3C,!SMALL && P && C3 );    //Tricon triangulate opt
      allowAction(A_AUGMENT_QUADRANGULATE_V,(G.nv() > 1) && B );    //Quadrangulate
      }
  allowAction( A_AUGMENT_BISSECT_ALL_E ,G.ne());    //Bissect all edges
  //Embed
  allowAction(A_EMBED_SCHNYDER_E,!SMALL && S && P && NotBigD);       //Schnyder
  allowAction(A_EMBED_SCHNYDER_V ,!SMALL && S && P && NotBigD);      //Schnyder V 
  allowAction(A_EMBED_FPP,!SMALL && S && P && NotBigD);              //FPP Fary
  allowAction(A_EMBED_CCD,!SMALL && S && P && C3 && NotBigD);         //Convex Compact
  allowAction(A_EMBED_CD,!SMALL && S && P && C3 && NotBigD);         //Convex 
  allowAction(A_EMBED_POLYLINE,!SMALL && S && P && NotBigD);         //Polyline
  allowAction(A_EMBED_TUTTE_CIRCLE,!SMALL && P && S && NotBigD);     //Tutte Circle 
  allowAction(A_EMBED_TUTTE,!SMALL && P && C3 && NotBigD);           //Tutte
  allowAction(A_EMBED_VISION,(!SMALL || G.ne() >= 1) && P && NotBigD);//Vision
#ifndef VERSION_ALPHA
  allowAction(A_EMBED_GVISION,(!SMALL || G.ne() > 1)  && NotBigD);//GVision
#else
  allowAction( A_EMBED_GVISION,(G.nv() > 1) && NotBigD);//GVision
#endif
  allowAction(A_EMBED_CONTACT_BIP,(G.nv() > 1) && B && P && NotBigD);//Biparti contact
  allowAction(A_EMBED_BIP_2PAGES,(G.nv() > 1) && B && P && NotBigD);//Biparti 2-pages
  allowAction(A_EMBED_FPP_RECTI,!SMALL && S && P && NotBigD);        //FPP vision
  allowAction(A_EMBED_T_CONTACT,!SMALL && S && P && NotBigD);        //T-contact
  allowAction(A_EMBED_TRIANGLE,!SMALL && S && P && NotBigD);        //Triangle
  allowAction(A_EMBED_POLAR,C1 && NotBigD);//
  allowAction(A_EMBED_POLREC_DFS,C1 && NotBigD);//
  allowAction(A_EMBED_POLREC_DFSLR,C1 && NotBigD);//
  allowAction(A_EMBED_POLREC_BFS,C1 && NotBigD);//
  allowAction(A_EMBED_SPRING,(G.ne() >= 1) && NotBigD);                               //spring
  allowAction(A_EMBED_SPRING_PM,(G.ne() >= 1) && NotBigD);                            //springPM
  allowAction(A_EMBED_CURVES, (G.ne() >= 1) && NotBigD);                               //curves
  allowAction(A_EMBED_JACQUARD,!SMALL && P && NotBigD);              //Jacquard
  allowAction(A_EMBED_3dSCHNYDER,!SMALL && S && P && NotBigD);       //Schnyder 3d
  //dual
  allowAction(A_GRAPH_DUAL,(G.nv() > 1) && P && C1); 
  allowAction(A_GRAPH_DUAL_G,(G.nv() > 1) && P && C1);
  allowAction(A_GRAPH_ANGLE,(G.nv() > 1) && C1 && P);
  allowAction(A_GRAPH_ANGLE_G,(G.nv() > 1) && C1 && P);
  //Algo
  allowAction(A_ALGO_KURATOWSKI,!P);
  allowAction(A_ALGO_COTREE_CRITICAL,!P);
  allowAction(A_ALGO_COLOR_BIPARTI,B);
  allowAction(A_ALGO_COLOR_EXT,P);
  allowAction(A_ALGO_COLOR_NON_CRITIC,!P);
  //Orient
  allowAction(A_ORIENT_TRICON,!SMALL && P && C3);       //planar 3-con 
  allowAction(A_ORIENT_BIPAR,(G.nv() > 1) && P && B);   //biparti 
  allowAction(A_ORIENT_SCHNYDER,!SMALL && P && S &C1);  //planar schnyder
  allowAction(A_ORIENT_BIPOLAR,(G.nv() > 1) && P && C2);//bipolar plan
  allowAction(A_ORIENT_BIPOLAR_NP,(G.nv() > 1) && C2);  //bipolar 
#ifdef TDEBUG
  if(debug())DebugPrintf("END INFO %d\n",print);
#endif
  if(!print)return; 

  mw->showInfoTab();
  // Modify the buttons
  RBSimple->setChecked(S);
  RBPlanar->setChecked(P);
  RBMxPlanar->setChecked(T);
  RBBipartite->setChecked(B);
  if(B)
      {if(MaxBi)RBBipartite->setText("Max. Bipartite");
      else     RBBipartite->setText("Bipartite");
      }
  RBAcyclic->setChecked(A);
  RBConnected->setChecked(C1);
  RB2Connected->setChecked(C2);
  // 3-connectivity
  if(!P)RB3Connected->hide();
  else  RB3Connected->show();
  RB3Connected->setChecked(C3);

  // Outerplanar
  RBOuPlanar->setChecked(Outer);
  if(Outer)
      {if(C2 && (G.ne() == 2*G.nv() - 3)) RBOuPlanar->setText(tr("Max. OuterPlanar"));
      else    RBOuPlanar->setText(tr("OuterPlanar"));
      }
  // Serie (test is only for 2-connected graphs)
  if(P && !C2)
      RBSerie->hide();
  else
      {RBSerie->show();  RBSerie->setChecked(Serie);}

  LE_N->setText(QString("%1").arg(G.nv()));
  LE_M->setText(QString("%1").arg(G.ne()));
  LE_Min->setText(QString("%1").arg(dmin));
  LE_Max->setText(QString("%1").arg(dmax));
  
  //Print informations
  if(debug())DebugPrintf("\nn:%d m:%d",G.nv(),G.ne());
  Tprintf("Name:%s",~title());

  if(getPigaleError())DebugPrintf("ERROR:%s",(const char *)getPigaleErrorString().toLatin1());
  if(G.nv() == 0 || G.ne() == 0) return;
  if(T && G.nv() == 3)
      ;
  else if(T && G.nv() > 3)
      {E = G.CheckNoC3Sep();
      if(getPigaleError()){setPigaleError();Tprintf("Error CheckNoC3Sep");}
      else if(E) Tprintf("No C3 Separator");
      else Tprintf("C3 Separator");
      }
  else if(!C3 && P && S && !M && G.CheckSubdivTriconnected())
      {if(getPigaleError()){setPigaleError();Tprintf("Error CheckSubdivTriconnected");}
      else Tprintf("Subdivision of a 3-Connected");
      }

  if(G.Set().exist(PROP_MAPTYPE))
      {Prop1<int> maptype(G.Set(),PROP_MAPTYPE);
      if(maptype() == PROP_MAPTYPE_UNKNOWN)Tprintf("Unknown origin map");
      else if(maptype() == PROP_MAPTYPE_ARBITRARY)Tprintf("Arbitrary map");
      else if(maptype() == PROP_MAPTYPE_GEOMETRIC)Tprintf("Geometric map");
      else if(maptype() == PROP_MAPTYPE_LRALGO)Tprintf("LRALGO map");
      }
  //int g = G.ComputeGenus();
  //if(g)Tprintf("Genus of the current map: %d",g);
  if(A & C1) Tprintf("Acyclic: %d sources,%d sinks",ns,nt);
  //Tprintf("extbrin:%d %d (index)",G.vin[G.extbrin()](),G.vin[-G.extbrin()]());
  }
void Graph_Properties::allowAction(int action,bool condition)
  {allow[action] = condition;
  if(_updateMenu && mw->menuIntAction.contains(action))
      {mw->menuIntAction[action]-> setEnabled(condition);
      //{mw->menuIntAction[action]-> setVisible(condition);
      }
  }
