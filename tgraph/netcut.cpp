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
\ingroup core
*/
/*! \file netcut.cpp
  \brief Embedder and Partitioner method implementation
*/

#include <TAXI/netcut.h>
#include <TAXI/color.h>
#include <TAXI/Tmessage.h>
static const char *ColorNames[] = 
    {"White",
    "Black",
    "Red",
    "Green",
    "Blue",
    "Yellow",
    "Violet",
    "Orange",
    "Cyan",
    "Brown",
    "Pink",
    "Green2",
    "Blue2",
    "Grey1",
    "Grey2",
    "Grey3",
    "Ivory"
    };
//! local structure 
static Locals *l;

//! Defines the distance that will be used to isometrically embed the graph in \f$\mathbb{R}^{n-1}\f$
/*!
  The returned reference has the following meaning:
  \arg \c 0 \link EmbedRnGraph::ComputeCzekanovskiDistances() Czekanovski-Dice distance \endlink
  \arg \c 1 \link EmbedRnGraph::ComputeBisectDistances() Bisection distance \endlink
  \arg \c 2 \link EmbedRnGraph::ComputeAdjacenceDistances() Adjacency distance \endlink
  \arg \c 3 \link EmbedRnGraph::ComputeAdjacenceMDistances() Translated adjacency distance \endlink
  \arg \c 4 \link EmbedRnGraph::ComputeLaplacianDistances() Laplacian distance \endlink
  \arg \c 5 \link EmbedRnGraph::ComputeOrientDistances() Oriented distance \endlink
  \arg \c 6 \link EmbedRnGraph::ComputeR2Distances() R2 distance \endlink
  \sa EmbedRnGraph::init()
  \relates EmbedRnGraph
*/

//! Embed the graph in \f$\mathbb{R}^{n-1}\f$ and fill the coordinates of the point in an \f$\mathbb{R}^3\f$ projection
/*!
  \param G0 Refrence to the topologcal graph to embed
  \pre The graph should have at least 3 vertices and 2 edges
*/
int Embed3d(TopologicalGraph &G0,int usedDistance)
  {if(G0.nv() < 3 || G0.ne() < 2)return -1;
  EmbedRnGraph G(G0,usedDistance);

  if(!G.ok){Tprintf("DIAG ERROR (Complete Graph?)"); return -1; }
  RnEmbedding &em=*new RnEmbedding(G0.nv(),G0.nv()-1,G0.TestPlanar());
  Prop1<RnEmbeddingPtr> embedp(G.Set(),PROP_RNEMBED);
  if (embedp().ptr!=0) delete embedp().ptr;
  embedp().ptr=&em;
  int i;
  for (i = 1; i<=em.dmax;i++)
    for (int v=1; v<= G0.nv(); v++)
      em.vector(i)[v]=G.Coords[v][i];

  Prop<Tpoint3> Coord3(G0.Set(tvertex()),PROP_COORD3);
  Coord3.vector()=em.Coord();
  Prop<double> EigenValues(G0.Set(tvertex()),PROP_EIGEN);
  for(i = 1;i <= G.nv();i++)
      EigenValues[i] = G.EigenValues[i];
  return 0;
  }
//! Split a graph in a prescribed number of classes
/*!
   \param G0 a simple graph with at least 3 vertices and 2 edges
   \param NumberOfClasses the number of desired classes
   \par Actions:
      \li creates a SplitGraph which derives from EmbedRnGraph which
       embeds the graph in \f$\mathbb{R}^{n-1}\f$
      \li calls SplitGraph::Segment to perform the partition 
      \li color the vertices according to its class G.ClassNumber[v()]
*/
/*!
  \param G0 Refrence to the topological graph to embed
  \param NumberOfClasses Required number of classes
  \pre The graph should have at least 3 vertices and 2 edges
*/
int split(Graph &G0,int &NumberOfClasses,int usedDistance)
// Pas de boucles ou aretes multiples
  {if(G0.nv() < 3 || G0.ne() < 2)return -1;
  int i;
  int MaxDimension = G0.nv()-1;
  SplitGraph G(G0,NumberOfClasses,MaxDimension,usedDistance);
  Tprintf("#Classes:%d",G.NumberOfClasses);
 if(debug()) 
      {LogPrintf("TSPLIT");
      LogPrintf("\n#Vertices:%d",G.nv());
      LogPrintf("\n#Edges:%d",G.ne());
      LogPrintf("\n#Classes:%d",G.NumberOfClasses);
      }
  if(!G.ok)
      {Tprintf("DIAG ERROR (Complete Graph?)");
      return -1;
      }
  if(G.Segment())
      {if(debug())LogPrintf("\nERROR NETCUT");
      return -2;
      }

  // Coloring vertices and edges
  tvertex v;
  short col = 0;
  ForAllVertices(v,G)
    {col = (short)G.ClassNumber[v()];// -1;
    G.vcolor[v] = (col + Yellow-2) %16 + 1;
      }
  tedge e;
  ForAllEdges(e,G)
      {if(G.ClassNumber[G.vin[e]()] != G.ClassNumber[G.vin[-e]()])
          {G.ewidth[e] = 2; G.ecolor[e] = Red;}
      else
          {G.ewidth[e] = 1; G.ecolor[e] = Black;}
      }
  if(debug())LogPrintf("\nFin de NETCUT");
  for(i = 1;i <= G.NumberOfClasses;i++)
      {col =  (i + Yellow-2) %16 + 1;
      Tprintf("Class (%d):%d (%s)",i,G.NumberElementsInClass[i] ,ColorNames[col]);
      }
  return 0;
  }



/******************************************************************************************/
//! Computes a distance among the vertices of the graph and embed it in \f$\mathbb{R}^{n-1}\f$
void EmbedRnGraph::init(int usedDistance)
  {EigenValues.resize(1,nv());
  // Allocation du tableau Distances
  Distances = new double*[nv() + 1];
  int i;
  for(i = 1; i <= nv(); i++)	
      Distances[i] = new double[nv() + 1];

  // Allocation du tableau Coords
  Coords = new double*[nv() + 1];
  for(i = 1; i <= nv(); i++)	
      Coords[i] = new double[nv() + 1];

  if(debug()) 
      {if(usedDistance == 0)
	  DebugPrintf("Distance: Czekanovski-Dice"); 
      else if(usedDistance == 1)
	  DebugPrintf("Distance: Bisection"); 
      else if(usedDistance == 2)
	  DebugPrintf("Distance: Adjacence"); 
      else if(usedDistance == 3)
	  DebugPrintf("Distance: Adjacence M"); 
      else if(usedDistance == 4) 
	  DebugPrintf("Distance: Laplacian"); 
      else if(usedDistance == 5) 
	  DebugPrintf("Distance: Q-distance");
      else if(usedDistance == 6) 
	  DebugPrintf("Distance: Orient");
      else if(usedDistance == 7) 
	  DebugPrintf("Distance: R2");
      else
	  DebugPrintf("Distance: Czekanovski-Dice!!!!"); 
      }
  bool project = true;
  if(usedDistance == 0)             // Czekanovski-Dice
      ComputeCzekanovskiDistances();            
  else if(usedDistance == 1)        // Bisection
      ComputeBisectDistances();
  else if(usedDistance == 2)        // Adjacence
      ComputeAdjacenceDistances();
  else if(usedDistance == 3)        // Adjacence M
      ComputeAdjacenceMDistances();
  else if(usedDistance == 4)        // Laplacian 
      {ComputeLaplacianDistances();project = false;}
  else if(usedDistance == 5)        // Q-distance 
      ComputeQDistances();
  else if(usedDistance == 6)        // Orient 
      ComputeOrientDistances();
  else if(usedDistance == 7)        // R2 
      ComputeR2Distances();
  else
      ComputeCzekanovskiDistances(); //Neigbour

  if(diag(Coords,nv(),Distances,EigenValues,project))ok = false;
  }
//! release the memory
void EmbedRnGraph::release()
  {// Desallocation du tableau Distances
  int i;	
  for(i = 1; i <= nv() ; i++)
      delete [] Distances[i];
  delete [] Distances;

  // Desallocation du tableau Coords
  for(i = 1; i <= nv() ; i++)
      delete [] Coords[i];
  delete [] Coords;
  }

int EmbedRnGraph::ComputeOrientDistances()
  {// Compute indegrees and outdegrees
  indegree.resize(1,nv());  outdegree.resize(1,nv()); 
  for(tvertex v = 1 ;v <= nv();v++)
      {indegree[v()] = InDegree(v);  outdegree[v()] = OutDegree(v);}

  // Compute inList and outList
  ComputeInOutList();
  // Fill distances
  int i;
  for(i = 1;i <= nv();i++)
      Distances[i][i] = .0;

  int vertex1,vertex2;
  double d;
  for(vertex1 = 2; vertex1 <= nv(); vertex1 ++)
      for(vertex2 = 1; vertex2 < vertex1; vertex2++)
          {d = .5*(ComputeInDist(vertex1,vertex2) + ComputeOutDist(vertex1,vertex2));
          Distances[vertex1][vertex2] = Distances[vertex2][vertex1] = d;
	  }

  // Release inList and outList
  for(i = 1; i<= nv(); i++)
      {delete [] inList[i];delete [] outList[i];}
  delete [] inList;  delete [] outList;
  return 0;
  }
int EmbedRnGraph::ComputeInOutList()
  {// Allocation du tableau d'adjacence circulaire des brins (OrderCir)
  // inBrin[v] incoming brin at v
  svector<tbrin>OrderCir(-ne(),ne());              OrderCir.SetName("OrderCir");
  svector<tbrin> inBrin(1,nv());  inBrin.clear();  inBrin.SetName("inBrin");
  svector<tbrin> outBrin(1,nv()); outBrin.clear(); outBrin.SetName("outBrin");
  // Allocate inList and outList
  inList  = new int*[nv() + 1];
  int i;
  for(i = 1;i <= nv();i++)
      inList[i] = new int[indegree[i] + 2];

  outList  = new int*[nv() + 1];
  for(i = 1;i <= nv();i++)
      outList[i] = new int[outdegree[i] + 2];

  // Compute OrderCir
  tvertex v,w;
  tbrin brin,pbrin;
  for(tedge e = 1; e <= ne(); e++)
      {v = vin[e]; w  = vin[-e()];
      // Autour de v: outgoing
      pbrin = 0;
      brin = outBrin[v];
      while((brin != 0) && (vin[-brin] < w))
          {pbrin = brin; brin = OrderCir[brin];}
      if(pbrin != 0)
          {OrderCir[pbrin] = e(); OrderCir[e] = brin;}
      else
          {outBrin[v] = e(); OrderCir[e] = brin;}
      // Autour de w: incoming
      pbrin = 0;
      brin = inBrin[w];
      while((brin != 0) && (vin[-brin] < v))
	  {pbrin = brin; brin = OrderCir[brin];}
      if(pbrin != 0)
	  {OrderCir[pbrin] = -e(); OrderCir[-e] = brin;}
      else
	  {inBrin[w] = -e(); OrderCir[-e] = brin;}
      }
  // Compute inList and outList
  int pos;
  for(v = 1;v <= nv();v++)
      {pos = 1;
      for(brin = inBrin[v];(brin!=0) && (vin[-brin] < v);brin=OrderCir[brin])
          {inList[v()][pos] = vin[-brin](); pos++;}
      inList[v()][pos] = v();      pos++;
      for(; brin!=0; brin=OrderCir[brin])
          {inList[v()][pos] = vin[-brin](); pos++;	  }
      }

  for(v = 1;v <= nv();v++)
      {pos = 1;
      for(brin = outBrin[v];(brin!=0) && (vin[-brin] < v);brin=OrderCir[brin])
          {outList[v()][pos] = vin[-brin](); pos++;}
      outList[v()][pos] = v();      pos++;
      for(; brin!=0; brin=OrderCir[brin])
          {outList[v()][pos] = vin[-brin](); pos++;}
      }
  return 0;
  }
double EmbedRnGraph::ComputeInDist(int vertex1,int vertex2)
  {int position1 = 1, position2 = 1;
  int incoming = 0;
  int v1,v2;
  while(position1 <= indegree[vertex1] + 1 && position2 <= indegree[vertex2] + 1)
      {v1 = inList[vertex1][position1]; v2 = inList[vertex2][position2];
      if(v1 < v2)
          ++position1;
      else if(v1 > v2)
          ++position2;
      else 
          {++incoming;
          ++position1; ++position2;
          }
      }
  int d = indegree[vertex1] +  indegree[vertex2] + 2;
  return (double)(d - 2.*incoming )/d;
  }
double EmbedRnGraph::ComputeOutDist(int vertex1,int vertex2)
  {int position1 = 1, position2 = 1;
  int outgoing = 0;
  int v1,v2;
  while(position1 <= outdegree[vertex1] + 1 && position2 <= outdegree[vertex2] + 1)
      {v1 = outList[vertex1][position1]; v2 = outList[vertex2][position2];
      if(v1 < v2)
          ++position1;
      else if(v1 > v2)
          ++position2;
      else 
          {++outgoing;
          ++position1; ++position2;
          }
      }
  int d = outdegree[vertex1] +  outdegree[vertex2] + 2;
  return (double)(d - 2.*outgoing )/d;
  }


int EmbedRnGraph::ComputeAdjacenceMatrix()
  {// Allocation du tableau d'adjacence circulaire des brins (OrderCir)
  svector<tbrin>OrderCir(-ne(),ne());     OrderCir.SetName("OrderCir");

  // Allocation du tableau d'incidence sommet / premier brin (VertexBrin)
  svector<tbrin> VertexBrin(1,nv()); VertexBrin.clear(); VertexBrin.SetName("VertexBrin");

  // Tant que les aretes vont vers un sommet plus petit
  // Insertion de edge entre pbrin et brin
  tedge e;
  tbrin brin,pbrin;
  tvertex v, w;

  // Mise � jour des listes tri�es (OrderCir)
  for(e = 1; e <= ne(); e++)
      {v = vin[e]; w  = vin[-e()];
      // Autour de v
      pbrin = 0;
      brin = VertexBrin[v];
      while((brin != 0) && (vin[-brin] < w))
          {pbrin = brin; brin = OrderCir[brin];}
      if(pbrin != 0)
          {OrderCir[pbrin] = e(); OrderCir[e] = brin;}
      else
          {VertexBrin[v] = e(); OrderCir[e] = brin;}
      // Autour de w
      pbrin = 0;
      brin = VertexBrin[w];
      while((brin != 0) && (vin[-brin] < v))
	  {pbrin = brin; brin = OrderCir[brin];}
      if(pbrin != 0)
	  {OrderCir[pbrin] = -e(); OrderCir[-e] = brin;}
      else
	  {VertexBrin[w] = -e(); OrderCir[-e] = brin;}
      }

  // Calcul du tableau d'adjacence sommet/sommet
  for(v = 1;v <= nv();v++)
      {w = 1;
      for(brin = VertexBrin[v];(brin!=0) && (vin[-brin] < v);brin=OrderCir[brin])
          {vvadj[v()][w()] = vin[-brin](); w++;
	  }
      vvadj[v()][w()] = v();      w++;
      for(; brin!=0; brin=OrderCir[brin])
          {vvadj[v()][w()] = vin[-brin](); w++;
	  }
      }
  return 0;
  }
double EmbedRnGraph::ComputeCzekanovskiDistance(int vertex1,int vertex2)
  {int position1 = 1, position2 = 1;
  int NCommonVertices = 0;
  int v1,v2;
  bool adjacent = false;
  while(position1 <= degree[vertex1] + 1 && position2 <= degree[vertex2] + 1)
      {v1 = vvadj[vertex1][position1]; v2 = vvadj[vertex2][position2];
      if(v1 < v2)
          ++position1;
      else if(v1 > v2)
          ++position2;
      else 
          {++NCommonVertices;
          ++position1; ++position2;
	  if(v1 == vertex1)adjacent = true;
          }
      }
  int SymDiff = degree[vertex1] + degree[vertex2] + 2 -2*NCommonVertices;
  double d = (double)SymDiff/(double)(degree[vertex1] + degree[vertex2] + 2);
  return d;
  }
int EmbedRnGraph::ComputeCzekanovskiDistances()
  {int i;
  // distance entre vertex1 et ses voisins inferieurs => triangle
  // now fill all distances to be safe

  // Compute degrees
  degree.resize(1,nv()); 
  for(tvertex v = 1 ;v <= nv();v++)
      degree[v()] = Degree(v);

  // Allocation du tableau d'adjacence vvadj
  vvadj  = new int*[nv() + 1];
  for(i = 1;i <= nv();i++)
      vvadj[i] = new int[degree[i] + 2];
  // Fill vvadj
  ComputeAdjacenceMatrix();

  for(i = 1;i <= nv();i++)
      Distances[i][i] = .0;
  int vertex1;
  double d;
  for(vertex1 = 2; vertex1 <= nv(); vertex1 ++)
      for(i = 1; i < vertex1; i++)
          {d = ComputeCzekanovskiDistance(vertex1,i);
          Distances[vertex1][i] = Distances[i][vertex1] = d;
	  }

  //Release vvadj
  for(i = 1; i<= nv(); i++)
      delete [] vvadj[i];
  delete [] vvadj;
  return 0;
  }

int EmbedRnGraph::ComputeQDistances()
  {// Compute degrees
  degree.resize(1,nv()); 
  for(tvertex v = 1 ;v <= nv();v++)
      degree[v()] = Degree(v);
  int i,j;	
  for(i = 1;i <= nv();i++)
      for(j = 1;j <= nv();j++)
	  Distances[i][j] = 1.;
  double d;
  for(tedge e = 1;e <= ne();e++)
      {tvertex v = vin[e];
      tvertex w  = vin[-e];
      d = 1.- 1./sqrt((double)(degree[v] * degree[w]));
      Distances[v()][w()] = Distances[w()][v()] = d;
      }
  for(i = 1;i <= nv();i++)
      Distances[i][i] = .0;
  return 0;
  }

int EmbedRnGraph::ComputeBisectDistances()
  {// Compute degrees
  degree.resize(1,nv()); 
  
  for(tvertex v = 1 ;v <= nv();v++)
      degree[v()] = Degree(v);
  int i,j;	
  for(i = 1;i <= nv();i++)
      for(j = 1;j <= nv();j++)
	  Distances[i][j] = 1.;
  double d;
  for(tedge e = 1;e <= ne();e++)
      {tvertex v = vin[e];
      tvertex w  = vin[-e];
      d = (degree[v] + degree[w])/(double)(degree[v] + degree[w] + 2);
      Distances[v()][w()] = Distances[w()][v()] = d;
      }
  for(i = 1;i <= nv();i++)
      Distances[i][i] = .0;
  return 0;
  }

int EmbedRnGraph::ComputeAdjacenceDistances()
  {int i,j;
  for(i = 1;i <= nv();i++)
      for(j = 1;j <= nv();j++)
	  Distances[i][j] = 1.;
  for(tedge e = 1;e <= ne();e++)
      Distances[vin[e]()][vin[-e]()] = Distances[vin[-e]()][vin[e]()] = .0;
  for(i = 1;i <= nv();i++)
      Distances[i][i] = .0;
  return 0;
  }

int EmbedRnGraph::ComputeAdjacenceMDistances()
  {int i,j;
  double a = 1. - 2./nv();
  for(i = 1;i <= nv();i++)
      for(j = 1;j <= nv();j++)
	  Distances[i][j] = 1.;
  for(tedge e = 1;e <= ne();e++)
      Distances[vin[e]()][vin[-e]()] = Distances[vin[-e]()][vin[e]()] = a;
  for(i = 1;i <= nv();i++)
      Distances[i][i] = .0;
  return 0;
  }

int EmbedRnGraph::ComputeLaplacianDistances()
// Compute the laplacian fo the complement
  {int i,j;
  for(i = 1;i <= nv();i++)
      for(j = 1;j <= nv();j++)
	  Distances[i][j] = -1.;
  for(tedge e = 1;e <= ne();e++)
      Distances[vin[e]()][vin[-e]()] = Distances[vin[-e]()][vin[e]()] = .0;
  for(tvertex v = 1;v <= nv();v++)
      Distances[v()][v()] = (double)(nv()-Degree(v));
  return 0;
  }

int EmbedRnGraph::ComputeR2Distances()
  {int i,j;
  Prop<Tpoint> vcoord(Set(tvertex()),PROP_COORD);
  for(i = 1;i <= nv();i++)
      Distances[i][i] = .0;
  for(i = 1;i <= nv();i++)
      for(j = 1;j < i;j++)
	  {Distances[i][j] = (vcoord[i].x() - vcoord[j].x())*(vcoord[i].x() - vcoord[j].x())
	  + (vcoord[i].y() - vcoord[j].y())*(vcoord[i].y() - vcoord[j].y());
	  Distances[j][i] = Distances[i][j];
	  }
  return 0;
  }




/**************************************************************************/	
void SplitGraph::init()
  {ClassNumber.resize(1,nv());  ClassNumber.SetName("ClassNumber");
  NumberOfClasses = bound(NumberOfClasses,2,nv());
  MaxDimension = Min(nv() -1,NumberOfClasses);
  MinDimension = Max(NumberOfClasses -2,1);
  NumberElementsInClass.resize(1,NumberOfClasses);
  NumberElementsInClass.SetName("NumberElementsInClass");
  }
void SplitGraph::ComputeProjectDistance(int dimension)
  {double aux;
  for(int dim = 0; dim < dimension; dim++)
      for(int i = 1;i < nv(); i++)
          for(int j = 0;j < i; j++)
              {aux = Coords[i+1][dim+1] - Coords[j+1][dim+1];
              l->ProjectDist(i,j) += aux * aux;
              }
  }
void SplitGraph::SearchFarVertices(int dimension)
  {// Compute distances in R(dimension)
  int  extrem0, extrem1;

  // rechercher extrem0 et extrem1 en dimension 1
  double min0 = Coords[1][1];
  double max0 = min0;
  extrem0 = extrem1 = 0;
  for(int i = 1;i < nv(); i++)
      {if(Coords[i][1] < min0)
	  {min0 = Coords[i+1][1];extrem0 = i;}
      else if(Coords[i+1][1] > max0)
	  {max0 = Coords[i+1][1];extrem1 = i;}
      }
  extrem0 += 1;extrem1 += 1;
  ComputeProjectDistance(dimension);
  AffectExtrems(extrem0, extrem1);
  if(debug())DebugPrintf("ext:%d %d",extrem0, extrem1);
  }

void SplitGraph::AffectExtrems(int extrem0,int extrem1)
  {// Construct 2 classes form extrem0,extrem1
  int min = extrem0-1;
  int max = extrem1-1;
  if (extrem0 > extrem1) {min = extrem1-1; max = extrem0-1;} //pourquoi ?

  l->Part.Cardinal[0] = l->Part.Cardinal[1] = 0;
  for(int i = 0; i < nv(); i++)
    {int c;
    if (i==min) c=0;
    else if (i==max) c=1;
    else c = (l->ProjectDist(min,i) > l->ProjectDist(max,i)) ? 1 : 0;
    l->Part.affect(i,c);
    }
  CurrentNumberOfClasses = 2;
  }
void SplitGraph::NewClass(int dimension,int worst) 
  {//worst becomes the barycenter of a new class 
  for(int d = 0; d < dimension;d++)
    l->BaryCoord(CurrentNumberOfClasses,d) = Coords[worst][d+1];
  CurrentNumberOfClasses += 1;
  }

void SplitGraph::SearchWorst(int dimension,int &worst) 
  {double max_dist,dist,x;
  max_dist = .0;
  for(int i = 0; i < nv(); i++)
    {for(int j = 0;j < CurrentNumberOfClasses; j++)
      {dist = 0;
      for(int d = 0; d < dimension; d++)
	{x = Coords[i+1][d+1] - l->BaryCoord(j,d);
	dist += x * x;
	}
      if(dist > max_dist){max_dist = dist; worst = i;}
      }
    }
  worst += 1;
  }
void SplitGraph::BuildClasses(int dimension, double& inertie,int& worst)
  {// construct classes from the barycenters dans R(dim)
  // worst est le sommet qui n'est proches d'aucun barycentre in R(n-1) ? in R(dim) 
  int i, j, d;
  double xmax,xmin,difference;
  double inter;

  l->Part.Cardinal.clear();
  inertie = .0;
  xmax = .0;
  for(i = 0; i < nv(); i++)
    {xmin=DBL_MAX;
    int c = 0;
    for(j = 0;j < CurrentNumberOfClasses; j++)
      {difference = 0;
      for(d = 0; d < dimension; d++)
	{inter = Coords[i+1][d+1] - l->BaryCoord(j,d);
	difference += inter * inter;
	}
      // difference = distance au barycentre
      if (difference < xmin)
	{c = j; xmin = difference; }
      }
    inertie += xmin;
    l->Part.affect(i,c); //affectation du sommet i � la classe c
    
    //definition  de worst, sommet le plus eloigne du barycentre de sa classe
    if (xmin > xmax)
      {worst = i+1; xmax = xmin; }
    }
  
  inertie /= nv();
  }
void SplitGraph::Optimize(int dimension,int& worst,double& inertie)
  {double criterion1, criterion2 = .0;
  do
      {criterion1 = criterion2;
      ComputeBarycenters(dimension);
      BuildClasses(dimension, inertie, worst);
      criterion2 = inertie;
      }while (Abs(criterion1 - criterion2) > SEUIL);
  }

void SplitGraph::ComputeBarycenters(int dimension)
  {int i,d;
   for(i = 0; i < CurrentNumberOfClasses; i++)
    for(d = 0; d < dimension; d++) 
      l->BaryCoord(i,d) = 0;
  
  for(i = 0; i < nv(); i++)
    for(d = 0; d < dimension;d++) 
      l->BaryCoord(l->Part.Class[i],d) += Coords[i+1][d+1];

  int ii = 0;
  for(i = 0; i < CurrentNumberOfClasses; i++)
    {if(l->Part.Cardinal[i])
      {for(d = 0; d < dimension; d++)
	l->BaryCoord(i,d) /= l->Part.Cardinal[ii];
      ++ii;
      }
    else // cas ou une classe disparait a l' optimisation 
	CurrentNumberOfClasses --; //problem ???
    } 
  }

double SplitGraph::TotalInertia(double& ClassVarianceNumber)
  {//card_max: nombre max d'elements d'une classe
  int card_max = l->Part.Cardinal[0];
  int i;
  for(i = 1;i < NumberOfClasses;i++)
      card_max = Max(card_max,l->Part.Cardinal[i]);

  // cluster[i*cardmax] -> cluster[i*card_max +cardmax] : elements de la classe i
  // clusterindex =? nombre d'elements places dans cluster
  svector<int> cluster(0,card_max * NumberOfClasses);  cluster.clear();
  svector<int> cluster_index(0,NumberOfClasses);  cluster_index.clear();
  int index;
  // l->Part.Class[vertex-1] = classe d'un sommet
  for(i = 0; i < nv(); i++)
      {index = cluster_index[l->Part.Class[i]];
      cluster[l->Part.Class[i] * card_max + index] = i;
      cluster_index[l->Part.Class[i]] += 1;
      }

  // calcul de l' inertie de chaque classe cluster
  double inert_glob = 0.0;
  double inert;
  int il,j;
  int ii; //first index of the class il
  for(il = 0; il < NumberOfClasses;il++)
      {inert = 0.0;
      ii = il * card_max; 
      for(i = 0; i < l->Part.Cardinal[il]; i++)
          for(j = 0; j < i; j++) 
              inert += Distances[1+cluster[ii+i]] [1+cluster[ii+j]];
      if(l->Part.Cardinal[il]) inert /= l->Part.Cardinal[il];
      inert_glob += inert;
      }

  inert_glob /= nv();

  // Calcul du nombre moyen d'elements d'une classe
  double ClassMeanNumber = .0;
  for(il = 0; il < NumberOfClasses;il++)
      ClassMeanNumber += l->Part.Cardinal[il];
  ClassMeanNumber /= NumberOfClasses;
  // Calcul de la variance du nombre d'elements
  ClassVarianceNumber = .0;
  for(il = 0; il < NumberOfClasses;il++)
      ClassVarianceNumber += (l->Part.Cardinal[il] - ClassMeanNumber)
      *(l->Part.Cardinal[il] - ClassMeanNumber);
  ClassVarianceNumber /= NumberOfClasses;

  
  if(ClassVarianceNumber > 1.E-200)
      ClassVarianceNumber = sqrt(ClassVarianceNumber);
  return inert_glob;
  }

int SplitGraph::Segment()
  {int  dimension, worst,dim_opt;
  double inertie, inert_glob;
  double critere_glob,critere_opt = DBL_MAX;
   MinDimension = Max(1,NumberOfClasses-2);
   MaxDimension = NumberOfClasses;
   l = new Locals(nv(),Max(MaxDimension,NumberOfClasses), NumberOfClasses);

  if(debug())
      {LogPrintf("\nRequested Number of Classes:%d):",NumberOfClasses);
      DebugPrintf("\nOptimisations de %d � %d :",MinDimension,MaxDimension);
      }

  double ClassVarianceNumber = .0;
  double ClassVarianceNumber_opt = .0;
  dim_opt = 0;

//   dimension = 1;
//   SearchFarVertices(dimension);
//   Optimize(dimension,worst,inertie);
//   while(CurrentNumberOfClasses < NumberOfClasses)
//       {SearchWorst(++dimension,worst); 
//       ComputeBarycenters(dimension);
//       NewClass(dimension,worst);
//       BuildClasses(dimension,inertie,worst);
//       Optimize(dimension, worst, inertie);
//       }

//   for(dimension = MinDimension; dimension <= MaxDimension; dimension++)
//        {Optimize(dimension, worst, inertie);
//        inert_glob = TotalInertia(ClassVarianceNumber);
//        critere_glob = inert_glob + ClassVarianceNumber/nv();
//        if(debug())DebugPrintf("dimension:%d \ninertie:%f var:%f"
//  			     ,dimension,inert_glob,ClassVarianceNumber);
//       if(critere_glob < critere_opt)
//           {critere_opt = critere_glob;
// 	  dim_opt = dimension;
//           ClassVarianceNumber_opt = ClassVarianceNumber;
// 	  for(int i = 1;i <= nv();i++)
// 	      ClassNumber[i] = l->Part.Class[i-1]+1;
// 	  for(int i = 1;i <= NumberOfClasses;i++)
// 	      NumberElementsInClass[i] = l->Part.Cardinal[i-1];
//           }
//        }

  for(dimension = MinDimension; dimension <= MaxDimension; dimension++)
      {SearchFarVertices(dimension);
      // Create more classes
      while(CurrentNumberOfClasses < NumberOfClasses)
          {Optimize(dimension,worst,inertie);
          NewClass(dimension,worst);
          BuildClasses(dimension,inertie,worst);
          }

      Optimize(dimension, worst, inertie);
      inert_glob = TotalInertia(ClassVarianceNumber);
      //printf("dimension:%d inertie:%f var:%f\n",dimension,inert_glob,ClassVarianceNumber);
      if(debug())DebugPrintf("dimension:%d \ninertie:%f var:%f"
			     ,dimension,inert_glob,ClassVarianceNumber);
      critere_glob = inert_glob + ClassVarianceNumber/nv();

      if(critere_glob < critere_opt)
          {critere_opt = critere_glob;
	  dim_opt = dimension;
          ClassVarianceNumber_opt = ClassVarianceNumber;
      int i;
	  for(i = 1;i <= nv();i++)
	      ClassNumber[i] = l->Part.Class[i-1]+1;
	  for(i = 1;i <= NumberOfClasses;i++)
	      NumberElementsInClass[i] = l->Part.Cardinal[i-1];
          }
      }



  Tprintf("Opt Dim.:%d (%3.3f)",dim_opt,ClassVarianceNumber_opt);
  if(debug())
      {LogPrintf("\nOptimal Dimension : %d ", dim_opt);
      LogPrintf("\nClasses");
	  int i;
      for(i = 1;i <= NumberOfClasses;i++)
          LogPrintf("\n Class (%d):%d",i,NumberElementsInClass[i]);
      LogPrintf("\n Final Affectations");
      for(i = 1;i <= nv();i++)
          LogPrintf("\n vertex:%d class:%d",i,ClassNumber[i]);
     }

  delete l;
  return 0;
  }

