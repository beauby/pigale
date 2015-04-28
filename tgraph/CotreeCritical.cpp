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



#include <TAXI/Tbase.h>
#include <TAXI/CotreeCritical.h>
#include <TAXI/DFSLow.h>

int DFSGraph::CotreeCritical()
  {int retval;
  bool found;
  {DFSKura2 DK(*this);
  retval = DK.GetRetVal();
  found = DK.KuraFound();
  }
  if (found)
      {MarkFundTree();
      Shrink();
      }
  //else retval=0;

  Set(tedge()).erase(PROP_MARK);
  Set(tvertex()).erase(PROP_LOW);
  Set(tvertex()).erase(PROP_ELOW);
  Set(tvertex()).erase(PROP_TSTATUS);
  Set(tvertex()).erase(PROP_DUALORDER);
  if(!found)
  	{setPigaleError(-1,"CotreeCritical NOT Found");
  	cout<<"CotreeCritical NOT Found retval:"<<retval<<endl;
  	retval=0;
    }
  return retval;
  }

int DFSKura::ShrinkToNonPlanar(bool show)
  {tedge e;
  int result;
  _LrSort LrSort(nv(),ne());
  LrSort.num.clear();
  _FastHist Hist(ne());
  {
  DFSLow DL(*this);
  DL.LralgoSort(LrSort);
  result = DL.Lralgo(LrSort,Hist);
  }
  if (show)
      {
      int i;
      for (i=1; i<nv(); i++)
          { ecolor[ie(i)]=Black;
          ewidth[ie(i)]=1;
          }
      for (i=nv(); i<=ne() ; i++)
          { ewidth[ie(i)]=2;
          if (LrSort.num[i]==0) ecolor[ie(i)]=Grey2;
          else ecolor[ie(i)]=Red;
          }
      }
  isplanar = (result!=0);
  if (isplanar)return 0;
  np = Hist.NP;
  e1 = Hist.LeftTop;
  //Tprintf("Cove : %d (bas : %d)",ie[Hist.Cove](),nvin[Hist.Cove]());
  //Tprintf("Thick : %d",Hist.Thick);

  e2 = Hist.RightTop;

  EgaliteBasse = (nvin[e1]==nvin[e2]) ? true:false;
  if (EgaliteBasse)    
      {if (LrSort.num[e1]>LrSort.num[e2])
          {tedge g=e1; e1=e2; e2=g;}
      }
  else SwapInOrder(e1,e2);
  // shrink _DFS nvin
  tedge newe=nv()-1;
    
  for (e=nv(); e<=ne(); e++)
      {if (LrSort.num[e])
          {newe++;
          if (e==e1) e1=newe;
          else if (e==e2) e2=newe;
          else if (e==np) np=newe;
          nvin[newe]=nvin[e];
          nvin[-newe]=nvin[-e];
          _ib[newe]=_ib[e];
          }
      }
  setsize(tedge(),newe());
  return 1;
  }

void DFSKura2::DFSDualOrder()
  { int DualNumber = nv();
  DualOrder[0] = 0; //convention
  if (nv()==0) return;
  DualOrder[1] = 1;
  if (nv()==1) return;
  tvertex v,w,fath;
        
  for (v = 1; v != nv(); v++)
      {fath = nvin[v]; // Backtrack avant d'aller en v+1
      for (w = v; w != fath; w = father(w))
          {DualOrder[w] = DualNumber--;
          }
      }
  // backtrack from n
  for (w = nv(); w != 1; w = father(w))
      {DualOrder[w] = DualNumber--;
      }
  if(debug() && DualNumber!=1)DebugPrintf("DualNumber");
  assert(DualNumber==1);
  return;
  }

tvertex DFSKura2::infimum(tvertex v, tvertex w)
  {if (v>w) {tvertex x=v; v=w; w=x;}
  while (!SmallerEqual(v,w))
      v = father(v);
  return v;
  }
tvertex DFSKura2::cover(tvertex from, tvertex to)
  {if (from==to) return 0;
  tvertex t=to;
  if(debug() && !SmallerThan(from,t))DebugPrintf("cover");
  assert(SmallerThan(from,t));
  tvertex fath;
  while(1)
      { fath = father(t);
      if (fath == from) break;
      t = fath;
      } 
  return t;
  }

tedge DFSKura2::HighestCove(tvertex v1, tvertex v2) // v1 < v2
  {for (tedge e=ne(); e>=nv(); e--)
      if(SmallerThan(nvin[e],v1) && SmallerEqual(v2,nvin[-e]))return e;
  return 0;
  }

void DFSKura2::ComputeAncestors() 
  {svector<bool> marque(0,nv()); marque.clear(); 
  marque.SetName("marque ancestors");
  tvertex v=t1;
  tvertex vv=0;
  tvertex vv_son=0;
  tedge ee;
  tvertex lv,tv;

  // On marque la chaine d'arbre de 1 a t1
  for (v=t1; v!=0; v=father(v)) marque[v]=true;

  // this is a DFS !
  // ancestor[v] : ancetre sur la chaine 1 -> t1
  // ancestor_son[v] : fils utilse (ou 0)

  for (v=1; v<=nv(); v++)
      { if (marque[v]) {vv = v; vv_son=0;}
      else if (marque[father(v)]) {vv = father(v); vv_son=v;}
      ancestor[v] = vv;
      ancestor_son[v] = vv_son;
      }
  }

void DFSKura2::ComputeDownEdge()
  { 
  down_edge=elow[t1];
  down_vertex=t1;  // sommet d'ou on descend
  down_edge_equal = 0;

  for (tedge e=nv(); e<=ne(); e++)
      { tvertex vto = ancestor[nvin[-e]];
      tvertex vfrom = nvin[e];
      if (SmallerEqual(l1,vfrom)) break;
      if (branch > vto) continue;
      else if (branch == vto) down_edge_equal = e;
      else if (vto < down_vertex)
          { down_vertex = vto;
          down_edge = e;
          }
      }
  }

tedge DFSKura2::FindInterlaced(tedge e, tvertex lv, tvertex tv)
  {
  while ((nvin[e] == lv) || (ancestor[nvin[-e]] <=tv))
      { /*Flash(e,orange);*/ e++; if (e>ne()) return 0;}
  if (nvin[e] >= tv) return 0;
  // on cherche le plus haut point haut pour le point bas
  tvertex v = nvin[e];
  tedge ee = e;
  /*FlashBest(e,violet);*/
  tvertex vv = ancestor[nvin[-e]];
  tvertex tmpv;
  while (++e <= ne() && nvin[e]>=v)
      {if (nvin[e]>v) continue;
      tmpv = ancestor[nvin[-e]];
      if (tmpv > vv)
          { vv = tmpv; ee = e; /*FlashBest(ee,violet);*/}
      }
  return ee;
  }
tedge DFSKura2::FindLastInterlaced(tedge e,tvertex lv,tvertex tv)
  {tvertex vason;
  while (1)
      {if (nvin[e]>=tv) return 0;
      if ((nvin[e] == lv) || (ancestor[nvin[-e]] < tv))
          {/*Flash(e,orange);*/ 
          e++; 
          if (e>ne()) return 0; 
          else continue;
          }
      if (((vason = ancestor_son[nvin[-e]])==0) || (low[vason] >= l1)) 
          {/*Flash(e,cyan);*/ 
          e++; 
          if (e>ne()) return 0; 
          else continue;
          }
      break;
      }
  return e;
  }
void DrawGraph();
void DFSKura2::Compute()
  {Success = true; // optimistic, isn't it ?
  // Mark tree
  mark.clear();

    // Ca commence ici
  Keep(e1,Blue); 
  Keep(e2,Blue); 
  Keep(np,Green); 
    
  // Calcul des sommets importants
  ComputeImportantVertices();
  tedge hc1 = HighestCove(branch,fils1);
  tedge hc2 = HighestCove(branch,fils2);
 /* 
   if(debug())
        {Tprintf("fils1=%d fils2=%d",iv(fils1)(),iv(fils2)());
        Tprintf("elow=%d elow=%d",ie(elow[fils1])(),ie(elow[fils2])());
        Tprintf("hcove=%d hcove=%d", ie(hc1)(),ie(hc2)());
        DebugPrintf("Compute n=%d m=%d",nv(),ne());
        DebugPrintf("fils1=%d fils2=%d",fils1(),fils2());
        DebugPrintf("iv fils1=%d fils2=%d",iv(fils1)(),iv(fils2)());
        DebugPrintf("elow=%d elow=%d",ie(elow[fils1])(),ie(elow[fils2])());
        DebugPrintf("hcove=%d hcove=%d", ie(hc1)(),ie(hc2)());
        DebugPrintf("branch=%d npattach=%d t1=%d",branch(),npattach(),t1());
        DebugPrintf("iv branch=%d",iv(branch)());
        DebugPrintf("iv npattach=%d",iv(npattach)());
        DebugPrintf("iv t1=%d",iv(t1)());
        // coloration de qqes sommets
        vcolor[iv(branch)]=Blue;
        vcolor[iv(npattach)]=Green;
        vcolor[iv(t1)]=Pink;
        DrawGraph();
        Twait("Commencement");
        }
*/    
    // Ici commencent les differents cas

  ComputeAncestors();
  ComputeDownEdge();

  if ((fils1!=0) && (down_edge_equal!=0) && (nvin[down_edge_equal]>nvin[np])
      && (low[fils1]<nvin[down_edge_equal]) && (npattach < branch))
      {Keep(down_edge_equal,Cyan);
      Keep(elow[fils1],Cyan);
      UnKeep(e2,Orange);
      //if(debug())Tprintf("NCritical again versio 2.0.0)");
      RetVal = 1;
      return;
      }
  if ((fils1!=0) && (nvin[down_edge] !=l1) && (nvin[down_edge]>nvin[np])
      && (low[fils1]<nvin[down_edge]) && (npattach < branch))
      {Keep(down_edge,Cyan);
      Keep(elow[fils1],Cyan);
      UnKeep(e2,Orange);
      //if(debug())Tprintf("NCritical again (version 2.0.1)");
      RetVal = 2;
      return;
      }
  if (fils2!=0)
      { if ((npattach < branch) && ! EgaliteBasse)
          { if (low[fils2]<l1)
              {Keep(elow[fils2],Cyan);
              //if(debug()) Tprintf("NCritical again (version 2.0.2)");
              RetVal = 3;
              return;
              }
          }
      if ( (low[fils1]<lp && low[fils2]<lp )
           || ( low[fils1]<l1 && low[fils2]<l1 && npattach < branch ) )
          {Keep(elow[fils1],Cyan);
          Keep(elow[fils2],Cyan);
          //if(debug())Tprintf("NCritical again (version 2.0.3)");
          RetVal = 4;
          return;
          }
      if (fils1!=0) // embranchement vrai
          {if ((hc1!=0) && (nvin[hc1]!=nvin[e1]))
              {if(IsDownEdgeOk(hc1))
                  {Keep(down_edge,Cyan);
                  Keep(hc1,Pink);
                  //if(debug())Tprintf("NCritical again (version 2.0.4)");
                  RetVal = 5;
                  return;
                  }
              else if (down_edge_equal!=0)
                  {if(!EgaliteBasse)
                      {Keep(down_edge_equal,Cyan);
                      Keep(hc1,Pink);
                      //if(debug())Tprintf("NCritical again (version 2.0.5)");
                      RetVal = 6;
                      return;
                      }
                  else if ((hc2!=0) && (nvin[hc2]!=nvin[e2]))
                      {Keep(down_edge_equal,Cyan);
                      Keep(hc1,Violet);
                      Keep(hc2,Violet);
                      //if(debug())Tprintf("NCritical again (version 2.0.6)");
                      RetVal = 7;
                      return;
                      }
                  }
              }
          }
      }
    
  tvertex filsnp = cover(npattach,nvin[-np]);
  //if(debug())Tprintf("filsnp = %d",iv(filsnp)());
  if ((filsnp!=0) && (fils1!=0) && (fils2!=0))
      {tedge hcnp = HighestCove(branch,filsnp);
      //if(debug())Tprintf("hcnp=%d",ie(hcnp)());
      if (hcnp !=0)
          { int mv = Max(low[fils1](),low[fils2]());
          mv = Max(mv,nvin[np]());
          if (mv < Min(nvin[hcnp](),l1()))
              {//if(debug())Tprintf("NCritical again (version 2.0.7)");
              Keep(elow[fils2],Cyan);
              Keep(hcnp,Violet);
              if(nvin[hcnp]<l1)
                  {UnKeep(np,Orange);
                  Keep(elow[fils1],Cyan);
                  RetVal = 8;
                  return;
                  }
              if ((nvin[hcnp]==l1)|| EgaliteBasse) 
                  {Keep(elow[fils1],Cyan);
                  RetVal = 9;
                  }
              else  RetVal = 10;
              return;
              }
          }
      }


  // entrelacement
  //if(debug())Tprintf("On cherche de l'entrelacement vers t1");
  //if(debug())Tprintf("down_edge:%d",ie(down_edge)());
  //Flash(down_edge,Cyan);
 
  tedge previous_pink=0;
  tedge e = e2;
  tvertex lv = l2;
  tvertex tv = branch;
  tedge f=0;
  int npink=0;
  while(1)
      {if ((tv==t1) || ((f = FindInterlaced(e, lv, tv))==0))
          break;
      previous_pink = e;
      npink++;
      e=f;
      Keep(e,Pink);

      // a-t-on gagne avec np ?
      if (IsNPEdgeOk(e))
          {RetVal = 11;//?
          return;
          }
      if ((f=FindLowEdge(e))!=0)
          {down_edge = f; 
          Keep(down_edge,Cyan);
          RetVal = 12;
          return;
          }
      else if (IsDownEdgeOk(e))
          {Keep(down_edge,Cyan);
          if (nvin[np] < nvin[down_edge])
              {if (npink % 2 == 0)
                  {UnKeep(e1,Orange);
                  RetVal = 13;//?
                  }
              else if (low[fils1]<nvin[down_edge])
                  {Keep(elow[fils1],Cyan);
                  UnKeep(e2,Orange);
                  RetVal = 14;//?
                  }
              else RetVal = 23;
              }
          else  RetVal = 15;
          return;
          }
      else if (IsDownEdgeEqualOk(e))
          {Keep(down_edge_equal,Cyan);
          if(EgaliteBasse)
              {f = elow[fils1];
              Keep(f,Yellow); RetVal = 16;//?
              if ((nvin[np]<nvin[down_edge]) 
                  && (nvin[down_edge_equal]<nvin[down_edge]))
                  {UnKeep(e1,Orange); RetVal = 17;}
              else if(ancestor[nvin[-f]] < ancestor[nvin[-e]])
                  {UnKeep(down_edge_equal,Orange); RetVal = 18;}//?
              else
                  RetVal = 19;
              }
          else
              RetVal = 20;
          return;
          }

      lv = nvin[e]; tv = ancestor[nvin[-e]];
      //if(debug())Tprintf("Interlacement of top vertex (%d)",iv(t1)()); 
      }
  //if(debug())Tprintf("Looking for last interlacement");
  f = FindLastInterlaced(e,lv,tv);
  if (f==0)
      {//if (debug()) Tprintf("UnKeep %d",ie(e)());
      UnKeep(e,Orange);
      RetVal = 21;
      e = previous_pink;
      f = FindLastInterlaced(e,nvin[e],ancestor[nvin[-e]]);
      if (f==0)
          {cout <<"No interlecament found"<<endl;
          Success=false;
          DrawGraph();Twait("coucou");
          //if(debug())myabort();
          return;
          }
      }
  else  RetVal = 22;
  e = f;
  Keep(e,Violet);
  down_edge = elow[ancestor_son[nvin[-e]]];
  Keep(down_edge,Cyan);
  //if(debug())Tprintf("Seems OK");
  return;
  }
