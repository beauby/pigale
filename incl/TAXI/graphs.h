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

#ifndef GRAPHS_H
#define GRAPHS_H

#include <TAXI/Tbase.h>
#include <TAXI/graph.h>
#include <TAXI/Tpoint.h>


struct _Bicon;
class Graph : public GraphAccess
{
 public :
     void keep() { Set(tbrin()).Keep(PROP_VIN);}
     void keepr()
        {keep();
        me().keepr();
        }
     void keeponly()
        {
        ClearKeep();
        keepr();
        }
 public :
    Prop<tvertex> vin;
    
    Graph(GraphContainer &G) : GraphAccess(G), vin(G.PB(),PROP_VIN)
        {vin[0]=0; keep();}
    Graph(GraphAccess &G) : GraphAccess(G), vin(G.PB(),PROP_VIN)
        {vin[0]=0; keep();}
    ~Graph() {}
    GraphAccess & me() {return *this;}
    const GraphAccess & me() const {return *this;}
    void StrictReset() {keeponly(); reset();}
    int GDFS(const svector<tbrin> &cir, svector<tvertex>&nvin,
             svector<tbrin> &tb, svector<int> &dfsnum);
    int GDFS(const svector<tbrin> &cir, svector<tvertex> &nvin)
        {int n = nv();
        svector<tbrin> tb(0,n);         tb.SetName("GDFS:tb");
        svector<int> dfsnum(0,n);       dfsnum.SetName("GDFS:dfsnum");
        return GDFS(cir,nvin,tb,dfsnum);
        }
    int GDFSRenum(const svector<tbrin> &cir, svector<tvertex> &nvin);
    void PrepDFS(svector<tbrin> &cir,tbrin b0=1);
    bool CheckBiconnected();
    int TestPlanar();
    int Planarity(tbrin b0 = 1);
};

class TopologicalGraph : public Graph
{
public :
  int ConstructedCir;
  int ConstructedPbrin;
  Prop<tbrin> cir;
  Prop<tbrin> acir;
  Prop<tbrin> pbrin;
  Prop1<tbrin> extbrin;
  Prop1<int>  planarMap;
  int genus;

  void init();
  void keep()
      { 
      Set(tbrin()).Keep(PROP_CIR);
      Set(tbrin()).Keep(PROP_ACIR);
      Set(tvertex()).Keep(PROP_PBRIN);
      Set().Keep(PROP_EXTBRIN);
      Set().Keep(PROP_PLANARMAP);
      if (Set().exist(PROP_HYPERGRAPH))
          { Set().Keep(PROP_BIPARTITE);
          Set().Keep(PROP_HYPERGRAPH);
          Set().Keep(PROP_NV);
          Set().Keep(PROP_NE);
          Set(tvertex()).Keep(PROP_HYPEREDGE);
          }
      }
    void keepr()
        { keep();
        me().keepr();
        }
    void keeponly()
        {
        ClearKeep();
        keepr();
        }
    protected :
    void PrivateReverseEdge(const tedge &e);
public :
    TopologicalGraph(Graph &G,int) : Graph(G),
        cir(G.PB(),PROP_CIR),
        acir(G.PB(),PROP_ACIR),pbrin(G.PV(),PROP_PBRIN),
        extbrin(G.Set(),PROP_EXTBRIN,1),
        planarMap(G.Set(),PROP_PLANARMAP,0),
        genus(-1)
        {keep();}
    TopologicalGraph(GraphContainer &G) : 
        Graph(G),
        ConstructedCir(!G.PB().exist(PROP_CIR)),
        ConstructedPbrin(!G.PV().exist(PROP_PBRIN)),
        cir(G.PB(),PROP_CIR),
        acir(G.PB(),PROP_ACIR),pbrin(G.PV(),PROP_PBRIN),
        extbrin(G.Set(),PROP_EXTBRIN,1),
        planarMap(G.Set(),PROP_PLANARMAP,0),
        genus(-1)
        {if (ConstructedCir || ConstructedPbrin)
            init();
        keep();
        }
    TopologicalGraph(GraphAccess &G) :
        Graph(G),  
        ConstructedCir(!G.PB().exist(PROP_CIR)),
        ConstructedPbrin(!G.PV().exist(PROP_PBRIN)),
        cir(G.PB(),PROP_CIR),
        acir(G.PB(),PROP_ACIR),pbrin(G.PV(),PROP_PBRIN),
        extbrin(G.Set(),PROP_EXTBRIN,1),
        planarMap(G.Set(),PROP_PLANARMAP,0),
        genus(-1)
        {if (ConstructedCir||ConstructedPbrin)
            init();
        keep();
        }
     ~TopologicalGraph() {}
     void StrictReset() {keeponly(); reset();}
     Graph & me() {return *this;}
     const Graph & me() const {return *this;}
     tbrin FirstBrin(const tvertex &v) const {return pbrin[v];}
     tedge FirstEdge(const tvertex &v) const {return FirstBrin(v).GetEdge();}
     tbrin NextCirBrin(const tbrin &b) const {return cir[b];}
     tbrin PrevCirBrin(const tbrin &b) const {return acir[b];}
     tbrin NextBrin(tvertex v, tbrin b) const {b=cir[b]; return (tbrin)((b()==pbrin[v]())?0:(b()));}
     tbrin PrevBrin(tvertex v, tbrin b) const {b=acir[b];return (tbrin)((b()==pbrin[v]())?0:(b()));}
     tedge NextCirEdge(tvertex v, tedge e) const {return ((vin[e]()==v())?cir[e].GetEdge():cir[-e].GetEdge());}
     tedge PrevCirEdge(const tvertex &v, const tedge &e) const {return ((vin[e]()==v())?acir[e].GetEdge():acir[-e].GetEdge());}
     tedge NextEdge(const tvertex &v, const tedge &e) const
        {tbrin b=e(); if(vin[e]!=v)b=-b; return ((cir[b]==pbrin[v])?(tedge)0:cir[b].GetEdge());}
     tedge PrevEdge(const tvertex &v, const tedge &e) const
        {tbrin b=e(); if(vin[e]!=v)b=-b; return ((cir[b]==pbrin[v])?(tedge)0:acir[b].GetEdge());}
    Graph * SmallDup() const;
    tedge NewEdge(const tbrin &ref1,const  tbrin &ref2); // insertion apres ref1 et avant ref2
    tedge NewEdge(const tvertex &vv1,const tvertex &vv2,tedge e0=0); // insertion "quelconque"
    void DeleteEdge(const tedge &e);
    void MoveBrin(const tbrin &b, const tvertex &to);
    void MoveBrinToFirst(const tbrin &b);
    tvertex ContractEdge(const tedge &e);
    tvertex BissectEdge(const tedge &e);
    void ReverseEdge(const tedge &e);
    void SwitchOrientations();
    void FixOrientation();
    void RestoreOrientation();
    tvertex NewVertex();
    void DeleteVertex(const tvertex &v);
    int Degree(const tvertex &v);
    int InDegree(const tvertex &v);
    int OutDegree(const tvertex &v);
    int RemoveLoops();
    bool CheckNoLoops();
    int RemoveIsolatedVertices();
    int ComputeGenus();
    int NumFaces();
    int Genus(){if(genus < 0)ComputeGenus();return genus;}
    int FaceNumberIsthmus(const tbrin &b0);
    int FaceWalkLength(const tbrin &b0);
    int FaceLength(const tbrin &b0)
        {return FaceWalkLength(b0) - FaceNumberIsthmus(b0);}
    int LongestFaceWalk(tbrin& b,int & len);
    int LongestFace(tbrin& b,int & len);
    bool DebugCir();
    int NumberOfConnectedComponents()
        {svector<int> comp(1,nv()); return BFS(comp);}
    int DFS(svector<tvertex> &nvin,svector<tbrin> &tb, svector<int> &dfsnum,tbrin b0 = tbrin(1));
    int DFS(svector<tvertex> &nvin,tbrin b0 = tbrin(1));
//     int DFS(svector<tvertex> &nvin,svector<tbrin> &tb, svector<int>
//             &dfsnum, const tbrin &b0=tbrin(1)) 
//         {cir[0] = b0; acir[0] = acir[b0]; cir[acir[b0]] = 0;
//         const svector<tbrin> &rcir = cir;
//         int ret=me().GDFS(rcir,nvin,tb,dfsnum);
//         cir[0] = 0; cir[acir[0]] = b0; acir[0] = 0;
//         return ret;
//         }
//     int DFS(svector<tvertex> &nvin,const tbrin &b0=tbrin(1))
//         {int n = nv();
//         svector<tbrin> tb(0,n);      tb.SetName("DFS:tb");
//         svector<int> dfsnum(0,n);    dfsnum.SetName("DFS:dfsnum");
//         return DFS(nvin,tb,dfsnum,b0);
//         }
    // in DFS.cpp
    int DFSRenum(svector<tvertex> &nvin, svector<tedge> &ie,tbrin b0=tbrin(1));
    
    // in TopoAlgs.cpp
    int Simplify();
    int ExpandEdges();
    int RemoveIsthmus();
    bool CheckSimple();
    bool CheckRegular(int &d);
    bool MinMaxDegree(int& dmin,int& dmax);  
    bool CheckConnected();
    int ColorConnectedComponents();
    bool CheckBiconnected();
    bool CheckTriconnected();
    bool CheckFourconnected();
    bool CheckNoC3Sep();
    bool CheckSubdivTriconnected();
    bool CheckBipartite(bool Color = false);
    bool CheckHypergraph(tvertex v0, bool v0ise=false);
    bool CheckAcyclic(int &ns,int &nt);
    bool  TopSort(svector<tbrin>&topin,svector<tvertex>&order,bool revert=false);
    bool CheckSerieParallel();
    int BFS(svector<int> &comp);
    int MakeConnected(bool mark_roots=false);
    int MakeConnectedVertex();
    bool FindPlanarMap();
    bool CheckPlanar() // Graph must be connected
       {if(FindPlanarMap()) return true;
       return false;
       }
    void ZigZag(tbrin start);
    int ZigZagTriangulate();
    svector<tbrin> &ComputeFpbrin();
    int Rotate(tedge e);
    int Opt2Biconnect();
    tedge IdentifyEdge(tvertex &v1,tvertex &v2);
    int VertexQuadrangulate();
    int _VertexQuadrangulate(bool First);
    int VertexTriangulate();
    GraphContainer * DualGraph();
    GraphContainer * AngleGraph();
    // in Biconnect.cpp
    int Biconnect();
    int Opt6Biconnect();
    // in NpBiconnect.cpp
    int NpBiconnect(bool withVertices = false);    
    int NpBiconnectVertex();    
    // in STList.cpp
    int BipolarPlan(tbrin FirstBrin);
    int PseudoBipolarPlan(tbrin& st_brin,int &NumberOfSinks);
    // in Schnyder.cpp
    int SchnyderOrient(tbrin FirstBrin);
    int Schnyder(bool schnyderRect,bool schnyderColor,bool schnyderLongestFace,tbrin FirstBrin);
    int SchnyderXYZ(bool schnyderColor,bool schnyderLongestFace
                    ,tbrin FirstBrin,svector<int>&x,svector<int>&y,svector<int>&z);
    int SchnyderV(bool schnyderRect,bool schnyderColor,bool schnyderLongestFace,tbrin FirstBrin);
    // in k-InfOrient.cpp
    int InfOrient();
    // in 3-ConOrientTriang.cpp
    int TriconTriangulate();
    int Tricon3orient();
    // in Planar.cpp
    int MaxPlanar();
    int MaxPlanar(svector<bool> &mark);
    int Planarity(tbrin b0 = 1);
    int NewPlanarity(tbrin b0 = 1);
    int CotreeCritical(int &ret);
    int CotreeCritical(svector<bool> &mark);
    int KCotreeCritical(int &);
    int Kuratowski();
    int Kuratowski(svector<bool> &mark);
    int KKuratowski();
    int TestPlanar();
    int TestNewPlanar();
    int TestPlanar2();
    int TestSinglePassPlanar();
    // in Jacquard.cpp
    GraphContainer * Vis_a_vis_Graph();
    GraphContainer * CrossingEdgesGraph();
protected :
    tbrin NewSwapLabel(const tbrin &b,const tedge &ref)
        {if (b==ne()) return ref();
        if (b==-ne()) return -ref();
        return b;
        }
    void SwapEdge(const tedge &e);
    void SwapVertex(const tvertex &v);
    void UnlinkBrin(const tbrin &b);
    void InsertBrin(const tvertex &v,const tbrin &b);
    // Other functions
    int ComputeSTlist(tbrin st_brin); 
    void ZigZag();     
};

class GeometricGraph : public TopologicalGraph
{

public :
    Prop<Tpoint> vcoord;
    Prop<short> vcolor;
    Prop<int> vlabel;
    Prop<int> ewidth;
    Prop<short> ecolor;
    Prop<int> elabel;
    int maxvlabel;
    int maxelabel;
private :
    void init();
 public:
    void keep()
        { 
        Set(tvertex()).Keep(PROP_COORD);
        Set(tvertex()).Keep(PROP_COLOR);
        Set(tvertex()).Keep(PROP_LABEL);
        Set(tedge()).Keep(PROP_WIDTH);
        Set(tedge()).Keep(PROP_COLOR);
        Set(tedge()).Keep(PROP_LABEL);
        }
    void keepr()
        { keep();
        me().keepr();
        }
    void keeponly()
        {
        ClearKeep();
        keepr();
        }
public :
    GeometricGraph(GraphContainer &G) : 
        TopologicalGraph(G), vcoord(G.PV(),PROP_COORD,Tpoint(0,0)),
        vcolor(G.PV(),PROP_COLOR,5),vlabel(G.PV(),PROP_LABEL,0),
        ewidth(G.PE(),PROP_WIDTH,1), ecolor(G.PE(),PROP_COLOR,1), 
        elabel(G.PE(),PROP_LABEL,0)
        {init();keep();}
    GeometricGraph(GraphAccess &G) : TopologicalGraph(G), vcoord(G.PV(),PROP_COORD),
                                     vcolor(G.PV(),PROP_COLOR,5),vlabel(G.PV(),PROP_LABEL,0),
        ewidth(G.PE(),PROP_WIDTH,1), ecolor(G.PE(),PROP_COLOR,1),
                                     elabel(G.PE(),PROP_LABEL,0)
        {init();keep();}

    ~GeometricGraph() {}
    void StrictReset() {keeponly(); reset();}
    TopologicalGraph & me() {return *this;}
    const TopologicalGraph & me() const {return *this;}
    GraphContainer * GeometricDualGraph();
    GraphContainer * DualGraph();
    GraphContainer * GeometricAngleGraph();
    GraphContainer * AngleGraph();
    
    tedge FindEdge(const Tpoint &p,double node_radius) const;
    tedge FindEdge(const Tpoint &p) const;
    tvertex FindVertex(const Tpoint & p,double node_radius) const;
    tvertex NewVertex(const Tpoint &p)
        {tvertex v=me().NewVertex(); vcoord[v]=p;
        vlabel[v]= ++maxvlabel;
        return v;
        }
    tedge NewEdge(const tbrin &ref1,const  tbrin &ref2)
        // insertion apres ref1 et avant ref2
        {tedge e=me().NewEdge(ref1,ref2);  elabel[e]= ++maxelabel; return e;}
    tedge NewEdge(const tvertex &vv1,const tvertex &vv2)
        // insertion "quelconque"
        {tedge e=me().NewEdge(vv1,vv2);  elabel[e]= ++maxelabel; return e;}
    tvertex BissectEdge(const tedge &e)
        {tvertex v1 = vin[e]; tvertex v2 = vin[-e];
        tvertex v = me().BissectEdge(e);
        vlabel[v]= ++maxvlabel;
        tedge ee = ne();
        elabel[ee]= ++maxelabel;
        ecolor[ee] = ecolor[e];
        ewidth[ee] = ewidth[e];
        vcoord[v].x() = (vcoord[v1].x()+vcoord[v2].x())/2;
        vcoord[v].y() = (vcoord[v1].y()+vcoord[v2].y())/2;
        //vcolor[v] = vcolor[v1];
        return v;
        }
  int ComputeGeometricCir();
  tbrin FindExteriorFace();
  tbrin FindExteriorFace(Tpoint& p);
  double MinMaxCoords(double &xmin,double &xmax, double &ymin,double &ymax);
  int Tutte();
  int ColorExteriorface();
  int Jacquard(int maxgen, double k_angle, double k_mindist, double k_electro, double k_spring, bool k_frontier);

};
// Other Prototypes
int bicon (int n,int m,const svector<tvertex> &nvin, _Bicon &Bicon,svector<tvertex> &low);

#define Forall_adj_brins(b,v,G) for(b=G.FirstBrin(v);b!=0;b=G.NextBrin(v,b))
#define Forall_adj_edges(e,v,G) for(e=G.FirstEdge(v);e!=0;e=G.NextEdge(v,e))
#define Forall_adj_brins_of_G(b,v)   for(b=FirstBrin(v);b!=0;b=NextBrin(v,b))
#define Forall_adj_edges_of_G(e,v)   for(e=FirstEdge(v);e!=0;e=NextEdge(v,e))
#endif
