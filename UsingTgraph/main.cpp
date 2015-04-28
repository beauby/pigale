#include <Pigale.h>
#include <limits>


    
int main ()
{   
  GraphContainer GC; // defined in TAXI/graph.h
  // GC is the object that will contains all the information of a graph.
  int n = 4; // n =  number of vertices [1,n]
  int m = 5; // m = number of edges     [1,m] 
  
  GC.setsize(n,m);  // defines the size of the container
  /*
   - a tvertex v is internally represented by an integer v(): 1 <= v() <= n = GC.nv() 
   - a tedge   e is internally represented by an integer e(): 1 <= e() <= m = GC.ne() 
   - a tedge   e is composed of 2 tbrin b0,b1 internally represented by e() and -e()
   tvertex, tedge, tbrin behave like integers in many respects
  */ 
  
  Prop<tvertex> vin(GC.Set(tbrin()),PROP_VIN);// vin is an array  of tbrin whose values are tvertex.

  // Create the edges: each edge (tedge) is incident to 2 vertices (tvertex)
  vin[1] = 1; vin[-1] = 2;  // means that the edge 1 is incident to the vertex 1 and 2
  vin[2] = 1; vin[-2] = 3;  
  vin[3] = 2; vin[-3] = 3; 
  vin[4] = 3; vin[-4] = 4; 
  vin[5] = 2; vin[-5] = 4;  // vin[-5] = 4 could be written vin[(tbrin)-5] = (tvertex)4;
  // create a topological graph access
  TopologicalGraph G(GC); // defined in TAXI/graphs.h
  // print the number of vertices and edges
  cout << "Nodes: " << G.nv() << "\tEdges: " << G.ne()<< endl;

  // print the edges (if e is a tedge, e() is the int that represents it)
  cout << "Edges:" << endl;
  for(tedge e = 1; e <= G.ne();e++)
    cout << e() << " = [" << G.vin[e] << "," << G.vin[-e] << "]" <<endl;

  // at this point loops are FORBIDDEN. You can remove them:
  // int nloops = RemoveLoops();
   cout<<"**********"<<endl;
  // Compute a planar embedding or return -1
  if(G.Planarity() == 0) 
    {cout << "not planar" << endl; return -1;}
  else
    cout << "planar graph" << endl;

  // At each vertex v  there is a tbrin G.pbrin[v] incident to it: G.vin[G.pbrin[v]] = v;
  // So we can print the planar map (cirular order of half edges around each  vertex)
  cout << "Map (half edges):"<<endl;
  for(tvertex v = 1; v <= G.nv() ; v++)
    {cout << v() <<"  -> ";
      tbrin first = G.pbrin[v];
      tbrin b = first;
      do
	{cout << b() << " ";
	}while((b = G.cir[b]) != first);
      cout << endl;
    }
  // Or you could print the circular order of vertices aroud each vertex
  cout << "Map (vertices):"<<endl;
  for(tvertex v = 1; v <= G.nv() ; v++)
    {cout << v() <<"  -> ";
      tbrin first = G.pbrin[v];
      tbrin b = first;
      do
	{cout << G.vin[-b]() << " ";
	}while((b = G.cir[b]) != first);
      cout << endl;
    } 
  return 0;      
  }


