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


#ifndef TSVECTOR_H
#define TSVECTOR_H

/*! \file Tsvector.h 
 * \brief Scalar vectors
 */

#include <TAXI/Tbase.h>
#include <TAXI/elmt.h>
#include <TAXI/Tdebug.h>

// Vector resizes by an increment of half the current size but at least 32.
#define TSVECTOR_INCREMENT (Max((finish-start)>>1,32))
//#define _CONSTRUCTOR

//! Simple dynamic scalar vector [start:finish-1].
/*! A scalar vector handles data types without constructor or destructor,
 * that may be copied using a binary copy. This "base" class is untyped; all
 * what is known here about the vector elements are their size. Therefore,
 * any auxiliary storage needs dynamicaly allocated memory. In order not
 * to fraction the memory and not to call allocation more than necessary,
 * the vector data buffer includes two extra rooms: one for temporary storage,
 * the other one to store a default value.
 * Moreover, allocation is done by increments configured by TSVECTOR_INCREMENT
 * constant. Therefore, the buffer bounds may differ from the user bounds.
 * User bounds will correspond to interval [start;finish[ (finish excluded),
 * although available bounds will be [_start+2;_finish[, as two extra rooms are
 * allocated.
 * The reallocation and reservation method will be as follows:
 * - if the vector only handles positive or null indices (_start+2 >= 0), 
 *   the buffer will only grow on its right side (upper indices)
 * - if the buffer handles both positive and negative indices,
 *   the buffer will grow both side symetrically.
 */

class _svector
{
 protected:
  int start;                    // first user index in vector
  int finish;                   // 1 past-end index
  void* ptr0;                   // pointer to buff element corr. with index 0
  void* buff;                   // container of elements + 2 extra
  int _start;                   // lowest allocated index
  int _finish;                  // 1 past-end of highest allocated index
  T_STD size_t size_elmt;       // size in bytes of an element
  void* init;                   // initial value for the elements (&buff[0])
  char name[16];                // name identifying this vector


  friend void Destroy(_svector &x);

  //! Checks if @a index may be valid for buff without reallocation.
  int InBuffRange(int index) const
  {
    return index >= _start+2 && index < _finish;
  }
  //! Pointer to temporary element at index _start+1.
  void* tmpptr()
  {
    return (void*) ((char*) buff+size_elmt);
  }
  const void* tmpptr() const
  {
    return (const void*) ((const char*) buff+size_elmt);
  }
  //! Returns pointer to user element at index i
  void* ptr(int i) {
    return (void*) ((char*) ptr0+i*size_elmt);
  }
  const void* ptr(int i) const
  {
    return (const void*) ((const char*) ptr0+i*size_elmt);
  }
  //! Makes sure vector can be indexed in range [a:b].
  /*! If a resize has to be performed, the size increment will be at least
   *  equal to TSVECTOR_INCREMENT. If the vector may handle negative user
   * indices (start<0) or growth is requested both sides, 
   * growth is performed in a symetric way.
   */
  void ReserveGrow(int a, int b)
  {
    if (InBuffRange(a) && InBuffRange(b))
      return;
    int inc = Max(Max(b+1-finish,start-a),TSVECTOR_INCREMENT);
    if (a < _start+2 || start < 0) 
      _start -= inc;
    if (b > _finish-1 || start < 0)
      _finish += inc;
    realloc_buffer();
  }
  //! Reallocates buffer to hold (_finish-_start) elements.
  void realloc_buffer()
  {
    // Actual buffer size: (nr. elements + 2) * size_elmt
    unsigned int nsize = (unsigned int) (_finish - _start)*size_elmt;
#ifndef _CONSTRUCTOR
    char* nbuff = (char*) ::operator new (nsize);
#else
	char* nbuff = new char[nsize];
	//DebugPrintf("NEW0:%p",nbuff);
#endif

    if (start != finish) { // vector is not empty
      memcpy((void*) (nbuff+(start-_start)*size_elmt), begin(), getsize());
    }
    if (init) { // a default value has been set
      memcpy((void*) nbuff, buff, size_elmt);
      init = nbuff;
    }
#ifndef _CONSTRUCTOR
	delete (char *) buff;
#else
    //DebugPrintf("DEL0 %s:%p",name,buff);
	delete [] (char *) buff;
#endif
    buff = (void*) nbuff;
    ptr0 = (void*) (nbuff - _start*size_elmt);
  }
 public:
  //! Reserves rooms to handle [a;b].
  /*! This methods concerns the case where the extremal user bounds are known
   * in advance. In such a case, we want a tight allocation (no need to reserve
   * more that what we know to be possibly used).
   */
  void reserve(int a, int b)
  {
    if (InBuffRange(a) && InBuffRange(b))
      return;
    if (a < _start+2)
       _start = a-2;
    if (b > _finish-1)
      _finish = b+1;
    realloc_buffer();
  }
 protected:
  //! Makes buffer [s:f] with elements of size @a nsize_elmt.
  /*! This method will be used to "reformat" a vector during a copy
   * operation. Two cases may occur: if the new element size is equal to
   * the old element size, we assume that the default value (when set)
   * will still be valid. It is hence copied, should a reallocation occur.
   * Otherwise, default value is unset.
   */
  void vreserve(int s, int f, T_STD size_t nsize_elmt)
  {
    unsigned int osize = (unsigned int) (_finish-_start)*size_elmt;
    unsigned int nsize = (unsigned int) (f-s)*nsize_elmt;

    if (nsize > osize) {
#ifndef _CONSTRUCTOR
    char* nbuff = (char*) ::operator new (nsize);
#else
	char* nbuff = new char[nsize];
	//DebugPrintf("NEW1:%p",nbuff);
#endif
      

      if (init)
        {if (nsize_elmt == size_elmt) {
          memcpy((void*) nbuff, buff, size_elmt);
          init = nbuff;
        }
        else
          init = 0;
          }

#ifndef _CONSTRUCTOR
	delete (char *) buff;
#else
    //DebugPrintf("DEL1:%p",nbuff);
	delete [] (char *) buff;
#endif
	
      buff = (void*) nbuff;
    }
    ptr0 = (void*) ((char*) buff-s*nsize_elmt);
    _start  = s;
    _finish = f;
    size_elmt = nsize_elmt;
  }
  //! copy helper (for copy constructor and assignment)
  _svector& copy(const _svector& s)
  {
    if (&s==this)
      return *this;
    vreserve(s.start-2, s.finish, s.size_elmt);
    start  = s.start;
    finish = s.finish;
    memcpy(begin(), s.begin(), getsize());
    return *this;
  }

public:

  //! Empty vector constructor.
  /*! No allocation performed at all for efficiency (transient state)
   */
  _svector(): start(0), finish(0), ptr0(0), buff(0),
              _start(0), _finish(0), size_elmt(0), init(0)
  {
    strcpy(name,"*");
    name[sizeof(name)-1]='\0';
  }
  //! Constructs vector [a:b] of elements of size @a size_elt init to @a *p.
  // [a:a-1] is a special case and creates an empty vector
  _svector(int a, int b, int size_elt, const void* p=0):
    start(a), finish(b+1),size_elmt(size_elt),init(0)
  {
    unsigned int sizebuff = getsize()+2*size_elmt;

    if (sizebuff)
	{
#ifndef _CONSTRUCTOR
    buff = (char*) ::operator new (sizebuff);
#else
	buff = new char [sizebuff];
	//DebugPrintf("NEW2:%p",buff);
#endif
	}
    else
      buff = 0;
    _start  = start-2;
    _finish = finish;
    ptr0 = (void*) ((char*) buff-start*size_elmt);
    definit(p);
    strcpy(name,"*");
    name[sizeof(name)-1]='\0';
    reset();
  }
  _svector(const _svector& s): start(0), finish(0), ptr0(0), buff(0),
                               _start(0), _finish(0), size_elmt(0),init(0)
  {
    strcpy(name,"*");
    name[sizeof(name)-1]='\0';
    copy(s); 
  }
  ~_svector() 
	{
#ifndef _CONSTRUCTOR
	delete (char *) buff;
#else
    //DebugPrintf("DESTRUCTOR:%s %p",name,buff);
	delete [] (char *) buff;
#endif
	 
  }	

  //! Tests whether vector is empty (no elements).
  int empty() const {return start==finish;}
  //! Checks whether @a user index is valid for vector.
  int InRange(int index) const {return index >= start && index < finish;}
  //! Returns size of vector in bytes.
  int getsize() const {return size_elmt*(finish-start);}
  //! Returns length of vector in number of elements.
  int n() const {return finish-start;}
  //! Returns size of a single element in bytes.
  int SizeElmt() const {return size_elmt;}
  //! Returns first index of vector.
  int starti() const {return start;}
  //! Returns 1 past-end index of vector.
  int stopi() const {return finish;}
  //! Returns pointer to first element of vector.
  void* begin() {return ptr(start);}
  const void* begin() const {return ptr(start);}
  //! Returns pointer to element at index 0.
  void* origin() {return ptr0;}
  const void* origin() const {return ptr0;}
  //! Returns pointer to 1 past-end element.
  void* end() {return ptr(finish);}
  const void* end() const {return ptr(finish);}
  //! Resize vector to index range [a:b].
  void resize(int a, int b)
  {
    ReserveGrow(a,b);
    int old_start  = start;
    int old_finish = finish;
    start  = a;
    finish = b+1;
    if (a <  old_start)
      reset(a, old_start-1);    // Init [a:old_start-1]
    if (b >= old_finish)
      reset(old_finish, b);     // Init [old_finish:b]
  }
  //! Clears all elements to 0.
  void clear()
  {
    int s=getsize();
    if (s!=0)
      memset(begin(), 0, s);
  }
  //! Fill vector [from:to] with @a byte values.
  void fill(int from, int to, char byte=0)
  {
    check(from);
    check(to);
    if (from>to)
      return;
    memset(ptr(from), byte, (to-from+1)*size_elmt);
  }
  //! Sets vector [a:b] to initial value (if defined).
  void reset(int a, int b)
  {
    if (!init)
      return;
    check(a);
    check(b);
    char* p = (char*) ptr(a);
    for (int i = a; i <= b; i++) {
      memcpy(p, init, (unsigned int) size_elmt);
      p += size_elmt;
    }
  }
  //! Sets all vector elements to initial value (if defined).
  void reset() {reset(start, finish-1);}
  //! Swaps vectors.
  void Tswap(_svector &s)
  {
    int tmp=start; start=s.start; s.start=tmp;
    tmp = finish; finish=s.finish; s.finish=tmp;
    tmp=_start; _start=s._start; s._start=tmp;
    tmp = _finish; _finish=s._finish; s._finish=tmp;
    void *ptr=buff; buff=s.buff; s.buff=ptr;
    ptr=ptr0; ptr0=s.ptr0; s.ptr0=ptr;
    ptr=init; init=s.init; s.init=ptr;
    tmp = size_elmt; size_elmt=s.size_elmt; s.size_elmt=tmp;
  }
  _svector& operator=(const _svector& s)
  {
    return copy(s);
  }
  //! Returns pointer to initial value.
  const void* pinit() const {return init;}
  void* pinit() {return init;}
  //! Defines @a p to be the initial value. When 0, clears it.
  void definit(const void* p)
  {
    if (p) {
      if (!init) {
        if (!buff) // empty vector without any allocation
          reserve(0,-1); // reserves extra rooms only (vector still empty)
        init=buff;
      }
      memcpy(init, p, size_elmt);
    }
    else
      init = 0;
  }
  void SetName(const char* txt) {strncpy(name, txt, sizeof(name)-1);}
  const char* GetName() const {return name;}

#ifdef TDEBUG
  //! checks wether an index belongs to user bounds and aborts if not
  void check(int i) const
  {
    if (!InRange(i)) {
      if (empty()) 
        DPRINTF(("%s indice %d for empty vector",name,i))//;
      else 
        DPRINTF(("%s indice %d out of [%d %d[",name,i,start,finish))//;
        myabort();
    }
  }
#else
  void check(int) const {}
#endif

  //! Swaps elements indexed by @a a and @a b.
  void SwapIndex(int a, int b)
  {
    check(a); check(b);
    memcpy(tmpptr(), ptr(a), size_elmt);
    memcpy(ptr(a), ptr(b), size_elmt);
    memcpy(ptr(b), tmpptr(), size_elmt);
  }
  //! Copies element @a b to @a a.
  void CopyIndex(int a,int b)
  {
    check(a);
    check(b);
    memcpy(ptr(a), ptr(b), size_elmt);
  }
  void qsort(int (*compare)(const void*, const void*))
  {
    ::qsort(begin(), n(), SizeElmt(), compare);
  }
  bool operator==(const _svector &v) const
  {
    if (&v==this) return true;
    if ((v.n()!=n()) || (v.SizeElmt()!=SizeElmt()) || (v.starti()!=starti())) return false;
    return (memcmp(begin(),v.begin(),getsize())==0);
  }
  bool operator!=(const _svector &v) const {return ! (v==*this);}
};


//! Dynamic vector with elements of type @a T.
template <class T>
class svector: public _svector {
private:
  _svector& me() {return *this;}
  const _svector& me() const {return *this;}
public:
  svector(): _svector(0, -1, sizeof(T)) {}
  svector(int a, int b): _svector(a, b, sizeof(T)) {}
  svector(int a, int b, const T& value):
  _svector(a, b, sizeof(T), (const void*) &value) {}
  svector(int n): _svector(0, n-1, sizeof(T)) {}
  svector(const svector& v): _svector(v) {}
  svector(int a, int b,const T *p): _svector(a,b,sizeof(T)) 
      {if(p == 0)myabort();
      memcpy(me().begin(),p,me().getsize());
      }
  ~svector() {}

  svector& operator=(const svector& v) {me() = v; return *this;}
  //! access to element with index i (should exist)
  T& operator[](int i) {check(i); return ((T*) ptr0)[i];}
  const T& operator[](int i) const {check(i); return ((T*) ptr0)[i];}
  T& operator[](tbrin i) {check(i()); return ((T*) ptr0)[i()];}
  const T& operator[](tbrin i) const {check(i()); return ((T*) ptr0)[i()];}
  T& operator[](tedge i) {check(i()); return ((T*) ptr0)[i()];}
  const T& operator[](tedge i) const {check(i()); return ((T*) ptr0)[i()];}
  T& operator[](tvertex i) {check(i()); return ((T*) ptr0)[i()];}
  const T& operator[](tvertex i) const {check(i()); return ((T*) ptr0)[i()];}

  //! access to element with index i (resizes vector when necessary)
  T& operator()(int i)
  {
    if (i < start)
      resize(i, finish-1);
    else if (i >= finish)
      resize(start, i);
    return ((T*) ptr0)[i];
  }
  T& operator()(tedge i) {return (*this)(i());}
  T& operator()(tvertex i) {return (*this)(i());}
  T& operator()(tbrin i) {return (*this)(i());}

  //! pointer to first element
  T* begin() {return (T*) (me().begin());}
  const T* begin() const {return (const T*) (me().begin());}
  //! pointer after last element (first invalid index)
  T* end() {return (T*) (me().end());}
  const T* end() const {return (const T*) (me().end());}
  //! pointer to origin element (index 0)
  T* origin() {return (T*) (me().origin());}
  const T* origin() const {return (const T*) (me().origin());}

  //! static explicit conversion method from untyped vector
  static svector& cast(_svector& v) {return *(svector*) &v;}
  static const svector& cast(const _svector& v)
  {
    return *(const svector*) &v;
  }

  //! pointer to default value
  const T* pinit() const {return (T*) (me().pinit());}
  //! defines a default value
  void definit(const T&value) {me().definit((const void *) &value);}
  //! gets the default value
  void getinit(T& value) const {const T* p = pinit(); if (p) value = *p;}
  //! deletes element with index i, by shifting all elements right to it to the left
  void DeletePack(int i) {
    check(i);
    ((T*) ptr0)[--finish]= ((T*) ptr0)[i];
  }
  void Tswap(svector<T>& v)
     {if(me().SizeElmt() != v.me().SizeElmt())
		{DebugPrintf("TWAP ERROR_SIZE");myabort();}
	  me().Tswap(v.me());
	 }
  //! swaps two elements
  void SwapIndex(int a, int b) {
    T& ra = (*this)[a]; T& rb = (*this)[b]; T tmp=ra; ra=rb; rb=tmp;
  }
  //! alias for the vector (for use in derived classes)
  svector& vector() {return *this;}
};

//! Reference to a dynamic vector with elements of type @a T.
/*! The fundamental reason why such a template has been written is the impossibility,
 * in C++, to derive a class from "reference to XXX". Hence, we have to build a
 * template base classe that won't be more than "reference to svector<T>".
 */
template <class T>
class rsvector {
protected:
  _svector& v;
public:
  rsvector(_svector& s): v(s) {}
  ~rsvector() {}

  T& operator[](int i) {v.check(i); return origin()[i];}
  const T& operator[](int i) const {v.check(i); return origin()[i];}
  T &operator()(int i) {return svector<T>::cast(v)(i);}

  T& operator[](tbrin i) {v.check(i()); return origin()[i()];}
  const T& operator[](tbrin i) const {v.check(i()); return origin()[i()];}
  T& operator[](tedge i) {v.check(i()); return origin()[i()];}
  const T& operator[](tedge i) const {v.check(i()); return origin()[i()];}
  T& operator[](tvertex i) {v.check(i()); return origin()[i()];}
  const T& operator[](tvertex i) const {v.check(i()); return origin()[i()];}
  T& operator()(tedge i) {return (*this)(i());}
  T& operator()(tvertex i) {return (*this)(i());}
  T& operator()(tbrin i) {return (*this)(i());}

  T* begin() {return (T*) v.begin();}
  T* origin() {return (T*) v.origin();}
  T* end() {return (T*) v.end();}
  const T* begin() const {return (T*) v.begin();}
  const T* origin() const {return (T*) v.origin();}
  const T* end() const {return (T*) v.end();}
  void clear() {v.clear();}
  void fill(int from, int to, char byte=0) {v.fill(from, to, byte);}
  void Tswap(_svector &s) 
	{if(v.SizeElmt() != s.SizeElmt())
		{DebugPrintf("TWAP ERROR_SIZE");myabort();}
	  v.Tswap(s);
	}
  rsvector &operator=(const _svector &s) {v=s; return *this;}
  rsvector &operator=(const rsvector &s) {v=s.v; return *this;}
  rsvector &operator=(const svector<T> &s) {v=s; return *this;}
  int starti() const {return v.starti();}
  int stopi() const {return v.stopi();}
  operator svector<T>& () { return svector<T>::cast(v);}
  void resize(int a, int b) {v.resize(a,b);}
  const T* pinit() const { return (T*) (v.pinit());}
  void definit(const T& value) {v.definit((const void*) &value);}
  void getinit(T& value) {svector<T>::cast(v).getinit(value);}
  int empty() const {return v.empty();}
  int InRange(int index){return v.InRange(index);}
  void SetName(const char *txt){v.SetName(txt);}
  const char *GetName() const {return v.GetName();}
  int n() const {return v.n();}
  void DeletePack(int i)  {svector<T>::cast(v).DeletePack(i);}
  void SwapIndex(int a, int b) {svector<T>::cast(v).SwapIndex(a,b);}
  svector<T>& vector() {return svector<T>::cast(v);}
  bool operator==(const _svector &v) const {return v==*this;}
  bool operator==(const rsvector &v) const {return v.v==*this;}
  bool operator==(const svector<T> &v) const {return v==*this;}
  bool operator!=(const _svector &v) const {return v==*this;}
  bool operator!=(const rsvector &v) const {return v.v==*this;}
  bool operator!=(const svector<T> &v) const {return v==*this;}
};

//! Computes the maximum element of a vector
template <class T>
T MaxElement(const svector<T>& v)
{
  const T* p = v.begin();
  T maxe = *p;

  for (; p != v.end(); p++)
    if (*p > maxe)
      maxe = *p;
  return maxe;
}

//! Computes the minimum element of a vector
template <class T>
T MinElement(const svector<T>& v)
{
  const T* p = v.begin();
  T mine = *p;

  for (; p != v.end(); p++)
    if (*p < mine)
      mine = *p;
  return mine;
}

//! Fills a vector with a value
template <class T>
void Fill(svector<T>& v, const T& x)
{
  for (T* p = v.begin(); p != v.end(); p++)
    *p = x;
}

template <class T>
T_STD  ostream & operator<<(T_STD  ostream & out, const svector<T *> &X)
{ for (int i=X.starti(); i<X.stopi(); i++) if (X[i]==(T *)0) out<<"(null) "; else out<<X[i]<<""; return out;}
#endif
