// Simple Planarity Testing by Daniel Ford (Department of Computer Science, Linfield College)
// single-pass recursive left-right planrity tester based on 
// de Fraysseix and Rosenstiehl's left-right planarity criteria 
#include <vector>
#include <algorithm>
#include <TAXI/graphs.h>
#include <TAXI/ConstraintList.h>

bool mergeConstraintLists(vector<ConstraintList>& vcl, const int v, ConstraintList& mcl);
bool SinglePasslralgo(int n, const svector<tvertex>& vin, const Prop<tbrin>& cir, vector<bool>& visited);

int TopologicalGraph::TestSinglePassPlanar() 
    {int m = ne();
  int n = nv();
  //int m_origin = m;

   if(m < 9 || n < 5) {Prop1<int> isplanar(Set(),PROP_PLANAR); return 1;}
 // changed n < 4 to n < 5

 // #ifdef TDEBUG
 
 //   if(!DebugCir())
 
//       {DebugPrintf("input Cir is wrong");setPigaleError(A_ERRORS_BAD_INPUT);return A_ERRORS_BAD_INPUT;}

 // #endif
  
    if(debug())DebugPrintf("Executing TopologicalGraph:TestSinglePassPlanar");
 


  // DFS calls GDFS after some initializations
 
  //svector<tvertex> nvin(-m,m); nvin.SetName("TestSinglePassPlanar:nvin");
 
  //if(!DFS(nvin)) 
 //Not connected graph
   
 //    {MakeConnected();
  
 //    m = ne();
  
 //    nvin.resize(-m,m);
  
 //    DFS(nvin);
  
 //    for(tedge e = m; e() > m_origin;--e)
  
 //        DeleteEdge(e);
  
 //    }
   
 //else
  
 //    Prop1<int>is_connected(Set(),PROP_CONNECTED);
    if(m - n < 3){Prop1<int> isplanar(Set(),PROP_PLANAR);return 1;}
    if(Set().exist(PROP_SIMPLE) && m > 3*n - 6)return 0;
 

  //svector<tvertex> low(0,n); low.SetName("low TestPlanar");
 
  //_Bicon Bicon(n);
  //if(bicon(n,m,nvin,Bicon,low))
  
 //    Prop1<int> isbicon(Set(),PROP_BICONNECTED);

  
 //_LrSort LrSort(n,m);
  
 //NewLralgoSort(n,m,nvin,Bicon,low,LrSort);
  
 //int ret = Newfastlralgo(n,m,nvin,Bicon,LrSort);
 
	int ret = 0;
 
	vector<bool> visited(nv() + 1);
 
	for (int v = 1; v <= nv(); ++v) 
        {if ( ! visited[v])
            {visited[v] = true;
        
	tbrin b0 = pbrin(v);
        
	cir[0] = b0; acir[0] = acir[b0]; cir[acir[b0]] = 0;
        
	if ((ret = SinglePasslralgo(n, vin, cir, visited)) == false)
                {cir[0] = 0;  cir[acir[0]] = b0; acir[0] = 0; // restitute cir
         
		break;
         
		}
         
  cir[0] = 0;  cir[acir[0]] = b0; acir[0] = 0; // restitute cir
         
  }
        }
    if(ret)Prop1<int> isplanar(Set(),PROP_PLANAR);
    else planarMap() = -1;
    if(debug())DebugPrintf("    END TopologicalGraph:TestSinglePassPlanar");
    return ret;
    }
         


// Returns true if the connected component of the graph is planar
// and false otherwise
// After the function returns mcl contains the list of constraints on 
// the fundamental cycles that contain the edge (v, vertices[i]) 

bool SinglePasslralgo(int n, const svector<tvertex>& vin, const Prop<tbrin>& cir, vector<bool>& visited) 
	{// vvcl.size() == depth of current edge in DFS tree.
	
	// For each DFS tree edge, one ConstraintList for every child tree edge already backtracked.
	
vector<vector<ConstraintList> > vvcl;
	svector<tbrin> tb(0,n);		tb.SetName("SPlralgo:tb");
	
	svector<int> dfsnum(0,n);	dfsnum.SetName("SPlralgo:dfsnum");
	tbrin b = cir[0];
	tvertex v = vin[b];
	tvertex w;

	tb.clear();
	tb[v] = b; 
	dfsnum[v] = 1;
	//for(int i = 1;i <= m ;i++) DebugPrintf("GDFS input:%d %d",vin[i](),vin[-i]());
	//DebugPrintf("N=%d  M=%d",n,m);


	// following copied and modified from Graph::GDFS
	
while(1)
	
		{w = vin[-b];						// w current vertex
	  
		if (tb[w] != 0)						// w known
	  			
{if (b == tb[v])				// backtraking on the tree
	  			
    {b.cross();
	  			
	 v = w;
	  			 
	 if ( vvcl.size() > 1 )
	  			  
	     {ConstraintList mcl;						// the constraint list for the edge being backtracked
	  			    
	 if ( ! mergeConstraintLists(vvcl.back(), dfsnum[v], mcl) )
	  			     
		    {return false; }					// constraint lists for mcl were contradictory - graph is non-planar
	  			     	 vvcl.pop_back();						// remove the ConstraintLists that were merged into mcl
	  			     	 if ( ! mcl.empty() )				 	// to facilitate sorting
	  			     		    {vvcl.back().push_back(mcl);
	  			     		    }		// add mcl to the list of ConstraintL+ists for its parent edge in the DFS tree

	  			         }
				
	  			     }
	  	         else if(dfsnum[v] > dfsnum[w])				// back edge
	  			 	{vvcl.back().push_back(ConstraintList(dfsnum[w])); }
	  		     }
	  		else											// tree edge (w not known)
	  		
	{if(w == 0) break;
	  		
	dfsnum[w] = dfsnum[v] + 1;
	  		 
	b.cross();									// opposite half edge
	  		 
	v = w;
	  		 
	tb[v] = b;
	  		 
	visited[w()] = true;
	  		 
	vvcl.push_back(vector<ConstraintList>());	// add a new vector of constraint lists
	  		 
	}
	  		b = cir[b];
	  		}//endwhile
	 return true;	  		  
     }

// returns false if the constraints in the first parameter vcl are inconsistent
// to signify that the graph is non-planar.
// otherwise returns true and mcl constains the merged constraints whith any cycles
// containing v pruned out
bool mergeConstraintLists(vector<ConstraintList>& vcl, const int v, ConstraintList& mcl) 
    {
	if (vcl.empty()) {	
		mcl.clear();
	}
	else {
		sort(vcl.begin(), vcl.end());
		mcl.swap(vcl[0]);
		for(unsigned int i = 1; i < vcl.size(); ++i) { 
			if (mcl.merge(vcl[i]) == false) { 
				return false; 
			} 
		}
		mcl.prune(v);
	}
	return true;
}
