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

#ifndef  _TPOINT_H_INCLUDED_
#define  _TPOINT_H_INCLUDED_


#include <TAXI/Tbase.h>

#ifndef PI
#define PI (3.1415926535)
#endif

template<class T> class Point2_;

template<class T> class Point3_
    {private:
        T xt,yt,zt;

     public:
        Point3_(T i,T j,T k): xt(i),yt(j),zt(k){}
        Point3_(Point3_ const &p){xt=p.xt;yt=p.yt;zt=p.zt;}
        Point3_(): xt((T)0),yt((T)0),zt((T)0){}
        Point3_(Point2_ <T> const & p) : xt(p.x()), yt(p.y()), zt((T)0) {}

        T & x(){return(xt);}
        const T & x() const {return(xt);}
        T & y(){return(yt);}
        const T & y() const {return(yt);}
        T & z(){return(zt);}
        const T & z() const {return(zt);}

        Point3_<T> &operator =(const Point3_<T> &point)
            {xt=point.xt;yt=point.yt;zt=point.zt;return(*this);}
        Point3_<T>& operator -=(const Point3_<T> &point2)
            {xt -= point2.xt,yt -= point2.yt,zt -= point2.zt;return *this;}
        Point3_<T>& operator +=(const Point3_<T> &point2)
            {xt += point2.xt,yt += point2.yt,zt += point2.zt;return *this;}
        Point3_<T>& operator +=(const T a)
            {xt += a,yt += a,zt += a;return *this;}
        Point3_<T>& operator -=(const T a)
            {xt -= a,yt -= a,zt -= a;return *this;}
        Point3_<T>& operator *=(const T a)
            {xt *= a,yt *= a,zt *= a;return *this;}
        Point3_<T>& operator /=(const T a)
            {xt /= a,yt /= a,zt /= a;return *this;}
        

    };

template<class T> int operator ==(const Point3_<T> &point1,const Point3_<T> &point2)
  {return((point1.x() == point2.x() && point1.y() == point2.y() && point1.z() == point2.z()) ? 1 : 0);}
template<class T> int operator !=(const Point3_<T> &point1,const Point3_<T> &point2)
  {return(point1.xt != point2.x() || point1.y() != point2.y() || point1.z() != point2.z());}
template<class T> Point3_<T> operator +(const Point3_<T> &point1,const Point3_<T> &point2)
  {return Point3_<T>(point1.x() + point2.x(),point1.y() + point2.y(),point1.z() + point2.z());}
template<class T> Point3_<T> operator -(const Point3_<T> &point1,const Point3_<T> &point2)
  {return Point3_<T>(point1.x() - point2.x(),point1.y() - point2.y(),point1.z() - point2.z());}
template<class T> T operator *(const Point3_<T> &point1,const Point3_<T> &point2)
  {return(point1.x() * point2.x() + point1.y() * point2.y() + point1.z() * point2.z());}

//Operateurs mixtes
template<class T> Point3_<T> operator +(const Point3_<T> &point1,const T a)
  {return Point3_<T>(point1.x() + a,point1.y() + a,point1.z() + a);}
template<class T> Point3_<T> operator -(const Point3_<T> &point1,const T a)
  {return Point3_<T>(point1.x() - a,point1.y() - a,point1.z() - a);}
template<class T> Point3_<T> operator *(const Point3_<T> &point1,const T a)
  {return Point3_<T>(point1.x() * a,point1.y() * a,point1.z() * a);}
template<class T> Point3_<T> operator /(const Point3_<T> &point1,const T a)
  {return Point3_<T>(point1.x() / a,point1.y() / a,point1.z() / a);}
template<class T> Point3_<T> operator *(const T a,Point3_<T> &point1)
  {return Point3_<T>(point1.x() * a,point1.y() * a,point1.z() * a);}


template<class T> T Distance2(const Point3_<T> &point1,const Point3_<T> &point2)
  {return( (point1-point2)*(point1-point2));}
template<class T> T Distance(const Point3_<T> &point1,const Point3_<T> &point2)
  {return( sqrt((point1-point2)*(point1-point2)) );}
template<class T> void Scale(Point3_<T> &point1,const Point3_<T> &mult,const Point3_<T> &add)
    {point1.x() = point1.x()*mult.x() + add.x();
     point1.y() = point1.y()*mult.y() + add.y();
     point1.z() = point1.z()*mult.z() + add.z();
    }

template<class T> T_STD  ostream& operator <<(T_STD  ostream& out,const Point3_<T> point)
    {out << point.x() << "," << point.y() << "," << point.z();return(out);}
template<class T> T_STD  istream& operator >>(T_STD  istream& in, Point3_<T> & point)
{char c; in >> point.x() >> c >> point.y() >> c >> point.z();return (in);}

template<class T> class Point2_
    {private:
        T xt,yt;
        

     public:
        Point2_(T i,T j): xt(i),yt(j){}
        Point2_(Point2_ const &p){xt=p.xt;yt=p.yt;}
        Point2_(Point3_<T> const &p){xt=p.x();yt=p.y();}
        Point2_(): xt((T)0),yt((T)0){}

        T & x(){return(xt);}
        const T & x() const {return(xt);}
        T & y(){return(yt);}
        const T & y() const {return(yt);}

        Point2_<T> &operator =(const Point2_<T> &point2)
            {xt=point2.xt;yt=point2.yt;return(*this);}
        Point2_<T>& operator -=(const Point2_<T> &point2)
            {xt -= point2.xt,yt -= point2.yt;return *this;}
        Point2_<T>& operator +=(const Point2_<T> &point2)
            {xt += point2.xt,yt += point2.yt;return *this;}
        Point2_<T>& operator +=(const T a)
            {xt += a,yt += a;return *this;}
        Point2_<T>& operator -=(const T a)
            {xt -= a,yt -= a;return *this;}
        Point2_<T>& operator *=(const T a)
            {xt *= a,yt *= a;return *this;}
        Point2_<T>& operator /=(const T a)
            {xt /= a,yt /= a;return *this;}
        
        //template<class T> friend Point2_<T> operator -(const Point2_<T> &point1);
        //template<class T>friend T operator *(const Point2_<T> &point1,const Point2_<T> &point2);
        //template<class T>friend int operator ==(const Point2_<T> &point1,const Point2_<T> &point2);
        //template<class T>friend int operator !=(const Point2_<T> &point1,const Point2_<T> &point2);
        //template<class T>friend Point2_<T> operator +(const Point2_<T> &point1,const Point2_<T> &point2);
        //template<class T>friend Point2_<T> operator -(const Point2_<T> &point1,const Point2_<T> &point2);
        //Operateurs mixtes
        //template<class T>friend Point2_<T> operator +(const Point2_<T> &point1,const T a);
        //template<class T>friend Point2_<T> operator -(const Point2_<T> &point1,const T a);
        //template<class T>friend Point2_<T> operator *(const Point2_<T> &point1,const T a);
        //template<class T>friend Point2_<T> operator /(const Point2_<T> &point1,const T a);
        // friend Point2_<T> operator /<T>(const Point2_<T> &point1,const T a);
        //template<class T>friend Point2_<T> operator *(const T a,Point2_<T> &point1);   
        

    };

template<class T> Point2_<T> operator -(const Point2_<T> &point1)
  {return Point2_<T>(-point1.x(),-point1.y());}
template<class T> T operator *(const Point2_<T> &point1,const Point2_<T> &point2)
  {return(point1.x() * point2.x() + point1.y() * point2.y());}
template<class T> int operator ==(const Point2_<T> &point1,const Point2_<T> &point2)
  {return((point1.x() == point2.x() && point1.y() == point2.y()) ? 1 : 0);}
template<class T> int operator !=(const Point2_<T> &point1,const Point2_<T> &point2)
  {return(point1.x() != point2.x() || point1.y() != point2.y());}
template<class T> Point2_<T> operator +(const Point2_<T> &point1,const Point2_<T> &point2)
  {return Point2_<T>(point1.x() + point2.x(),point1.y() + point2.y());}
template<class T> Point2_<T> operator -(const Point2_<T> &point1,const Point2_<T> &point2)
  {return Point2_<T>(point1.x() - point2.x(),point1.y() - point2.y());}

//Operateurs mixtes
template<class T> Point2_<T> operator +(const Point2_<T> &point1,const T a)
  {return Point2_<T>(point1.x() + a,point1.y() + a);}
template<class T> Point2_<T> operator -(const Point2_<T> &point1,const T a)
  {return Point2_<T>(point1.x() - a,point1.y() - a);}
template<class T> Point2_<T> operator *(const Point2_<T> &point1,const T a)
  {return Point2_<T>(point1.x() * a,point1.y() * a);}
template<class T> Point2_<T> operator /(const Point2_<T> &point1,const T a)
  {return Point2_<T>(point1.x() / a,point1.y() / a);}
template<class T> Point2_<T> operator *(const T a,const Point2_<T> &point1)
  {return Point2_<T>(point1.x() * a,point1.y() * a);}

template<class T> T Determinant(const Point2_<T>& point1,const Point2_<T>& point2)
    {return(point1.x() * point2.y() - point1.y()*point2.x());}
template<class T> T x2_x1(const Point2_<T>& point1,const Point2_<T>& point2)
    {return(point2.x()-point1.x());}
template<class T> T y2_y1(const Point2_<T>& point1,const Point2_<T>& point2)
    {return(point2.y()-point1.y());}
template<class T> T Distance2(const Point2_<T> &point1,const Point2_<T> &point2)
    {return( (point1-point2)*(point1-point2));}
template<class T> T Distance(const Point2_<T> &point1,const Point2_<T> &point2)
    {return( sqrt((point1-point2)*(point1-point2)) );}
template<class T> void Scale(Point2_<T> &point1,const Point2_<T> &mult,const Point2_<T> &add)
    {point1.x() = point1.x()*mult.x() + add.x();
     point1.y() = point1.y()*mult.y() + add.y();
    }

template<class T> T_STD  ostream& operator <<(T_STD  ostream& out,const Point2_<T> & point)
    {out << point.x() << "," <<point.y();return (out);}

template<class T> T_STD  istream& operator >>(T_STD  istream& in, Point2_<T> & point)
{char c; in >> point.x() >> c >> point.y();return (in);}

template<class T>double Angle(Point2_<T> p)
    {double b,x,y;
    x = (double)p.x();y = (double)p.y();
    if(x >= 0.0)
        {if(y >= 0.0)b=0.0;
        else {b = x;x = -y;y = b;b = 0.75;}
        }
    else
        {if(y>=0.0){b = x;x = y;y = -b;b = 0.25;}
        else{x = -x;y = -y;b=0.5;}
        }
    if(y <= x)
        {if(x == 0)return(0);
		  return(b + atan(y / x)/(2.0*PI));
		  }
	 else
		  return(b + 0.25 - atan(x/y)/(2.0*PI));
	 }

template<class T>double angle(Point2_<T> p) { return atan2(p.y(), p.x()); }

template <class T>
double dist_line(const Point2_<T> &p0,const Point2_<T> &p1,const Point2_<T> &p2)
// retourne distance au carre du point p0 a la  droite (p1,p2)
	 {if(p1 == p2)return distance2(p0,p1);  //Points confondus
    double det = Determinant(p2-p1,p0-p1);
    return det*det/Distance2(p2,p1);
    }

template <class T>
double dist_seg(const Point2_<T> &p0,const Point2_<T> &p1,const Point2_<T> &p2)
// retourne distance au carre du point p0 au segment (p1,p2)
    {
    double scal =  (p1 - p0)*(p1 - p2);
    double d_12 =  Distance2(p1,p2);
    double d_10 =  Distance2(p1,p0);

    if(p1 == p2){return d_10;}  //Points confondus

    if(scal < 0)                             //distance a p1
        {return(d_10);}
    else if(scal > d_12)
        {return(d_10 - 2.*scal + d_12);}     //distance a p2
    else
        {return(d_10 - scal*scal/d_12);}     //entre p1 et p2
    }
template <class T>
double dist_seg(const Point2_<T> &p0,const Point2_<T> &p1,const Point2_<T> &p2,Point2_<T> &pmin)
// retourne distance au carre du point p0 au segment (p1,p2)
// p realize the shortest distance
    {double scal =  (p1 - p0)*(p1 - p2);
    double d_12 =  Distance2(p1,p2);
    double d_10 =  Distance2(p1,p0);
    pmin = p1;
    if(p1 == p2 || scal < 0)//distance a p1
	{return d_10;}  
    else if(scal > d_12)//distance a p2
        {pmin = p2;
	return(d_10 - 2.*scal + d_12);
	}     
    else //entre p1 et p2
        {pmin =  p1 + (scal/d_12)*(p2 - p1);
	return(d_10 - scal*scal/d_12);
	}     
    }
//! Computes a,b such that x+au=y+bv (x,y are points, u,v are vectors). returns true if a solution exists
/*
 * Princip: if x+au=y+bv, then x^v + a u^v = y^v (as v^v=0). Thus, a=[y-x,v,k]/[u,v,k]
 *                             u^x = u^y + b u^v. Thus, b=[y-x,u,k]/[u,v,k]
 */

template <class T>
inline bool intersect(const Point2_<T> &x,const Point2_<T> &u,
		      const Point2_<T> &y,const Point2_<T> &v,
		      double &a, double &b)
    { double det=Determinant(u,v);
    if (det==0) return false;
    a = Determinant(y-x,v)/det;
    b = Determinant(y-x,u)/det;
    return true;
    }

typedef Point3_<double> Tpoint3;
typedef Point2_<double> Tpoint;

inline void read(T_STD  istream &in,Tpoint &p)
{double x,y;
    in >> x >> y;
    p = Tpoint(x,y);
    }
inline void write(T_STD  ostream &out,const Tpoint &p)
   {out << p.x() <<" "<< p.y()<< T_STD  endl;}


inline void read(T_STD  istream &in,Tpoint3 &p)
{double x,y,z;
    in >> x >> y >> z;
    p = Tpoint3(x,y,z);
    }

inline void write(T_STD  ostream &out,const Tpoint3 &p)
  {out << p.x() <<" "<< p.y()<<" " << p.z()<< T_STD  endl;}

#endif

