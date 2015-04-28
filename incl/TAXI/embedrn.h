#ifndef EMBEDRN_H
#define EMBEDRN_H
#include <TAXI/graphs.h>
#include <TAXI/Tbase.h>
#include <TAXI/Tpoint.h>

class RnEmbedding {
 public:
  const int dmax;
  const int facets;
  const int n;
 private:
  svector<svector<double> *>Space;   
  int i1,i2,i3;
  bool Coord3ok;
  svector<Tpoint3> Coord3; // normalized coordinates

  void ComputeCoord3() {
    double min1,min2,min3,max1,max2,max3;
    min1 = max1 = x(1);
    min2 = max2 = y(1);
    min3 = max3 = z(1);
    int i;
    for(i = 2;i <= n;i++)
      {min1 = Min(min1, x(i)); max1 = Max(max1,x(i));
       min2 = Min(min2, y(i)); max2 = Max(max2,y(i));
       min3 = Min(min3, z(i)); max3 = Max(max3,z(i));
      }
    double min123 = min1; min123 = Min(min123,min2);min123 = Min(min123,min3);
    double max123 = max1; max123 = Max(max123,max2);max123 = Max(max123,max3);
    double alpha = Max(-min123,max123);
    alpha = (alpha < 1.E-12) ? .0 : 1./alpha;
    for(i = 1;i <= n;i++)
      {Coord3[i].x() = alpha * x(i);
      Coord3[i].y() = alpha * y(i);
      Coord3[i].z() = alpha * z(i);
      }
    Coord3ok=true;
    }
 public:
  svector<double> &vector(int d) {return *Space[d];}  
  svector<double> &x() {return *Space[i1];}
  svector<double> &y() {return *Space[i2];}
  svector<double> &z() {return *Space[i3];}
  double &x(int i) {return x()[i];}
  double &y(int i) {return y()[i];}
  double &z(int i) {return z()[i];}
  const svector<double> &x() const {return *Space[i1];}
  const svector<double> &y() const {return *Space[i2];}
  const svector<double> &z() const {return *Space[i3];}
  double x(int i) const {return x()[i];}
  double y(int i) const {return y()[i];}
  double z(int i) const {return z()[i];}
  double rx(tvertex i) 
    { if (!Coord3ok) ComputeCoord3();
      return Coord3[i].x();
    }
  double ry(tvertex i) 
    { if (!Coord3ok) ComputeCoord3();
      return Coord3[i].y();
    }
  double rz(tvertex i) 
    { if (!Coord3ok) ComputeCoord3();
      return Coord3[i].z();
    }
  Tpoint3 operator[] (int i) const { return Tpoint3(x(i),y(i),z(i));}
  void SetAxes(int dx,int dy,int dz) 
    {i1=dx; i2=dy; i3=dz; Coord3ok=false;}
  const svector<Tpoint3> &Coord()
    {if (!Coord3ok) ComputeCoord3();
    return Coord3;
    }
  RnEmbedding(int nv, int dm, int faces): dmax(dm),facets(faces),n(nv),
    Space(0,dm), Coord3ok(false),Coord3(0,n)
    {for (int i=0; i<=dmax; i++)
      Space[i]=new svector<double>(0,n);
    Space[0]->clear();
    SetAxes(1,2,3);
    }
  ~RnEmbedding()
    {for (int i=0; i<=dmax; i++)
      delete Space[i];
    }
};

struct RnEmbeddingPtr
{
  RnEmbedding *ptr;
  RnEmbeddingPtr() : ptr(0) {}
  RnEmbeddingPtr(const RnEmbeddingPtr &) : ptr(0) {} // no copy!
  ~RnEmbeddingPtr() { if (ptr!=0) delete ptr;}
};
T_STD  ostream& operator <<(T_STD  ostream &os,const RnEmbeddingPtr &x);
T_STD  istream& operator >>(T_STD  istream &is, RnEmbeddingPtr &x);
#endif
