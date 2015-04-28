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
\file GraphSym.cpp
\brief Compute and display the symetries of a graph
*/

#include "pigaleWindow.h"
#include "GraphSym.h"
#include <QT/SymWindow.h>
#include <QT/pigaleQcolors.h>
#include <QT/Misc.h>

#include <QPushButton>
#include <QCheckBox>
#include <QBoxLayout>
#include <QPainter>
#include <QSvgGenerator>

bool Equal(double x, double y)
  {if(fabs(x-y) < epsilon)return true;
  return false;
  } 
bool Less(double x, double y)
  {if(!Equal(x,y) && x < y)return true;
  return false;
  }
bool Null(double x)
  {if(fabs(x) < epsilon )return true;
  return false; 
  }
int cmp(int i,int j)
  {double a = xcoord[i];
  double b = xcoord[j];
  if(Less(a,b))return 1;
  if( Equal(a,b) && Less(fabs(ycoord[i]),fabs(ycoord[j])))return 1;
  return 0;
  }
int AngleCmp(int i,int j)
  {double a = angles[i];
  double b = angles[j];
  if(Less(a,b))return 1;
  return 0;
  }
void FillCoords(GeometricGraph &G,double *x,int i1)
  {Prop<Tpoint3> Coord3(G.Set(tvertex()),PROP_COORD3);
  if(i1 == 1)
      {for(int i = 1; i <= G.nv(); i++)
	  x[i] = Coord3[i].x();
      }
  else if(i1 == 2)
      {for(int i = 1; i <= G.nv(); i++)
	  x[i] = Coord3[i].y();
      }
  else if(i1 == 3)
      {for(int i = 1; i <= G.nv(); i++)
	  x[i] = Coord3[i].z();
      }
  }
void FillCoords(GeometricGraph &G,int i1,int i2,int i3)
  {FillCoords(G,xcoord,i1);FillCoords(G,ycoord,i2);FillCoords(G,zcoord,i3);
  }
void RestaureCoords(GeometricGraph &G)
  {for(tvertex v = 1;v <= G.nv();v++)
      {xcoord[v()] =  G.vcoord[v()].x();
       ycoord[v()] =  G.vcoord[v()].y();
      }
  }
void Rotate(GeometricGraph &G,int rotate)		
  {if(rotate == 1)	
      {// Count nrho = # of extremal points
      double maxrho = xcoord[1]*xcoord[1] + ycoord[1]*ycoord[1];
      double xy;
      int iymax = 1; // point le plus eloigne ayant un y max
	  int i;
      for(i = 2;i <= G.nv();i++)
          {xy = xcoord[i]*xcoord[i] + ycoord[i]*ycoord[i];
          if(Less(maxrho,xy)){maxrho = xy;iymax = i;}
          }
      int nrho = 0;
      iymax = 1;
      double ymax = -1;
      for(i = 1;i <= G.nv();i++)
          {xy = xcoord[i]*xcoord[i] + ycoord[i]*ycoord[i];
          if(Equal(maxrho,xy))
              {++nrho;
              if(Less(ymax,ycoord[i])){ymax = ycoord[i];iymax = i;}
              }
          }
      if(Null(xcoord[iymax]))return;
      Tpoint p(xcoord[iymax],ycoord[iymax]);
      double theta = Angle(p);
      double cosinus = cos(theta*2.*PI);
      double sinus   = sin(theta*2.*PI);
      double x,y;
      for(i = 1;i <= G.nv();i++)
          {x = cosinus*xcoord[i] + sinus*ycoord[i];
          y = - sinus*xcoord[i] + cosinus*ycoord[i];
          xcoord[i] = x; ycoord[i] = y;
          }
      }
  else if(rotate == 2) // POM
      {double xmax = .0;
      int i,ixmax = 0;
      for(i = 1;i <= G.nv();i++)
          if(Less(xmax,xcoord[i])){xmax = xcoord[i];ixmax = i;}
      double xmin = xcoord[1];
      int ixmin = 1;
      for(i = 2;i <= G.nv();i++)
          if(Less(xcoord[i],xmin)){xmin = xcoord[i];ixmin = i;}
      if (Null(ycoord[ixmax]-ycoord[ixmin])) return;
      Tpoint p(xcoord[ixmax]-xcoord[ixmin],ycoord[ixmax]-ycoord[ixmin]);
      double theta = Angle(p);
      double cosinus = cos((theta*2.+.5)*PI);
      double sinus   = sin((theta*2.+.5)*PI);
      double x,y;
      for(i = 1;i <= G.nv();i++)
          {x = cosinus*xcoord[i] + sinus*ycoord[i];
          y = - sinus*xcoord[i] + cosinus*ycoord[i];
          xcoord[i] = x; ycoord[i] = y;
          }
      }
  else if(rotate == 12) 
      {double cosinus = cos(theta_12*2*PI);
       double sinus   = sin(theta_12*2.*PI);
       double x,y;
       
       for(int i = 1;i <= G.nv();i++)
           {x = cosinus*xcoord[i] + sinus*ycoord[i];
           y = - sinus*xcoord[i] + cosinus*ycoord[i];
           xcoord[i] = x; ycoord[i] = y;
           }
       }
     else if(rotate == 21)
         {double cosinus = cos((theta_12*2+.5)*PI);
         double sinus   = sin((theta_12*2.+.5)*PI);
         double x,y;
         for(int i = 1;i <= G.nv();i++)
             {x = cosinus*xcoord[i] + sinus*ycoord[i];
             y = - sinus*xcoord[i] + cosinus*ycoord[i];
             xcoord[i] = x; ycoord[i] = y;
             }
         }
    else if(rotate == 13)	
      {// Count nrho = # of extremal points
      double maxrho = ycoord[1]*ycoord[1] + zcoord[1]*zcoord[1];
      double yz;
      int i,izmax = 1; // point le plus eloigne ayant un z max
      for(i = 2;i <= G.nv();i++)
          {yz = ycoord[i]*ycoord[i] + zcoord[i]*zcoord[i];
          if(Less(maxrho,yz)){maxrho = yz;izmax = i;}
          }
      int nrho = 0;
      izmax = 1;
      double zmax = -1;
      for(i = 1;i <= G.nv();i++)
          {yz = ycoord[i]*ycoord[i] + zcoord[i]*zcoord[i];
          if(Equal(maxrho,yz))
              {++nrho;
              if(Less(zmax,zcoord[i])){zmax = zcoord[i];izmax = i;}
              }
          }
      if(Null(ycoord[izmax]))return;
      Tpoint p(ycoord[izmax],zcoord[izmax]);
      double theta = Angle(p);
      double cosinus = cos(theta*2.*PI);
      double sinus   = sin(theta*2.*PI);
      double y,z;
      for(i = 1;i <= G.nv();i++)
          {y = cosinus*ycoord[i] + sinus*zcoord[i];
          z = - sinus*ycoord[i] + cosinus*zcoord[i];
          ycoord[i] = z; zcoord[i] = y;
          }
      }
    else if(rotate == 23) 
      {
       double cosinus = cos(theta_23*2*PI);
       double sinus   = sin(theta_23*2.*PI);
       double x,y;
       
       for(int i = 1;i <= G.nv();i++)
           {x = cosinus*xcoord[i] + sinus*ycoord[i];
           y = - sinus*xcoord[i] + cosinus*ycoord[i];
           xcoord[i] = x; ycoord[i] = y;
           }
       }
     else if(rotate == 32)
         {
         double cosinus = cos((theta_23*2+.5)*PI);
         double sinus   = sin((theta_23*2.+.5)*PI);
         double x,y;
         for(int i = 1;i <= G.nv();i++)
             {x = cosinus*xcoord[i] + sinus*ycoord[i];
             y = - sinus*xcoord[i] + cosinus*ycoord[i];
             xcoord[i] = x; ycoord[i] = y;
             }
         }
  }
bool CheckSimilar(int *heap,int nrho,int i0)
  {int ntocompare = i0;
  int ntodo = nrho/i0 -1;
  double theta0 = angles[heap[i0] + 1];
  double a0,a1; // angles resp from 0 and i0

  int k = 0;
  for(int todo = 1;todo <= ntodo;++todo)
      {for(int i = 0;i < ntocompare;i++)
          {//if(debug())fprintf(stderr,"\ncmp(todo:%d)  %d %d (#ext<=%d)"
          //                           ,todo,i,k+i0+i,nrho);
          a0 = angles[heap[i] + 1];
          a1 = angles[heap[k+i0+i] + 1];
          //if(debug())fprintf(stderr,"\n %f %f -> %f"
          //                   ,a0*360.,a1*360.,(a1-a0-todo*theta0)*360.);
          if(!Equal(fabs(a1-a0),todo*theta0))return false;
          }
      k += ntocompare;
      }
  
  return true;
  }
int ComputeRotation(GeometricGraph &G,int i1,int i2,double &theta)
  {FillCoords(G,i1,i2,i1);
  // Count nrho = # of different extremal points
  // Compute max distance from origin
  double xy;
  double maxrho = xcoord[1]*xcoord[1] + ycoord[1]*ycoord[1];
  int i;
  for(i = 2;i <= G.nv();i++)
      {xy = xcoord[i]*xcoord[i] + ycoord[i]*ycoord[i];
      if(Less(maxrho,xy))maxrho = xy;
      }
  // Compute and Fill angles
  //Compute the furthest point with max x coordinate & y positif
  int nrho = 0;
  double xmax = -1;
  int ixmax = 1;
  angles= new double[G.nv()+1];
  Tpoint p;
  double angle;
  for(i = 1;i <= G.nv();i++)
      {xy = xcoord[i]*xcoord[i] + ycoord[i]*ycoord[i];
      if(Equal(maxrho,xy))
          {p.x() = xcoord[i]; p.y() = ycoord[i];
          angle = Angle(p);
          //if(debug())fprintf(stderr,"\n %d theta=%f",i,angle*360.);
          angles[++nrho] = angle;
          if(Less(xmax,xcoord[i]) && Less(.0,ycoord[i]))
              {xmax = xcoord[i];ixmax = i;}
          }
      }
  p.x() = xcoord[ixmax]; p.y() = ycoord[ixmax];
  double theta0 = Angle(p);
  //Sort extremal points according to their angle (0,+1)
  for(i = 1;i <= nrho;i++)
      {angles[i] -= theta0;
      if(angles[i] < .0)angles[i] += 1.;
      }
  
  int *heap = new int[nrho];
  HeapSort(AngleCmp,1,nrho,heap);
  //Remove extremal points with the same angle
  bool success = false;
  double theta1= .0;
  int k;
  int iter = (nrho == 2) ?  2 : (nrho+1)/2;
  for(i = 1;i < iter;i++)
      {k = heap[i] + 1;
      theta1 = angles[k];
      success = CheckSimilar(heap,nrho,i);
      if(success)break;
      }
  int nsym = 0;
  // i = nombre d'elements de la classe
  if(success )
      {nsym = (int)(1./theta1 +.5);
      if(nsym %2 == 0)
          //{k = heap[i/2] + 1; //diagonales
          {k = heap[(i+1)/2] + 1;
          theta = theta0 + angles[k]/2.;
          }
      else // Odd symmetry
          {if(i%2 == 0)
              {k = heap[1] + 1;
              theta = theta0  + angles[k]/2.;
              }
          else if(nrho != 2)
              {k = heap[(i+1)/2] + 1;
              theta = theta0  + angles[k];
              }
          else
             theta = theta0 +  angles[heap[1]+1]/2.; 
          }
      }
  //if(success)Tprintf("\n nrho=%d nsym=%d nclass=%d",nrho,nsym,i);
  //else Tprintf("\n NO ROTATION nrho=%d",nrho);
  
  delete [] heap;
  delete [] angles;
  return nsym;
  }
int FindSymetry(GeometricGraph &G,int ii1,int ii2,int ii3,int rotate
	    ,int &confusion,int &identify,int &NumFixPoints)
  {match0 = match1 = 0;
  FillCoords(G,ii1,ii2,ii3);
  if(rotate)Rotate(G,rotate);
  int i,k;
  int n = G.nv();
  // Tri des coordonnees
  int *heap = new int[n];
  HeapSort(cmp,1,n,heap);
  Prop<int> symlabel(G.Set(tvertex()),PROP_SYMLABEL,0); symlabel[0] = 0;
  double xprev = 20.;  double yprev = 20.;
  double xk,yk;
  int kprev = 0;
  int match_positif = 0;  int match_negatif = 0;
  int label = 0;
  bool x_Equals = false;
  confusion = 0;
  identify = 0;
  int sym = 1;
  	
  bool NewBloc;
  bool NewBlocNul;
  bool NeedCheck = false;
  int i0;
  i = k = i0  = 0;
  NumFixPoints = 0;
  int kprevFixPoint = 0;
  // Checking symmetry along x axis
  do
      {kprev = k;
      k = heap[i] +1;
      xk = xcoord[k]; yk = ycoord[k];
      NewBlocNul = Equal(yk,.0) ? true :false;
      if(NewBlocNul) // probably new fixed point
          {G.vcolor[k] = Yellow;symlabel[k] = ++label;++NumFixPoints;
          if(NumFixPoints > 1)
              {if(Equal(xk,xcoord[kprevFixPoint]))
                  G.vcolor[k] = G.vcolor[kprev] = Grey1;
              }
          kprevFixPoint = k;
          }
      else symlabel[k] = 0;
      
      NewBloc = (NewBlocNul || !Equal(xk,xprev) ||
                 !Equal(fabs(yk),fabs(ycoord[kprev]))) ? true : false;
      if(NewBloc)
          {if(NeedCheck) // Verification des precedents
              {if(!match_positif || match_positif != match_negatif)
                  {sym = 0;
                  break;
                  }
              if(match_positif > 1)
                  {confusion += 1;
                  identify += RemoveConfusion(G,heap,i0,i-1,label);
                  }
              match_positif = match_negatif = 0;
              NeedCheck = false;
              }
          if(!NewBlocNul)
              NeedCheck = true;
          x_Equals = false;
          }
      else if(!NewBlocNul)
          {if(!x_Equals) //premiere fois egalite
              {x_Equals = true;
              NeedCheck = true;
              i0 = i ? i - 1: 0; // index du prev
              yprev = ycoord[kprev];
              if(Less(.0,yprev)){++match_positif;G.vcolor[kprev] = Red;}
              else {++match_negatif;G.vcolor[kprev] = Blue;}
              if(Less(.0,yk)){++match_positif;G.vcolor[k] = Red;}
              else {++ match_negatif;G.vcolor[k] = Blue;}
              if(match_positif == match_negatif)
                  {symlabel[k] = symlabel[kprev] = ++label;}
              }
          else
              {if(Less(.0,yk)){++match_positif;G.vcolor[k] = Red;}
              else {++match_negatif;G.vcolor[k] = Blue;}
              }		
          }
      xprev = xk;
      kprev = k;
      }while(sym && ++i < n);
  		
  // Checking last block
  if(sym)
      {if(match_positif != match_negatif)
          sym = 0;
      else if(match_positif > 1)
          {confusion += 1;
          identify += RemoveConfusion(G,heap,i0,n-1,label);
          }
      }
  if(sym)SymColorEdges(G);
  if(sym && identify) // Identification failed
      {for(int i = 1;i <= G.nv();i++)
          if(G.vcolor[i] == Blue)G.vcolor[i] = Green;
      G.vcolor[match0] = G.vcolor[match1] = Pink;
      Tprintf("IDENT NO-SUCCESS:%d(%d) R:%d",ii1,ii2,rotate);
      }
  else if(sym && !identify) 
      {int NumImprove = CheckSymmetry(G,label);// After SymColoRedge
      if(!NumImprove && !debug())
          {sym = 0;
          if(debug())Tprintf("CheckSymmetry failed %d(%d)",ii1,ii2);
          }
      else if(!NumImprove && debug())
          {sym = -1;
          Prop1<int> is_sym(G.Set(),PROP_SYM);
          Tprintf("NO SYM %d(%d) R:%d Opt=%d",ii1,ii2,rotate,Opt);
          }
      else
          {if(Opt && NumImprove > 1)
              {RemoveFixedPoints(G,heap);
              SymColorEdges(G);
              sym = CheckSymmetry(G,label);
              if(!sym)Tprintf("Optimisation failed");
              }
          if(sym)
              {Prop1<int> is_sym(G.Set(),PROP_SYM);
              Tprintf("FOUND SYM %d(%d) R:%d Opt=%d",ii1,ii2,rotate,Opt);
              // For Printing
              for(int i = 1;i <= G.nv();i++)
                  {if(G.vcolor[i] == Blue)G.vcolor[i] = Blue2;
                  if(G.vcolor[i] == Red)G.vcolor[i] = Brown;
                  }
              }
          else if(debug())
              {sym = -1;
              Prop1<int> is_sym(G.Set(),PROP_SYM);
              Tprintf("NO SYM %d(%d) R:%d Opt=%d",ii1,ii2,rotate,Opt);
              }
          Opt = false;
          }
      }
  else if(!sym && !debug())
      for(int i = 1;i <= G.nv();i++)G.vcolor[i] = Yellow;
  else
      {sym = -1;
      Tprintf("NO SYM %d(%d) R:%d Opt=%d",ii1,ii2,rotate,Opt);
      }
  
  delete [] heap;
  return sym;
  }
void RecolorFixedPoint(GeometricGraph &G,int v)
  {tbrin b;
  tvertex w;
  tedge e;
  int col =  G.vcolor[v];
  Forall_adj_brins(b,v,G)
      {w = G.vin[-b];e = b.GetEdge();
      if(G.vcolor[w] == Yellow)
          G.ecolor[e] = col;
      else if(G.vcolor[w] != col)
          {G.ecolor[e] = Black;G.ewidth[e] = 1;}
      }
  }
int RemoveFixedPoints(GeometricGraph &G,int *heap)
  {Prop<int> symlabel(G.Set(tvertex()),PROP_SYMLABEL);
  double xprev = 20.;  
  double xk;
  int k = 0;
  int i = 0;
  int i0 = -1;
  int k0,k1;
  
  while(i < G.nv())
       {k = heap[i] +1;
       xk = xcoord[k];
       if(G.vcolor[k] != Ivory || (!Equal(xprev,xk) && (i0!=-1)))
           {if(i0 != -1)
               {if(i == i0 + 2)
                   {k0 = heap[i0] + 1;k1 = heap[i0+1] + 1;
                   G.vcolor[k0] =  Red;    /* RecolorFixedPoint(G,k0);*/
                   G.vcolor[k1] =  Blue;   /* RecolorFixedPoint(G,k1);*/
                   symlabel[k1] = symlabel[k0];
                   }
               i0 = -1;
               }
           xprev = 20;
           }
       if(!Equal(xprev,xk) && G.vcolor[k] == Ivory)
           //discovering first Ivory point at xk
           {xprev = xk;i0 = i;}
       i++;
       }
  if(i0 != -1)
      {if(i == i0 + 2)
          {k0 = heap[i0] + 1;k1 = heap[i0+1] + 1;
          G.vcolor[k0] =  Red;     RecolorFixedPoint(G,k0);
          G.vcolor[k1] =  Blue;     RecolorFixedPoint(G,k1);
          symlabel[k1] = symlabel[k0];
          }
      }
  return 0;
  }
int RemoveConfusion(GeometricGraph &G,int *heap,int i0,int i1,int &label)
  {Prop<int> symlabel(G.Set(tvertex()),PROP_SYMLABEL);
  svector<int> positif(i1-i0+1);
  svector<int> negatif(i1-i0+1);
  int npos = 0;
  int nneg = 0;
  int i,k;

  for(i = i0;i <= i1;i++)
      {k = heap[i] + 1;
      if(Less(.0,ycoord[k])) positif[npos++] = k;
      else                   negatif[nneg++] = k;
      }
  if(npos != nneg)return 1;

  int kpos,kneg;
  int match = 0;
  for(i = 0;i < npos;i++)
      {kpos = positif[i];
      if(MatchZ(negatif,kpos,npos,kneg))
          {symlabel[kpos] = symlabel[kneg] = ++label;
          ++match;
          }
      else
          break;
      }
  return npos-match;
  }
bool MatchZ(svector<int> & negatif,int kpos,int npos,int &kneg)
  {bool success = false;
  double z = zcoord[kpos];
	
  int j = 0;
  do   // search a negatif with same zcoord
      {kneg = negatif[j];
      success = (kneg != 0) && Equal(z,zcoord[kneg]);
      if(success)negatif[j] = 0;
      }while(!success && ++j < npos);
  if(!success && !match0)
      {match0 = kneg;match1 = kpos;}
  return success;
  }
void SymColorEdges(GeometricGraph &G)
  {int cola,colb;
  
  for(tedge e = 1;e <= G.ne();e++)
      {cola = G.vcolor[G.vin[e]]; colb = G.vcolor[G.vin[-e]];
      G.ecolor[e] = Black;G.ewidth[e] = 1;
						
      if(cola == Red && colb != Blue )
          {G.ecolor[e] = Red;G.ewidth[e] = 2;}
      else if(colb == Red && cola != Blue )
          {G.ecolor[e] = Red;G.ewidth[e] = 2;}
			
      else if(cola == Blue && colb != Red )
          {G.ecolor[e] = Blue;G.ewidth[e] = 2;}
      else if(colb == Blue && cola != Red )
          {G.ecolor[e] = Blue;G.ewidth[e] = 2;}

      // Colour in Green edges between fix points
      else if(cola != Red && cola !=Blue && colb !=Red && colb != Blue) 
          {G.ecolor[e] = Green;G.ewidth[e] = 2;}

      //if(G.ecolor[e] != Green && (cola == Grey1 || colb == Grey1)) 
      //{G.ecolor[e] = Black;G.ewidth[e] = 1;}
      }
  }
int CheckSymmetry(GeometricGraph &G,int MaxLabel)
// return 0 if fails
// return > 1 if success -> # of fixed points that may be deplaced
  {Prop<int> symlabel(G.Set(tvertex()),PROP_SYMLABEL);
  svector<tedge>link(1,G.ne()); link.clear();
  svector<tedge>top1(1,MaxLabel); top1.clear();
  svector<tedge>top1b(1,MaxLabel); top1b.clear();
  svector<tedge>top2(1,MaxLabel); top2.clear();
  svector<tedge>top2b(1,MaxLabel); top2b.clear();
  tvertex v;
  tedge e,next;
  int label,lab,lab0,lab1;

  //First sort edges not coloRed Green with respect to biggest label,
  for(e = 1;e <= G.ne();e++)
      {if(G.ecolor[e] == Green)continue;
      lab0 = symlabel[G.vin[e]] ; lab1 = symlabel[G.vin[-e]];
      if(lab0 == lab1)continue; // Black edge (axial symmetry)
      lab = (lab0 < lab1 ) ? lab1 :lab0;
      if(G.ecolor[e] != Black)
          {link[e] = top1[lab]; top1[lab] = e;}
      else
          {link[e] = top1b[lab]; top1b[lab] = e;}
      
      }
  // Then sort with respect to smallest label non Black edges
  for(label = MaxLabel;label > 1 ;label--)
      {e = top1[label];
      while(e != 0)
          {next = link[e];     //as link is modified
          lab0 = symlabel[G.vin[e]] ; lab1 = symlabel[G.vin[-e]];
          lab = (lab0 < lab1 ) ? lab0 :lab1;
          link[e] = top2[lab]; top2[lab] = e;
          e = next;
          }
      }
  // Then sort with respect to smallest label Black edges
  for(label = MaxLabel;label > 1 ;label--)
      {e = top1b[label];
      while(e != 0)
          {next = link[e];     //as link is modified
          lab0 = symlabel[G.vin[e]] ; lab1 = symlabel[G.vin[-e]];
          lab = (lab0 < lab1 ) ? lab0 :lab1;
          link[e] = top2b[lab]; top2b[lab] = e;
          e = next;
          }
      }
  
  // Check that each Red and Blue edge appear twice
  int labb0,labb1;
  for(lab = MaxLabel;lab > 1;lab--)
      {e = top2[lab];
      while(e != 0)
          {next = link[e];
          lab0 = symlabel[G.vin[e]]; lab1 = symlabel[G.vin[-e]];
          if(lab1 < lab0)Tswap(lab0,lab1);
          if(next == 0)return 0;
          labb0 = symlabel[G.vin[next]]; labb1 = symlabel[G.vin[-next]];
          if(labb1 < labb0)Tswap(labb0,labb1);
          if( lab0 != labb0 || lab1 != labb1)return 0;
          e = link[next];// check next 2 edges
          }
      }

  // Check that each Black edge appear twice
  for(lab = MaxLabel;lab > 1;lab--)
      {e = top2b[lab];
      while(e != 0)
          {next = link[e];
          lab0 = symlabel[G.vin[e]]; lab1 = symlabel[G.vin[-e]];
          if(lab1 < lab0)Tswap(lab0,lab1);
          if(next == 0)return 0;
          labb0 = symlabel[G.vin[next]]; labb1 = symlabel[G.vin[-next]];
          if(labb1 < labb0)Tswap(labb0,labb1);
          if( lab0 != labb0 || lab1 != labb1)return 0;
          e = link[next];// check next 2 edges
          }
      }
  int ret = 1;
  for(v = 1; v <= G.nv();v++)
      if(G.vcolor[v] == Grey1){G.vcolor[v] = Ivory;++ret;}
  if(ret > 1)--ret;
  return ret;
  }
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class GraphSymPrivate
{public:
  GraphSymPrivate()
      {is_init = false;
      SymLabel = true;
      Optimal = false;
      Factorial = false;
      editor = NULL;
      bt_sym = bt_opt = bt_fact = NULL;
      pGG = NULL;
      }
  ~GraphSymPrivate()
      {delete editor;}
  bool is_init; 
  bool Factorial;
  bool Optimal;
  bool OtherCoords;
  bool SymLabel;
  QCheckBox *bt_sym,*bt_opt,*bt_fact;
  pigaleWindow *mw;
  GeometricGraph *pGG;
  GraphContainer *pGC; 
  SymWindow  *editor;
};
//*****************************************************

GraphSym::GraphSym(QWidget *parent,pigaleWindow *mw)
    : QWidget(parent)
  {d = new GraphSymPrivate;
  d->pGC = new GraphContainer;
  d->mw = mw;
  }
GraphSym::~GraphSym()
  {delete d->pGG;delete d->pGC; delete d;}

int GraphSym::update()
  {if(!d->is_init)
      {QVBoxLayout* vb = new QVBoxLayout(this);
      vb->setMargin(2);
      d->editor = new SymWindow(d,this);
      vb->addWidget(d->editor);
      QHBoxLayout* hb = new QHBoxLayout();  
      vb->addStrut(35); // why necessary ??? 30 = hb->minimumSize().height()
      vb->addLayout(hb);
      QPushButton* bt_next = new QPushButton( "Next",this);
      bt_next->setMaximumWidth(60);
      d->bt_fact = new QCheckBox( "Fact",this);           
      d->bt_opt = new QCheckBox( "Opt",this);
      d->bt_sym = new QCheckBox( "Sym Lab",this);
      hb->addWidget(bt_next); 
      hb->insertSpacing(1,300);
      hb->addWidget(d->bt_fact);
      hb->addWidget(d->bt_opt);
      hb->addWidget(d->bt_sym);
      d->is_init = true;
      connect(bt_next,SIGNAL(clicked()),SLOT(Next()));
      connect(d->bt_fact,SIGNAL(clicked()),SLOT(Factorial()));
      connect(d->bt_opt,SIGNAL(clicked()),SLOT(Optimal()));
      connect(d->bt_sym,SIGNAL(clicked()),SLOT(SymLabel()));
      d->bt_fact->setPalette(d->mw->LightPalette);
      d->bt_opt->setPalette(d->mw->LightPalette);
      d->bt_sym->setPalette(d->mw->LightPalette);
      }
  else
      {delete d->pGG;
      delete [] xcoord;delete [] ycoord;delete [] zcoord;
      }

  d->Factorial = false; d->SymLabel = true;
  d->Optimal = false;  d->OtherCoords = false;
  d->bt_fact->setChecked(d->Factorial);
  d->bt_opt->setChecked(d->Optimal);
  d->bt_sym->setChecked(d->SymLabel);
  //Copy the graph
  *(d->pGC) = d->mw->GC;
  d->pGG = new GeometricGraph(*(d->pGC));
  d->editor->start = 1;d->editor->start0 = 1;
  GeometricGraph & G = *(d->pGG);
  int n = G.nv();
  xcoord	= new double[n+1];	
  ycoord	= new double[n+1];
  zcoord	= new double[n+1];
  Prop<double> EigenValues(G.Set(tvertex()),PROP_EIGEN);
  bool EqualEigenVal = Equal(EigenValues[1],EigenValues[2]) ? true : false;
  int Rotation = EqualEigenVal ?  ComputeRotation(G,1,2,theta_12) :0;
  bool EqualEigenVal_23 = Equal(EigenValues[2],EigenValues[3]) ? true : false;
  int Rotation_23 = EqualEigenVal_23 ? ComputeRotation(G,2,3,theta_23) :0;
  if(Rotation)Tprintf("Probable Rotation (1-2) of order:%d",Rotation);
  if(Rotation_23)Tprintf("Probable Rotation (2-3) of order:%d",Rotation_23);
  d->mw->tabWidget->setTabText(d->mw->tabWidget->indexOf(this),"Symetrie");
  d->mw->tabWidget->setCurrentIndex(d->mw->tabWidget->indexOf(this));

  d->editor->FindSym();
  if(!d->editor->sym)Tprintf("No symetrie found");  
  return 0;
  }
void GraphSym::print(QPrinter *printer)
  {d->editor->print(printer);
  }

void GraphSym::image(QPrinter* printer, QString suffix)
  {d->editor->image(printer,suffix);
  }
void GraphSym::resizeEvent(QResizeEvent* e)
  {if(d->editor)d->editor->initialize(); 
  QWidget::resizeEvent(e);
  }
void GraphSym::Factorial()
  {d->Factorial = d->bt_fact->isChecked();
  //d->OtherCoords = true;
  d->editor->DrawSym();
  //d->editor->FindSym();
  }
void GraphSym::Optimal()
  {d->Optimal = d->bt_opt->isChecked();
  d->OtherCoords = true;
  d->editor->FindSym();
  }
void GraphSym::SymLabel()
  {d->SymLabel = d->bt_sym->isChecked();
  d->editor->DrawSym();
  }
void GraphSym::Next()
  {d->editor->FindSym();
  }
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
SymWindow::SymWindow(GraphSymPrivate *g,QWidget * parent)
    : QWidget(parent),gsp(g),is_init(false),isHidden(true)
 {}
void SymWindow::showEvent(QShowEvent*)
  {isHidden = false;}
void SymWindow::hideEvent(QHideEvent*)
  {isHidden = true;}
void SymWindow::resizeEvent(QResizeEvent* e)
  {Normalise();
  QWidget::resizeEvent(e);
  }
void SymWindow::initialize()
  {if(is_init)return;
  //resize(sizeHint());
  is_init = true;
  }
void SymWindow::Axes(int i1,int i2)
  {axe1 = i1;axe2 = i2;
  }
void SymWindow::FindSym()
  {GeometricGraph & G = *(gsp->pGG);
  int confusion,identify;
  Prop<double> EigenValues(G.Set(tvertex()),PROP_EIGEN);
  bool EqualEigenVal = Equal(EigenValues[1],EigenValues[2]) ? true : false;
  int Rotation = EqualEigenVal ?  ComputeRotation(G,1,2,theta_12) :0;
  bool EqualEigenVal_23 = Equal(EigenValues[2],EigenValues[3]) ? true : false;
  int Rotation_23 = EqualEigenVal_23 ? ComputeRotation(G,2,3,theta_23) :0;
  int NumFixPoints;
  
  sym = 0;
  Opt = gsp->Optimal;
  if(gsp->OtherCoords){start = start0;gsp->OtherCoords = false;}
  start0 = start;
  if(start && start <= 1 && !EqualEigenVal)
      {if(FindSymetry(G,2,1,3,0,confusion,identify,NumFixPoints))
	  {sym = 1;start0 = start;start = 2;Axes(2,1);DrawSym();return;}
      }
  if(start && start <= 2 && !EqualEigenVal)
      {if(FindSymetry(G,1,2,3,0,confusion,identify,NumFixPoints))
	  {sym = 1;start0 = start;start = 3;Axes(1,2);DrawSym();return;}
      }
  // rot hub
  if(start && start <= 3 && EqualEigenVal)
      {if(FindSymetry(G,2,1,3,1,confusion,identify,NumFixPoints))
	  {sym = 1;start0 = start;start = 4;Axes(2,1);DrawSym();return;}
      }
  if(start && start <= 4 && EqualEigenVal)
      {if(FindSymetry(G,1,2,3,1,confusion,identify,NumFixPoints))
	  {sym = 1;start0 = start;start = 5;Axes(1,2);DrawSym();return;}
      }
  // even sym and odd
  if(start && start <= 5 && EqualEigenVal)//odd,even
      {if(FindSymetry(G,1,2,3,12,confusion,identify,NumFixPoints))
	  {sym = 1;start0 = start;start = 6;Axes(1,2);DrawSym();return;}
      }
  if(start && start <= 6 && EqualEigenVal && (Rotation %2 == 0))// even
      {if(FindSymetry(G,1,2,3,21,confusion,identify,NumFixPoints))
	  {sym = 1;start0 = start;start = 9;Axes(1,2);DrawSym();return;}
      }
  // pom
  if(start && start <= 9 && EqualEigenVal)
      {if(FindSymetry(G,2,1,3,2,confusion,identify,NumFixPoints))
	  {sym = 1;start0 = start;start = 10;Axes(2,1);DrawSym();return;}
      }
  if(start && start <= 10 && EqualEigenVal)
      {if(FindSymetry(G,1,2,3,2,confusion,identify,NumFixPoints))
	  {sym = 1;start0 = start;start = 11;Axes(1,2);DrawSym();return;}
      }
  // autres axes: symetrie centrale
  if(start && start <= 11)
      {if(FindSymetry(G,3,1,2,0,confusion,identify,NumFixPoints))
	  {sym = 1;start0 = start;start = 12;Axes(3,1);DrawSym();return;}
      }
  if(start && start <= 12)
      {if(FindSymetry(G,1,3,2,0,confusion,identify,NumFixPoints))
	  {sym = 1;start0 = start;start = 13;Axes(1,3);DrawSym();return;}
      }
  // 1-3 avec rot 13
  if(start && start <= 13 && EqualEigenVal_23)
      {if(FindSymetry(G,1,3,2,13,confusion,identify,NumFixPoints))
	  {sym = 1;start0 = start;start = 14;Axes(1,3);DrawSym();return;}
      }
      
  if(start && start <= 14)
      {if(FindSymetry(G,3,2,1,0,confusion,identify,NumFixPoints))
	  {sym = 1;start0 = start;start = 15;Axes(3,2);DrawSym();return;}
      }
  if(start && start <= 15)
      {if(FindSymetry(G,2,3,1,0,confusion,identify,NumFixPoints))
	  {sym = 1;start0 = start;start = 16;Axes(2,3);DrawSym();return;}
      }
  // axes 2-3 with rotation
  if(start && start <= 16 && Rotation_23)
      {if(FindSymetry(G,2,3,1,23,confusion,identify,NumFixPoints))
	  {sym = 1;start0 = start;start = 17;Axes(2,3);DrawSym();return;}
      }
  if(start && start <= 17 && Rotation_23)
      {if(FindSymetry(G,2,3,1,32,confusion,identify,NumFixPoints))
	  {sym = 1;start0 = start;start = 1;Axes(2,3);DrawSym();return;}
      }
  start = 1;start0 = start;
  }
void SymWindow::paintEvent(QPaintEvent *e)
  {//initialize the SymWindow
  if(!is_init){qDebug("A");is_init = true;initialize();}
  if(isHidden)return;
  QWidget::paintEvent(e);
  //Ne doit surtout pas effacer la background
  QPainter p(this);
  if(sym)update(&p);
  } 
void SymWindow::DrawSym()
  {GeometricGraph & G = *(gsp->pGG);
  if(!gsp->Factorial)RestaureCoords(G);
  else FillCoords(G,axe1,axe2,1);
  Normalise();
  //setBackgroundColor(Qt::white);
  repaint(geometry());
 //  QPainter p(this);
//   update(&p);
  }
void SymWindow::update(QPainter *p)
  {GeometricGraph & G = *(gsp->pGG);
  QPen pn = p->pen();
  QPoint ps,pt;
  QString t;
  QFont font;

  p->fillRect(geometry(),Qt::white);
  if(gsp->Factorial)//draw axes with labels
      {pn.setColor(color[Grey2]);pn.setWidth(3); p->setPen(pn);
      ps = QPoint(0,height()-(int)ytr); pt = QPoint(width(),this->height()-(int)ytr);
      p->drawLine(ps,pt);
      pn.setWidth(1); p->setPen(pn);
      ps = QPoint((int)xtr,0); pt = QPoint((int)xtr,height());
      p->drawLine(ps,pt);
      //draw labels
      font = QFont("sans",10);
      p->setFont(font);
      pn.setColor(color[Violet]);p->setPen(pn);
      t.sprintf("(%d)",axe1);
      p->drawText(QPoint(width()-18,height()-(int)ytr-3),t);//hor
      t.sprintf("(%d)",axe2);
      p->drawText(QPoint((int)xtr+5,15),t); //ver
      }
  //Draw edges
  for(tedge e = 1; e <= G.ne();e++)
      {ps = QPoint((int)xcoord[G.vin[e]()],this->height() - (int)ycoord[G.vin[e]()]);
       pt = QPoint((int)xcoord[G.vin[-e]()],this->height() - (int)ycoord[G.vin[-e]()]);
       pn.setColor(color[G.ecolor[e]]); pn.setWidth(2);
       p->setPen(pn);
       p->drawLine(ps,pt);
      }
  //Draw vertices
  QColor  col;
  font = QFont("sans",fs);p->setFont(font);
  QBrush pb = p->brush();pb.setStyle(Qt::SolidPattern);
  Prop<int> symlabel(G.Set(tvertex()),PROP_SYMLABEL);
  for(tvertex v = 1; v <= G.nv();v++)
      {if(gsp->SymLabel)
          t.sprintf("%2.2d",(int)symlabel(v));
      else
          t = getVertexLabel(G.Container(),v);
      QSize size = QFontMetrics(font).size(Qt::AlignCenter,t);
      int dx =size.width() + 6;  int dy =size.height() + 2;
      int x = (int) xcoord[v()];
      int y =  this->height() - (int) ycoord[v()];
      QRect rect = QRect(x-dx/2,y-dy/2,dx,dy);
      pn.setWidth(1); pn.setColor(Qt::black); pb.setColor(color[G.vcolor[v]]);
      p->setPen(pn);p->setBrush(pb);
      p->drawRect(rect);
      col = color[G.vcolor[v]];
      pn.setColor(OppCol(col));p->setPen(pn);
      p->drawText(rect,Qt::AlignCenter,t);
      }
  }
#define border 30
void SymWindow::Normalise()
  {if(!is_init)return;
  GeometricGraph & G = *(gsp->pGG);
  int n = G.nv();
  double x_min = border;
  double x_max = width() -border;
  double y_min = border;
  double y_max = height() - border;
  double min_used_x,max_used_x,min_used_y,max_used_y;
  fs = (int)((double)Min(width(),height())/50.); 
  if((fs%2) == 1)++fs; fs = Min(fs,10);
  max_used_x = min_used_x = xcoord[1]; 
  max_used_y = min_used_y = ycoord[1];
  for(int i = 2;i <= n;i++)
      {min_used_x = Min(min_used_x,xcoord[i]);
      max_used_x = Max(max_used_x,xcoord[i]);
      min_used_y = Min(min_used_y,ycoord[i]);
      max_used_y = Max(max_used_y,ycoord[i]);
      }
  
  if(max_used_x > min_used_x + 1E-5)
      {xmul = (x_min - x_max)/(min_used_x - max_used_x);
      xtr  = (min_used_x*x_max - max_used_x*x_min)/(min_used_x - max_used_x);
      }
  else
      {xmul = 1.;
      xtr = (x_max + x_min)/2.-min_used_x;
      }

  if(max_used_y > min_used_y + 1E-5)
      {ymul = (y_min - y_max)/(min_used_y - max_used_y);
      ytr  = (min_used_y*y_max - max_used_y*y_min)/(min_used_y - max_used_y);
      }
  else
      {ymul = 1.;
      ytr = (y_max + y_min)/2.-min_used_y;
      }
  for(int i = 1;i <= n;i++)
      {xcoord[i] = xmul*xcoord[i] + xtr;
       ycoord[i] = ymul*ycoord[i] + ytr;
      }
  }
void SymWindow::print(QPrinter *printer)
  {QPainter pp(printer);
  update(&pp);
  }
void SymWindow::image(QPrinter* printer, QString suffix)
  {QRect geo = geometry();
  resize(staticData::sizeImage,staticData::sizeImage);
  qApp->processEvents();
  if(suffix == "png" || suffix == "jpg")
      {QPixmap pixmap = QPixmap::grabWidget(this); 
      pixmap.save(staticData::fileImage);
      }
  else if(suffix == "svg") 
      {QSvgGenerator *svg = new QSvgGenerator();
      svg->setFileName(staticData::fileImage);
      svg->setResolution(90); 
      svg->setSize(QSize(width(),height()));
      QPainter pp(svg);
      update(&pp);
      }
  else if(suffix == "pdf" || suffix == "ps")
      {QPainter pp(printer);
      update(&pp);
      }
  setGeometry(geo);
  }
